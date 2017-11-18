#include "err.h"
#include <stdio.h>
#include <stdlib.h>
#include "memwork.h"

void error(int code) {
    fprintf(stderr, "Error v syntakticke analyze - spatny typ tokenu.\n");
    exit(code);
}

void semerror(int code) {
    fprintf(stderr, "Error v semanticke analyze.\n");
    clear_all();
    exit(code);
}

void is_undefined(char *name) {
    fprintf(stderr, "Prvok \"%s\" nebol definovany.\n", name);
    clear_all();
    exit(ERR_SEM_P);
}

void redefine_error(char *name) {
    fprintf(stderr, "Prvok \"%s\" uz bol raz definovany.\n", name);
    clear_all();
    exit(ERR_SEM_P);
}

void internall_err() {
    fprintf(stderr, "Interna chyba prekladaca.\n");
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
