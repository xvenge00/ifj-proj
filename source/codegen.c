#include "codegen.h"
#include "memwork.h"
#include "err.h"
#include "parser.h"

int used_length = 0;
int used_substr = 0;
int used_asc = 0;
int used_chr = 0;

void define_length(){
    if (used_length){
        t_3ac j;

        j.operation = I_LABEL;
        j.op1 = NULL;
        j.op2 = NULL;
        j.dest = "length";
        print_operation(&j);

        j.operation = I_DEFVAR;
        j.op1 = NULL;
        j.op2 = NULL;
        j.dest = "TF@s";
        print_operation(&j);

        j.operation = I_POPS;
        j.op1 = NULL;
        j.op2 = NULL;
        j.dest = "TF@s";
        print_operation(&j);

        j.operation = I_STRLEN;
        j.op1 = "TF@s";
        j.op2 = NULL;
        j.dest = "TF@%RETVAL";
        print_operation(&j);

        j.operation = I_RETURN;
        j.op1 = NULL;
        j.op2 = NULL;
        j.dest = NULL;
        print_operation(&j);
    }
}

void define_substr(){
    if (used_substr){
        t_3ac j;

        j.operation = I_LABEL;
        j.op1 = NULL;
        j.op2 = NULL;
        j.dest = "substr";
        print_operation(&j);

        j.operation = I_DEFVAR;
        j.op1 = NULL;
        j.op2 = NULL;
        j.dest = "TF@s";
        print_operation(&j);

        j.operation = I_POPS;
        j.op1 = NULL;
        j.op2 = NULL;
        j.dest = "TF@s";
        print_operation(&j);

        j.operation = I_DEFVAR;
        j.op1 = NULL;
        j.op2 = NULL;
        j.dest = "TF@i";
        print_operation(&j);

        j.operation = I_POPS;
        j.op1 = NULL;
        j.op2 = NULL;
        j.dest = "TF@i";
        print_operation(&j);

        j.operation = I_SUB;
        j.op1 = "TF@i";
        j.op2 = "int@1";
        j.dest = "TF@i";
        print_operation(&j);

        j.operation = I_DEFVAR;
        j.op1 = NULL;
        j.op2 = NULL;
        j.dest = "TF@n";
        print_operation(&j);

        j.operation = I_POPS;
        j.op1 = NULL;
        j.op2 = NULL;
        j.dest = "TF@n";
        print_operation(&j);

//        j.operation = I_MOVE;
//        j.op1 = "str@";
//        j.op2 = NULL;
//        j.dest = "TF@%RETVAL";
//        print_operation(&j);

        //todo definovat f substr(s as string, a as integer, b as integer) as string
        char *ret_o = gen_label("return0");
        char *n_ok = gen_label("back");

        printf("DEFVAR TF@tlac\n"
                       "MOVE TF@tlac string@\n"
                       "\n"
                       "DEFVAR TF@temp\n"
                       "\n"
                       "DEFVAR TF@len\n"
                       "STRLEN TF@len TF@s\n"
                       "SUB TF@len TF@len TF@i\n"
                       "\n"
                       "LT TF@temp TF@i int@0\n"
                       "JUMPIFEQ l_return0_0 TF@temp bool@true\n"
                       "\n"
                       "LT TF@temp TF@len int@1\n"
                       "JUMPIFEQ l_return0_0 TF@temp bool@true\n"
                       "\n"
                       "\n"
                       "GT TF@temp TF@n int@-1\n"
                       "PUSHS TF@temp\n"
                       "LT TF@temp TF@n TF@len\n"
                       "PUSHS TF@temp\n"
                       "EQ TF@temp TF@n TF@len\n"
                       "PUSHS TF@temp\n"
                       "\n"
                       "ORS\n"
                       "ANDS\n"
                       "POPS TF@temp\n"
                       "\n"
                       "\n"
                       "JUMPIFEQ l_back_1 TF@temp bool@true\n"
                       "\n"
                       "MOVE TF@n TF@len\n"
                       "\n"
                       "LABEL l_back_1\n"
                       "\n"
                       "\n"
                       "LABEL $substr_while\n"
                       "GT TF@temp TF@n int@0\n"
                       "JUMPIFEQ $substr_end_while TF@temp bool@false\n"
                       "GETCHAR TF@temp TF@s TF@i\n"
                       "\n"
                       "ADD TF@i TF@i int@1\n"
                       "SUB TF@n TF@n int@1\n"
                       "\n"
                       "CONCAT TF@tlac  TF@tlac TF@temp\n"
                       "JUMP $substr_while\n"
                       "\n"
                       "LABEL $substr_end_while\n"
                       "LABEL l_return0_0\n"
                       "\n"
                       "\n"
                       "\n"
                       "MOVE TF@%RETVAL TF@tlac\n");


//        j.operation = I_DEFVAR;
//        j.op1 = NULL;
//        j.op2 = NULL;
//        j.dest = "TF@temp";
//        print_operation(&j);
//
//        char *label = gen_label("length");
//        char *back = gen_label("back");
//
//        j.operation = I_LT;
//        j.op1 = "TF@i";
//        j.op2 = "int@0";
//        j.dest = "TF@temp";
//        print_operation(&j);
//
//        j.operation = I_JUMPIFEQ;
//        j.op1 = "TF@temp";
//        j.op2 = "boolean@TRUE";
//        j.dest = label;
//        print_operation(&j);
//
//        j.operation = I_STRLEN;
//        j.op1 = "TF@s";
//        j.op2 = NULL;
//        j.dest = "TF@temp";
//        print_operation(&j);
//
//        j.operation = I_PUSHS;
//        j.op2 = NULL;
//        j.op1 = NULL;
//        j.dest = "TF@temp";
//        print_operation(&j);
//
//        j.operation = I_SUB;
//        j.op1 = "TF@temp";
//        j.op2 = "TF@i";
//        j.dest = "TF@temp";
//        print_operation(&j);
//
//        char *change_s = gen_label("change_s");
//        j.operation = I_GT;
//        j.op2 = "TF@temp";
//        j.op1 = "TF@n";
//        j.dest = "TF@temp";
//        print_operation(&j);
//
//        j.operation = I_JUMPIFEQ;
//        j.op1 = "TF@temp";
//        j.op2 = "boolean@TRUE";
//        j.dest = change_s;
//        print_operation(&j);
//
//        j.operation = I_POPS;
//        j.op2 = NULL;
//        j.op1 = NULL;
//        j.dest = "TF@n";
//        print_operation(&j);
//
//        j.operation = I_LABEL;
//        j.op1 = NULL;
//        j.op2 = NULL;
//        j.dest = change_s;
//        print_operation(&j);
//
//        //
//
//        j.operation = I_LABEL;
//        j.op1 = NULL;
//        j.op2 = NULL;
//        j.dest = back;
//        print_operation(&j);
//
//        j.operation = I_JUMPIFEQ;
//        j.op1 = "TF@n";
//        j.op2 = "int@0";
//        j.dest = label;
//        print_operation(&j);
//
//        j.operation = I_GETCHAR;
//        j.op1 = "TF@s";
//        j.op2 = "TF@i";
//        j.dest = "TF@temp";
//        print_operation(&j);
//
//        j.operation = I_ADD;
//        j.op1 = "int@1";
//        j.op2 = "TF@i";
//        j.dest = "TF@i";
//        print_operation(&j);
//
//        j.operation = I_SUB;
//        j.op1 = "TF@n";
//        j.op2 = "int@1";
//        j.dest = "TF@n";
//        print_operation(&j);
//
//        j.operation = I_CONCAT;
//        j.op2 = "TF@temp";
//        j.op1 = "TF@%RETVAL";
//        j.dest = "TF@%RETVAL";
//        print_operation(&j);
//
//        j.operation = I_JUMP;
//        j.op1 = NULL;
//        j.op2 = NULL;
//        j.dest = back;
//        print_operation(&j);
//
//        j.operation = I_LABEL;
//        j.op1 = NULL;
//        j.op2 = NULL;
//        j.dest = label;
//        print_operation(&j);
//
//
//    //konec def





        j.operation = I_RETURN;
        j.op1 = NULL;
        j.op2 = NULL;
        j.dest = NULL;
        print_operation(&j);
    }
}

