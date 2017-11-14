#include "codegen.h"
#include "memwork.h"
#include <stdio.h>

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

t_3ac *create_3ac(char *operation,char *op1, char *op2, char *dest){
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
    if (code->operation != NULL) {
        printf("%s", code->operation);
    }
    if (code->dest != NULL) {
        printf(" %s", code->dest);
    }
    if (code->op1 != NULL) {
        printf(" %s", code->op1);
    }
    if (code->op2 != NULL) {
        printf(" %s", code->op2);
    }
    printf("\n");
}




int generate_code(){
    return 0;
}