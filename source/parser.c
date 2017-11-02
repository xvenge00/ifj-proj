#include "parser.h"
#include "main.h"
#include <stdlib.h>


#define SUCCESS 1

t_token* check_next_token_type(int type){
    t_token * input = get_token();
    if(input == NULL){
        fprintf(stderr,"Error v syntakticke analyze - spatny typ tokenu.\n");
        exit(ERR_SYNTA);
    }
    if(input->token_type != type){
        fprintf(stderr,"Error v syntakticke analyze - spatny typ tokenu.\n");
        exit(ERR_SYNTA);
    }
    return input;
}
bool check_token_int_value(t_token * input, int value){
    if(input == NULL){
        fprintf(stderr,"Error v syntakticke analyze - spatny typ tokenu.\n");
        exit(ERR_SYNTA);
    }
    if(input->data.i != value){
        return false;
    }
    return true;
}

bool check_pointer(t_token* input) {
    if(input == NULL){
        fprintf(stderr,"Error v syntakticke analyze - spatny typ tokenu.\n");
        exit(ERR_SYNTA);
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
            case 2: //declare
                input = check_next_token_type(KEY_WORD);
                check_token_int_value(input,9);
                function();
                return parse();
            case 9: //function
                function(); //parametry, konci tokenem EOL
                if(commandsAndVariables()==9){ //function
                    return parse();
                }
                else{
                    error(ERR_SYNTA);
                }
            case 17: //scope
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
    check_token_int_value(input, 0); //as

    input = check_next_token_type(KEY_WORD);
    if(check_token_int_value(input, 5) || check_token_int_value(input, 12) || check_token_int_value(input, 18)) //return_type je datovy typ
    {
        check_next_token_type(EOL);
        return SUCCESS;
    } else {
        error(ERR_SYNTA);
    }
}

int params() {
    t_token *input = check_next_token_type(KEY_WORD);
    if (check_token_int_value(input, 0)) { //as
        input = check_next_token_type(KEY_WORD);

        if (check_token_int_value(input, 5) || check_token_int_value(input, 12) ||
            check_token_int_value(input, 18)) //return_type je datovy typ
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
            expressionOrFunction();
    }
    else if (isCorrect) {
        switch (value.i){
            case 3: //dim
                check_next_token_type(ID);
                    input = check_next_token_type(KEY_WORD);
                    if (check_token_int_value(input, 0)) { //AS
                        input = check_next_token_type(KEY_WORD);
                        if (check_token_int_value(input, 5) || check_token_int_value(input, 12) || check_token_int_value(input, 18)) { //typ
                            check_next_token_type(EOL);
                            return commandsAndVariables();
                        }
                    }
                error(ERR_SYNTA);
            case 11: //input
                check_next_token_type(ID);
                check_next_token_type(EOL);
                return commandsAndVariables();
            case 15: //print
                print_params(); //skonci vcetne EOL
                commandsAndVariables();
            case 10: //if
                    expression();//taky je tu asi token navic
                    input =check_next_token_type(KEY_WORD);
                        if(check_token_int_value(input,20)){ //then
                            check_next_token_type(EOL);
                                if(commandsAndVariables() == 10){//skoncilo to end if
                                    return SUCCESS;
                                }
                        }
                error(ERR_SYNTA);
            case 6: //else
                check_next_token_type(EOL);
                    return commandsAndVariables();
            case 7: //end
                input = check_next_token_type(KEY_WORD);
                if (check_token_int_value(input,10)){ //if
                    check_next_token_type(EOL);
                        return 10;
                } else if (check_token_int_value(input,17)){ //Scope
                        return 17;
                } else if (check_token_int_value(input,9)){ //Function
                    check_next_token_type(EOL);
                        return 9;
                }
                error(ERR_SYNTA);
            case 4: //do
                input = check_next_token_type(KEY_WORD);
                if (check_token_int_value(input,21)){ //while
                    expression(); //tohle asi nepojede
                    check_next_token_type(EOL);
                        if(commandsAndVariables() == 14){ //skoncilo to loop
                            return SUCCESS;
                        }
                    error(ERR_SYNTA);
                }
                error(ERR_SYNTA);
            case 14: //loop
                check_next_token_type(EOL);
                return 14;
            case 16: //return
                expression();
                return 16;
            case 9: //function - jen ve scope
                return 9+17; //function+scope
            default:
                error(ERR_SYNTA);
        }
    }
}

int print_params(){
    expression();
    check_next_token_type(SEMICOLLON);
    t_token * input = get_token();
    check_pointer(input);

    if(input->token_type == EOL){
        return SUCCESS;
    }
    return print_params();
}

int expressionOrFunction(){
    return SUCCESS; //az s vyrazy, mozna tahle funkce nebude vubec potreba
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
    if(res == 26){ //byla tam definice funkce
        function();
        scope();
    } else if(res == 17){ //skoncil uspesne SCOPE
        return SUCCESS;
    } else { //vsechny ostatni returny jsou blbe
        error(ERR_SYNTA);
    }
}