void define_asc(){
    if (used_asc){
        t_3ac j;

        j.operation = I_LABEL;
        j.op1 = NULL;
        j.op2 = NULL;
        j.dest = "substr";
        print_operation(&j);

        j.operation = I_DEFVAR;
        j.op1 = NULL;
        j.op2 = NULL;
        j.dest = "TF@s";
        print_operation(&j);

        j.operation = I_POPS;
        j.op1 = NULL;
        j.op2 = NULL;
        j.dest = "TF@s";
        print_operation(&j);

        j.operation = I_DEFVAR;
        j.op1 = NULL;
        j.op2 = NULL;
        j.dest = "TF@i";
        print_operation(&j);

        j.operation = I_POPS;
        j.op1 = NULL;
        j.op2 = NULL;
        j.dest = "TF@i";
        print_operation(&j);

        //todo def f asc(...
        //konec def
        j.operation = I_DEFVAR;
        j.op1 = NULL;
        j.op2 = NULL;
        j.dest = "TF@temp";
        print_operation(&j);

        j.operation = I_STRLEN;
        j.op1 = "TF@s";
        j.op2 = NULL;
        j.dest = "TF@temp";
        print_operation(&j);

        j.operation = I_SUB;
        j.op1 = "TF@temp";
        j.op2 = NULL;
        j.dest = "TF@i";
        print_operation(&j);

        char *ret_0 = gen_label("change_s");
        j.operation = I_LT;
        j.op2 = "TF@temp";
        j.op1 = "TF@0";
        j.dest = "TF@temp";
        print_operation(&j);

        j.operation = I_PUSHS;
        j.op1 = NULL;
        j.op2 = NULL;
        j.dest = "TF@temp";
        print_operation(&j);

        j.operation = I_LT;
        j.op2 = "TF@i";
        j.op1 = "TF@1";
        j.dest = "TF@temp";
        print_operation(&j);

        j.operation = I_PUSHS;
        j.op1 = NULL;
        j.op2 = NULL;
        j.dest = "TF@temp";
        print_operation(&j);

        j.operation = I_ORS;
        j.op1 = NULL;
        j.op2 = NULL;
        j.dest = NULL;
        print_operation(&j);

        j.operation = I_POPS;
        j.op1 = NULL;
        j.op2 = NULL;
        j.dest = "TF@temp";
        print_operation(&j);

        j.operation = I_JUMPIFEQ;
        j.op1 = "TF@temp";
        j.op2 = "boolean@TRUE";
        j.dest = ret_0;
        print_operation(&j);

        j.operation = I_GETCHAR;
        j.op1 = "TF@s";
        j.op2 = "TF@i";
        j.dest = "TF@temp";
        print_operation(&j);

        j.operation = I_STRI2INT;
        j.op1 = "TF@s";
        j.op2 = "TF@i";
        j.dest = "TF@%RETVAL";
        print_operation(&j);


        j.operation = I_LABEL;
        j.op1 = NULL;
        j.op2 = NULL;
        j.dest = ret_0;
        print_operation(&j);


        j.operation = I_RETURN;
        j.op1 = NULL;
        j.op2 = NULL;
        j.dest = NULL;
        print_operation(&j);
    }
}

