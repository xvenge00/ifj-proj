#ifndef IFJ_PROJ_ERR_H
#define IFJ_PROJ_ERR_H

#define ERR_LEXIK 1
#define ERR_SYNTA 2
#define ERR_SEM_P 3
#define ERR_SEM_T 4
#define ERR_SEM_E 6
#define ERR_INTER 99

void error(int code);
void semerror(int code);
void is_undefined(char *name);
void redefine_error(char *name);

#endif //IFJ_PROJ_ERR_H
