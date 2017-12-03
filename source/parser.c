#include "parser.h"
#include "err.h"
#include "expression.h"
#include "symtable.h"
#include "memwork.h"
#include "scanner.h"
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include "codegen.h"
#include "token_stack.h"

int line = 0;
int act_type = -1;
char *gen_label(char *ret) {
    static int label = 0;
    char *result = my_malloc(sizeof(char) * BUFFSIZE);
    if (ret == NULL) {
        snprintf(result, BUFFSIZE, "l_%i", label++);
    } else {
        snprintf(result, BUFFSIZE, "l_%s_%i", ret, label++);
    }

    return result;
}

t_token *check_next_token_type(int type) {
    t_token *input = get_token();
    line = input->line;
    check_pointer(input);

    if (input->token_type != type) {
        syntax_error(ERR_SYNTA,line);
    }
    return input;
}

bool check_token_int_value(t_token *input, int value) {
    check_pointer(input);

    if (input->data.i != value) {
        return false;
    }
    return true;
}

bool check_pointer(void *input) {
    if (input == NULL) {
        syntax_error(ERR_SYNTA,line);
    }
    return true;
}

bool token_is_data_type(t_token *input) {
    check_pointer(input);
    return is_data_type(input->data.i);
}

void all_defined(TTable *tbl){
    if (tbl != NULL) {
        for (unsigned int i = 0; i < tbl->size; i++) {
            TElement *active = tbl->list_firsts[i];
            if (active != NULL) { //je tam prvni

                while (active->next != NULL) { //je jich vic
                    active = active->next;
                    if (active->data->isDefined != true) {
                        semerror(ERR_SEM_DEF, line); //neni definovana funckia
                    }
                }
                if (active->data->isDefined != true) {
                    semerror(ERR_SEM_DEF, line); //neni definovana funckia
                }
            }
        }
    }
}

int parse(TTable *func_table) {
    t_token *input = get_token();
    line = input->line;
    check_pointer(input);
    while (input->token_type == EOL) {  //preskoc prazdne riadky
        input = get_token();
        line = input->line;
        check_pointer(input);
    }

    //prvni token musi byt Declare nebo Function nebo SCOPE
    bool isKeyWord = input->token_type == KEY_WORD;
    tdata value = input->data;
    if (isKeyWord) {
        switch (value.i) {
            case k_declare:
                input = check_next_token_type(KEY_WORD);
                check_token_int_value(input, k_function);
                function(k_declare, func_table, NULL);

                return parse(func_table);
            case k_function: {
                TTable *local = Tbl_Create(8);
                int correct = function(k_function, func_table, local); //parametry, konci tokenem EOL
                if (correct == k_function) { //function
                    return parse(func_table);
                } else {
                    syntax_error(ERR_SYNTA,line);
                }
            }
            case k_scope: //scope
                if (scope(func_table) == SUCCESS) {
                    //todo skontrolovat ze vsetky deklarovane funkcie su aj definovate
                    all_defined(func_table);
                    return SUCCESS;
                }
            default:
                syntax_error(ERR_SYNTA,line);
        }
    } else {
        syntax_error(ERR_SYNTA,line);
    }
    return SUCCESS;
}

