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


/*  vrati dalsiu lexemu */
char *get_lexema(){
    return NULL;
}

/*  zoberie lexemu a vrati token    */
t_token tget_token(char *lex){
    //priprava bufferu
    append_buff(0);
    null_buffer();

    t_token result = {empty_token, NULL};
    static int state = s_START;
    char loaded = getchar();
    while (loaded != EOF){
        switch (state){
            case s_START:
                if (isalpha(loaded) || (loaded == '_')){
                    append_buff(loaded);
                    //next state
                    state = s_ID;
                } //todo pre ine
                break;
            case s_ID:
                if (isalnum(loaded)||(loaded == '_')){
                    //append buffer
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

                    //todo zistenie dalsiho stavu a pozadaovanieho spracovania nacitaneho znaku zatial sa znak zahodi a stav sa da start
                    state = s_START;


                    return result;

                }
                break;
                //todo dorobit ostatne typy a doplnit
        }
        loaded = getchar();
    }


    return result;
}