#include <stdio.h>
#include "scanner.h"
#include "parser.h"
#include "symtable.h"
#include "codegen.h"
#include "err.h"
#include "memwork.h"

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
    load_all_token();
    TTable* Table = Tbl_Create(8);
    parse(Table);
    generate_code();
    return 0;
}
