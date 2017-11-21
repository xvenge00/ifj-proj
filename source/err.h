#ifndef IFJ_PROJ_ERR_H
#define IFJ_PROJ_ERR_H

#define ERR_LEXIK 1
#define ERR_SYNTA 2
#define ERR_SEM_DEF 3
#define ERR_SEM_TYPE 4
#define ERR_SEM_OTH 6
#define ERR_INTER 99

void error(char *str, int err_code);
void syntax_error(int code);
void semerror(int code);
void undefined_err(char *name);
void redefine_error(char *name);
void internall_err();
void check_null(void *ptr);
void bad_operands_err();

#endif //IFJ_PROJ_ERR_H
