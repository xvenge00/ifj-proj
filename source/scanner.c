#include <stdio.h>
#include "scanner.h"
#include <string.h>
#include <ctype.h>
#include <stdlib.h>
#include "memwork.h"
#include "str_buff.h"
#include "main.h"



const char *key_worlds[] = {
        "As",
        "Asc",
        "Declare",
        "Dim",
        "Do",
        "Double",
        "Else",
        "End",
        "Chr",
        "Function",
        "If",
        "Input",
        "Integer",
        "Length",
        "Loop",
        "Print",
        "Return",
        "Scope",
        "String",
        "SubStr",
        "Then",
        "While"
};

const char *rez_key_worlds[] = {
        "And",
        "Boolean",
        "Continue",
        "Elseif",
        "Exit",
        "False",
        "For",
        "Next",
        "Not",
        "Or",
        "Shared",
        "Static",
        "True"
};


/*  zoberie lexemu a vrati token    */
t_token tget_token(char *lex){
    //priprava bufferu
    append_buff(0);
    null_buffer();
    int str_tmp = 0;
    double double_tmp = 0;

    static int line = 0;

    t_token result = {empty_token, NULL};
    static int state = s_START;
    char loaded = getchar();
    while (loaded != EOF){
        if (loaded == '\n'){
            line++;
        }
        switch (state){
            case s_START:
                null_buffer();
                if (isspace(loaded)){
                    state = s_START;
                } else if (isalpha(loaded) || (loaded == '_')){
                    append_buff(loaded);
                    state = s_ID;
                } else if (loaded == '\''){
                    state = s_line_comment;
                } else if (loaded == '/'){
                    state = s_block_coment_0;
                } else if (loaded == ':'){
                    state = s_asign_0;
                } else if (isdigit(loaded)){
                    append_buff(loaded);
                    state = s_INT;
                } else if (loaded == '!'){
                    state = s_str0;
                } else if (loaded == '+'){
                    t_token token = {PLUS, NULL};
                    return token;
                } else if (loaded == '-'){
                    t_token token = {MINUS, NULL};
                    return token;
                } else if (loaded == '*'){
                    t_token token = {KRAT, NULL};
                    return token;
                } else if (loaded == '\\'){
                    t_token token = {MOD,NULL};
                    return token;
                } else if (loaded == '='){
                    t_token token = {EQ,NULL};
                    return token;
                } else if (loaded == '>'){
                    t_token token = {QE, NULL};
                    return token;
                } else if (loaded == '<'){
                    state = s_LT;
                } else {
                    clear_all();
                    exit(ERR_LEXIK);
                }//todo pre ine
                break;
            case s_line_comment:
                if (loaded == '\n'){
                    state = s_START;
                } else {
                    state = s_line_comment;
                }
                break;
            case s_block_coment_0:
                if (loaded == '\''){
                    state = s_block_coment_1;
                } else if (isalpha(loaded) || (loaded == '_')){
                    null_buffer();
                    t_token token = {DELENO, NULL};
                    state = s_ID;
                    append_buff(loaded);
                    return token;
                } else if (isdigit(loaded)){
                    null_buffer();
                    t_token token = {DELENO, NULL};
                    state = s_INT;
                    append_buff(loaded);
                    return token;
                } else {
                    clear_all();
                    exit(ERR_INTER);
                }
                break;
            case s_block_coment_1:
                if (loaded == '\''){
                    state = s_block_coment_2;
                } else {
                    state = s_block_coment_1;
                }
                break;
            case s_block_coment_2:
                if (loaded == '/'){
                    state = s_START;
                } else {
                    state = s_block_coment_1;
                }
                break;
            case s_ID:
                if (isalnum(loaded)||(loaded == '_')){
                    append_buff(loaded);
                } else {
                    // generovanie tokenu
                    char *name;
                    char *buff = get_buff();
                    name = my_malloc(sizeof(char)*(buff_size()+1));
                    //prekopirovanie str
                    for (unsigned i=0; i<buff_size() + 1;i++){
                        name[i] = buff[i];
                    }

                    null_buffer();
                    //vytvorenie tokenu
                    result.token_type = ID;
                    result.data = (void *)name;

                    //nasledujuci stav
                    if (isspace(loaded)){
                        state = s_START;
                    } else if (loaded == ':'){
                        state = s_asign_0;
                    } else {
                        //todo skontrolovat ci moze byt
                        clear_all();
                        fprintf(stderr, "JE ZLE ZADANE ID za nim je zly znak na riadku %i \n",line);
                        exit(ERR_LEXIK);
                    }
                    //todo zistenie dalsiho stavu a pozadaovanieho spracovania nacitaneho znaku zatial sa znak zahodi a stav sa da start
                    state = s_START;


                    return result;

                }
                break;
            case s_INT:
                if (isdigit(loaded)){
                    append_buff(loaded);
                } else if (isspace(loaded)){
                    state = s_START;
                } else if (loaded == ':'){
                    state = s_asign_0;
                } else if (loaded == '.'){
                    state = s_double_0;
                    append_buff(loaded);
                } else if ((loaded == 'e') || (loaded == 'E')){
                    state = s_double_1;

                } else {
                    clear_all();
                    fprintf(stderr, "ZLE ZADANE CISLO NA RIADKU %i", line);
                    exit(ERR_LEXIK);
                }
                if ((state != s_INT)||(state != s_double_0)|| (state !=s_double_1)){
                    //vygeneruj token
                    int *tmp;
                    tmp = my_malloc(sizeof(int));
                    *tmp = strtol(get_buff(),NULL, 10);
                    t_token token = {INT, tmp};
                    null_buffer();
                    return token;
                }
                break;
            case s_double_0:
                if (isdigit(loaded)){
                    append_buff(loaded);
                } else {

                }
                //todo dorobit ostatne typy a doplnit
        }
        loaded = getchar();
    }


    return result;
}