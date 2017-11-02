#include <stdbool.h>
#include "scanner.h"

#ifndef IFJ_PROJ_PARSER_H
#define IFJ_PROJ_PARSER_H

int parse();
int function();
int params();
int commandsAndVariables();
int expression();
int expressionOrFunction();
int idList();
int scope();
int print_params();
t_token* check_next_token_type(int type);
bool check_token_int_value(t_token * input, int value);
bool check_pointer(t_token* input);
void error(int code);



#endif //IFJ_PROJ_PARSER_H
