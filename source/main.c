#include <stdio.h>
#include "scanner.h"
#include "parser.h"
#include "symtable.h"
#include "codegen.h"
#include "main.h"

#define FILE_ERROR 5

int main(int argc, char** argv) {

    /* z dovodov testovania */
    /* neviem automaticky testovat vstup zo stdin */
    /* namiesto getchar pouzivajte teda fgetc(FILE *stream) */
    if (argc == 2){
        f = fopen(argv[1],"r");
    } else {                        //ked neni zadany argument, cita vstup zo stdin
        f = stdin;
    }


    int result;     //do premennej sa zapisuju vysledky krokov


    //if (result = init()){   //inicializacia struktur
    //    //osetrenie chyb
    //}
    if (result = parse()){
        //osetrenie chyb
    }
    if(result=generate_code()){
        //osetrenie chyb
    }

    return 0;
}
