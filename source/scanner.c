#include <stdio.h>
#include "scanner.h"
#include <string.h>

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

FILE *file;     //zdrojovy subor

void set_source_file(FILE *f){
    file = f;
}

/*  vrati dalsiu lexemu */
char *get_lexema(){
    return NULL;
}

/*  zoberie lexemu a vrati token    */
t_token tget_token(char *lex){

}