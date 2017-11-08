#include "parser.h"
#include "main.h"


#include "symtable.h"
#include "memwork.h"
#include "scanner.h"
#include <stdlib.h>
#include <stdbool.h>


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

void error(int code) {
    fprintf(stderr, "Error v syntakticke analyze - spatny typ tokenu.\n");
    exit(code);
}

void semerror(int code) {
    fprintf(stderr, "Error v semanticke analyze.\n");
    exit(code);
}

int parse(TTable *Table) {

    t_token *input = get_token();
    check_pointer(input);
    while (input->token_type == EOL) {
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
                function(k_function, Table, local); //parametry, konci tokenem EOL

                int correct = commandsAndVariables(local);
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
    TElement *getElement = Tbl_GetDirect(Table, input->data.s);
    TFunction *func = NULL;
    //TFunction *tempfunc = NULL;
    TSymbol *sym = NULL;
    if (getElement == NULL) {
        func = my_malloc(sizeof(TFunction));
        sym = my_malloc(sizeof(TSymbol));
        sym->data = my_malloc(sizeof(TData));
        sym->isDeclared = false;
        sym->isDefined = false;
        func->attr_count = 0;
        func->attributes = my_malloc(sizeof(TType));
        if (decDef == k_declare) {
            sym->isDeclared = true;
        } else if (decDef == k_function) {
            sym->isDefined = true;
        }
        sym->name = input->data.s;
        sym->type = ST_Function;
        sym->data->func = func;
    } else {
        if (getElement->data->isDefined == true && decDef == k_function) {
            semerror(ERR_SEM_P);
        } else if (getElement->data->isDeclared == true && decDef == k_declare) {
            semerror(ERR_SEM_P);
        } else if (getElement->data->isDefined == true && decDef == k_declare) {
            semerror(ERR_SEM_P);
        }
        func = my_malloc(sizeof(TFunction));
        // func = getElement->data->data->func;
        func->attr_count = 0;
        func->attributes = my_malloc(sizeof(TType));
        sym = getElement->data;

    }
    check_next_token_type(LPAR);
    input = get_token();
    check_pointer(input);
    char *name;
    if (input->token_type == ID) {
        if (decDef == k_function) {
            name = input->data.s;

        }
        params(func, local,
               name); //vraci success, mozna kontrola? ale ono je to celkem jedno, protoze to pri chybe stejne spadne driv
    } else if (input->token_type == RPAR) {}
    else {
        error(ERR_SYNTA);
    }

    input = check_next_token_type(KEY_WORD);
    check_token_int_value(input, k_as); //as

    input = check_next_token_type(KEY_WORD);
    if (check_token_int_value(input, k_integer) || check_token_int_value(input, k_double) ||
        check_token_int_value(input, k_string)) //return_type je datovy typ
    {
        func->return_type = input->data.i;
        check_next_token_type(EOL);
        if (getElement != NULL) {
            if (func->attr_count == sym->data->func->attr_count
                && *(func->attributes) == *(sym->data->func->attributes)
                && func->return_type == sym->data->func->return_type) {
                sym->isDefined = true;
            } else {
                semerror(ERR_SEM_P);
            }
        } else {
            Tbl_Insert(Table, El_Create(sym));
        }
        return SUCCESS;
    } else {
        error(ERR_SYNTA);
    }


}


int params(TFunction *functions, TTable *local, char *name) {
    t_token *input = check_next_token_type(KEY_WORD);
    if (check_token_int_value(input, k_as)) { //as
        functions->attr_count++;
        functions->attributes = my_realloc(functions->attributes, sizeof(TType) * functions->attr_count);
        input = check_next_token_type(KEY_WORD);

        if (check_token_int_value(input, k_integer) || check_token_int_value(input, k_double) ||
            check_token_int_value(input, k_string)) //return_type je datovy typ
        {
            functions->attributes[functions->attr_count - 1] = input->data.i;
            if (name != NULL) {
                TElement *lElement = Tbl_GetDirect(local, name);

                /*create symbol*/
                TData *var;
                TValue value;
                switch (input->data.i) {
                    case k_integer:
                        value.i = 0;
                        break;
                    case k_double:
                        value.d = 0.0;
                        break;
                    case k_string:
                        value.s = "";
                }
                var = Var_Create(value, input->token_type);
                TSymbol *lSymbol = Sym_Create(ST_Variable, var, name);
                lSymbol->isDeclared = true;     //neni v konstruktore

                if (lElement != NULL) {
                    semerror(ERR_SEM_P);
                }

                TElement *element = El_Create(lSymbol);
                Tbl_Insert(local, element);
            }

            input = get_token();
            check_pointer(input);

            if (input->token_type == COMMA) {
                input = check_next_token_type(ID);
                name = input->data.s;
                return params(functions, local, name);
            } else if (input->token_type == RPAR) {
                return SUCCESS;
            }
            error(ERR_SYNTA);
        }
        error(ERR_SYNTA);
    }
    error(ERR_SYNTA);
}

int commandsAndVariables(TTable *local) {
    t_token *input = get_token();
    check_pointer(input);

    while (input->token_type == EOL) {
        input = get_token();
    }

    tdata value = input->data;
    bool isCorrect = input->token_type == KEY_WORD;

    if (input->token_type == ID) {
        char *name;
        name = input->data.s;
        check_next_token_type(EQ);
        expression();
        return commandsAndVariables(local);
    }

    if (isCorrect) {
        switch (value.i) {
            case k_dim: //dim
                input = check_next_token_type(ID);
                char *name = input->data.s;
                input = check_next_token_type(KEY_WORD);
                if (check_token_int_value(input, 0)) { //AS
                    input = check_next_token_type(KEY_WORD);
                    int type = input->data.i;
                    if (check_token_int_value(input, k_integer) || check_token_int_value(input, k_double) ||
                        check_token_int_value(input, k_string)) { //typ
                        check_next_token_type(EOL);

                        TElement *lElement = Tbl_GetDirect(local, name);

                        TData *var;
                        TValue value;
                        switch (input->data.i) {
                            case k_integer:
                                value.i = 0;
                                break;
                            case k_double:
                                value.d = 0.0;
                                break;
                            case k_string:
                                value.s = "";
                        }
                        var = Var_Create(value, input->token_type);
                        TSymbol *lSymbol = Sym_Create(ST_Variable, var, name);
                        lSymbol->isDeclared = true;

                        if (lElement != NULL) {
                            semerror(ERR_SEM_P);
                        }

                        Tbl_Insert(local, El_Create(lSymbol));

                        return commandsAndVariables(local);
                    }
                }
                error(ERR_SYNTA);
            case k_input: //input
                check_next_token_type(ID);
                check_next_token_type(EOL);
                return commandsAndVariables(local);
            case k_print: //print
                print_params(); //skonci vcetne EOL
                return commandsAndVariables(local);
            case k_if: //if
                if (expression() != k_then) {
                    error(ERR_SYNTA);
                }
                check_next_token_type(EOL);
                int correct = commandsAndVariables(local);
                if (correct == k_if) {//skoncilo to end if
                    return commandsAndVariables(local);
                }
                error(ERR_SYNTA);
            case k_else: //else
                check_next_token_type(EOL);
                return commandsAndVariables(local);
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
                    if (expression() != EOL) {
                        error(ERR_SYNTA);
                    } //tohle asi nepojede
                    if (commandsAndVariables(local) == k_loop) { //skoncilo to loop
                        return commandsAndVariables(local);
                    }
                    error(ERR_SYNTA);
                }
                error(ERR_SYNTA);
            case k_loop: //loop
                check_next_token_type(EOL);
                return k_loop; //tady to ma asi vracet k_loop
            case k_return: //return
                expression();
                return commandsAndVariables(local);
            default:
                error(ERR_SYNTA);
        }
    }
}

int print_params() {
    int result = expression();
    while (result == SEMICOLLON) {
        result = expression();
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
    TTable *local = Tbl_Create(8);
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
