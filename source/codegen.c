#include "codegen.h"
#include "memwork.h"
#include "expression.h"
#include <stdio.h>
#include "scanner.h"
#include "err.h"

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
    result->op1 = op1;
    result->op2 = op2;
    result->dest = dest;
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
//    j.op1 = j.op2 = NULL;
//    j.dest = NULL;
//    j.operation = I_CREATEFRAME;
//    print_operation(&j);
//    j.op1 = j.op2 = NULL;
//    j.dest = NULL;
//    j.operation = I_PUSHFRAME;
//    print_operation(&j);

    // na zaciatku musi byt .IFJcode17
    j.op1 = j.op2 = j.dest = NULL;
    j.operation = I_HEADER;
    print_operation(&j);

    j.dest = "l_main";
    j.operation = I_JUMP;
    print_operation(&j);

    t_3ac *i = head;
    while (i != NULL) {
        print_operation(i);
        i = i->next;
    }
    return 0;
}

void call_function(char *name, char *dest) {
    create_3ac(I_DEFVAR, NULL, NULL, cat_string("TF@", dest));
    create_3ac(I_PUSHFRAME, NULL, NULL, NULL);
    create_3ac(I_CREATEFRAME, NULL, NULL, NULL);
    create_3ac(I_DEFVAR, NULL, NULL, "TF@%RETVAL");
    create_3ac(I_CALL, NULL, NULL, name);
    create_3ac(I_MOVE, "TF@%RETVAL", NULL, cat_string("LF@", dest));
    create_3ac(I_POPFRAME, NULL, NULL, NULL);
}

bool is_data_type(int typ) {
    return typ == k_integer || typ == k_double || typ == k_string;
}

bool is_num_type(int typ) {
    return typ == k_integer || typ == k_double;
}

/*
 * vrati dest, aby sa potom mohlo pushnut na zasobnik
 * funguje len +,-,*,/
 * / sa zacykli!!!
 */
