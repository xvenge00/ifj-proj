#include "parser.h"
#include "err.h"
#include "expression.h"
#include "symtable.h"
#include "memwork.h"
#include "scanner.h"
#include <stdlib.h>
#include <stdbool.h>
#include "codegen.h"
#include "token_stack.h"

char *gen_label(char *ret) {
    static int label = 0;
    char *result = my_malloc(sizeof(char) * 130);
    if (ret == NULL) {
        sprintf(result, "l_%i", label++);
    } else {
        sprintf(result, "l_%s_%i", ret, label++);
    }
    result[129] = 0;

    return result;
}

t_token *check_next_token_type(int type) {
    t_token *input = get_token();
    check_pointer(input);

    if (input->token_type != type) {
        error(ERR_SYNTA);
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
        error(ERR_SYNTA);
    }
    return true;
}

int parse(TTable *Table) {
    t_token *input = get_token();
    check_pointer(input);
    while (input->token_type == EOL) {  //preskoc prazdne riadky
        input = get_token();
        check_pointer(input);
    }

    //prvni token musi byt Declare nebo Function nebo SCOPE
    bool isCorrect = input->token_type == KEY_WORD;
    tdata value = input->data;
    if (isCorrect) {
        switch (value.i) {
            case k_declare: //declare
                input = check_next_token_type(KEY_WORD);
                check_token_int_value(input, k_function);
                function(k_declare, Table, NULL);

                return parse(Table);
            case k_function: { //function
                TTable *local = Tbl_Create(8);
                int correct = function(k_function, Table, local); //parametry, konci tokenem EOL

//                int correct = commandsAndVariables(local);

                if (correct == k_function) { //function
                    return parse(Table);
                } else {
                    error(ERR_SYNTA);
                }
            }
            case k_scope: //scope
                if (scope() == SUCCESS) {
                    return SUCCESS;
                }
            default:
                error(ERR_SYNTA);
        }
    } else {
        error(ERR_SYNTA);
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
        create_3ac(I_DEFVAR, NULL, NULL, "%RETVAL");
        create_3ac(I_PUSHFRAME, NULL, NULL, NULL);
    }
    TData *func = NULL;
    TSymbol *sym = NULL;
    TFunction *f = NULL;

    unsigned attr_count = 0;
    int *attr_types = NULL;
    check_next_token_type(LPAR);
    params(local, &attr_count, attr_types, decDef);
    check_token_int_value(check_next_token_type(KEY_WORD), k_as);
    input = check_next_token_type(KEY_WORD);
    if (check_token_int_value(input, k_integer)) {
        ret_type = E_integer;
    } else if (check_token_int_value(input, k_double)) {
        ret_type = E_double;
    } else if (check_token_int_value(input, k_string)) {
        ret_type = E_string;
    } else {
        error(ERR_SYNTA);
    }
    check_next_token_type(EOL);
    /**
     * zapis do symtable
     */
    TElement *getElement = Tbl_GetDirect(Table, input->data.s);
    if (getElement == NULL) {
        func = Func_Create(ret_type, attr_count, attr_types);
        sym = Sym_Create(ST_Function, func, name_f);
        if (decDef == k_function) {
            sym->isDefined = true;
            sym->isDeclared = true;
        } else if (decDef == k_declare) {
            sym->isDeclared = true;
        } else {
            clear_all();
            exit(ERR_INTER);
        }
        getElement = El_Create(sym);
        Tbl_Insert(Table, getElement);
    } else {
        if (getElement->data->type != ST_Function) {
            semerror(ERR_SEM_P);

        } else if (getElement->data->isDefined == true && decDef == k_function) {
            semerror(ERR_SEM_P);
        } else if (getElement->data->isDeclared == true && decDef == k_declare) {
            semerror(ERR_SEM_P);
        } else if (getElement->data->isDefined == true && decDef == k_declare) {
            semerror(ERR_SEM_P);
        }
        f = getElement->data->data->func;
        if (f->attr_count != attr_count || f->return_type != ret_type) {
            semerror(ERR_SEM_P);
        }
        for (int i = 0; i < attr_count; ++i) {
            if (f->attributes[i] != attr_types[i]) {
                semerror(ERR_SEM_P);
            }
        }
    }

    //nacitanie prikazouv ak je toto definicia

     int i ;
    if (decDef == k_function) {
        i = commandsAndVariables(local);
    }


    create_3ac(I_POPFRAME, NULL, NULL, NULL);
    create_3ac(I_RETURN, NULL, NULL, NULL);
    return i;
}
//    int function(int decDef, TTable *Table, TTable *local) {
//    if (getElement == NULL) {
//
//
//        func = my_malloc(sizeof(TFunction));        //TODO prerob na func_create
//        sym = my_malloc(sizeof(TSymbol));
//        sym->data = my_malloc(sizeof(TData));
//        sym->isDeclared = false;
//        sym->isDefined = false;
//        func->attr_count = 0;
//        func->attributes = my_malloc(sizeof(TType));
//
//        func->lok_table = local;
//
//        if (decDef == k_declare) {
//            sym->isDeclared = true;
//        } else if (decDef == k_function) {
//            sym->isDefined = true;
//            create_3ac(I_LABEL, NULL, NULL, input->data.s);  //vytvorenie operacii
//            create_3ac(I_DEFVAR, NULL, NULL, "%RETVAL"); //deklarovanie operandu
//            create_3ac(I_PUSHFRAME, NULL, NULL, NULL);  //vytvorenie operacii
//        }
//        sym->name = input->data.s;
//        sym->type = ST_Function;
//        sym->data->func = func;
//    } else {
//        if (getElement->data->isDefined == true && decDef == k_function) {
//            semerror(ERR_SEM_P);
//        } else if (getElement->data->isDeclared == true && decDef == k_declare) {
//            semerror(ERR_SEM_P);
//        } else if (getElement->data->isDefined == true && decDef == k_declare) {
//            semerror(ERR_SEM_P);
//        }
//        func = my_malloc(sizeof(TFunction));
//        // func = getElement->data->data->func;
//        func->attr_count = 0;
//        func->attributes = my_malloc(sizeof(TType));
//        sym = getElement->data;
//
//    }
//    check_next_token_type(LPAR);
//    input = get_token();
//    check_pointer(input);
//    char *name;
//    if (input->token_type == ID) {
//        if (decDef == k_function) {
//            name = input->data.s;
//            create_3ac("DEFVAR", NULL, NULL, name); //deklarovanie operandu
//            create_3ac("POPS", NULL, NULL, name);  //vytvorenie operacii
//        }
//        params(func, local,
//               name,
//               decDef); //vraci success, mozna kontrola? ale ono je to celkem jedno, protoze to pri chybe stejne spadne driv
//    } else if (input->token_type == RPAR) {}
//    else {
//        error(ERR_SYNTA);
//    }
//
//    input = check_next_token_type(KEY_WORD);
//    check_token_int_value(input, k_as); //as
//
//    input = check_next_token_type(KEY_WORD);
//    if (check_token_int_value(input, k_integer) || check_token_int_value(input, k_double) ||
//        check_token_int_value(input, k_string)) //return_type je datovy typ
//    {
//        func->return_type = input->data.i;
//        check_next_token_type(EOL);
//        if (getElement != NULL) {
//            if (func->attr_count == sym->data->func->attr_count
//                && *(func->attributes) == *(sym->data->func->attributes)
//                && func->return_type == sym->data->func->return_type) {
//                sym->isDefined = true;
//            } else {
//                semerror(ERR_SEM_P);
//            }
//        } else {
//            Tbl_Insert(Table, El_Create(sym));
//        }
//        create_3ac("POPFRAME", NULL, NULL, NULL);   //vytvorenie operacii
//        create_3ac("RETURN", NULL, NULL, NULL);     //vytvorenie operacii
//        return SUCCESS;
//    } else {
//        error(ERR_SYNTA);
//    }
//}




