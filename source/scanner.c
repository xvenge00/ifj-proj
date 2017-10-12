#include <stdio.h>
#include "scanner.h"
#include <string.h>
#include <ctype.h>
#include <stdlib.h>

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
    t_token result = {empty_token, NULL};
    static int state = s_START;

    /*todo struktura buffery*/
    static char buff[100] = {0,}; // todo prerobyt na prementlivu dlzku
    static int top = 0;
    static int max = 100;
    /* konec bufferu */

    char loaded = getchar();
    while (loaded != EOF){
        switch (state){
            case s_START:
                if (isalpha(loaded) || (loaded == '_')){
                    //append buffer
                    buff[top] = loaded;
                    top++;
                    //next state
                    state = s_ID;
                } //todo pre ine
                break;
            case s_ID:
                if (isalnum(loaded)||(loaded == '_')){
                    //append buffer
                    buff[top] = loaded;
                    top++;
                } else {
                    char *name;
                    name = malloc(sizeof(char)*(top+1));
                    if (name == NULL){
                        fprintf(stderr,"Nedostatok pamete\n");
                        //todo uvolnenie vsetkej pamete a potom ukoncenie programu s kodom 99
                    }
                    //prekopirovanie str
                    for (unsigned i=0; i<top + 1;i++){
                        name[i] = buff[i];
                    }
                    top = 0;
                    //vytvorenie tokenu
                    result.token_type = ID;
                    result.data = (void *)name;

                    //nulovanie bufferu
                    for (unsigned i=0; i<max; i++){
                        buff[i] = 0;
                    }

                    //todo zistenie dalsiho stavu a pozadaovanieho spracovania nacitaneho znaku zatial sa znak zahodi a stav sa da start
                    state = s_START;


                    return result;

                }
                break;
                //todo dorobit ostatne typy a doplnit 
        }
    }


    return result;
}