void define_chr(){
    if (used_chr){
        t_3ac j;

        j.operation = I_LABEL;
        j.op1 = NULL;
        j.op2 = NULL;
        j.dest = "chr";
        print_operation(&j);

        j.operation = I_DEFVAR;
        j.op1 = NULL;
        j.op2 = NULL;
        j.dest = "TF@s";
        print_operation(&j);

        j.operation = I_POPS;
        j.op1 = NULL;
        j.op2 = NULL;
        j.dest = "TF@s";
        print_operation(&j);

        j.operation = I_INT2CHAR;
        j.op1 = "TF@s";
        j.op2 = NULL;
        j.dest = "TF@%RETVAL";
        print_operation(&j);

        j.operation = I_RETURN;
        j.op1 = NULL;
        j.op2 = NULL;
        j.dest = NULL;
        print_operation(&j);
    }
}




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

    define_length();
    define_substr();
    define_asc();
    define_chr();

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
    char *tmp = my_strcpy(dest);
    tmp[0] = 'L';
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
    if (operation != E_PLUS){
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
        char *tmp = gen_temp_var();
        point_swap(&tmp, &dest);


        if (l_typ == k_integer && r_typ == k_double) {
            create_3ac(I_FLOAT2R2EINT, r_op_str, NULL, tmp);
            create_3ac(I_ADD, l_op_str, tmp, dest);
        } else if (l_typ == k_double && r_typ == k_integer) {
            create_3ac(I_INT2FLOAT, r_op_str, NULL, tmp);
            create_3ac(I_ADD, l_op_str, r_op_str, dest);
        } else {    //je tam string
            bad_operands_err(line);
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
        char *tmp = gen_temp_var();
        point_swap(&tmp, &dest);

        if (l_typ == k_integer && r_typ == k_double) {
            create_3ac(I_FLOAT2R2EINT, r_op_str, NULL, tmp);
            create_3ac(operation, l_op_str, tmp, dest);
        } else if (l_typ == k_double && r_typ == k_integer) {
            create_3ac(I_INT2FLOAT, r_op_str, NULL, tmp);
            create_3ac(operation, l_op_str, tmp, dest);
        } else {
            bad_operands_err(line);
        }
    }
    return dest;
}

char *op_div(int operation, Element *l_operand, Element *r_operand){
    if (operation != E_DIV){
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
        char *tmp = gen_temp_var();
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

char *op_mod(int operation, Element *l_operand, Element *r_operand){
    if (operation != E_MOD){
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

char *op_lt_gt_eq(int operation, Element *l_operand, Element *r_operand){
    if (operation == E_LT){
        operation = I_LT;
    } else if (operation == E_GT){
        operation = I_GT;
    } else if (operation == E_EQ){
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
        char *tmp = gen_temp_var();
        point_swap(&tmp, &dest);

        if (l_typ == k_integer && r_typ == k_double) {
            create_3ac(I_FLOAT2R2EINT, r_op_str, NULL, tmp);
            create_3ac(operation, l_op_str, tmp, dest);
        } else if (l_typ == k_double && r_typ == k_integer) {
            create_3ac(I_INT2FLOAT, r_op_str, NULL, tmp);
            create_3ac(operation, l_op_str, tmp, dest);
        } else {
            bad_operands_err(line);
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
            bad_operands_err(line);
        }
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
            internall_err(__LINE__);
    }

    return dest;
}