#ifndef IFJ_PROJ_PARSER_H
#define IFJ_PROJ_PARSER_H

#include <stdbool.h>
#include "scanner.h"
#include "symtable.h"

#define SUCCESS 1

int parse(TTable* Table);
int function(int decDef,TTable* Table,TTable* local);
//int params(TFunction *functions,TTable* local,char* name, int decDef);
int params(TTable *local, unsigned *attr_count, int **attributes, int decDef);


int commandsAndVariables(TTable* local);

//int expression();
int expressionOrFunction();
int idList();
int scope(TTable *Table);
int print_params(TTable *local);
t_token* check_next_token_type(int type);
bool check_token_int_value(t_token * input, int value);
bool check_pointer(void* input);
void error(int code);



#endif //IFJ_PROJ_PARSER_H
