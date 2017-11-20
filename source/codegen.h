#ifndef IFJ_PROJ_CODEGEN_H
#define IFJ_PROJ_CODEGEN_H

#include "expression.h"

#define BUFFSIZE 150    //"TF@"+256 pre ID + \0

enum {
    I_MOVE = 0,         //0
    I_CREATEFRAME,
    I_PUSHFRAME,
    I_POPFRAME,
    I_DEFVAR,
    I_CALL,             //5
    I_RETURN,
    I_PUSHS,
    I_POPS,
    I_CLEARS,
    I_ADD,              //10
    I_SUB,
    I_MUL,
    I_DIV,
    I_ADDS,
    I_SUBS,             //15
    I_MULS,
    I_DIVS,
    I_LT,
    I_GT,
    I_EQ,               //20
    I_LTS,
    I_GTS,
    I_EQS,
    I_AND,
    I_OR,               //25
    I_NOT,
    I_ANDS,
    I_ORS,
    I_NOTS,
    I_INT2FLOAT,        //30
    I_FLOAT2R2EINT,
    I_FLOAT2R2OINT,
    I_INT2CHAR,
    I_STRI2INT,
    I_READ,             //35
    I_WRITE,
    I_CONCAT,
    I_STRLEN,
    I_GETCHAR,
    I_SETCHAR,          //40
    I_TYPE,
    I_LABEL,
    I_JUMP,
    I_JUMPIFEQ,
    I_JUMPIFNEQ,        //45
    I_JUMPIFEQS,
    JUMPIFNEQS,
    I_BREAK,
    I_DPRINT,
    I_HEADER            //50
};


typedef struct s_3ac{
    int operation;
    char *op1;
    char *op2;
    char *dest;
    struct s_3ac *next;
}t_3ac;

void append_3ac(t_3ac *code);
t_3ac *create_3ac(int operation,char *op1, char *op2, char *dest);
void print_operation(t_3ac *code);

int generate_code();

char *gen_temp_var();

char *cat_string(char *frame, char *variable);
char *call_function(char *name, Element *params, unsigned param_count);
char *gen_and_convert(int operation, Element *l_operand, Element *r_operand);
bool is_data_type (int typ);

#endif //IFJ_PROJ_CODEGEN_H
