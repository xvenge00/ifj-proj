#include <stdio.h>
#include "scanner.h"
#include "parser.h"
#include "symtable.h"
#include "codegen.h"

#define FILE_ERROR 5

int main(int argc, char** argv) {
    FILE *f;

    if (argc == 1) {
        printf("Nebol zadany vstupny subor\n");
        return FILE_ERROR;
    }

    if ((f = fopen(argv[1], "r")) == NULL) {
        printf("Subor sa nepodarilo otvorit\n");
        return FILE_ERROR;
    }

    int result;     //do premennej sa zapisuju vysledky krokov


    set_source_file(f);

    if (result = init()){   //inicializacia struktur
        //osetrenie chyb
    }
    if (result = parse()){
        //osetrenie chyb
    }
    if(result=generate_code()){
        //osetrenie chyb
    }

    return 0;
}
