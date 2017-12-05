/*  * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *
 *  Implementace prekladace imperativniho jazyka IFJ17
 *
 *  Autori:
 *      xvenge00 - Adam Venger
 *      xbabka01 - Peter Babka
 *      xrandy00 - Vojtech Randysek
 *      xdosed08 - Ondrej Dosedel
 *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#ifndef IFJ_PROJ_ERR_H
#define IFJ_PROJ_ERR_H

#define ERR_LEXIK 1
#define ERR_SYNTA 2
#define ERR_SEM_DEF 3
#define ERR_SEM_TYPE 4
#define ERR_SEM_OTH 6
#define ERR_INTER 99

void error(char *str, int err_code, int line);
void syntax_error(int code, int line);
void semerror(int code, int line);
void undefined_err(char *name, int line);
void redefine_error(char *name, int line);
void internall_err(int line, char*file);
void check_null(void *ptr);
void bad_operands_err(int line);
void incompatible_types_err(int line);

#endif //IFJ_PROJ_ERR_H
