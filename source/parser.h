#ifndef IFJ_PROJ_PARSER_H
#define IFJ_PROJ_PARSER_H

#include <stdbool.h>
#include "scanner.h"
#include "symtable.h"

#define SUCCESS 1
extern int line;

int parse(TTable* func_table);
int function(int decDef,TTable* func_table,TTable* local);
//int params(TFunction *functions,TTable* local,char* name, int decDef);
int params(TTable *local, unsigned *attr_count, int **attributes, int decDef);


int commandsAndVariables(TTable *Table,TTable *local);

//int expression();
int expressionOrFunction();
int idList();
int scope(TTable *Table);
int print_params(TTable *Table,TTable *local);
t_token* check_next_token_type(int type);
bool check_token_int_value(t_token * input, int value);
bool check_pointer(void* input);



#endif //IFJ_PROJ_PARSER_H