int params(TTable *local, unsigned *attr_count, int *attributes, int decDef) {
    int start = 1;
    TData *var = NULL;
    TValue value;
    TSymbol *lSymbol = NULL;
    TElement *element = NULL;

    t_token *input = get_token();
    if (start && input->token_type == RPAR) {
        return SUCCESS;
    }
    while (input->token_type == ID) {

        char *name = input->data.s;
        if (Tbl_GetDirect(local, name) != NULL) { //bola premenna uz definovana alebo dekralovana ako func
            //semerror(ERR_SEM_P);
            redefine_error(name);
        }

        check_token_int_value(check_next_token_type(KEY_WORD), k_as);
        input = check_next_token_type(KEY_WORD);
        //je nacitane sa id as typ

        (*attr_count)++;
        attributes = my_realloc(attributes, sizeof(int) * (*attr_count));
        if (check_token_int_value(input, k_integer)) {
            attributes[(*attr_count) - 1] = E_integer;
            value.i = 0;
        } else if (check_token_int_value(input, k_double)) {
            attributes[(*attr_count) - 1] = E_double;
            value.d = 0.0;

        } else if (check_token_int_value(input, k_string)) {
            attributes[(*attr_count) - 1] = E_string;
            value.s = "";
        } else {
            error(ERR_SYNTA);
        }


        var = Var_Create(value, attributes[(*attr_count) - 1]);
        lSymbol = Sym_Create(ST_Variable, var, name);
        lSymbol->isDeclared = true;
        lSymbol->isDefined = true;
        element = El_Create(lSymbol);
        Tbl_Insert(local, element);

        if (decDef == k_function) {
            create_3ac(I_DEFVAR, NULL, NULL, name); //deklarovanie operandu
            create_3ac(I_POPS, NULL, NULL, name);  //vytvorenie operacii
        }
        input = get_token();
        if (input->token_type == RPAR) {
            return SUCCESS;
        } else if (input->token_type != COMMA) {
            error(ERR_SYNTA);
        }
        input = get_token();
    }
    error(ERR_SYNTA);
    return 0;
}

