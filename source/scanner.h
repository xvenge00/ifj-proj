#ifndef IFJ_PROJ_SCANNER_H
#define IFJ_PROJ_SCANNER_H

typedef struct {
    int token_type;      // typ tokenu
    void *data;
} t_token;

void set_source_file(FILE *f);
t_token get_token();
char *get_lexema();

#endif //IFJ_PROJ_SCANNER_H
