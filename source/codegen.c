#include "codegen.h"
#include "memwork.h"
#include "err.h"
#include "parser.h"

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

int used_length = 0;
int used_substr = 0;
int used_asc = 0;
int used_chr = 0;

//printf("DEFVAR ");
//printf("POPS ");
void define_length() {
    if (used_length) {
        printf("LABEL length\n");
        printf("DEFVAR %s\n POPS %s\n", "TF@s", "TF@s");
        printf("STRLEN TF@%RETVAL TF@s\n");
        printf("RETURN\n");
    }
}

void define_substr() {
    if (used_substr) {
        t_3ac j;
        printf("LABEL substr\n");
        printf("DEFVAR %s\n POPS %s\n", "TF@s", "TF@s");
        printf("DEFVAR %s\n POPS %s\n", "TF@i", "TF@i");
        printf("DEFVAR %s\n POPS %s\n", "TF@n", "TF@n");

        printf("SUB TF@i TF@i int@1\n");

        char *ret_o = gen_label("return0");
        char *n_ok = gen_label("back");

        printf("DEFVAR TF@tlac\n");
        printf("MOVE TF@tlac string@\n");
        printf("DEFVAR TF@temp\n");
        printf("DEFVAR TF@len\n");
        printf("STRLEN TF@len TF@s\n");
        printf("SUB TF@len TF@len TF@i\n");
        printf("LT TF@temp TF@i int@0\n");
        printf("JUMPIFEQ l_return0_0 TF@temp bool@true\n");
        printf("LT TF@temp TF@len int@1\n");
        printf("JUMPIFEQ l_return0_0 TF@temp bool@true\n");
        printf("GT TF@temp TF@n int@-1\n");
        printf("PUSHS TF@temp\n");
        printf("LT TF@temp TF@n TF@len\n");
        printf("PUSHS TF@temp\n");
        printf("EQ TF@temp TF@n TF@len\n");
        printf("PUSHS TF@temp\n");
        printf("ORS\n");
        printf("ANDS\n");
        printf("POPS TF@temp\n");
        printf("JUMPIFEQ l_back_1 TF@temp bool@true\n");
        printf("MOVE TF@n TF@len\n");
        printf("LABEL l_back_1\n");
        printf("LABEL $substr_while\n");
        printf("GT TF@temp TF@n int@0\n");
        printf("JUMPIFEQ $substr_end_while TF@temp bool@false\n");
        printf("GETCHAR TF@temp TF@s TF@i\n");
        printf("ADD TF@i TF@i int@1\n");
        printf("SUB TF@n TF@n int@1\n");
        printf("CONCAT TF@tlac  TF@tlac TF@temp\n");
        printf("JUMP $substr_while\n");
        printf("LABEL $substr_end_while\n");
        printf("LABEL l_return0_0\n");
        printf("MOVE TF@%RETVAL TF@tlac\n");


        printf("RETURN\n");

    }
}

void define_asc() {
    if (used_asc) {
        printf("LABEL asc\n");
        printf("DEFVAR %s\n POPS %s\n", "TF@s", "TF@s");
        printf("DEFVAR %s\n POPS %s\n", "TF@i", "TF@i");


        //todo def f asc(...
        //konec def
        printf("DEFVAR TF@temp\n");
        printf("DEFVAR TF@len\n");


        printf("STRLEN TF@len TF@s\n");
        printf("SUB TF@len TF@len int@1");

        printf("LT TF@temp TF@i int@0\n");
        printf("PUSHS TF@temp\n");
        printf("GT TF@temp TF@i int@len\n");
        printf("PUSHS TF@temp\n");
        printf("ORS\n");
        printf("POPS TF@temp\n");


        char *ret_0 = gen_label("change_s");


        printf("JUMPIFEQ %s TF@temp boolean@TRUE", ret_0);

        printf("STRI2INT TF@%RETVAL TF@temp TF@i");

        printf("LABEL %s\n", ret_0);
        printf("RETURN\n");

    }
}

void define_chr() {
    if (used_chr) {
        printf("LABEL chr\n");
        printf("DEFVAR %s\n POPS %s\n", "TF@i", "TF@i");
        printf("INT2CHAR TF@%RETVAL TF@i\n");
        printf("RETURN\n");
    }
}


void point_swap(char **p1, char **p2) {
    char *tmp = *p1;
    *p1 = *p2;
    *p2 = tmp;
}


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
    // na zaciatku musi byt .IFJcode17
    printf("%s\n", oper[I_HEADER]);
    printf("JUMP $l_main\n");
    printf("# --------------------------------------------------------------\n");
    printf("# ---------------------TU-SA-DEF-BUILT-IN-----------------------\n");
    printf("# --------------------------------------------------------------\n");

    define_length();
    define_substr();
    define_asc();
    define_chr();

    printf("# --------------------------------------------------------------\n");
    printf("# ---------------------TU-SA-DEF-ZDROJ--------------------------\n");
    printf("# --------------------------------------------------------------\n");


    t_3ac *i = head;
    while (i != NULL) {
        print_operation(i);
        i = i->next;
    }

    printf("# ---------------------KONEC-ZDROJU------------------------------\n");

    printf("LABEL $l_end");
    return 0;

}