//int params(TFunction *functions, TTable *local, char *name, int decDef) {
//    t_token *input = check_next_token_type(KEY_WORD);
//    if (check_token_int_value(input, k_as)) { //as
//        functions->attr_count++;
//        functions->attributes = my_realloc(functions->attributes, sizeof(TType) * functions->attr_count);
//        input = check_next_token_type(KEY_WORD);
//
//        if (check_token_int_value(input, k_integer) || check_token_int_value(input, k_double) ||
//            check_token_int_value(input, k_string)) //return_type je datovy typ
//        {
//            functions->attributes[functions->attr_count - 1] = input->data.i;
//            if (name != NULL) {
//                TElement *lElement = Tbl_GetDirect(local, name);
//
//                /*create symbol*/
//                TData *var;
//                TValue value;
//                switch (input->data.i) {
//                    case k_integer:
//                        value.i = 0;
//                        break;
//                    case k_double:
//                        value.d = 0.0;
//                        break;
//                    case k_string:
//                        value.s = "";
//                }
//                var = Var_Create(value, input->token_type);
//                TSymbol *lSymbol = Sym_Create(ST_Variable, var, name);
//                lSymbol->isDeclared = true;     //neni v konstruktore
//
//                if (lElement != NULL) {
//                    semerror(ERR_SEM_P);
//                }
//
//                TElement *element = El_Create(lSymbol);
//                Tbl_Insert(local, element);
//            }
//
//            input = get_token();
//            check_pointer(input);
//
//            if (input->token_type == COMMA) {
//                input = check_next_token_type(ID);
//                name = input->data.s;
//                if (decDef) {
//                    create_3ac("DEFVAR", NULL, NULL, name); //deklarovanie operandu
//                    create_3ac("POPS", NULL, NULL, name);  //vytvorenie operacii
//                }
//                return params(functions, local, name, decDef);
//            } else if (input->token_type == RPAR) {
//                return SUCCESS;
//            }
//            error(ERR_SYNTA);
//        }
//        error(ERR_SYNTA);
//    }
//    error(ERR_SYNTA);
//}


