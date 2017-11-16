#include "codegen.h"
#include "memwork.h"
#include <stdio.h>

char oper[50][15] = {
        "MOVE",
        "CREATEFRAME",
        "PUSHFRAME",
        "POPFRAME",
        "DEFVAR",
        "CALL",
        "RETURN",
        "PUSHS",
        "POPS",
        "CLEARS",
        "ADD",
        "SUB",
        "MUL",
        "DIV",
        "ADDS",
        "SUBS",
        "MULS",
        "DIVS",
        "LT",
        "GT",
        "EQ",
        "LTS",
        "GTS",
        "EQS",
        "AND",
        "OR",
        "NOT",
        "ANDS",
        "ORS",
        "NOTS",
        "INT2FLOAT",
        "FLOAT2R2EINT",
        "FLOAT2R2OINT",
        "INT2CHAR",
        "STRI2INT",
        "READ",
        "WRITE",
        "CONCAT",
        "STRLEN",
        "GETCHAR",
        "SETCHAR",
        "TYPE",
        "LABEL",
        "JUMP",
        "JUMPIFEQ",
        "JUMPIFNEQ",
        "JUMPIFEQS",
        "JUMPIFNEQS",
        "BREAK",
        "DPRINT"
};





t_3ac *head = NULL;
t_3ac *tail = NULL;


void append_3ac(t_3ac *code){
    if (head == NULL){
        head = code;
        tail = code;
    } else {
        tail->next = code;
        tail = code;
    }
}

t_3ac *create_3ac(int operation,char *op1, char *op2, char *dest){
    t_3ac *result = my_malloc(sizeof(t_3ac));
    result->operation = operation;
    result->op1 = op1;
    result->op2 = op2;
    result->dest = dest;
    result->next = NULL;
//    append_3ac(result);
    print_operation(result);
    return result;
}

void print_operation(t_3ac *code){
    if (code->operation >= 0 && code->operation <= 50) {
        if (code->operation != I_LABEL){
            printf("    ");
        }
        printf("%s", oper[code->operation]);
        if (code->dest != NULL) {
            printf(" %s", code->dest);
        }
        if (code->op1 != NULL) {
            printf(" %s", code->op1);
        }
        if (code->op2 != NULL) {
            printf(" %s", code->op2);
        }
    }

    printf("\n");
}




int generate_code(){
    t_3ac j;
    j.op1 = j.op2 = NULL;
    j.dest = "!l_main";
    j.operation = I_JUMP;
    print_operation(&j);

    t_3ac *i = head;
    while ( i != NULL){
        print_operation(i);
        i = i->next;
    }
    return 0;
}