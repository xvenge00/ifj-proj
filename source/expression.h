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

#ifndef IFJ_PROJ_EXPRESSION_H
#define IFJ_PROJ_EXPRESSION_H

#include <stddef.h>
#include "scanner.h"
#include "symtable.h"

enum{
    xx
}table_values;

enum{
    E_PLUS = 0,
    E_MINUS,
    E_MUL,
    E_DIV,
    E_MOD,
    E_LPAR, //5
    E_RPAR,
    E_ID,
    E_LT,
    E_LE,
    E_GT, //10
    E_GE,
    E_EQ,
    E_NEQ,
    E_DOLLAR, //14
    E_FUNC,
    E_COMMA,
    E_E //17
}types;

typedef struct element{
    int type;
    char  *operand;
    int typ_konkretne;
    struct element *next;
} Element; //prvek zásobníku - type bude zakodovany typ symbolu, next ukazatel na dalsi

typedef struct stack{
    Element *active; //ukazuje na nejvrchnejsi terminal
    Element *top; //ukazuje na vrchol zasobniku
}Stack;

char *token2operand(t_token *token);
int get_id();

int expression(TTable *func_table,TTable *local, int typ, char **ret_var);
char *cat_string(char *frame, char *variable);
char *my_strcpy(char *src);
bool is_imm_val(char *value);

#endif //IFJ_PROJ_EXPRESSION_H