char *call_function(char *name, Element **params, unsigned param_count) {
    char *dest = gen_temp_var();
    char *tmp = my_strcpy(dest);
    tmp[0] = 'L';
    tmp[1] = 'F';
    for (unsigned i = 0; i < param_count; ++i) {
        create_3ac(I_PUSHS, NULL, NULL, params[i]->operand);
    }
    create_3ac(I_PUSHFRAME, NULL, NULL, NULL);
    create_3ac(I_CREATEFRAME, NULL, NULL, NULL);
    create_3ac(I_DEFVAR, NULL, NULL, "TF@%RETVAL");
    create_3ac(I_MOVE, "int@0", NULL, "TF@%RETVAL");
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

char *gen_temp_var() {
    char *result = malloc(sizeof(char) * BUFFSIZE);
    snprintf(result, BUFFSIZE, "TF@$E_E%i", get_id());
    create_3ac(I_DEFVAR, NULL, NULL, result);
    return result;
}

char *op_add(int operation, Element *l_operand, Element *r_operand) {
    if (operation != E_PLUS) {
        internall_err(__LINE__);
    }
    char *dest = gen_temp_var();

    char *l_op_str = l_operand->operand;
    char *r_op_str = r_operand->operand;

    int l_typ = l_operand->typ_konkretne;
    int r_typ = r_operand->typ_konkretne;


    if (!is_data_type(l_typ) || !is_data_type(r_typ)) {
        bad_operands_err(line);
    }

    if (l_typ == r_typ) {   //netreba konverziu
        if (l_typ == k_string) {
            create_3ac(I_CONCAT, l_op_str, r_op_str, dest);
        } else {    //double, int
            create_3ac(I_ADD, l_op_str, r_op_str, dest);
        }
    } else {               //treba konverziu
        char *tmp = dest;
        point_swap(&tmp, &dest);

        if (l_typ == k_integer && r_typ == k_double) {

            create_3ac(I_INT2FLOAT, l_op_str, NULL, tmp);
            create_3ac(I_ADD, tmp, r_op_str, dest);
        } else if (l_typ == k_double && r_typ == k_integer) {
            create_3ac(I_INT2FLOAT, r_op_str, NULL, tmp);
            create_3ac(I_ADD, l_op_str, r_op_str, dest);
        } else {    //je tam string
            bad_operands_err(line);
        }
        l_operand->typ_konkretne = k_double;

    }
    return dest;
}

char *op_sub_mul(int operation, Element *l_operand, Element *r_operand) {
    if (operation == E_MINUS) {
        operation = I_SUB;
    } else if (operation == E_MUL) {
        operation = I_MUL;
    } else {
        internall_err(__LINE__);
    }

    char *dest = gen_temp_var();

    char *l_op_str = l_operand->operand;
    char *r_op_str = r_operand->operand;

    int l_typ = l_operand->typ_konkretne;
    int r_typ = r_operand->typ_konkretne;

    if (!is_num_type(l_typ) || !is_num_type(r_typ)) {
        bad_operands_err(line);
    }

    if (l_typ == r_typ) {   //netreba konverziu
        create_3ac(operation, l_op_str, r_op_str, dest);
    } else {                //treba konverziu
        char *tmp = dest;
        point_swap(&tmp, &dest);

        if (l_typ == k_integer && r_typ == k_double) {
            create_3ac(I_INT2FLOAT, l_op_str, NULL, tmp);
            create_3ac(operation, tmp, r_op_str, dest);
        } else if (l_typ == k_double && r_typ == k_integer) {
            create_3ac(I_INT2FLOAT, r_op_str, NULL, tmp);
            create_3ac(operation, l_op_str, tmp, dest);
        } else {
            bad_operands_err(line);
        }
        l_operand->typ_konkretne = k_double;

    }
    return dest;
}

char *op_div(int operation, Element *l_operand, Element *r_operand) {
    if (operation != E_DIV) {
        internall_err(__LINE__);

    }
    char *dest = gen_temp_var();

    char *l_op_str = l_operand->operand;
    char *r_op_str = r_operand->operand;

    int l_typ = l_operand->typ_konkretne;
    int r_typ = r_operand->typ_konkretne;

    if (!is_num_type(l_typ) || !is_num_type(r_typ)) {
        bad_operands_err(line);
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
        char *tmp = dest;
        point_swap(&tmp, &dest);

        if (l_typ == k_integer && r_typ == k_double) {
            create_3ac(I_INT2FLOAT, l_op_str, NULL, tmp);
            create_3ac(I_DIV, tmp, r_op_str, dest);
        } else if (l_typ == k_double && r_typ == k_integer) {
            create_3ac(I_INT2FLOAT, r_op_str, NULL, tmp);
            create_3ac(I_DIV, l_op_str, tmp, dest);
        } else {
            bad_operands_err(line);
        }
    }
    return dest;
}

char *op_mod(int operation, Element *l_operand, Element *r_operand) {
    if (operation != E_MOD) {
        internall_err(__LINE__);

    }
    char *tmp = gen_temp_var();

    char *dest = gen_temp_var();

    char *l_op_str = l_operand->operand;
    char *r_op_str = r_operand->operand;

    int l_typ = l_operand->typ_konkretne;
    int r_typ = r_operand->typ_konkretne;

    if (!is_num_type(l_typ) || !is_num_type(r_typ)) {
        bad_operands_err(line);
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

char *op_lt_gt_eq(int operation, Element *l_operand, Element *r_operand) {
    if (operation == E_LT) {
        operation = I_LT;
    } else if (operation == E_GT) {
        operation = I_GT;
    } else if (operation == E_EQ) {
        operation = I_EQ;
    } else {
        internall_err(__LINE__);

    }
    char *dest = gen_temp_var();

    char *l_op_str = l_operand->operand;
    char *r_op_str = r_operand->operand;

    int l_typ = l_operand->typ_konkretne;
    int r_typ = r_operand->typ_konkretne;

    if (!is_data_type(l_typ) || !is_data_type(r_typ)) {
        bad_operands_err(line);
    }

    if (l_typ == r_typ) {
        create_3ac(I_LT, l_op_str, r_op_str, dest);
    } else {
        char *tmp = dest;
        point_swap(&tmp, &dest);

        if (l_typ == k_integer && r_typ == k_double) {
            create_3ac(I_INT2FLOAT, l_op_str, NULL, tmp);
            create_3ac(operation, tmp, r_op_str, dest);
        } else if (l_typ == k_double && r_typ == k_integer) {
            create_3ac(I_INT2FLOAT, r_op_str, NULL, tmp);
            create_3ac(operation, l_op_str, tmp, dest);
        } else {
            bad_operands_err(line);
        }
        l_operand->typ_konkretne = k_double;

    }
    return dest;
}

char *op_le_ge(int operation, Element *l_operand, Element *r_operand) {
    if (operation == E_LE) {
        operation = I_LT;
    } else if (operation == E_GE) {
        operation = I_GT;
    } else {
        internall_err(__LINE__);

    }
    char *dest = gen_temp_var();

    char *l_op_str = l_operand->operand;
    char *r_op_str = r_operand->operand;

    int l_typ = l_operand->typ_konkretne;
    int r_typ = r_operand->typ_konkretne;

    if (!is_data_type(l_typ) || !is_data_type(r_typ)) {
        bad_operands_err(line);
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
            create_3ac(I_INT2FLOAT, l_op_str, NULL, tmp);
            create_3ac(operation, tmp, r_op_str, dest);
            create_3ac(I_PUSHS, NULL, NULL, dest);
            create_3ac(I_EQ, tmp, r_op_str, dest);
            create_3ac(I_PUSHS, NULL, NULL, dest);
            create_3ac(I_ORS, NULL, NULL, NULL);
            create_3ac(I_POPS, NULL, NULL, dest);
        } else if (l_typ == k_double && r_typ == k_integer) {
            create_3ac(I_INT2FLOAT, r_op_str, NULL, tmp);
            create_3ac(operation, l_op_str, tmp, dest);
            create_3ac(I_PUSHS, NULL, NULL, dest);
            create_3ac(I_EQ, l_op_str, tmp, dest);
            create_3ac(I_PUSHS, NULL, NULL, dest);
            create_3ac(I_ORS, NULL, NULL, NULL);
            create_3ac(I_POPS, NULL, NULL, dest);
        } else {
            bad_operands_err(line);
        }
        l_operand->typ_konkretne = k_double;

    }
    return dest;
}

/*
 * vrati dest, aby sa potom mohlo pushnut na zasobnik
 */
char *gen_and_convert(int operation, Element *l_operand, Element *r_operand) {
    check_null(l_operand);
    check_null(r_operand);
    char *dest = NULL;

    switch (operation) {
        case E_PLUS:
            dest = op_add(operation, l_operand, r_operand);
            break;
        case E_MINUS:
            dest = op_sub_mul(operation, l_operand, r_operand);
            break;
        case E_MUL:
            dest = op_sub_mul(operation, l_operand, r_operand);
            break;
        case E_DIV:
            dest = op_div(operation, l_operand, r_operand);
            break;
        case E_MOD:     //!!!netestovane, len skopirovane z expression.c
            dest = op_mod(operation, l_operand, r_operand);
            break;
        case E_LT:
            dest = op_lt_gt_eq(operation, l_operand, r_operand);
            break;
        case E_LE:
            dest = op_le_ge(operation, l_operand, r_operand);
            break;
        case E_GT:
            dest = op_lt_gt_eq(operation, l_operand, r_operand);
            break;
        case E_GE:
            dest = op_le_ge(operation, l_operand, r_operand);
            break;
        case E_EQ:
            dest = op_lt_gt_eq(operation, l_operand, r_operand);
            break;
        case E_NEQ:
            dest = op_lt_gt_eq(operation, l_operand, r_operand);
            create_3ac(I_NOT, dest, NULL, dest);    //E_EQ ale na koniec zneguj
            break;
        default:
            internall_err(__LINE__);
    }

    return dest;
}