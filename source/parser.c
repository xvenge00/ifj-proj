#include "parser.h"
#include "scanner.h"
#include "main.h"
#include <stdbool.h>


#define SUCCESS 0

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
                function(); //parametry, konci tokenem EOL
                if(commandsAndVariables()==9){
                    return SUCCESS;
                }
                else{
                    return ERR_LEXIK;
                }
            case 17: //scope
                input = get_token();
                if(input == NULL){
                    return ERR_LEXIK;
                }
                if(input->token_type == EOL){
                    if(commandsAndVariables() == 17){
                        return SUCCESS;
                    }
                    else{
                        return ERR_LEXIK;
                    }
                }

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
    t_token * input = get_token();
    if(input == NULL){
        return ERR_LEXIK;
    }

    tdata value = input->data;
    bool isCorrect = input->token_type == MIN_KEY_WORLD;

    if(input->token_type == ID){
        input = get_token();
        if(input == NULL){
            return ERR_LEXIK;
        }
        if(input->token_type == EQ){
            expression();
        }
        else
        {
            return ERR_LEXIK;
        }
    }
    else if (isCorrect) {
        switch (value.i){
            case 11: //input
                input = get_token();
                if(input == NULL){
                    return ERR_LEXIK;
                }
                if(input->token_type == ID){
                    input = get_token();
                    if(input == NULL){
                        return ERR_LEXIK;
                    }
                    if(input->token_type == EOL){
                        return commandsAndVariables();
                    }
                }
            case 15: //print
                input = get_token();
                if(input == NULL){
                    return ERR_LEXIK;
                }
                if(input->token_type == ID){
                    while(input->token_type == ID){
                        expression();

                        input = get_token();
                        if(input == NULL){
                            return ERR_LEXIK;
                        }

                        if(input->token_type == comma) //zmenit na semicolon!
                        {
                            input = get_token(); //posuneme se na dalsi token a znova vyhodnoti vyraz
                            if(input == NULL){
                                return ERR_LEXIK;
                            }
                        }
                        else
                        {
                            return ERR_LEXIK;
                        }
                    }
                    if(input->token_type == EOL){
                        return commandsAndVariables();
                    }
                    else
                    {
                        return ERR_LEXIK;
                    }
                }
                else
                {
                    return ERR_LEXIK;
                }
                //vyraz
                //; vyraz
                //EOL a konec
            case 10: //if
                input = get_token();
                if(input == NULL){
                    return ERR_LEXIK;
                }
                if (input->token_type == ID){
                    expression();
                    input = get_token();
                    if(input == NULL){
                        return ERR_LEXIK;
                    }
                    if (input->token_type == MIN_KEY_WORLD){
                        if(input->data.i == 20){ //then
                            input = get_token();
                            if(input == NULL){
                                return ERR_LEXIK;
                            }
                            if (input->token_type == EOL){
                                return commandsAndVariables();
                            }
                            else
                            {
                                return ERR_LEXIK;
                            }
                        }
                        else
                        {
                            return ERR_LEXIK;
                        }

                    }
                    else
                    {
                        return ERR_LEXIK;
                    }
                }
                else
                {
                    return ERR_LEXIK;
                }
            case 6: //else
                input = get_token();
                if(input == NULL){
                    return ERR_LEXIK;
                }
                if (input->token_type == EOL){
                    return commandsAndVariables();
                }
                else
                {
                    return ERR_LEXIK;
                }
            case 7: //end
                input = get_token();
                if(input == NULL){
                    return ERR_LEXIK;
                }
                if (input->token_type == MIN_KEY_WORLD && input->data.i == 10){ //if
                    input = get_token();
                    if(input == NULL){
                        return ERR_LEXIK;
                    }
                    if (input->token_type == EOL){
                        return commandsAndVariables();
                    }
                    else
                    {
                        return ERR_LEXIK;
                    }
                } else if (input->token_type == MIN_KEY_WORLD && input->data.i == 17){ //Scope
                    input = get_token();
                    if(input == NULL){
                        return ERR_LEXIK;
                    }
                    if (input->token_type == EOL){
                        return 17;
                    }
                    else
                    {
                        return ERR_LEXIK;
                    }
                } else if (input->token_type == MIN_KEY_WORLD && input->data.i == 9){ //Function
                    input = get_token();
                    if(input == NULL){
                        return ERR_LEXIK;
                    }
                    if (input->token_type == EOL){
                        return 9;
                    }
                    else
                    {
                        return ERR_LEXIK;
                    }
                }
                else
                {
                    return ERR_LEXIK;
                }

            case 4: //do
                input = get_token();
                if(input == NULL){
                    return ERR_LEXIK;
                }
                if (input->token_type == MIN_KEY_WORLD && input->data.i == 21){ //while
                    input = get_token();
                    if(input == NULL){
                        return ERR_LEXIK;
                    }
                    if (input->token_type == ID){
                        expression(); //tohle asi nepojede
                        input = get_token();
                        if(input == NULL){
                            return ERR_LEXIK;
                        }
                        if (input->token_type == EOL){
                            return commandsAndVariables();
                        }
                        else
                        {
                            return ERR_LEXIK;
                        }
                    }
                    else
                    {
                        return ERR_LEXIK;
                    }
                }
                else
                {
                    return ERR_LEXIK;
                }
            case 14: //loop
                input = get_token();
                if(input == NULL){
                    return ERR_LEXIK;
                }
                if (input->token_type == EOL){
                    return commandsAndVariables();
                }
                else
                {
                    return ERR_LEXIK;
                }
            case 16:
                input = get_token();
                if(input == NULL){
                    return ERR_LEXIK;
                }
                expression();
                return SUCCESS;
            default:
                return ERR_LEXIK;
        }
    }
}

int expression(){
    return SUCCESS;
}