int function(int decDef, TTable *func_table, TTable *local) {
    t_token *input;
    input = check_next_token_type(ID);
    char *name_f = input->data.s;
    int ret_type = -1;
    if (decDef == k_function) {
        create_3ac(I_LABEL, NULL, NULL, name_f);
        create_3ac(I_PUSHFRAME, NULL, NULL, NULL);
        create_3ac(I_CREATEFRAME, NULL, NULL, NULL);
        create_3ac(I_DEFVAR, NULL, NULL, "TF@%RETVAL");
    }
    TData *func = NULL;
    TSymbol *sym = NULL;
    TFunction *f = NULL;

    unsigned attr_count = 0;
    int *attr_types = NULL;

    check_next_token_type(LPAR);
    params(func_table, local, &attr_count, &attr_types, decDef);                //spracuj parametre a )
    check_token_int_value(check_next_token_type(KEY_WORD), k_as);
    input = check_next_token_type(KEY_WORD);
    if (token_is_data_type(input)) {
        ret_type = input->data.i;
    } else {
        syntax_error(ERR_SYNTA,line);
    }

    check_next_token_type(EOL);

    //zapis do symtable
    TElement *getElement = Tbl_GetDirect(func_table, name_f);
    if (getElement == NULL) {
        func = Func_Create(ret_type, attr_count, attr_types);
        sym = Sym_Create(ST_Function, func, name_f);
        if (decDef == k_function) {
            sym->isDefined = true;
            sym->isDeclared = true;
        } else if (decDef == k_declare) {
            sym->isDeclared = true;
            sym->isDefined = false;
        } else {
            clear_all();
            exit(ERR_INTER);
        }
        getElement = El_Create(sym);
        Tbl_Insert(func_table, getElement);
    } else {
        if (getElement->data->type != ST_Function) {
            semerror(ERR_SEM_DEF,line);
        } else if (getElement->data->isDefined == true && decDef == k_function) {
            semerror(ERR_SEM_DEF,line);
        } else if (getElement->data->isDeclared == true && decDef == k_declare) {
            semerror(ERR_SEM_DEF,line);
        } else if (getElement->data->isDefined == true && decDef == k_declare) {
            semerror(ERR_SEM_DEF,line);
        }
        getElement->data->isDefined = true;
        getElement->data->isDeclared = true;
        f = getElement->data->data->func;
        if (f->attr_count != attr_count ||  f->return_type != ret_type) {
            semerror(ERR_SEM_DEF,line);
        }
        for (unsigned i = 0; i < attr_count; ++i) {
            if (f->attributes[i] != attr_types[i]) {
                semerror(ERR_SEM_DEF,line);
            }
        }
    }

    //nacitanie prikazov ak je toto definicia
    int i = 0;
    if (decDef == k_function) {
        act_type = ret_type;
        switch (ret_type){
            case k_integer:
                create_3ac(I_MOVE, "int@0", NULL, "TF@%RETVAL");
                break;
            case k_double:
                create_3ac(I_MOVE, "float@0", NULL, "TF@%RETVAL");
                break;
            case k_string:
                create_3ac(I_MOVE, "string@", NULL, "TF@%RETVAL");
        }


        i = commandsAndVariables(func_table, local);
        act_type = -1;
        create_3ac(I_PUSHS, NULL, NULL, "TF@%RETVAL");
        create_3ac(I_POPFRAME, NULL, NULL, NULL);
        create_3ac(I_RETURN, NULL, NULL, NULL);
    }

    return i;
}

//TODO pridaj func_table a pozri sa aj tam ci uz neexistuje funckia s rovn menom
int params(TTable *func_table, TTable *local, unsigned *attr_count, int **attributes, int decDef) {
    int start = 1;
    TValue value;
    TData *var = NULL;
    TSymbol *lSymbol = NULL;
    TElement *element = NULL;

    t_token *input = get_token();
    line = input->line;
    if (start && input->token_type == RPAR) {
        return SUCCESS;
    }
    while (input->token_type == ID) {

        char *name = input->data.s;
        if (Tbl_GetDirect(local, name) != NULL || Tbl_GetDirect(func_table, name) != NULL) {
            redefine_error(name,line);
        }

        check_token_int_value(check_next_token_type(KEY_WORD), k_as);
        input = check_next_token_type(KEY_WORD);
        //je nacitane id as typ

        (*attr_count)++;
        *attributes = my_realloc(*attributes, sizeof(int) * (*attr_count));
        if (check_token_int_value(input, k_integer)) {
            (*attributes)[(*attr_count) - 1] = E_integer;
            value.i = 0;
        } else if (check_token_int_value(input, k_double)) {
            (*attributes)[(*attr_count) - 1] = E_double;
            value.d = 0.0;

        } else if (check_token_int_value(input, k_string)) {
            (*attributes)[(*attr_count) - 1] = E_string;
            value.s = "";
        } else {
            syntax_error(ERR_SYNTA,line);
        }


        var = Var_Create(value, (*attributes)[(*attr_count) - 1]);
        lSymbol = Sym_Create(ST_Variable, var, name);
        lSymbol->isDeclared = true;
        lSymbol->isDefined = true;
        element = El_Create(lSymbol);
        Tbl_Insert(local, element);

        //!!!menim name
        name = cat_string("TF@", name);

        if (decDef == k_function) {
            create_3ac(I_DEFVAR, NULL, NULL, name); //deklarovanie operandu
            create_3ac(I_POPS, NULL, NULL, name);  //vytvorenie operacii
        }
        input = get_token();
        line = input->line;
        if (input->token_type == RPAR) {
            return SUCCESS;
        } else if (input->token_type != COMMA) {
            syntax_error(ERR_SYNTA,line);
        }
        input = get_token();
        line = input->line;
    }
    syntax_error(ERR_SYNTA,line);
    return 0;
}

