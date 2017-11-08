#ifndef IFJ_PROJ_EXPRESSION_H
#define IFJ_PROJ_EXPRESSION_H

#include <stddef.h>

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
    E_DOLLAR, //15
    E_FUNC,
    E_COMMA,
    E_E //18
}types;

typedef struct element{
    int type;
    struct element *next;
} Element; //prvek zásobníku - type bude zakodovany typ symbolu, next ukazatel na dalsi

typedef struct stack{
    Element *active; //ukazuje na nejvrchnejsi terminal
    Element *top; //ukazuje na vrchol zasobniku
}Stack;


#endif //IFJ_PROJ_EXPRESSION_H
