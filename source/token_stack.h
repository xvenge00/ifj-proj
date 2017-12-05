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

#include "scanner.h"

#ifndef IFJ_PROJ_TOKEN_STACK_H
#define IFJ_PROJ_TOKEN_STACK_H


void token_push(t_token *token);

t_token *token_pop();

void str_push(char *str);
char *str_pop();

#endif //IFJ_PROJ_TOKEN_STACK_H