/*
 * @brief Spracovanie volania funkcie alebo priradenia premennej.
 */
int command_func_var(t_token *input, TTable *local, TTable *func_table) {
    char *name = input->data.s;
    char *ret_var = NULL;
    int dollar_source;

    t_token *loaded = get_token();
    line = loaded->line;
    check_pointer(loaded);
    if (loaded->token_type != EQ) {
        error("Neocakavany symbol.\n", ERR_SEM_DEF, line);  //zmena z oth
    }

    TElement *el_var = Tbl_GetDirect(local, name);
    if (el_var == NULL) {
        undefined_err(name, line);
    }
    dollar_source = expression(func_table, local, el_var->data->data->var->type, &ret_var);
    if (ret_var == NULL || dollar_source != EOL) {
        syntax_error(ERR_SYNTA, line);
    }
    create_3ac(I_MOVE, ret_var, NULL, cat_string("TF@", name));
//    create_3ac(I_POPS, NULL, NULL, cat_string("TF@", name));

    return 0;
}

int command_keyword(t_token *input, TTable *local, TTable *func_table) {
    t_token *tmp1 = NULL;
    tdata value = input->data;
    char *ret_var = NULL;
    int dollar_source;
    switch (value.i) {
        case k_dim: //dim
            input = check_next_token_type(ID);
            tmp1 = input;
            char *name = input->data.s;
            TElement *found_func = Tbl_GetDirect(func_table, name);
            if (found_func != NULL) {
                redefine_error(name, line);
            }

            create_3ac(I_DEFVAR, NULL, NULL, cat_string("TF@", name)); //deklarovanie lok. premennej
            input = check_next_token_type(KEY_WORD);
            if (check_token_int_value(input, k_as)) { //AS
                input = check_next_token_type(KEY_WORD);
                if (!token_is_data_type(input)) { //typ
                    syntax_error(ERR_SYNTA,line);
                }
                int type = input->data.i;

                t_token *input2 = get_token();
                line = input2->line;

                check_pointer(input2);
                TValue value;

                if (input2->token_type == EOL) {
                    switch (input->data.i) {
                        case k_integer:
                            value.i = 0;
                            create_3ac(I_MOVE, "int@0", NULL, cat_string("TF@", name));
                            break;
                        case k_double:
                            value.d = 0.0;
                            create_3ac(I_MOVE, "float@0.0", NULL, cat_string("TF@", name));
                            break;
                        case k_string:
                            value.s = "";
                            create_3ac(I_MOVE, "string@", NULL, cat_string("TF@", name));
                            break;
                        default:
                            ;
                    }
                } else if (input2->token_type == EQ) {
                    dollar_source = expression(func_table, local, type, &ret_var);
                    if (dollar_source != EOL || ret_var == NULL) {
                        syntax_error(ERR_SYNTA, line);
                    }
                    create_3ac(I_MOVE, ret_var, NULL, cat_string("TF@", name));
                } else {
                    syntax_error(ERR_SYNTA,line);
                }

                if (Tbl_GetDirect(local, name) != NULL) {
                    semerror(ERR_SEM_DEF,line); //druhy raz definovana
                }

                TData *var = Var_Create(value, type);
                TSymbol *lSymbol = Sym_Create(ST_Variable, var, name);
                lSymbol->isDeclared = true;

                Tbl_Insert(local, El_Create(lSymbol));

                return commandsAndVariables(func_table, local);

            }
            syntax_error(ERR_SYNTA,line);
        case k_input: //input
            tmp1 = check_next_token_type(ID);
            TElement *el = Tbl_GetDirect(local, tmp1->data.s);
            int i = 1;

            if (el == NULL || el->data->type != ST_Variable) {
                el = Tbl_GetDirect(func_table, tmp1->data.s);
                if (el == NULL || el->data->type != ST_Variable) {
                    undefined_err(tmp1->data.s, line);
                    return -1;
                }
            }
            if (el->data->data->var->type == k_integer) {
                i = 1;
            } else if (el->data->data->var->type == k_double) {
                i = 0;
            } else if (el->data->data->var->type == k_string) {
                i = 2;
            } else {
                internall_err(__LINE__);
            }

            char *type[10] = {"float", "int", "string"};

            create_3ac(I_WRITE, "string@?\\032", NULL, NULL);
            create_3ac(I_READ, type[i], NULL, cat_string("TF@", tmp1->data.s)); //deklarovanie operandu

            check_next_token_type(EOL);

            return commandsAndVariables(func_table, local);
        case k_print: //print
            print_params(func_table, local); //skonci vcetne EOL
            return commandsAndVariables(func_table, local);
        case k_if: //if
            dollar_source = expression(func_table, local, -1, &ret_var);
            create_3ac(I_PUSHS, NULL, NULL, ret_var);
            if (dollar_source != k_then) {
                syntax_error(ERR_SYNTA,line);
            }
            create_3ac(I_PUSHS, NULL, NULL, "bool@false");  //vytvorenie operacii

            char *label = gen_label("if");
            str_push(label);
            create_3ac(I_JUMPIFEQS, NULL, NULL, label);  //vytvorenie operacii

            check_next_token_type(EOL);
            int correct = commandsAndVariables(func_table, local);
            if (correct == k_if) {//skoncilo to end if
                create_3ac(I_LABEL, NULL, NULL, str_pop());
                return commandsAndVariables(func_table, local);
            } else if (correct == k_else) {             //prosel uspesne else
                char *new = gen_label("else");
                char *tmp = str_pop();
                create_3ac(I_JUMP, NULL, NULL, new);
                str_push(new);
                create_3ac(I_LABEL, NULL, NULL, tmp);
                if (commandsAndVariables(func_table, local) == k_if) {
                    create_3ac(I_LABEL, NULL, NULL, str_pop());
                    return commandsAndVariables(func_table, local);
                } else {
                    syntax_error(ERR_SYNTA,line);
                }
            }
            syntax_error(ERR_SYNTA,line);
        case k_else: //else
            check_next_token_type(EOL);
            return k_else; //vrat else, odchytne si to if a if zavola dalsi prikazy
        case k_end: //end
            input = check_next_token_type(KEY_WORD);
            if (check_token_int_value(input, k_if)) { //if
                check_next_token_type(EOL);
                return k_if;
            } else if (check_token_int_value(input, k_scope)) { //Scope
                return k_scope;
            } else if (check_token_int_value(input, k_function)) { //Function
                check_next_token_type(EOL);
                return k_function;
            }
            syntax_error(ERR_SYNTA,line);
        case k_do: //do
            input = check_next_token_type(KEY_WORD);
            if (check_token_int_value(input, k_while)) { //while
                char *new_b = gen_label("w_b");
                char *new_e = gen_label("w_e");
                str_push(new_b);
                str_push(new_e);
                create_3ac(I_LABEL, NULL, NULL, new_b);
                if (expression(func_table, local, -1, &ret_var) != EOL) {
                    syntax_error(ERR_SYNTA,line);
                } //tohle asi nepojede
                create_3ac(I_PUSHS, NULL, NULL, ret_var);
                create_3ac(I_PUSHS, NULL, NULL, "bool@false");  //vytvorenie operacii
                create_3ac(I_JUMPIFEQS, NULL, NULL, new_e);  //vytvorenie operacii

                if (commandsAndVariables(func_table, local) == k_loop) { //skoncilo to loop
                    new_e = str_pop();
                    new_b = str_pop();
                    create_3ac(I_JUMP, NULL, NULL, new_b);  //vytvorenie operacii
                    create_3ac(I_LABEL, NULL, NULL, new_e);  //vytvorenie operacii

                    return commandsAndVariables(func_table, local);
                }
                syntax_error(ERR_SYNTA,line);
            }
            syntax_error(ERR_SYNTA,line);
        case k_loop: //loop
            check_next_token_type(EOL);
            return k_loop; //tady to ma asi vracet k_loop
        case k_return: //return

            if (local->isScope) {
                syntax_error(ERR_SYNTA,line);
            } else {
                dollar_source = expression(func_table, local, act_type, &ret_var); //todo neviem zistit akeho typu ma byt navrat
                if (dollar_source != EOL){
                    syntax_error(ERR_SYNTA, line);
                }
                if (ret_var !=NULL) {
                    //create_3ac(I_MOVE, ret_var, NULL, "TF@%RETVAL");
                    create_3ac(I_PUSHS, NULL, NULL, ret_var);
                }
                create_3ac(I_POPFRAME, NULL, NULL, NULL);
                create_3ac(I_RETURN, NULL, NULL, NULL);
                return commandsAndVariables(func_table, local);
            }

        default:
            syntax_error(ERR_SYNTA,line);
    }
    return 0;
}