int commandsAndVariables(TTable *local) {
    create_3ac(-1, NULL, NULL, NULL);
    int i = 0;
    t_token *input = get_token();
    check_pointer(input);

    while (input->token_type == EOL) {
        input = get_token();
    }

    tdata value = input->data;
    bool isCorrect = input->token_type == KEY_WORD;

    if (input->token_type == ID) { //todo co ak je id funckia a ma sa volat funkcia bez parametru
        char *name;
        name = input->data.s;
        TElement *el_func = Tbl_GetDirect(local, name);
        if (el_func == NULL){
            semerror(ERR_SEM_P);
        }
        t_token *loaded = get_token();

        if (loaded->token_type == EQ) {
            if (el_func->data->type != ST_Variable){
                semerror(ERR_SEM_P);
            }
            expression(local, el_func->data->data->var->type);
            create_3ac(I_POPS, NULL, NULL, name);  //vytvorenie operacii

        } else if (loaded->token_type == LPAR) {
            create_3ac(I_CREATEFRAME, NULL, NULL, NULL);  //vytvorenie operacii

            TFunction *f = NULL;
            if (el_func == NULL) {
                semerror(ERR_SEM_P); //func nebola definovana
            } else if (el_func->data->type != ST_Function) {
                semerror(ERR_SEM_P); //neni to funkcia
            } else {
                f = el_func->data->data->func;
            }

            loaded = get_token();
            while (loaded->token_type == ID || loaded->token_type == INT || loaded->token_type == STR ||
                   loaded->token_type == DOUBLE) {
                token_push(loaded);
                if (loaded->token_type == ID) {
                    TElement *tmp = Tbl_GetDirect(local, loaded->data.s);
                    if (tmp == NULL) {
                        semerror(ERR_SEM_P); //premenna nebola definovana
                    } else {
                        if (tmp->data->type == ST_Variable) {
                            token_push(loaded);
                            //sem kontrola typu
                            if (i + 1 > f->attr_count) {
                                semerror(ERR_SEM_T); // je viac parametrou ako zhltne
                            } else if (f->attributes[i++] != tmp->data->data->var->type) {
                                semerror(ERR_SEM_T);
                            }
                        } else if (tmp->data->type == ST_Function) {
                            clear_all();
                            exit(ERR_SYNTA); //todo parameter je funkcia
                        } else {
                            clear_all();
                            exit(ERR_INTER);
                        }
                    }

                } else if (loaded->token_type == STR){
                    token_push(loaded);
                    if (i + 1 > f->attr_count) {
                        semerror(ERR_SEM_T); // je viac parametrou ako zhltne
                    } else if (f->attributes[i++] != E_string) {
                        semerror(ERR_SEM_T);
                    }
                } else if (loaded->token_type == INT){
                    if (i + 1 > f->attr_count) {
                        semerror(ERR_SEM_T); // je viac parametrou ako zhltne
                    } else if (f->attributes[i++] != E_integer) {
                        if (f->attributes[i-1] != E_double){
                            semerror(ERR_SEM_T);
                        }
                        char ret[130];
                        sprintf(ret, "$E_E%i",get_id());
                        create_3ac(I_DEFVAR, NULL, NULL, ret);
                        create_3ac(I_INT2FLOAT, token2operand(loaded), NULL, ret);
                        loaded->data.s = ret;
                    }
                    token_push(loaded);

                }else if (loaded->token_type == DOUBLE){
                    if (i + 1 > f->attr_count) {
                        semerror(ERR_SEM_T); // je viac parametrou ako zhltne
                    } else if (f->attributes[i++] != E_double) {
                        if (f->attributes[i-1] != E_integer){
                            semerror(ERR_SEM_T);
                        }
                        char ret[130];
                        sprintf(ret, "$E_E%i",get_id());
                        create_3ac(I_DEFVAR, NULL, NULL, ret);
                        create_3ac(I_FLOAT2R2EINT, token2operand(loaded), NULL, ret);
                        loaded->data.s = ret;
                    }
                    token_push(loaded);

                }
                loaded = get_token();
                if (loaded->token_type != COMMA ){
                    break;
                }
                loaded = get_token();
                /*
              //            if (loaded->token_type == ID) {
              //                t_token *arr_el[100] = {NULL,};
              //                unsigned i = 0;
              //                arr_el[i++] = loaded;
              //
              //                check_token_int_value(check_next_token_type(KEY_WORD), k_as);
              //                t_token *tmp = check_next_token_type(KEY_WORD);
              //                if (tmp->data.i != k_string && tmp->data.i != k_integer && tmp->data.i != k_double) {
              //                    clear_all();
              //                    exit(ERR_SYNTA);
              //                }
              //                loaded = get_token();
              //                while (loaded->token_type != RPAR) {
              //                    if (loaded->token_type != COMMA) {
              //                        clear_all();
              //                        exit(ERR_SYNTA);
              //                    }
              //                    loaded = check_next_token_type(ID);
              //                    arr_el[i++] = loaded;
              //                    check_token_int_value(check_next_token_type(KEY_WORD), k_as);
              //                    t_token *tmp = check_next_token_type(KEY_WORD);
              //                    if (tmp->data.i != k_string && tmp->data.i != k_integer && tmp->data.i != k_double) {
              //                        clear_all();
              //                        exit(ERR_SYNTA);
              //                    }
              //                    loaded = get_token();
              //                }
              //                while (i) {
              //                    create_3ac("PUSHS", NULL, NULL, arr_el[--i]->data.s);  //vytvorenie operacii
              //                }
              */
            }
            if (i != f->attr_count) {
                semerror(ERR_SEM_P);
            }
            if (loaded->token_type != RPAR) {
                error(ERR_SYNTA);

            }


            //todo load function and call
        } else {
            semerror(ERR_SEM_P);
        }


        return commandsAndVariables(local);
    }

    t_token *tmp1, *tmp2, *tmp3;
    tmp1 = NULL;
    tmp2 = NULL;
    tmp3 = NULL;

    if (isCorrect) {
        switch (value.i) {
            case k_dim: //dim
                input = check_next_token_type(ID);
                tmp1 = input;
                char *name = input->data.s;
                create_3ac(I_DEFVAR, NULL, NULL, name); //deklarovanie operandu
                input = check_next_token_type(KEY_WORD);
                if (check_token_int_value(input, 0)) { //AS
                    input = check_next_token_type(KEY_WORD);
                    int type = input->data.i;
                    if (check_token_int_value(input, k_integer) || check_token_int_value(input, k_double) ||
                        check_token_int_value(input, k_string)) { //typ
                        //check_next_token_type(EOL);

                        int type = -1;

                        switch (input->data.i) {
                            case k_integer:
                                type = k_integer;
                                break;
                            case k_double:
                                type = k_double;
                                break;
                            case k_string:
                                type = k_string;
                                break;
                            default:
                                clear_all();
                                exit(ERR_INTER);
                        }

                        /* merge z masteru */
                        t_token *input2 = get_token();
                        check_pointer(input2);
                        TValue value;


                        if (input2->token_type == EOL) {
                            switch (input->data.i) {
                                case k_integer:
                                    value.i = 0;
                                    create_3ac(I_MOVE, "int@0", NULL,name );  //vytvorenie operacii
                                    break;
                                case k_double:
                                    value.d = 0.0;
                                    create_3ac(I_MOVE, "float@0.0", NULL, name);  //vytvorenie operacii
                                    break;
                                case k_string:
                                    value.s = "";
                                    create_3ac(I_MOVE, "string@",NULL, name );  //vytvorenie operacii

                            }
                        } else if (input2->token_type == EQ) {
                            expression(local,type);
                            create_3ac(I_POPS, NULL, NULL, name);
                        } else {
                            error(ERR_SYNTA);
                        }
                        /* koniec mergu */

                        if (Tbl_GetDirect(local, name) != NULL){
                            semerror(ERR_SEM_P); //druhy raz definovana
                        }

                        TData *var = Var_Create(value, type);
                        TSymbol *lSymbol = Sym_Create(ST_Variable, var, name);
                        lSymbol->isDeclared = true;

                        Tbl_Insert(local, El_Create(lSymbol));

                        return commandsAndVariables(local);
                    }
                }
                error(ERR_SYNTA);
            case k_input: //input
                tmp1 = check_next_token_type(ID);
                TElement *el = Tbl_GetDirect(local, tmp1->data.s);
                int i = 1;

                if (el == NULL || el->data->type != ST_Variable) {
                    semerror(ERR_SEM_T);
                } else {
                    if (el->data->data->var->type == k_integer) {
                        i = 1;
                    } else if (el->data->data->var->type == k_double) {
                        i = 0;
                    } else if (el->data->data->var->type == k_integer) {
                        i = 2;
                    } else {
                        clear_all();
                        exit(ERR_INTER);
                    }
                }
                char *type[10] = {"float", "int", "str"};


                create_3ac(I_READ, type[i], NULL, tmp1->data.s); //deklarovanie operandu

                check_next_token_type(EOL);

                return commandsAndVariables(local);
            case k_print: //print
                print_params(local); //skonci vcetne EOL
                return commandsAndVariables(local);
            case k_if: //if
                if (expression(local, -1) != k_then) {
                    error(ERR_SYNTA);
                }
                create_3ac(I_PUSHS, NULL, NULL, "bool@false");  //vytvorenie operacii

                char *label = gen_label("if");
                str_push(label);
                create_3ac(I_JUMPIFEQS, NULL, NULL, label);  //vytvorenie operacii

                check_next_token_type(EOL);
                int correct = commandsAndVariables(local);
                if (correct == k_if) {//skoncilo to end if
                    create_3ac(I_LABEL, NULL, NULL, str_pop());
                    return commandsAndVariables(local);
                } else if (correct == k_else) {             //prosel uspesne else
                    char *new = gen_label("else");
                    char *tmp = str_pop();
                    create_3ac(I_JUMP, NULL, NULL, new);
                    str_push(new);
                    create_3ac(I_LABEL, NULL, NULL, tmp);
                    if (commandsAndVariables(local) == k_if){
                        create_3ac(I_LABEL, NULL, NULL, str_pop());
                        return commandsAndVariables(local);
                    } else{
                        error(ERR_SYNTA);
                    }
                }
                error(ERR_SYNTA);
            case k_else: //else
                check_next_token_type(EOL);
                return k_else; //vrat else, odchytne si to if a if zavola dalsi prikazy

//                correct = commandsAndVariables(local);
//                if (correct == k_if) {//skoncilo to end if
//                }
                error(ERR_SYNTA);
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
                error(ERR_SYNTA);
            case k_do: //do
                input = check_next_token_type(KEY_WORD);
                if (check_token_int_value(input, k_while)) { //while
                    char *new_b = gen_label("w_b");
                    char *new_e = gen_label("w_e");
                    str_push(new_b);
                    str_push(new_e);
                    create_3ac(I_LABEL, NULL, NULL, new_b);
                    if (expression(local, -1) != EOL) {
                        error(ERR_SYNTA);
                    } //tohle asi nepojede
                    create_3ac(I_PUSHS, NULL, NULL, "bool@false");  //vytvorenie operacii
                    create_3ac(I_JUMPIFEQS, NULL, NULL, new_e);  //vytvorenie operacii

                    if (commandsAndVariables(local) == k_loop) { //skoncilo to loop
                        new_e = str_pop();
                        new_b = str_pop();
                        create_3ac(I_JUMP, NULL, NULL, new_b);  //vytvorenie operacii
                        create_3ac(I_LABEL, NULL, NULL, new_e);  //vytvorenie operacii

                        return commandsAndVariables(local);
                    }
                    error(ERR_SYNTA);
                }
                error(ERR_SYNTA);
            case k_loop: //loop
                check_next_token_type(EOL);
                return k_loop; //tady to ma asi vracet k_loop
            case k_return: //return

                if (local->isScope) {
                    error(ERR_SYNTA);
                } else {
                    expression(local, -2); //todo neviem zistit akeho typu ma byt navrat
                    create_3ac(I_POPS, NULL, NULL, "%RETVAL");
                    create_3ac(I_POPFRAME, NULL, NULL, NULL);
                    create_3ac(I_RETURN, NULL, NULL, NULL);
                    return commandsAndVariables(local);
                }

            default:
                error(ERR_SYNTA);
        }
    }
}

