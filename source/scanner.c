#include <stdio.h>
#include "scanner.h"
#include <string.h>

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