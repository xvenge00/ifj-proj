#include <stdio.h>
#include "scanner.h"
#include "parser.h"
#include "symtable.h"
#include "codegen.h"
#include "main.h"

#define FILE_ERROR 5

int main(int argc, char** argv) {


    int result;     //do premennej sa zapisuju vysledky krokov


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