char *gen_and_convert(int operation, Element *l_operand, Element *r_operand, int new_id) {
    check_null(l_operand);
    check_null(r_operand);

    //TODO zjednotit velkost vsetkych stringov

    int l_typ = l_operand->typ_konkretne;
    int r_typ = r_operand->typ_konkretne;

    char *dest_noframe = NULL;
    char *dest = NULL;
    char *tmp = NULL;
    char *l_op_str = cat_string("TF@", l_operand->operand);
    char *r_op_str = cat_string("TF@", r_operand->operand);

    dest_noframe = my_malloc(sizeof(char) * 260);
    sprintf(dest_noframe, "TF@$E_E%i", new_id);
    dest = cat_string("TF@", dest_noframe);

    switch (operation) {
        case E_PLUS:
            if (!is_data_type(l_typ) || !is_data_type(r_typ)) {
                bad_operands_err();
            }

            if (l_typ == r_typ) {   //netreba konverziu
                if (l_typ == k_string) {
                    create_3ac(I_DEFVAR, NULL, NULL, dest);
                    create_3ac(I_CONCAT, l_op_str, r_op_str, dest);
                } else {    //double, int
                    create_3ac(I_DEFVAR, NULL, NULL, dest);
                    create_3ac(I_ADD, l_op_str, r_op_str, dest);
                }
            } else {               //treba konverziu
                tmp = my_malloc(sizeof(char) * 260);
                sprintf(tmp, "TF@$E_E%i", get_id());    //generuj nazov pre tmp

                if (l_typ == k_integer && r_typ == k_double) {
                    create_3ac(I_DEFVAR, NULL, NULL, dest);
                    create_3ac(I_DEFVAR, NULL, NULL, tmp);
                    create_3ac(I_FLOAT2R2EINT, r_op_str, NULL, tmp);
                    create_3ac(I_ADD, l_op_str, tmp, dest);
                } else if (l_typ == k_double && r_typ == k_integer) {
                    create_3ac(I_DEFVAR, NULL, NULL, dest);
                    create_3ac(I_DEFVAR, NULL, NULL, tmp);
                    create_3ac(I_INT2FLOAT, r_op_str, NULL, tmp);
                    create_3ac(I_ADD, l_op_str, r_op_str, dest);
                } else {    //je tam string
                    bad_operands_err();
                }
            }
            break;
        case E_MINUS:
            if (!is_num_type(l_typ) || !is_num_type(r_typ)) {
                bad_operands_err();
            }

            if (l_typ == r_typ) {   //netreba konverziu
                create_3ac(I_DEFVAR, NULL, NULL, dest);
                create_3ac(I_SUB, l_op_str, r_op_str, dest);
            } else {                //treba konverziu
                tmp = my_malloc(sizeof(char) * 260);
                sprintf(tmp, "TF@$E_E%i", get_id());    //generuj nazov pre tmp

                if (l_typ == k_integer && r_typ == k_double) {
                    create_3ac(I_DEFVAR, NULL, NULL, dest);
                    create_3ac(I_DEFVAR, NULL, NULL, tmp);
                    create_3ac(I_FLOAT2R2EINT, r_op_str, NULL, tmp);
                    create_3ac(I_SUB, l_op_str, tmp, dest);
                } else if (l_typ == k_double && r_typ == k_integer) {
                    create_3ac(I_DEFVAR, NULL, NULL, dest);
                    create_3ac(I_DEFVAR, NULL, NULL, tmp);
                    create_3ac(I_INT2FLOAT, r_op_str, NULL, tmp);
                    create_3ac(I_SUB, l_op_str, tmp, dest);
                }   //else err??
            }
            break;
        case E_MUL:
            if (!is_num_type(l_typ) || !is_num_type(r_typ)) {
                bad_operands_err();
            }
            if (l_typ == r_typ) {   //netreba konverziu
                create_3ac(I_DEFVAR, NULL, NULL, dest);
                create_3ac(I_MUL, l_op_str, r_op_str, dest);
            } else {                //treba konverziu
                tmp = my_malloc(sizeof(char) * 260);
                sprintf(tmp, "TF@$E_E%i", get_id());    //generuj nazov pre tmp

                if (l_typ == k_integer && r_typ == k_double) {
                    create_3ac(I_DEFVAR, NULL, NULL, dest);
                    create_3ac(I_DEFVAR, NULL, NULL, tmp);
                    create_3ac(I_FLOAT2R2EINT, r_op_str, NULL, tmp);
                    create_3ac(I_MUL, l_op_str, tmp, dest);
                } else if (l_typ == k_double && r_typ == k_integer) {
                    create_3ac(I_DEFVAR, NULL, NULL, dest);
                    create_3ac(I_DEFVAR, NULL, NULL, tmp);
                    create_3ac(I_INT2FLOAT, r_op_str, NULL, tmp);
                    create_3ac(I_MUL, l_op_str, tmp, dest);
                }   //else err??
            }
            break;
        case E_DIV:
            if (!is_num_type(l_typ) || !is_num_type(r_typ)) {
                bad_operands_err();
            }

            if (l_typ == r_typ) {   //netreba konverziu
                if (l_typ == k_integer) {
                    create_3ac(I_DEFVAR, NULL, NULL, dest);
                    create_3ac(I_INT2FLOAT, l_op_str, NULL, dest);
                    create_3ac(I_PUSHS, NULL, NULL, dest);
                    create_3ac(I_INT2FLOAT, r_op_str, NULL, dest);
                    create_3ac(I_PUSHS, NULL, NULL, dest);
                    create_3ac(I_DIVS, NULL, NULL, NULL);
                    create_3ac(I_POPS, NULL, NULL, dest);
                } else {            //double
                    create_3ac(I_DEFVAR, NULL, NULL, dest);
                    create_3ac(I_DIV, l_op_str, r_op_str, dest);
                }
            } else {                //treba konverziu
                tmp = my_malloc(sizeof(char) * 260);
                sprintf(tmp, "TF@$E_E%i", get_id());    //generuj nazov pre tmp

                if (l_typ == k_integer && r_typ == k_double) {
                    create_3ac(I_DEFVAR, NULL, NULL, dest);
                    create_3ac(I_DEFVAR, NULL, NULL, tmp);
                    create_3ac(I_INT2FLOAT, l_op_str, NULL, tmp);
                    create_3ac(I_DIV, tmp, r_op_str, dest);
                } else if (l_typ == k_double && r_typ == k_integer) {
                    create_3ac(I_DEFVAR, NULL, NULL, dest);
                    create_3ac(I_DEFVAR, NULL, NULL, tmp);
                    create_3ac(I_INT2FLOAT, r_op_str, NULL, tmp);
                    create_3ac(I_DIV, l_op_str, tmp, dest);
                }   //else err??
            }
            break;
        case E_MOD:     //!!!netestovane, len skopirovane z expression.c
            if (!is_num_type(l_typ) || !is_num_type(r_typ)) {
                bad_operands_err();
            }

            if (l_typ == r_typ) {
                if (l_typ == k_integer) {
                    create_3ac(I_DEFVAR, NULL, NULL, dest);
                    create_3ac(I_INT2FLOAT, l_op_str, NULL, dest);
                    create_3ac(I_PUSHS, NULL, NULL, dest);
                    create_3ac(I_INT2FLOAT, r_op_str, NULL, dest);
                    create_3ac(I_PUSHS, NULL, NULL, dest);
                    create_3ac(I_DIVS, NULL, NULL, NULL);
                    create_3ac(I_POPS, NULL, NULL, dest);
                } else {    //double
                    create_3ac(I_DEFVAR, NULL, NULL, dest);
                    create_3ac(I_DIV, l_op_str, r_op_str, dest);
                }
            } else {    //treba konverziu
                tmp = my_malloc(sizeof(char) * 260);
                sprintf(tmp, "TF@$E_E%i", get_id());    //generuj nazov pre tmp

                if (l_typ == k_integer && r_typ == k_double) {
                    create_3ac(I_DEFVAR, NULL, NULL, dest);
                    create_3ac(I_DEFVAR, NULL, NULL, tmp);
                    create_3ac(I_INT2FLOAT, l_op_str, NULL, tmp);
                    create_3ac(I_DIV, tmp, r_op_str, dest);
                } else {    //double && int
                    create_3ac(I_DEFVAR, NULL, NULL, dest);
                    create_3ac(I_DEFVAR, NULL, NULL, tmp);
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
                sprintf(tmp, "$E_E%i", get_id());
                create_3ac(I_DEFVAR, NULL, NULL, tmp);
                create_3ac(I_INT2FLOAT, r_op_str, NULL, tmp);
                create_3ac(I_PUSHS, NULL, NULL, NULL);
            } else {
                create_3ac(I_PUSHS, NULL, NULL, NULL);
            }
            create_3ac(I_MULS, NULL, NULL, NULL);
            create_3ac(I_POPS, NULL, NULL, tmp);
            create_3ac(I_FLOAT2R2EINT, tmp, NULL, dest);
            break;
        case E_LT:
            if (!is_data_type(l_typ) || !is_data_type(r_typ)) {
                bad_operands_err();
            }

            if (l_typ == r_typ) {
                create_3ac(I_DEFVAR, NULL, NULL, dest);
                create_3ac(I_LT, l_op_str, r_op_str, dest);
            } else {
                tmp = my_malloc(sizeof(char) * 260);
                sprintf(tmp, "TF@$E_E%i", get_id());    //generuj nazov pre tmp

                if (l_typ == k_integer && r_typ == k_double) {
                    create_3ac(I_DEFVAR, NULL, NULL, dest);
                    create_3ac(I_DEFVAR, NULL, NULL, tmp);
                    create_3ac(I_FLOAT2R2EINT, r_op_str, NULL, tmp);
                    create_3ac(I_LT, l_op_str, tmp, dest);
                } else if (l_typ == k_double && r_typ == k_integer) {
                    create_3ac(I_DEFVAR, NULL, NULL, dest);
                    create_3ac(I_DEFVAR, NULL, NULL, tmp);
                    create_3ac(I_INT2FLOAT, r_op_str, NULL, tmp);
                    create_3ac(I_LT, l_op_str, tmp, dest);
                } else {
                    bad_operands_err();
                }
            }
            break;
        case E_LE:
            if (!is_data_type(l_typ) || !is_data_type(r_typ)) {
                bad_operands_err();
            }

            if (l_typ == r_typ) {
                create_3ac(I_DEFVAR, NULL, NULL, dest);
                create_3ac(I_LT, l_op_str, r_op_str, dest);
                create_3ac(I_PUSHS, NULL, NULL, dest);
                create_3ac(I_EQ, l_op_str, r_op_str, dest);
                create_3ac(I_PUSHS, NULL, NULL, dest);
                create_3ac(I_ORS, NULL, NULL, NULL);
                create_3ac(I_POPS, NULL, NULL, dest);
            } else {
                tmp = my_malloc(sizeof(char) * 260);
                sprintf(tmp, "TF@$E_E%i", get_id());    //generuj nazov pre tmp

                if (l_typ == k_integer && r_typ == k_double) {
                    create_3ac(I_DEFVAR, NULL, NULL, tmp);
                    create_3ac(I_DEFVAR, NULL, NULL, dest);
                    create_3ac(I_FLOAT2R2EINT, r_op_str, NULL, tmp);
                    create_3ac(I_LT, l_op_str, tmp, dest);
                    create_3ac(I_PUSHS, NULL, NULL, dest);
                    create_3ac(I_EQ, l_op_str, tmp, dest);
                    create_3ac(I_PUSHS, NULL, NULL, dest);
                    create_3ac(I_ORS, NULL, NULL, NULL);
                    create_3ac(I_POPS, NULL, NULL, dest);
                } else if (l_typ == k_double && r_typ == k_integer) {
                    create_3ac(I_DEFVAR, NULL, NULL, tmp);
                    create_3ac(I_DEFVAR, NULL, NULL, dest);
                    create_3ac(I_FLOAT2R2EINT, r_op_str, NULL, tmp);
                    create_3ac(I_LT, l_op_str, tmp, dest);
                    create_3ac(I_PUSHS, NULL, NULL, dest);
                    create_3ac(I_EQ, l_op_str, tmp, dest);
                    create_3ac(I_PUSHS, NULL, NULL, dest);
                    create_3ac(I_ORS, NULL, NULL, NULL);
                    create_3ac(I_POPS, NULL, NULL, dest);
                } else {
                    bad_operands_err();
                }
            }
            break;
        case E_GT:
            if (!is_data_type(l_typ) || !is_data_type(r_typ)) {
                bad_operands_err();
            }
            if (l_typ == r_typ) {
                create_3ac(I_DEFVAR, NULL, NULL, dest);
                create_3ac(I_GT, l_op_str, r_op_str, dest);
            } else {
                tmp = my_malloc(sizeof(char) * 260);
                sprintf(tmp, "TF@$E_E%i", get_id());    //generuj nazov pre tmp

                if (l_typ == k_integer && r_typ == k_double) {
                    create_3ac(I_DEFVAR, NULL, NULL, dest);
                    create_3ac(I_DEFVAR, NULL, NULL, tmp);
                    create_3ac(I_FLOAT2R2EINT, r_op_str, NULL, tmp);
                    create_3ac(I_GT, l_op_str, tmp, dest);
                } else if (l_typ == k_double && r_typ == k_integer) {
                    create_3ac(I_DEFVAR, NULL, NULL, dest);
                    create_3ac(I_DEFVAR, NULL, NULL, tmp);
                    create_3ac(I_INT2FLOAT, r_op_str, NULL, tmp);
                    create_3ac(I_GT, l_op_str, tmp, dest);
                } else {
                    bad_operands_err();
                }
            }
            break;
        case E_GE:
            if (!is_data_type(l_typ) || !is_data_type(r_typ)) {
                bad_operands_err();
            }
            if (l_typ == r_typ) {
                create_3ac(I_DEFVAR, NULL, NULL, dest);
                create_3ac(I_GT, l_op_str, r_op_str, dest);
                create_3ac(I_PUSHS, NULL, NULL, dest);
                create_3ac(I_EQ, l_op_str, r_op_str, dest);
                create_3ac(I_PUSHS, NULL, NULL, dest);
                create_3ac(I_ORS, NULL, NULL, NULL);
                create_3ac(I_POPS, NULL, NULL, dest);
            } else {
                tmp = my_malloc(sizeof(char) * 260);
                sprintf(tmp, "TF@$E_E%i", get_id());    //generuj nazov pre tmp

                if (l_typ == k_integer && r_typ == k_double) {
                    create_3ac(I_DEFVAR, NULL, NULL, dest);
                    create_3ac(I_DEFVAR, NULL, NULL, tmp);
                    create_3ac(I_FLOAT2R2EINT, r_op_str, NULL, tmp);
                    create_3ac(I_GT, l_op_str, tmp, dest);
                    create_3ac(I_PUSHS, NULL, NULL, dest);
                    create_3ac(I_EQ, l_op_str, tmp, dest);
                    create_3ac(I_PUSHS, NULL, NULL, dest);
                    create_3ac(I_ORS, NULL, NULL, NULL);
                    create_3ac(I_POPS, NULL, NULL, dest);
                } else if (l_typ == k_double && r_typ == k_integer) {
                    create_3ac(I_DEFVAR, NULL, NULL, tmp);
                    create_3ac(I_DEFVAR, NULL, NULL, dest);
                    create_3ac(I_FLOAT2R2EINT, r_op_str, NULL, tmp);
                    create_3ac(I_GT, l_op_str, tmp, dest);
                    create_3ac(I_PUSHS, NULL, NULL, dest);
                    create_3ac(I_EQ, l_op_str, tmp, dest);
                    create_3ac(I_PUSHS, NULL, NULL, dest);
                    create_3ac(I_ORS, NULL, NULL, NULL);
                    create_3ac(I_POPS, NULL, NULL, dest);
                } else {
                    bad_operands_err();
                }
            }
            break;
        case E_EQ:
            if (!is_data_type(l_typ) || !is_data_type(r_typ)) {
                bad_operands_err();
            }
            if (l_typ == r_typ) {
                create_3ac(I_DEFVAR, NULL, NULL, dest);
                create_3ac(I_EQ, l_op_str, r_op_str, dest);
            } else {
                tmp = my_malloc(sizeof(char) * 260);
                sprintf(tmp, "TF@$E_E%i", get_id());    //generuj nazov pre tmp

                if (l_typ == k_integer && r_typ == k_double) {
                    create_3ac(I_DEFVAR, NULL, NULL, dest);
                    create_3ac(I_DEFVAR, NULL, NULL, tmp);
                    create_3ac(I_FLOAT2R2EINT, r_op_str, NULL, tmp);
                    create_3ac(I_EQ, l_op_str, tmp, dest);
                } else if (l_typ == k_double && r_typ == k_integer) {
                    create_3ac(I_DEFVAR, NULL, NULL, dest);
                    create_3ac(I_DEFVAR, NULL, NULL, tmp);
                    create_3ac(I_INT2FLOAT, r_op_str, NULL, tmp);
                    create_3ac(I_EQ, l_op_str, tmp, dest);
                } else {
                    bad_operands_err();
                }
            }
            break;
        case E_NEQ:
            if (!is_data_type(l_typ) || !is_data_type(r_typ)) {
                bad_operands_err();
            }
            if (l_typ == r_typ) {
                create_3ac(I_DEFVAR, NULL, NULL, dest);
                create_3ac(I_EQ, l_op_str, r_op_str, dest);
            } else {
                tmp = my_malloc(sizeof(char) * 260);
                sprintf(tmp, "TF@$E_E%i", get_id());    //generuj nazov pre tmp

                if (l_typ == k_integer && r_typ == k_double) {
                    create_3ac(I_DEFVAR, NULL, NULL, dest);
                    create_3ac(I_DEFVAR, NULL, NULL, tmp);
                    create_3ac(I_FLOAT2R2EINT, r_op_str, NULL, tmp);
                    create_3ac(I_EQ, l_op_str, tmp, dest);
                } else if (l_typ == k_double && r_typ == k_integer) {
                    create_3ac(I_DEFVAR, NULL, NULL, dest);
                    create_3ac(I_DEFVAR, NULL, NULL, tmp);
                    create_3ac(I_INT2FLOAT, r_op_str, NULL, tmp);
                    create_3ac(I_EQ, l_op_str, tmp, dest);
                } else {
                    bad_operands_err();
                }
                create_3ac(I_NOT, dest, NULL, dest);    //E_EQ ale na koniec zneguj
            }
            break;
        default:
            internall_err();
    }

    return dest_noframe;
}