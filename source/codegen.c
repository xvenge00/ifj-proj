#include "codegen.h"
#include "memwork.h"
#include "expression.h"
#include <stdio.h>
#include "scanner.h"
#include "err.h"
#include "parser.h"

void point_swap(char** p1, char **p2){
    char *tmp = *p1;
    *p1 = *p2;
    *p2 = tmp;
}


char oper[52][15] = {
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
        "DPRINT",
        ".IFJcode17",
        "#"
};


t_3ac *head = NULL;
t_3ac *tail = NULL;


void append_3ac(t_3ac *code) {
    if (head == NULL) {
        head = code;
        tail = code;
    } else {
        tail->next = code;
        tail = code;
    }
}

t_3ac *create_3ac(int operation, char *op1, char *op2, char *dest) {
    t_3ac *result = my_malloc(sizeof(t_3ac));
    result->operation = operation;
    result->op1 = my_strcpy(op1);
    result->op2 = my_strcpy(op2);
    result->dest = my_strcpy(dest);
    result->next = NULL;
    append_3ac(result);
//    print_operation(result);
    return result;
}

void print_operation(t_3ac *code) {
    if (code->operation >= 0 && code->operation <= 50) {
        if (code->operation != I_LABEL) {
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


int generate_code() {
    t_3ac j;
    // na zaciatku musi byt .IFJcode17
    j.op1 = j.op2 = j.dest = NULL;
    j.operation = I_HEADER;
    print_operation(&j);

    j.dest = "$l_main";
    j.operation = I_JUMP;
    print_operation(&j);

    t_3ac *i = head;
    while (i != NULL) {
        print_operation(i);
        i = i->next;
    }
    j.dest = "$l_end";
    j.operation = I_LABEL;
    print_operation(&j);
    return 0;

}

char *call_function(char *name, Element **params, unsigned param_count) {
    char *dest = gen_temp_var();
    char *tmp = my_strcpy(dest);tmp[0] = 'L';
    tmp[1] = 'F';
    for (unsigned i = 0; i < param_count; ++i) {
        create_3ac(I_PUSHS, NULL, NULL, params[i]->operand);
    }
    create_3ac(I_PUSHFRAME, NULL, NULL, NULL);
    create_3ac(I_CREATEFRAME, NULL, NULL, NULL);
    create_3ac(I_DEFVAR, NULL, NULL, "TF@%RETVAL");
    create_3ac(I_CALL, NULL, NULL, name);
    create_3ac(I_MOVE, "TF@%RETVAL", NULL, tmp);
    create_3ac(I_POPFRAME, NULL, NULL, NULL);
    return dest;
}

bool is_data_type(int typ) {
    return typ == k_integer || typ == k_double || typ == k_string;
}

bool is_num_type(int typ) {
    return typ == k_integer || typ == k_double;
}

char *gen_temp_var(){
    char *result = malloc(sizeof(char) * BUFFSIZE);
    snprintf(result, BUFFSIZE, "TF@$E_E%i", get_id());
    create_3ac(I_DEFVAR,NULL,NULL,result);
    return result;
}

char *op_add(int operation, Element *l_operand, Element *r_operand){
    if (operation == E_PLUS){
        operation = I_ADD;
    } else {
        internall_err();
    }
    char *dest = gen_temp_var();

    char *l_op_str = l_operand->operand;
    char *r_op_str = r_operand->operand;

    int l_typ = l_operand->typ_konkretne;
    int r_typ = r_operand->typ_konkretne;


    if (!is_data_type(l_typ) || !is_data_type(r_typ)) {
        bad_operands_err();
    }

    if (l_typ == r_typ) {   //netreba konverziu
        if (l_typ == k_string) {
            create_3ac(I_CONCAT, l_op_str, r_op_str, dest);
        } else {    //double, int
            create_3ac(I_ADD, l_op_str, r_op_str, dest);
        }
    } else {               //treba konverziu
        char *tmp = gen_temp_var();
        point_swap(&tmp, &dest);


        if (l_typ == k_integer && r_typ == k_double) {
            create_3ac(I_FLOAT2R2EINT, r_op_str, NULL, tmp);
            create_3ac(I_ADD, l_op_str, tmp, dest);
        } else if (l_typ == k_double && r_typ == k_integer) {
            create_3ac(I_INT2FLOAT, r_op_str, NULL, tmp);
            create_3ac(I_ADD, l_op_str, r_op_str, dest);
        } else {    //je tam string
            bad_operands_err();
        }
    }
    return dest;
}

char *op_sub_mul(int operation, Element *l_operand, Element *r_operand){
    if (operation == E_MINUS){
        operation = I_SUB;
    } else if (operation == E_MUL) {
        operation = I_MUL;
    } else {
        internall_err();
    }

    char *dest = gen_temp_var();

    char *l_op_str = l_operand->operand;
    char *r_op_str = r_operand->operand;

    int l_typ = l_operand->typ_konkretne;
    int r_typ = r_operand->typ_konkretne;

    if (!is_num_type(l_typ) || !is_num_type(r_typ)) {
        bad_operands_err();
    }

    if (l_typ == r_typ) {   //netreba konverziu
        create_3ac(operation, l_op_str, r_op_str, dest);
    } else {                //treba konverziu
        char *tmp = gen_temp_var();
        point_swap(&tmp, &dest);

        if (l_typ == k_integer && r_typ == k_double) {
            create_3ac(I_FLOAT2R2EINT, r_op_str, NULL, tmp);
            create_3ac(operation, l_op_str, tmp, dest);
        } else if (l_typ == k_double && r_typ == k_integer) {
            create_3ac(I_INT2FLOAT, r_op_str, NULL, tmp);
            create_3ac(operation, l_op_str, tmp, dest);
        } else {
            bad_operands_err();
        }
    }
    return dest;
}

char *op_div(int operation, Element *l_operand, Element *r_operand){
    if (operation == E_DIV){
        operation = I_DIV;
    } else {
        internall_err();

    }
    char *dest = gen_temp_var();

    char *l_op_str = l_operand->operand;
    char *r_op_str = r_operand->operand;

    int l_typ = l_operand->typ_konkretne;
    int r_typ = r_operand->typ_konkretne;

    if (!is_num_type(l_typ) || !is_num_type(r_typ)) {
        bad_operands_err();
    }

    if (l_typ == r_typ) {   //netreba konverziu
        if (l_typ == k_integer) {
            create_3ac(I_INT2FLOAT, l_op_str, NULL, dest);
            create_3ac(I_PUSHS, NULL, NULL, dest);
            create_3ac(I_INT2FLOAT, r_op_str, NULL, dest);
            create_3ac(I_PUSHS, NULL, NULL, dest);
            create_3ac(I_DIVS, NULL, NULL, NULL);
            create_3ac(I_POPS, NULL, NULL, dest);
        } else {            //double
            create_3ac(I_DIV, l_op_str, r_op_str, dest);
        }
    } else {                //treba konverziu
        char *tmp = gen_temp_var();
        point_swap(&tmp, &dest);

        if (l_typ == k_integer && r_typ == k_double) {
            create_3ac(I_INT2FLOAT, l_op_str, NULL, tmp);
            create_3ac(I_DIV, tmp, r_op_str, dest);
        } else if (l_typ == k_double && r_typ == k_integer) {
            create_3ac(I_INT2FLOAT, r_op_str, NULL, tmp);
            create_3ac(I_DIV, l_op_str, tmp, dest);
        } else {
            bad_operands_err();
        }
    }
    return dest;
}

char *op_mod(int operation, Element *l_operand, Element *r_operand){
    if (operation != E_MOD){
        internall_err();

    }
    char *tmp = gen_temp_var();

    char *dest = gen_temp_var();

    char *l_op_str = l_operand->operand;
    char *r_op_str = r_operand->operand;

    int l_typ = l_operand->typ_konkretne;
    int r_typ = r_operand->typ_konkretne;

    if (!is_num_type(l_typ) || !is_num_type(r_typ)) {
        bad_operands_err();
    }

    if (l_typ == r_typ) {
        if (l_typ == k_integer) {
            create_3ac(I_INT2FLOAT, l_op_str, NULL, dest);
            create_3ac(I_PUSHS, NULL, NULL, dest);
            create_3ac(I_INT2FLOAT, r_op_str, NULL, dest);
            create_3ac(I_PUSHS, NULL, NULL, dest);
            create_3ac(I_DIVS, NULL, NULL, NULL);
            create_3ac(I_POPS, NULL, NULL, dest);
        } else {    //double
            create_3ac(I_DIV, l_op_str, r_op_str, dest);
        }
    } else {    //treba konverziu
        if (l_typ == k_integer && r_typ == k_double) {
            create_3ac(I_INT2FLOAT, l_op_str, NULL, tmp);
            create_3ac(I_DIV, tmp, r_op_str, dest);
        } else {    //double && int
            create_3ac(I_INT2FLOAT, r_op_str, NULL, tmp);
            create_3ac(I_DIV, l_op_str, tmp, dest);
        }
    }
    create_3ac(I_PUSHS, NULL, NULL, dest);
    //zaokruhlenie
    create_3ac(I_FLOAT2R2EINT, dest, NULL, dest);
    create_3ac(I_INT2FLOAT, dest, NULL, dest);

    create_3ac(I_PUSHS, NULL, NULL, dest);
    create_3ac(I_SUBS, NULL, NULL, NULL);
    if (r_typ == k_integer) {
        create_3ac(I_INT2FLOAT, r_op_str, NULL, tmp);
        create_3ac(I_PUSHS, NULL, NULL, tmp);
    } else {
        create_3ac(I_PUSHS, NULL, NULL, r_op_str);
    }
    create_3ac(I_MULS, NULL, NULL, NULL);
    create_3ac(I_POPS, NULL, NULL, tmp);
    create_3ac(I_FLOAT2R2EINT, tmp, NULL, dest);

    return dest;
}

char *op_lt_gt_eq(int operation, Element *l_operand, Element *r_operand){
    if (operation == E_LT){
        operation = I_LT;
    } else if (operation == E_GT){
        operation = I_GT;
    } else if (operation == E_EQ){
        operation = I_EQ;
    } else {
        internall_err();

    }
    char *dest = gen_temp_var();

    char *l_op_str = l_operand->operand;
    char *r_op_str = r_operand->operand;

    int l_typ = l_operand->typ_konkretne;
    int r_typ = r_operand->typ_konkretne;

    if (!is_data_type(l_typ) || !is_data_type(r_typ)) {
        bad_operands_err();
    }

    if (l_typ == r_typ) {
        create_3ac(I_LT, l_op_str, r_op_str, dest);
    } else {
        char *tmp = gen_temp_var();
        point_swap(&tmp, &dest);

        if (l_typ == k_integer && r_typ == k_double) {
            create_3ac(I_FLOAT2R2EINT, r_op_str, NULL, tmp);
            create_3ac(operation, l_op_str, tmp, dest);
        } else if (l_typ == k_double && r_typ == k_integer) {
            create_3ac(I_INT2FLOAT, r_op_str, NULL, tmp);
            create_3ac(operation, l_op_str, tmp, dest);
        } else {
            bad_operands_err();
        }
    }
    return dest;
}

char *op_le_ge(int operation, Element *l_operand, Element *r_operand){
    if (operation == E_LE){
        operation = I_LT;
    } else if (operation == E_GE){
        operation = I_GT;
    } else {
        internall_err();

    }
    char *dest = gen_temp_var();

    char *l_op_str = l_operand->operand;
    char *r_op_str = r_operand->operand;

    int l_typ = l_operand->typ_konkretne;
    int r_typ = r_operand->typ_konkretne;

    if (!is_data_type(l_typ) || !is_data_type(r_typ)) {
        bad_operands_err();
    }

    if (l_typ == r_typ) {
        create_3ac(operation, l_op_str, r_op_str, dest);
        create_3ac(I_PUSHS, NULL, NULL, dest);
        create_3ac(I_EQ, l_op_str, r_op_str, dest);
        create_3ac(I_PUSHS, NULL, NULL, dest);
        create_3ac(I_ORS, NULL, NULL, NULL);
        create_3ac(I_POPS, NULL, NULL, dest);
    } else {
        char *tmp = gen_temp_var();
        point_swap(&tmp, &dest);

        if (l_typ == k_integer && r_typ == k_double) {
            create_3ac(I_FLOAT2R2EINT, r_op_str, NULL, tmp);
            create_3ac(operation, l_op_str, tmp, dest);
            create_3ac(I_PUSHS, NULL, NULL, dest);
            create_3ac(I_EQ, l_op_str, tmp, dest);
            create_3ac(I_PUSHS, NULL, NULL, dest);
            create_3ac(I_ORS, NULL, NULL, NULL);
            create_3ac(I_POPS, NULL, NULL, dest);
        } else if (l_typ == k_double && r_typ == k_integer) {
            create_3ac(I_FLOAT2R2EINT, r_op_str, NULL, tmp);
            create_3ac(operation, l_op_str, tmp, dest);
            create_3ac(I_PUSHS, NULL, NULL, dest);
            create_3ac(I_EQ, l_op_str, tmp, dest);
            create_3ac(I_PUSHS, NULL, NULL, dest);
            create_3ac(I_ORS, NULL, NULL, NULL);
            create_3ac(I_POPS, NULL, NULL, dest);
        } else {
            bad_operands_err();
        }
    }
    return dest;
}



/*
 * vrati dest, aby sa potom mohlo pushnut na zasobnik
 * funguje len +,-,*,/
 * / sa zacykli!!!
 */
char *gen_and_convert(int operation, Element *l_operand, Element *r_operand) {
    check_null(l_operand);
    check_null(r_operand);
    char *dest = NULL;

    //TODO zjednotit velkost vsetkych stringov

    switch (operation) {
        case E_PLUS:
            dest = op_add(I_ADD,l_operand, r_operand);
            break;
        case E_MINUS:
            dest = op_sub_mul(operation,l_operand, r_operand);
            break;
        case E_MUL:
            dest = op_sub_mul(operation,l_operand, r_operand);
            break;
        case E_DIV:
            dest = op_div(operation,l_operand, r_operand);
            break;
        case E_MOD:     //!!!netestovane, len skopirovane z expression.c
            dest = op_mod(operation,l_operand, r_operand);
            break;
        case E_LT:
            dest = op_lt_gt_eq(operation, l_operand, r_operand);
            break;
        case E_LE:
            dest = op_le_ge(operation,l_operand,r_operand);
            break;
        case E_GT:
            dest = op_lt_gt_eq(operation,l_operand, r_operand);
            break;
        case E_GE:
            dest =  op_le_ge(operation,l_operand,r_operand);
            break;
        case E_EQ:
            dest = op_lt_gt_eq(operation,l_operand, r_operand);
            break;
        case E_NEQ:
            dest = op_lt_gt_eq(operation,l_operand, r_operand);
            create_3ac(I_NOT, dest, NULL, dest);    //E_EQ ale na koniec zneguj
            break;
        default:
            internall_err();
    }

    return dest;
}