#include "parser.h"
#include "scanner.h"
#include "main.h"
#include <stdbool.h>


#define SUCCESS 0;

int parse(){
    t_token * input = get_token();
    if(input == NULL){
        return ERR_LEXIK;
    }

    //prvni token musi byt Declare nebo Function nebo SCOPE
    bool isCorrect = input->token_type == MIN_KEY_WORLD;
    tdata value = input->data;
    if (isCorrect) {
        switch (value.i) {

            case 2: //declare
                input = get_token();
                if (input == NULL) {
                    return ERR_LEXIK;
                }
                isCorrect = input->token_type == MIN_KEY_WORLD && input->data.i == 9;
                if (isCorrect) {
                        function();
                }
                else{
                        return ERR_LEXIK;
                }
                break;
            case 9: //function
                function();
                commandsAndVariables();
                break;
            case 17: //scope
                commandsAndVariables();
                //pridat END SCOPE
                break;
            default:
                return ERR_LEXIK;
        }
    }
    else {
            return ERR_LEXIK;
    }
    return SUCCESS;
}

int function(){
    t_token * input = get_token();
    if(input == NULL){
        return ERR_LEXIK;
    }

    bool isCorrect = input->token_type == ID;

    if (isCorrect) {
        input = get_token();
        if (input == NULL) {
            return ERR_LEXIK;
        }
        if (input->token_type == LPAR) {
            input = get_token();
            if (input == NULL) {
                return ERR_LEXIK;
            }
            if (input->token_type == ID) {
                params();
            }
            else if (input->token_type == RPAR) {}
            else {
                return ERR_LEXIK;
            }
        }
        else {
            return ERR_LEXIK;
        }
    }
    else{
        return ERR_LEXIK;
    } //byla nactena posledni zavorka, zbyva return typ

    input = get_token();
    if(input == NULL){
        return ERR_LEXIK;
    }

    if(input->token_type == MIN_KEY_WORLD && input->data.i == 0){ //AS
        input = get_token();
        if(input == NULL){
            return ERR_LEXIK;
        }
        if(input->token_type == 2 || input->token_type == 3 || input->token_type == 4) //return_type je datovy typ
        {
            input = get_token();
            if(input == NULL){
                return ERR_LEXIK;
            }
            if(input->token_type == EOL){
                return SUCCESS;
            }
        }
    }
    else {
        return ERR_LEXIK;
    }
}

int params() {
    t_token *input = get_token();
    if (input == NULL) {
        return ERR_LEXIK;
    }

    bool isCorrect = input->token_type == MIN_KEY_WORLD && input->data.i == 0; //AS
    if (isCorrect) {
        input = get_token();
        if(input == NULL){
            return ERR_LEXIK;
        }
        if(input->token_type == 2 || input->token_type == 3 || input->token_type == 4) //typ
        {
            input = get_token();
            if(input == NULL){
                return ERR_LEXIK;
            }
            if(input->token_type == COMMA){
                input = get_token();
                if(input == NULL){
                    return ERR_LEXIK;
                }
                if(input->token_type == ID){
                    params();
                } else {
                    return ERR_LEXIK;
                }
            }
        }
        else if(input->token_type == RPAR){
            return SUCCESS;
        } else {
            return ERR_LEXIK;
        }
    }
    else {
        return ERR_LEXIK;
    }

}

int commandsAndVariables(){
    return SUCCESS;
}
