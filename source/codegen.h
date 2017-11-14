#ifndef IFJ_PROJ_CODEGEN_H
#define IFJ_PROJ_CODEGEN_H

typedef struct s_3ac{
    char *operation;
    char *op1;
    char *op2;
    char *dest;
    struct s_3ac *next;
}t_3ac;

void append_3ac(t_3ac *code);
t_3ac *create_3ac(char *operation,char *op1, char *op2, char *dest);
void print_operation(t_3ac *code);

int generate_code();

#endif //IFJ_PROJ_CODEGEN_H
