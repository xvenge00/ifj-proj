//
// Created by pbabka on 16.11.2017.
//

#include "scanner.h"

#ifndef IFJ_PROJ_TOKEN_STACK_H
#define IFJ_PROJ_TOKEN_STACK_H


void token_push(t_token *token);

t_token *token_pop();

void str_push(char *str);
char *str_pop();

#endif //IFJ_PROJ_TOKEN_STACK_H
