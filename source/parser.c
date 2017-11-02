#include "parser.h"
#include "main.h"
#include <stdlib.h>

t_token* check_next_token_type(int type){
    t_token * input = get_token();
    check_pointer(input);

    if(input->token_type != type){
        error(ERR_SYNTA);
    }
    return input;
}
bool check_token_int_value(t_token * input, int value){
    check_pointer(input);

    if(input->data.i != value){
        return false;
    }
    return true;
}

bool check_pointer(void * input) {
    if(input == NULL){
        error(ERR_SYNTA);
    }
    return true;
}

void error(int code){
    fprintf(stderr,"Error v syntakticke analyze - spatny typ tokenu.\n");
    exit(code);
}

int parse(){
    t_token * input = get_token();
    check_pointer(input);

    //prvni token musi byt Declare nebo Function nebo SCOPE
    bool isCorrect = input->token_type == KEY_WORD;
    tdata value = input->data;
    if (isCorrect) {
        switch (value.i) {
            case k_declare: //declare
                input = check_next_token_type(KEY_WORD);
                check_token_int_value(input,k_function);
                function();
                return parse();
            case k_function: //function
                function(); //parametry, konci tokenem EOL
                int correct = commandsAndVariables();
                if(correct == k_function){ //function
                    return parse();
                }
                else{
                    error(ERR_SYNTA);
                }
            case k_scope: //scope
                if(scope() == SUCCESS){
                    return SUCCESS;
                }
            default:
                error(ERR_SYNTA);
        }
    }
    else {
        error(ERR_SYNTA);
    }
    return SUCCESS;
}

int function(){
    t_token * input;
    check_next_token_type(ID);
        check_next_token_type(LPAR);
            input = get_token();
            check_pointer(input);
            if (input->token_type == ID) {
                params(); //vraci success, mozna kontrola? ale ono je to celkem jedno, protoze to pri chybe stejne spadne driv
            }
            else if (input->token_type == RPAR) {}
            else {
                error(ERR_SYNTA);
            }

    input = check_next_token_type(KEY_WORD);
    check_token_int_value(input, k_as); //as

    input = check_next_token_type(KEY_WORD);
    if(check_token_int_value(input, k_integer) || check_token_int_value(input, k_double) || check_token_int_value(input, k_string)) //return_type je datovy typ
    {
        check_next_token_type(EOL);
        return SUCCESS;
    } else {
        error(ERR_SYNTA);
    }
}

int params() {
    t_token *input = check_next_token_type(KEY_WORD);
    if (check_token_int_value(input, k_as)) { //as
        input = check_next_token_type(KEY_WORD);

        if (check_token_int_value(input, k_integer) || check_token_int_value(input, k_double) ||
            check_token_int_value(input, k_string)) //return_type je datovy typ
        {
            input = get_token();
            check_pointer(input);

            if (input->token_type == COMMA) {
                input = get_token();
                if (input == NULL) {
                    error(ERR_SYNTA);
                }
                check_next_token_type(ID);
                return params();
            } else if (input->token_type == RPAR) {
                return SUCCESS;
            } else {
                error(ERR_SYNTA);
            }
        }
    }
}

int commandsAndVariables(){
    t_token * input = get_token();
    check_pointer(input);

    tdata value = input->data;
    bool isCorrect = input->token_type == KEY_WORD;

    if(input->token_type == ID){
        check_next_token_type(EQ);
        expression();
        return commandsAndVariables();
    }
    else if (isCorrect) {
        switch (value.i) {
            case k_dim: //dim
                check_next_token_type(ID);
                input = check_next_token_type(KEY_WORD);
                if (check_token_int_value(input, 0)) { //AS
                    input = check_next_token_type(KEY_WORD);
                    if (check_token_int_value(input, 5) || check_token_int_value(input, 12) ||
                        check_token_int_value(input, 18)) { //typ
                        check_next_token_type(EOL);
                        return commandsAndVariables();
                    }
                }
                error(ERR_SYNTA);
            case k_input: //input
                check_next_token_type(ID);
                check_next_token_type(EOL);
                return commandsAndVariables();
            case k_print: //print
                print_params(); //skonci vcetne EOL
                return commandsAndVariables();
            case k_if: //if
                if(expression()!=k_then){
                    error(ERR_SYNTA);
                }
                check_next_token_type(EOL);
                int correct = commandsAndVariables();
                if (correct == k_if) {//skoncilo to end if
                    return commandsAndVariables();
                }
                error(ERR_SYNTA);
            case k_else: //else
                check_next_token_type(EOL);
                    return commandsAndVariables();
            case k_end: //end
                input = check_next_token_type(KEY_WORD);
                if (check_token_int_value(input,k_if)){ //if
                    check_next_token_type(EOL);
                        return k_if;
                } else if (check_token_int_value(input,k_scope)){ //Scope
                        return k_scope;
                } else if (check_token_int_value(input,k_function)){ //Function
                    check_next_token_type(EOL);
                        return k_function;
                }
                error(ERR_SYNTA);
            case k_do: //do
                input = check_next_token_type(KEY_WORD);
                if (check_token_int_value(input,k_while)){ //while
                    if(expression() != EOL){
                        error(ERR_SYNTA);
                    } //tohle asi nepojede
                        if(commandsAndVariables() == k_loop){ //skoncilo to loop
                            return commandsAndVariables();
                        }
                    error(ERR_SYNTA);
                }
                error(ERR_SYNTA);
            case k_loop: //loop
                check_next_token_type(EOL);
                return k_loop; //tady to ma asi vracet k_loop
            case k_return: //return
                expression();
                return commandsAndVariables();
            default:
                error(ERR_SYNTA);
        }
    }
}

int print_params(){
    int result = expression();
    while(result == SEMICOLLON){
        result = expression();
    }
    if(result == EOL){
        return SUCCESS;
    }
    error(ERR_SYNTA);
}

/*parametry funkce, mozna to k necemu bude, mozna ne, zatim se to nidke nepouziva*/
int idList(){
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
}

int scope(){
    check_next_token_type(EOL);
    int res = commandsAndVariables();
    if(res == k_scope){
        return SUCCESS;
    }
    error(ERR_SYNTA);
}
