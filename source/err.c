#include "err.h"
#include <stdio.h>
#include <stdlib.h>
#include "memwork.h"
#include "expression.h"

/* univerzalna chybova funkcia */
void error(char *str, int err_code, int line) {
    fprintf(stderr,"line: %i %s\n", line, str);
    clear_all();
    exit(err_code);
}

void syntax_error(int code, int line) {
    fprintf(stderr, "Error v syntakticke analyze - spatny typ tokenu na riadku %i.\n",line);
    exit(code);
}

void semerror(int code, int line) {
    fprintf(stderr, "Error v semanticke analyze na riadku %i.\n",line);
    clear_all();
    exit(code);
}

void undefined_err(char *name, int line) {
    fprintf(stderr, "Line %i: Prvok \"%s\" nebol definovany.\n", line, name);
    clear_all();
    exit(ERR_SEM_DEF);
}

void redefine_error(char *name, int line) {
    fprintf(stderr, "Line %i: Prvok \"%s\" uz bol raz definovany.\n", line, name);
    clear_all();
    exit(ERR_SEM_DEF);
}

void internall_err(int line) {
    fprintf(stderr, "Interna chyba prekladaca riadok %i. zdrojoveho kodu\n",line);
    clear_all();
    exit(ERR_INTER);
}

void check_null(void *ptr) {
    if (ptr != NULL) {
        return;
    }
    fprintf(stderr, "Chybny ukazatel. \n");
    clear_all();
    exit(ERR_INTER);
}

void bad_operands_err(int line) {
    fprintf(stderr, "Nekompatibilne operandy na riadku %i.\n",line);
    clear_all();
    exit(ERR_SEM_TYPE);
}

void incompatible_types_err(int line) {
    fprintf(stderr, "Line: %i: nekompatibilne typy.\n", line);
    clear_all();
    exit(ERR_SEM_TYPE);
}
