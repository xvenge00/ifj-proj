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

int parse(TTable *Table) {
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
            case k_declare: //declare
                input = check_next_token_type(KEY_WORD);
                check_token_int_value(input, k_function);
                function(k_declare, Table, NULL);

                return parse(Table);
            case k_function: { //function
                TTable *local = Tbl_Create(8);
                int correct = function(k_function, Table, local); //parametry, konci tokenem EOL
                if (correct == k_function) { //function
                    return parse(Table);
                } else {
                    syntax_error(ERR_SYNTA,line);
                }
            }
            case k_scope: //scope
                if (scope(Table) == SUCCESS) {
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

int function(int decDef, TTable *Table, TTable *local) {
    t_token *input;
    input = check_next_token_type(ID);
    char *name_f = input->data.s;
    int ret_type = -1;
    if (decDef == k_function) {
        create_3ac(I_LABEL, NULL, NULL, name_f);
    }
    TData *func = NULL;
    TSymbol *sym = NULL;
    TFunction *f = NULL;

    unsigned attr_count = 0;
    int *attr_types = NULL;
    check_next_token_type(LPAR);
    params(local, &attr_count, &attr_types, decDef);                //spracuj parametre a )
    check_token_int_value(check_next_token_type(KEY_WORD), k_as);
    input = check_next_token_type(KEY_WORD);
    if (token_is_data_type(input)) {      //to dole by som nahradil tymto
        ret_type = input->data.i;
    } else {
        syntax_error(ERR_SYNTA,line);
    }

    check_next_token_type(EOL);
    /**
     * zapis do symtable
     */
    TElement *getElement = Tbl_GetDirect(Table, name_f);
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
        Tbl_Insert(Table, getElement);
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
        if (f->attr_count != attr_count || f->return_type != ret_type) {
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
        i = commandsAndVariables(Table, local);
        create_3ac(I_RETURN, NULL, NULL, NULL);
    }

    return i;
}

int params(TTable *local, unsigned *attr_count, int **attributes, int decDef) {
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
        if (Tbl_GetDirect(local, name) != NULL) { //bola premenna uz definovana alebo dekralovana ako func
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
 * @brief funckia skontroluje typ a pocet parametrov
 * zistuje ci sedi typ parametru a ci sa zatial neprekrocil definovany pocet parametrov
 * @return vrati 1, ked je treba skonvertovat, 0 ked netreba
 * ked nesedia parametre skonci program, ked typ nie je ani string ani double ani string vrati -1
 */
int check_param_cnttype(unsigned arg_cnt, TFunction *func, TType typ) {
    if (arg_cnt + 1 > func->attr_count) {
        error("Prilis vela parametrov.\n", ERR_SEM_TYPE,line);
    }

    switch (typ) {
        case E_string:
            if (func->attributes[arg_cnt] != E_string) {
                error("Nevhodny typ parametru.\n", ERR_SEM_TYPE,line);
            }
            return 0;
        case E_double:
            if (func->attributes[arg_cnt] == E_double) {
                return 0;
            } else if (func->attributes[arg_cnt] == E_integer) {
                return 1;
            } else {
                error("Nevhodny typ parametru.\n", ERR_SEM_TYPE,line);
            }
        case E_integer:
            if (func->attributes[arg_cnt] == E_integer) {
                return 0;
            } else if (func->attributes[arg_cnt] == E_double) {
                return 1;
            } else {
                error("Nevhodny typ parametru.\n", ERR_SEM_TYPE,line);
            }
        default:
            return -1;
    }
}

/*
 * @brief Kontrola parametrov pri volani funkcie.
 */
int check_params(TTable *local, TTable *func_table, TFunction *func){
    t_token *loaded = get_token();
    line = loaded->line;
    unsigned arg_cnt = 0;

    while (loaded->token_type == ID || loaded->token_type == INT || loaded->token_type == STR ||
           loaded->token_type == DOUBLE) {
        //TODO!!!
        //skontroluj pocet paramtrov
        //arg_cnt++;
        //posli expression na parameter
        //skontroluj navratovy typ
        //skonvertuj ak treba, potom pushni
        //TYPE bude ttype zistim ho z funkcie
        return_token(loaded);
        int dollar_source = expression(func_table, local, E_integer);
        /*token_push(loaded);
        if (loaded->token_type == ID) {
            TElement *el_param = Tbl_GetDirect(local, loaded->data.s);
            if (el_param == NULL) {
                el_param = Tbl_GetDirect(func_table, loaded->data.s);
                if (el_param == NULL) {
                    undefined_err(loaded->data.s,line);
                    return 0;
                }
            }

            if (el_param->data->type == ST_Variable) {
                int convert = check_param_cnttype(arg_cnt, func, el_param->data->data->var->type);
                if (convert) {
                    //todo convert
                }
                arg_cnt++;
            } else if (el_param->data->type == ST_Function) {
                //asi call function
                //check_params
                clear_all();
                exit(ERR_SYNTA); //todo parameter je funkcia
            } else {
                clear_all();
                exit(ERR_INTER);
            }
        } else if (loaded->token_type == STR) {
            check_param_cnttype(arg_cnt, func, E_string);
            arg_cnt++;
        } else if (loaded->token_type == INT) {
            int convert = check_param_cnttype(arg_cnt, func, E_integer);
            if (convert){
                char *ret = my_malloc(sizeof(char) * BUFFSIZE);
                snprintf(ret, BUFFSIZE, "$E_E%i", get_id());
                create_3ac(I_DEFVAR, NULL, NULL, ret);
                create_3ac(I_INT2FLOAT, token2operand(loaded), NULL, ret);
                loaded->data.s = ret;
            }
            arg_cnt++;
        } else if (loaded->token_type == DOUBLE) {
            int convert = check_param_cnttype(arg_cnt, func, E_double);
            if (convert) {
                char *ret = my_malloc(sizeof(char) * BUFFSIZE);
                snprintf(ret, BUFFSIZE, "$E_E%i", get_id());
                create_3ac(I_DEFVAR, NULL, NULL, ret);
                create_3ac(I_FLOAT2R2EINT, token2operand(loaded), NULL, ret);
                loaded->data.s = ret;
            }
            arg_cnt++;
        }
        loaded = get_token();
        line = loaded->line;
        if (loaded->token_type != COMMA) {
            break;
        }
        loaded = get_token();
        line = loaded->line;*/

    }
    return_token(loaded);
    return 0;
}

/*
 * @brief Spracovanie volania funkcie alebo priradenia premennej.
 */
int command_func_var(t_token *input, TTable *local, TTable *func_table) {
    char *name = input->data.s;

    TElement *el_var = Tbl_GetDirect(local, name);
    TElement *el_func = Tbl_GetDirect(func_table, name);

    if (el_var == NULL && el_func == NULL) {
        undefined_err(name,line);
    }

    t_token *loaded = get_token();
    line = loaded->line;

    check_pointer(loaded);
    if (loaded->token_type == EQ) { //musi byt premenna
        if (el_var == NULL) {
            error("Funkcia nemoze byt na lavej strane priradenia.\n", ERR_SEM_DEF,line);
            return -1;  //inak Clion hovori ze el_var moze byt NULL
        }
        expression(func_table, local, el_var->data->data->var->type);  //TODO navrat a typova kontrola
        create_3ac(I_POPS, NULL, NULL, cat_string("TF@", name));
    } else if (loaded->token_type == LPAR) {    //musi byt funkcia
        if (el_func == NULL) {
            error("Premenna nemoze mat zatvorky", ERR_SEM_DEF,line);
            return -1;
        }
        check_params(local, func_table, el_func->data->data->func);
        check_next_token_type(RPAR);
    }
    return 0;   //TODO
}

int command_keyword(t_token *input, TTable *local, TTable *func_table) {
    t_token *tmp1 = NULL;
    tdata value = input->data;
    switch (value.i) {
        case k_dim: //dim
            input = check_next_token_type(ID);
            tmp1 = input;
            char *name = input->data.s;
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
                    expression(func_table, local, type);             //TODO moze tam byt aj funkcia?
                    create_3ac(I_POPS, NULL, NULL, cat_string("TF@", name));
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
                    semerror(ERR_SEM_TYPE,line);
                    return -1;
                }
            }
            if (el->data->data->var->type == k_integer) {
                i = 1;
            } else if (el->data->data->var->type == k_double) {
                i = 0;
            } else if (el->data->data->var->type == k_integer) {
                i = 2;
            } else {
                internall_err(__LINE__);
            }

            char *type[10] = {"float", "int", "str"};


            create_3ac(I_READ, type[i], NULL, cat_string("TF@", tmp1->data.s)); //deklarovanie operandu

            check_next_token_type(EOL);

            return commandsAndVariables(func_table, local);
        case k_print: //print
            print_params(func_table, local); //skonci vcetne EOL
            return commandsAndVariables(func_table, local);
        case k_if: //if
            if (expression(func_table, local, -1) != k_then) {
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
                create_3ac(I_LABEL, NULL, NULL, cat_string("TF@", new_b));
                if (expression(func_table, local, -1) != EOL) {
                    syntax_error(ERR_SYNTA,line);
                } //tohle asi nepojede
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
                expression(func_table, local, -2); //todo neviem zistit akeho typu ma byt navrat
                create_3ac(I_POPS, NULL, NULL, cat_string("TF@", "%RETVAL"));
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

    if (input->token_type == ID) { //todo co ak je id funckia a ma sa volat funkcia bez parametru
        //bude sa volat funkcia alebo priradovat do premennej
        command_func_var(input, local, Table);
        return commandsAndVariables(Table, local);
    } else if (input->token_type == KEY_WORD) {
        //!!!!!!!nepriama rekurzia, vo funcii sa vola commandsAndVariables()
        return command_keyword(input, local, Table);
    }
}

int print_params(TTable *Table, TTable *local) {
    int result = expression(Table, local, -2);
    char ret[BUFFSIZE];
    static int print_par = 0;
    snprintf(ret, BUFFSIZE, "TF@$P_E%i", print_par++);    //generovanie operandu pre vysledok medzisuctu
    create_3ac(I_DEFVAR, NULL, NULL, ret);
    create_3ac(I_POPS, NULL, NULL, ret);
    create_3ac(I_WRITE, NULL, NULL, ret);

    while (result == SEMICOLLON) {
        result = expression(Table, local, -2);
        if (result != EOL) {
            snprintf(ret, BUFFSIZE, "TF@$P_E%i", print_par++);    //generovanie operandu pre vysledok medzisuctu
            create_3ac(I_DEFVAR, NULL, NULL, ret); //deklarovanie operandu
            create_3ac(I_POPS, NULL, NULL, ret); //deklarovanie operandu
            create_3ac(I_WRITE, NULL, NULL, ret); //deklarovanie operandu
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
    create_3ac(I_CREATEFRAME, NULL, NULL, NULL);

    TTable *local = Tbl_Create(8);    //todo preco tu sa generuje nova tabulka ??????
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