int commandsAndVariables(TTable *Table, TTable *local) {
    if (local == NULL || Table == NULL) {
        internall_err(__LINE__);
    }

    create_3ac(-1, NULL, NULL, NULL);   //generuj prazdny riadok

    t_token *input = get_token();
    line = input->line;

    check_pointer(input);

    while (input->token_type == EOL) {  //preskoc prazdne riadky
        input = get_token();
        line = input->line;

        check_pointer(input);
    }

    if (input->token_type == ID) {
        //bude sa volat funkcia alebo priradovat do premennej
        command_func_var(input, local, Table);
        return commandsAndVariables(Table, local);
    } else if (input->token_type == KEY_WORD) {
        //!!!!!!!nepriama rekurzia, vo funcii sa vola commandsAndVariables()
        return command_keyword(input, local, Table);
    }
    error("Neocakavany vyraz.\n", ERR_SYNTA, line);
    return -1;
}

int print_params(TTable *Table, TTable *local) {
    char *ret_var = NULL;
    //char ret[BUFFSIZE];
    //static int print_par = 0;

    int result = expression(Table, local, -2, &ret_var);
    if (result != SEMICOLLON){
        syntax_error(ERR_SYNTA, line);
    }

    create_3ac(I_WRITE, NULL, NULL, ret_var);

    while (result == SEMICOLLON) {
        result = expression(Table, local, -2, &ret_var);
        if (result != EOL) {
            create_3ac(I_WRITE, NULL, NULL, ret_var);
        } else if (ret_var != NULL){
            syntax_error(ERR_SYNTA, line);
        }
    }
    if (result == EOL) {
        return SUCCESS;
    }
    syntax_error(ERR_SYNTA,line);
    return 0;
}

int scope(TTable *Table) {
    create_3ac(I_LABEL, NULL, NULL, "$l_main");
    create_3ac(I_DEFVAR, NULL, NULL, "GF@$conv_help");
    create_3ac(I_CREATEFRAME, NULL, NULL, NULL);

    TTable *local = Tbl_Create(8);
    local->isScope = true;

    check_next_token_type(EOL);
    int res = commandsAndVariables(Table, local);
    if (res == k_scope) {
        t_token *input = get_token();
        line = input->line;

        check_pointer(input);
        while (input->token_type == EOL) {
            input = get_token();
            line = input->line;

            check_pointer(input);
        }
        if (input->token_type == EMPTY) {
            create_3ac(I_JUMP, NULL, NULL, "$l_end");
            return SUCCESS;
        }
    }
    syntax_error(ERR_SYNTA,line);
    return 0;
}