int print_params(TTable *local) {
    int result = expression(local, -2);
    char ret[130];
    static int print_par = 0;
    sprintf(ret, "$P_E%i", print_par++);    //generovanie operandu pre vysledok medzisuctu
    create_3ac(I_DEFVAR, NULL, NULL, ret); //deklarovanie operandu
    create_3ac(I_POPS, NULL, NULL, ret); //deklarovanie operandu
    create_3ac(I_WRITE, NULL, NULL, ret); //deklarovanie operandu

    while (result == SEMICOLLON) {
        result = expression(local, -2);
        if (result != EOL) {
            sprintf(ret, "$P_E%i", print_par++);    //generovanie operandu pre vysledok medzisuctu
            create_3ac(I_DEFVAR, NULL, NULL, ret); //deklarovanie operandu
            create_3ac(I_POPS, NULL, NULL, ret); //deklarovanie operandu
            create_3ac(I_WRITE, NULL, NULL, ret); //deklarovanie operandu
        }
    }
    if (result == EOL) {
        return SUCCESS;
    }
    error(ERR_SYNTA);
}

/*parametry funkce, mozna to k necemu bude, mozna ne, zatim se to nidke nepouziva*/
/*int idList(){
    t_token * input = get_token();
    check_pointer(input);

    if(input->token_type == ID){
        input = get_token();
        check_pointer(input);

        if(input->token_type == COMMA){
            return idList();
        }
        else if(input->token_type == RPAR){
            check_next_token_type(EOL);
            return SUCCESS;
        }
        error(ERR_SYNTA);
    }
    else if(input->token_type == RPAR){
        check_next_token_type(EOL);
        return SUCCESS;
    }
    error(ERR_SYNTA);
}*/

int scope() {
    create_3ac(I_LABEL, NULL, NULL, "!l_main");

    TTable *local = Tbl_Create(8);
    local->isScope = true;

    check_next_token_type(EOL);
    int res = commandsAndVariables(local);
    if (res == k_scope) {
        t_token *input = get_token();
        check_pointer(input);
        while (input->token_type == EOL) {
            input = get_token();
            check_pointer(input);
        }
        if (input->token_type == EMPTY) {

            return SUCCESS;
        }
    }
    error(ERR_SYNTA);
}
