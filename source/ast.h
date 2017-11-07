//
// Created by sneakysquid on 07.11.2017.
//
#include "parser.h"
#ifndef IFJ_PROJ_AST_H
#define IFJ_PROJ_AST_H

typedef enum {
    k_as = 0,       //0
    k_asc,
    k_declare,
    k_dim,
    k_do,
    k_double,   //5
    k_else,
    k_end,
    k_chr,
    k_function,
    k_if,       //10
    k_input,
    k_integer,
    k_length,
    k_loop,
    k_print,    //15
    k_return,
    k_scope,
    k_string,
    k_subStr,
    k_then,     //20
    k_while,
} ASTNodeType;



/* define the actual AST nodes */
typedef struct astNode {
    ASTNodeType typ;
    struct astnode* left;
    struct astnode* right;

} * astList;
#endif //IFJ_PROJ_AST_H
