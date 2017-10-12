#ifndef IFJ_PROJ_SCANNER_H
#define IFJ_PROJ_SCANNER_H

#define empty_token 0

//stavy automatu
#define s_START 0
#define s_ID 1


//zaheslovane typ tokenu
#define ID 1




typedef struct {
    int token_type;      // typ tokenu
    void *data;
} t_token;

t_token get_token();
char *get_lexema();

#endif //IFJ_PROJ_SCANNER_H
