#include <stdio.h>
#include "parser.h"
#include "err.h"
#include "memwork.h"
#include "symtable.h"
#include "scanner.h"
#include "expression.h"
#include "symtable.h"
#include "codegen.h"
#include <string.h>
#include <ctype.h>

char *my_strcpy(char *src) {
    char *dest;
    if (src == NULL){
        dest = NULL;
    } else {
        size_t size = strlen(src) + sizeof(char);
        dest = my_malloc(size);
        int i;
        for (i = 0; src[i] != 0; ++i) {
            dest[i] = src[i];
        }
        dest[i] = 0;

    }
    return dest;
}

int get_id() {
    static int id = 0;
    return id++;
}

void Stack_init(Stack * stack){
    stack->top = NULL;
    stack->active = NULL;
}

//smaze zasobnik, uvolni pamet
int Stack_dispose(Stack * stack){
    check_pointer(stack);
    Element * tmp;

    while(stack->top != NULL){
        tmp = stack->top;
        stack->top = tmp->next;
        my_free(tmp);
    }
    return SUCCESS;
}

//vlozi na vrchol zasobniku element s type
int Stack_push(Stack * stack, int type, char *operand, int t_dat){
    Element * new = my_malloc(sizeof(Element));
    check_pointer(new);

    new->type = type;
    new->next = stack->top;
    new->operand = my_strcpy(operand);
    new->typ_konkretne = t_dat;

    stack->top = new;

    if(new->type != E_E){ //isTerminal
        stack->active = new;
    }
    return SUCCESS;

}
//po zavolani tehle funkce je potreba uvolnit ten ukazatel
Element * Stack_pop(Stack * stack){
    Element * tmp = stack->top;
    stack->top = tmp->next;
    if(stack->active == tmp){ //pokud byl prvni aktivni, posuneme aktivitu na dalsi neterminal
        while(stack->active->type == E_E){
            stack->active = stack->active->next;
        }
    }
    return tmp;
}

Element* Stack_top(Stack *stack){
    Element *tmp = stack->top;
    while(tmp->type == E_E){
        tmp = tmp->next;
    }
    return tmp;
}

//nad nejvrchnejsi terminal (stack.active) vlozi novy element s "<"
//implementace - vlozi a prekopiruje aktivni pod aktivni a vrchni premaze na <
int Stack_expand(Stack *stack){
    Element *tmp = stack->top;
    while(tmp->type == E_E){
        tmp = tmp->next;
    }

    Element * new = my_malloc(sizeof(Element));
    check_pointer(new);

    new->type = tmp->type;
    new->next = tmp->next;
    tmp->next = new;
    tmp->type = E_LT;

    return SUCCESS;
}

Element* check_next_element_type(int type, Stack* stack){
    Element * input = Stack_pop(stack);
    check_pointer(input);
    if(input->type == type){
        return input;
    }
    error(ERR_SYNTA);
}

/*cte elementy zeshora zasobniku dokud nenajde "<", musi overit, jestli je precteny retezec pravidlo
 * pokud je pravidlo, vymeni pravidlo z vrcholu zasobniku za jeho levou stranu
 * priklad: pravidlo: E -> E + E
 *          zasobnik:  $E+id+<E+E
 *          vysledek na zasobniku: $E+id+E  */
int rule(Stack *stack, TTable *local){
    Element * input = Stack_pop(stack);
    check_pointer(input);
    int new_id = get_id();
    char *dest = my_malloc(sizeof(char) * 130);


    Element *tmp2 = input;
    Element *tmp1 = NULL;

    int typ1;
    int typ2;

    char tmp[130];

    Element *arr_el[100] = {NULL, };


    switch(input->type){
        case E_E:
            input = Stack_pop(stack);
            switch(input->type){
                case E_PLUS:
                    tmp1 = check_next_element_type(E_E, stack);
                    check_next_element_type(E_LT, stack);

                    /*
                     * SEMANTIKA A GEN KODU PRE A + B
                     */
                    typ1 = tmp1->typ_konkretne;
                    typ2 = tmp2->typ_konkretne;

                    if (typ1 == k_string && typ2 == k_string) {
                        sprintf(dest, "$E_E%i", new_id);
                        create_3ac(I_DEFVAR, NULL, NULL, dest);
                        create_3ac(I_CONCAT, tmp1->operand, tmp2->operand, dest);
                    } else if ((typ1 == k_integer && typ2 == k_integer) || (typ1 == k_double && typ2 == k_double)) {
                        sprintf(dest, "$E_E%i", new_id);
                        create_3ac(I_DEFVAR, NULL, NULL, dest);
                        create_3ac(I_ADD, tmp1->operand, tmp2->operand, dest);
                    } else if (typ1 == k_integer && typ2 == k_double) {
                        sprintf(tmp, "$E_E%i", get_id());
                        sprintf(dest, "$E_E%i", new_id);
                        create_3ac(I_DEFVAR, NULL, NULL, dest);
                        create_3ac(I_DEFVAR, NULL, NULL, tmp);
                        create_3ac(I_FLOAT2R2EINT, tmp2->operand, NULL, tmp);
                        create_3ac(I_ADD, tmp1->operand, tmp, dest);
                    } else if (typ1 == k_double && typ2 == k_integer) {
                        sprintf(tmp, "$E_E%i", get_id());
                        sprintf(dest, "$E_E%i", new_id);
                        create_3ac(I_DEFVAR, NULL, NULL, dest);
                        create_3ac(I_DEFVAR, NULL, NULL, tmp);
                        create_3ac(I_INT2FLOAT, tmp2->operand, NULL, tmp);
                        create_3ac(I_ADD, tmp1->operand, tmp, dest);
                    } else {
                        clear_all();
                        exit(ERR_SEM_T);
                    }

                    //todo semanticka konrola ci tie 2 prvky su upravitelne
                    //gen vnutorneho kodu


                    Stack_push(stack, E_E, dest, typ1);
                    return 1;
                case E_MINUS:
                    tmp1 = check_next_element_type(E_E, stack);
                    check_next_element_type(E_LT, stack);

                    /*
                     * SEMANTIKA A GEN KODU PRE A - B
                     */
                    typ1 = tmp1->typ_konkretne;
                    typ2 = tmp2->typ_konkretne;
                    //todo semanticka konrola ci tie 2 prvky su upravitelne

                    if ((typ1 == k_integer && typ2 == k_integer) || (typ1 == k_double && typ2 == k_double)) {
                        sprintf(dest, "$E_E%i", new_id);
                        create_3ac(I_DEFVAR, NULL, NULL, dest);
                        create_3ac(I_SUB, tmp1->operand, tmp2->operand, dest);
                    } else if (typ1 == k_integer && typ2 == k_double) {
                        sprintf(tmp, "$E_E%i", get_id());
                        sprintf(dest, "$E_E%i", new_id);

                        create_3ac(I_DEFVAR, NULL, NULL, dest);
                        create_3ac(I_DEFVAR, NULL, NULL, tmp);

                        create_3ac(I_FLOAT2R2EINT, tmp2->operand, NULL, tmp);
                        create_3ac(I_SUB, tmp1->operand, tmp, dest);

                    } else if (typ1 == k_double && typ2 == k_integer) {
                        sprintf(tmp, "$E_E%i", get_id());
                        sprintf(dest, "$E_E%i", new_id);
                        create_3ac(I_DEFVAR, NULL, NULL, dest);
                        create_3ac(I_DEFVAR, NULL, NULL, tmp);

                        create_3ac(I_INT2FLOAT, tmp2->operand, NULL, tmp); //deklarovanie operandu
                        create_3ac(I_SUB, tmp1->operand, tmp, dest);
                    } else {
                        clear_all();
                        exit(ERR_SEM_T);
                    }

                    Stack_push(stack, E_E, dest, typ1);
                    return 2;
                case E_MUL:
                    tmp1 = check_next_element_type(E_E, stack);
                    check_next_element_type(E_LT, stack);

                    /*
                     * SEMANTIKA A GEN KODU PRE A * B
                     */

                    typ1 = tmp1->typ_konkretne;
                    typ2 = tmp2->typ_konkretne;
                    if ((typ1 == k_integer && typ2 == k_integer) || (typ1 == k_double && typ2 == k_double)) {
                        sprintf(dest, "$E_E%i", new_id);
                        create_3ac(I_DEFVAR, NULL, NULL, dest);
                        create_3ac(I_MUL, tmp1->operand, tmp2->operand, dest);
                    } else if (typ1 == k_integer && typ2 == k_double) {
                        sprintf(tmp, "$E_E%i", get_id());
                        sprintf(tmp, "$E_E%i", new_id);

                        create_3ac(I_DEFVAR, NULL, NULL, dest);
                        create_3ac(I_DEFVAR, NULL, NULL, tmp);

                        create_3ac(I_FLOAT2R2EINT, tmp2->operand, NULL, tmp);
                        create_3ac(I_MUL, tmp1->operand, tmp, dest);

                    } else if (typ1 == k_double && typ2 == k_integer) {
                        sprintf(tmp, "$E_E%i", get_id());
                        sprintf(tmp, "$E_E%i", new_id);

                        create_3ac(I_DEFVAR, NULL, NULL, dest);
                        create_3ac(I_DEFVAR, NULL, NULL, tmp);

                        create_3ac(I_INT2FLOAT, tmp2->operand, NULL, tmp);
                        create_3ac(I_MUL, tmp1->operand, tmp, dest);
                    } else {
                        clear_all();
                        exit(ERR_SEM_T);
                    }



                    Stack_push(stack, E_E, dest, typ1);
                    return 3;
                case E_DIV:
                    tmp1 = check_next_element_type(E_E, stack);
                    check_next_element_type(E_LT, stack);


                    /*
                     * SEMANTIKA A GEN KODU PRE A / B
                     */
                    typ1 = tmp1->typ_konkretne;
                    typ2 = tmp2->typ_konkretne;
                    //todo semanticka konrola ci tie 2 prvky su upravitelne

                    if (typ1 == k_integer && typ2 == k_integer) {
                        sprintf(dest, "$E_E%i", new_id);
                        create_3ac(I_DEFVAR, NULL, NULL, dest);
                        create_3ac(I_INT2FLOAT, tmp1->operand, NULL, dest);
                        create_3ac(I_PUSHS, NULL, NULL, dest);
                        create_3ac(I_INT2FLOAT, tmp2->operand, NULL, dest);
                        create_3ac(I_PUSHS, NULL, NULL, dest);
                        create_3ac(I_DIVS, NULL, NULL, NULL);
                        create_3ac(I_POPS, NULL, NULL, dest);

                    } else if (typ1 == k_double && typ2 == k_double) {
                        sprintf(dest, "$E_E%i", new_id);
                        create_3ac(I_DEFVAR, NULL, NULL, dest);
                        create_3ac(I_DIV, tmp1->operand, tmp2->operand, dest);
                    } else if (typ1 == k_integer && typ2 == k_double) {
                        sprintf(tmp, "$E_E%i", get_id());
                        sprintf(dest, "$E_E%i", new_id);

                        create_3ac(I_DEFVAR, NULL, NULL, dest);
                        create_3ac(I_DEFVAR, NULL, NULL, tmp);

                        create_3ac(I_INT2FLOAT, tmp1->operand, NULL, tmp);
                        create_3ac(I_DIV, tmp, tmp2->operand, dest);

                    } else if (typ1 == k_double && typ2 == k_integer) {
                        sprintf(tmp, "$E_E%i", get_id());
                        sprintf(dest, "$E_E%i", new_id);
                        create_3ac(I_DEFVAR, NULL, NULL, dest);
                        create_3ac(I_DEFVAR, NULL, NULL, tmp);

                        create_3ac(I_INT2FLOAT, tmp2->operand, NULL, tmp);
                        create_3ac(I_DIV, tmp1->operand, tmp, dest);
                    } else {
                        clear_all();
                        exit(ERR_SEM_T);
                    }

                    Stack_push(stack, E_E, dest, k_double);
                    return 4;
                case E_MOD:
                    tmp1 = check_next_element_type(E_E, stack);
                    check_next_element_type(E_LT, stack);

                    typ1 = tmp1->typ_konkretne;
                    typ2 = tmp2->typ_konkretne;
                    if (typ1 == k_integer && typ2 == k_integer) {
                        sprintf(dest, "$E_E%i", new_id);
                        create_3ac(I_DEFVAR, NULL, NULL, dest);
                        create_3ac(I_INT2FLOAT, tmp1->operand, NULL, dest);
                        create_3ac(I_PUSHS, NULL, NULL, dest);
                        create_3ac(I_INT2FLOAT, tmp2->operand, NULL, dest);
                        create_3ac(I_PUSHS, NULL, NULL, dest);
                        create_3ac(I_DIVS, NULL, NULL, NULL);
                        create_3ac(I_POPS, NULL, NULL, dest);

                    } else if (typ1 == k_double && typ2 == k_double) {
                        sprintf(dest, "$E_E%i", new_id);
                        create_3ac(I_DEFVAR, NULL, NULL, dest);
                        create_3ac(I_DIV, tmp1->operand, tmp2->operand, dest);
                    } else if (typ1 == k_integer && typ2 == k_double) {
                        sprintf(tmp, "$E_E%i", get_id());
                        sprintf(dest, "$E_E%i", new_id);

                        create_3ac(I_DEFVAR, NULL, NULL, dest);
                        create_3ac(I_DEFVAR, NULL, NULL, tmp);

                        create_3ac(I_INT2FLOAT, tmp1->operand, NULL, tmp);
                        create_3ac(I_DIV, tmp, tmp2->operand, dest);

                    } else if (typ1 == k_double && typ2 == k_integer) {
                        sprintf(tmp, "$E_E%i", get_id());
                        sprintf(dest, "$E_E%i", new_id);
                        create_3ac(I_DEFVAR, NULL, NULL, dest);
                        create_3ac(I_DEFVAR, NULL, NULL, tmp);

                        create_3ac(I_INT2FLOAT, tmp2->operand, NULL, tmp);
                        create_3ac(I_DIV, tmp1->operand, tmp, dest);
                    } else {
                        clear_all();
                        exit(ERR_SEM_T);
                    }
                    create_3ac(I_PUSHS, NULL, NULL, dest);
                    //zaokruhlenie
                    create_3ac(I_FLOAT2R2EINT, dest, NULL, dest);
                    create_3ac(I_INT2FLOAT, dest, NULL, dest);

                    create_3ac(I_PUSHS, NULL, NULL, dest);
                    create_3ac(I_SUBS, NULL, NULL, NULL);
                    if (typ2 == k_integer) {
                        sprintf(tmp, "$E_E%i", get_id());
                        create_3ac(I_DEFVAR, NULL, NULL, tmp);
                        create_3ac(I_INT2FLOAT, tmp2->operand, NULL, tmp);
                        create_3ac(I_PUSHS, NULL, NULL, NULL);
                    } else {
                        create_3ac(I_PUSHS, NULL, NULL, NULL);
                    }
                    create_3ac(I_MULS, NULL, NULL, NULL);
                    create_3ac(I_POPS, NULL, NULL, tmp);
                    create_3ac(I_FLOAT2R2EINT, tmp, NULL, dest);

                    Stack_push(stack, E_E, dest, k_integer);
                    return 5;
                case E_LT:
                    tmp1 = check_next_element_type(E_E, stack);
                    check_next_element_type(E_LT, stack);

                    typ1 = tmp1->typ_konkretne;
                    typ2 = tmp2->typ_konkretne;

                    if ((typ1 == k_integer && typ2 == k_integer) || (typ1 == k_double && typ2 == k_double) ||
                        (typ1 == k_string && typ2 == k_string)) {
                        sprintf(dest, "$E_E%i", new_id);
                        create_3ac(I_DEFVAR, NULL, NULL, dest);
                        create_3ac(I_LT, tmp1->operand, tmp2->operand, dest);
                    } else if (typ1 == k_integer && typ2 == k_double) {
                        sprintf(tmp, "$E_E%i", get_id());
                        sprintf(dest, "$E_E%i", new_id);

                        create_3ac(I_DEFVAR, NULL, NULL, dest);
                        create_3ac(I_DEFVAR, NULL, NULL, tmp);

                        create_3ac(I_FLOAT2R2EINT, tmp2->operand, NULL, tmp);
                        create_3ac(I_LT, tmp1->operand, tmp, dest);

                    } else if (typ1 == k_double && typ2 == k_integer) {
                        sprintf(tmp, "$E_E%i", get_id());
                        sprintf(dest, "$E_E%i", new_id);
                        create_3ac(I_DEFVAR, NULL, NULL, dest);
                        create_3ac(I_DEFVAR, NULL, NULL, tmp);


                        create_3ac(I_INT2FLOAT, tmp2->operand, NULL, tmp);
                        create_3ac(I_LT, tmp1->operand, tmp, dest);
                    } else {
                        clear_all();
                        exit(ERR_SEM_T);
                    }

                    Stack_push(stack, E_E, dest, k_boolean);
                    return 6;
                case E_LE:
                    tmp1 = check_next_element_type(E_E, stack);
                    check_next_element_type(E_LT, stack);
                    typ1 = tmp1->typ_konkretne;
                    typ2 = tmp2->typ_konkretne;
                    //todo semanticka konrola ci tie 2 prvky su upravitelne

                    if ((typ1 == k_integer && typ2 == k_integer) || (typ1 == k_double && typ2 == k_double) ||
                        (typ1 == k_string && typ2 == k_string)) {
                        sprintf(dest, "$E_E%i", new_id);
                        create_3ac(I_DEFVAR, NULL, NULL, dest);
                        create_3ac(I_LT, tmp1->operand, tmp2->operand, dest);
                        create_3ac(I_PUSHS, NULL, NULL, dest);
                        create_3ac(I_EQ, tmp1->operand, tmp2->operand, dest);
                        create_3ac(I_PUSHS, NULL, NULL, dest);
                        create_3ac(I_ORS, NULL, NULL, NULL);
                        create_3ac(I_POPS, NULL, NULL, dest);
                    } else if (typ1 == k_integer && typ2 == k_double) {
                        sprintf(tmp, "$E_E%i", get_id());
                        sprintf(dest, "$E_E%i", new_id);

                        create_3ac(I_DEFVAR, NULL, NULL, tmp);
                        create_3ac(I_DEFVAR, NULL, NULL, dest);


                        create_3ac(I_FLOAT2R2EINT, tmp2->operand, NULL, tmp);

                        create_3ac(I_LT, tmp1->operand, tmp, dest);
                        create_3ac(I_PUSHS, NULL, NULL, dest);
                        create_3ac(I_EQ, tmp1->operand, tmp, dest);
                        create_3ac(I_PUSHS, NULL, NULL, dest);
                        create_3ac(I_ORS, NULL, NULL, NULL);
                        create_3ac(I_POPS, NULL, NULL, dest);

                    } else if (typ1 == k_double && typ2 == k_integer) {
                        sprintf(tmp, "$E_E%i", get_id());
                        sprintf(dest, "$E_E%i", new_id);

                        create_3ac(I_DEFVAR, NULL, NULL, tmp);
                        create_3ac(I_DEFVAR, NULL, NULL, dest);


                        create_3ac(I_FLOAT2R2EINT, tmp2->operand, NULL, tmp);

                        create_3ac(I_LT, tmp1->operand, tmp, dest);
                        create_3ac(I_PUSHS, NULL, NULL, dest);
                        create_3ac(I_EQ, tmp1->operand, tmp, dest);
                        create_3ac(I_PUSHS, NULL, NULL, dest);
                        create_3ac(I_ORS, NULL, NULL, NULL);
                        create_3ac(I_POPS, NULL, NULL, dest);
                    } else {
                        clear_all();
                        exit(ERR_SEM_T);
                    }

                    Stack_push(stack, E_E, dest, k_boolean);
                    return 7;
                case E_GT:
                    tmp1 = check_next_element_type(E_E, stack);
                    check_next_element_type(E_LT, stack);

                    typ1 = tmp1->typ_konkretne;
                    typ2 = tmp2->typ_konkretne;


                    if ((typ1 == k_integer && typ2 == k_integer) || (typ1 == k_double && typ2 == k_double) ||
                        (typ1 == k_string && typ2 == k_string)) {
                        sprintf(dest, "$E_E%i", new_id);
                        create_3ac(I_DEFVAR, NULL, NULL, dest);
                        create_3ac(I_GT, tmp1->operand, tmp2->operand, dest);
                    } else if (typ1 == k_integer && typ2 == k_double) {
                        sprintf(tmp, "$E_E%i", get_id());
                        sprintf(dest, "$E_E%i", new_id);

                        create_3ac(I_DEFVAR, NULL, NULL, dest);
                        create_3ac(I_DEFVAR, NULL, NULL, tmp);

                        create_3ac(I_FLOAT2R2EINT, tmp2->operand, NULL, tmp);
                        create_3ac(I_GT, tmp1->operand, tmp, dest);

                    } else if (typ1 == k_double && typ2 == k_integer) {
                        sprintf(tmp, "$E_E%i", get_id());
                        sprintf(dest, "$E_E%i", new_id);
                        create_3ac(I_DEFVAR, NULL, NULL, dest);
                        create_3ac(I_DEFVAR, NULL, NULL, tmp);

                        create_3ac(I_INT2FLOAT, tmp2->operand, NULL, tmp);
                        create_3ac(I_GT, tmp1->operand, tmp, dest);
                    } else {
                        clear_all();
                        exit(ERR_SEM_T);
                    }

                    Stack_push(stack, E_E, dest, k_boolean);
                    return 8;
                case E_GE:
                    tmp1 = check_next_element_type(E_E, stack);
                    check_next_element_type(E_LT, stack);

                    typ1 = tmp1->typ_konkretne;
                    typ2 = tmp2->typ_konkretne;


                    if ((typ1 == k_integer && typ2 == k_integer) || (typ1 == k_double && typ2 == k_double) ||
                        (typ1 == k_string && typ2 == k_string)) {
                        sprintf(dest, "$E_E%i", new_id);
                        create_3ac(I_DEFVAR, NULL, NULL, dest);
                        create_3ac(I_GT, tmp1->operand, tmp2->operand, dest);
                        create_3ac(I_PUSHS, NULL, NULL, dest);
                        create_3ac(I_EQ, tmp1->operand, tmp2->operand, dest);
                        create_3ac(I_PUSHS, NULL, NULL, dest);
                        create_3ac(I_ORS, NULL, NULL, NULL);
                        create_3ac(I_POPS, NULL, NULL, dest);
                    } else if (typ1 == k_integer && typ2 == k_double) {
                        sprintf(tmp, "$E_E%i", get_id());
                        sprintf(dest, "$E_E%i", new_id);

                        create_3ac(I_DEFVAR, NULL, NULL, dest);
                        create_3ac(I_DEFVAR, NULL, NULL, tmp);

                        create_3ac(I_FLOAT2R2EINT, tmp2->operand, NULL, tmp);


                        create_3ac(I_GT, tmp1->operand, tmp, dest);
                        create_3ac(I_PUSHS, NULL, NULL, dest);
                        create_3ac(I_EQ, tmp1->operand, tmp, dest);
                        create_3ac(I_PUSHS, NULL, NULL, dest);
                        create_3ac(I_ORS, NULL, NULL, NULL);
                        create_3ac(I_POPS, NULL, NULL, dest);

                    } else if (typ1 == k_double && typ2 == k_integer) {

                        sprintf(tmp, "$E_E%i", get_id());
                        sprintf(dest, "$E_E%i", new_id);

                        create_3ac(I_DEFVAR, NULL, NULL, tmp);
                        create_3ac(I_DEFVAR, NULL, NULL, dest);

                        create_3ac(I_FLOAT2R2EINT, tmp2->operand, NULL, tmp);

                        create_3ac(I_GT, tmp1->operand, tmp, dest);
                        create_3ac(I_PUSHS, NULL, NULL, dest);
                        create_3ac(I_EQ, tmp1->operand, tmp, dest);
                        create_3ac(I_PUSHS, NULL, NULL, dest);
                        create_3ac(I_ORS, NULL, NULL, NULL);
                        create_3ac(I_POPS, NULL, NULL, dest);
                    } else {
                        clear_all();
                        exit(ERR_SEM_T);
                    }

                    Stack_push(stack, E_E, dest, k_boolean);
                    return 9;
                case E_EQ:
                    tmp1 = check_next_element_type(E_E, stack);
                    check_next_element_type(E_LT, stack);

                    typ1 = tmp1->typ_konkretne;
                    typ2 = tmp2->typ_konkretne;


                    if ((typ1 == k_integer && typ2 == k_integer) || (typ1 == k_double && typ2 == k_double) ||
                        (typ1 == k_string && typ2 == k_string)) {
                        sprintf(dest, "$E_E%i", new_id);
                        create_3ac(I_DEFVAR, NULL, NULL, dest);
                        create_3ac(I_EQ, tmp1->operand, tmp2->operand, dest);
                    } else if (typ1 == k_integer && typ2 == k_double) {
                        sprintf(tmp, "$E_E%i", get_id());
                        sprintf(dest, "$E_E%i", new_id);

                        create_3ac(I_DEFVAR, NULL, NULL, dest);
                        create_3ac(I_DEFVAR, NULL, NULL, tmp);

                        create_3ac(I_FLOAT2R2EINT, tmp2->operand, NULL, tmp);
                        create_3ac(I_EQ, tmp1->operand, tmp, dest);

                    } else if (typ1 == k_double && typ2 == k_integer) {
                        sprintf(tmp, "$E_E%i", get_id());
                        sprintf(dest, "$E_E%i", new_id);
                        create_3ac(I_DEFVAR, NULL, NULL, dest);
                        create_3ac(I_DEFVAR, NULL, NULL, tmp);

                        create_3ac(I_INT2FLOAT, tmp2->operand, NULL, tmp);
                        create_3ac(I_EQ, tmp1->operand, tmp, dest);
                    } else {
                        clear_all();
                        exit(ERR_SEM_T);
                    }

                    Stack_push(stack, E_E, dest, k_boolean);
                    return 10;
                case E_NEQ:
                    tmp1 = check_next_element_type(E_E, stack);
                    check_next_element_type(E_LT, stack);

                    typ1 = tmp1->typ_konkretne;
                    typ2 = tmp2->typ_konkretne;


                    if ((typ1 == k_integer && typ2 == k_integer) || (typ1 == k_double && typ2 == k_double) ||
                        (typ1 == k_string && typ2 == k_string)) {
                        sprintf(dest, "$E_E%i", new_id);
                        create_3ac(I_DEFVAR, NULL, NULL, dest);
                        create_3ac(I_EQ, tmp1->operand, tmp2->operand, dest);
                    } else if (typ1 == k_integer && typ2 == k_double) {
                        sprintf(tmp, "$E_E%i", get_id());
                        sprintf(dest, "$E_E%i", new_id);

                        create_3ac(I_DEFVAR, NULL, NULL, dest);
                        create_3ac(I_DEFVAR, NULL, NULL, tmp);

                        create_3ac(I_FLOAT2R2EINT, tmp2->operand, NULL, tmp);
                        create_3ac(I_EQ, tmp1->operand, tmp, dest);

                    } else if (typ1 == k_double && typ2 == k_integer) {
                        sprintf(tmp, "$E_E%i", get_id());
                        sprintf(dest, "$E_E%i", new_id);
                        create_3ac(I_DEFVAR, NULL, NULL, dest);
                        create_3ac(I_DEFVAR, NULL, NULL, tmp);

                        create_3ac(I_INT2FLOAT, tmp2->operand, NULL, tmp);
                        create_3ac(I_EQ, tmp1->operand, tmp, dest);
                    } else {
                        clear_all();
                        exit(ERR_SEM_T);
                    }


                    create_3ac(I_NOT, dest, NULL, dest);

                    Stack_push(stack, E_E, dest, k_boolean);
                    return 11;
                default:
                    error(ERR_SYNTA);

            }
        case E_RPAR:
            input = Stack_pop(stack);
            int i = 0;
            switch (input->type) {
                case E_LPAR:
                    input = check_next_element_type(E_FUNC, stack);
                    char *name = input->operand;

                    //todo semanticky skontrolovat ze tato funkcie ma 0 parametrou jej meno je name
                    sprintf(dest, "$E_E%i", new_id);
                    create_3ac(I_DEFVAR, NULL, NULL, dest);
                    create_3ac(I_CREATEFRAME, NULL, NULL, NULL);  //vytvorenie operacii
                    create_3ac(I_CALL, NULL, NULL, name);  //vytvorenie operacii
                    create_3ac(I_MOVE, "%RETVAL", NULL, dest); //deklarovanie operandu
                    check_next_element_type(E_LT, stack);
                    Stack_push(stack, E_E, dest, input->typ_konkretne);
                    return 13;
                    break;
                case E_E:
                    tmp1 = input;
                    input = Stack_pop(stack);
                    switch (input->type) {
                        case E_LPAR:
                            input = Stack_pop(stack);
                            check_pointer(input);
                            switch (input->type) {
                                case E_LT:
                                    Stack_push(stack, E_E, tmp1->operand, tmp1->typ_konkretne);
                                    return 12;
                                case E_FUNC: //zmenit pak nejspis na E_FUNCT
                                    //todo skontrolovat sematiku ze tato funkcia sa ma volat s jednym parametrom a to typ tmp1->typ_konktretne ci implicitne prekonvertovatelny typ
                                    //gen vnutorneho kodu
                                    sprintf(dest, "$E_E%i", new_id);
                                    create_3ac(I_DEFVAR, NULL, NULL, dest);
                                    create_3ac(I_CREATEFRAME, NULL, NULL, NULL);  //vytvorenie operacii
                                    create_3ac(I_PUSHS, NULL, NULL, tmp1->operand);  //vytvorenie operacii
                                    create_3ac(I_CALL, NULL, NULL, input->operand);  //vytvorenie operacii
                                    create_3ac(I_MOVE, "%RETVAL", NULL, dest); //deklarovanie operandu
                                    check_next_element_type(E_LT, stack);
                                    Stack_push(stack, E_E, dest, input->typ_konkretne);
                                    return 13;
                                default:
                                    error(ERR_SYNTA);
                            }
                        case E_COMMA:

                            arr_el[i++] = tmp1;
                            arr_el[i++] =  check_next_element_type(E_E, stack);

                            input = Stack_pop(stack);
                            while (input->type == E_COMMA) {
                                arr_el[i++] = check_next_element_type(E_E, stack);
                                input = Stack_pop(stack);
                            }
                            if (input->type == E_LPAR) {
                                input = check_next_element_type(E_FUNC, stack);
                                check_next_element_type(E_LT, stack);

                                char *name = input->operand;

                                sprintf(dest, "$E_E%i", new_id);
                                create_3ac(I_DEFVAR, NULL, NULL, dest);
                                create_3ac(I_CREATEFRAME, NULL, NULL, NULL);  //vytvorenie operacii
                                for (int j = 0; j < i; ++j) {
                                    create_3ac(I_PUSHS, NULL, NULL, arr_el[j]->operand);  //vytvorenie operacii
                                    //parametre su nacitane v arr_el a su su nacitane od konca

                                    //todo semantika skonrolovat ci funckia pod menon name ma prave i parametrou a ci sa tieto parametre zhoduju ci su prekonvergovatelne implicitne

                                    //todo overenie typu parametru rob tu ja tu potom do toho doplnim premeni urob to obdobne ako to je urobene pri e_plus ...

                                }

                                create_3ac(I_CALL, NULL, NULL, input->operand);  //vytvorenie operacii
                                create_3ac(I_MOVE, "%RETVAL", NULL, dest); //deklarovanie operandu

                                Stack_push(stack, E_E, dest, input->typ_konkretne);
                                return 13;
                            } else {
                                error(ERR_SYNTA);
                            }
                    }
                    error(ERR_SYNTA);

                default:
                    error(ERR_SYNTA);
            }


        case E_ID:
            check_next_element_type(E_LT, stack);
            sprintf(dest, "$E_E%i", new_id);    //generovanie operandu pre vysledok medzisuctu
            create_3ac(I_DEFVAR, NULL, NULL, dest); //deklarovanie operandu
            create_3ac(I_MOVE, tmp2->operand, NULL, dest);
            Stack_push(stack, E_E, dest, tmp2->typ_konkretne);
            return 15;
        default:
            error(ERR_SYNTA);
    }
}

// Precedencni tabulka, rozsirena pro FUNEXP
const int precedence_table[17][17] = {
        /*    +   -   *   /   \   (   )  id   <  <=   >  >=   =  <>   $   f   ,  */
/* +    */ { GT, GT, LT, LT, LT, LT, GT, LT, GT, GT, GT, GT, GT, GT, GT, LT, GT, },
/* -    */ { GT, GT, LT, LT, LT, LT, GT, LT, GT, GT, GT, GT, GT, GT, GT, LT, GT, },
/* *    */ { GT, GT, GT, GT, GT, LT, GT, LT, GT, GT, GT, GT, GT, GT, GT, LT, GT, },
/* /    */ { GT, GT, GT, GT, GT, LT, GT, LT, GT, GT, GT, GT, GT, GT, GT, LT, GT, },
/* \    */ { GT, GT, LT, LT, GT, LT, GT, LT, GT, GT, GT, GT, GT, GT, GT, LT, GT, },
/* (    */ { LT, LT, LT, LT, LT, LT, EQ, LT, LT, LT, LT, LT, LT, LT, xx, LT, EQ, },
/* )    */ { GT, GT, GT, GT, GT, xx, GT, xx, GT, GT, GT, GT, GT, GT, GT, xx, GT, },
/* id   */ { GT, GT, GT, GT, GT, xx, GT, xx, GT, GT, GT, GT, GT, GT, GT, xx, GT, }, //ID x ( ma byt xx, tohle je kvuli debugu funkci
/* <    */ { LT, LT, LT, LT, LT, LT, GT, LT, xx, xx, xx, xx, xx, xx, GT, LT, GT, },
/* <=   */ { LT, LT, LT, LT, LT, LT, GT, LT, xx, xx, xx, xx, xx, xx, GT, LT, GT, },
/* >    */ { LT, LT, LT, LT, LT, LT, GT, LT, xx, xx, xx, xx, xx, xx, GT, LT, GT, },
/* >=   */ { LT, LT, LT, LT, LT, LT, GT, LT, xx, xx, xx, xx, xx, xx, GT, LT, GT, },
/* =    */ { LT, LT, LT, LT, LT, LT, GT, LT, xx, xx, xx, xx, xx, xx, GT, LT, GT, },
/* <>   */ { LT, LT, LT, LT, LT, LT, GT, LT, xx, xx, xx, xx, xx, xx, GT, LT, GT, },
/* $    */ { LT, LT, LT, LT, LT, LT, xx, LT, LT, LT, LT, LT, LT, LT, xx, LT, xx, },
/* f    */ { xx, xx, xx, xx, xx, EQ, xx, xx, xx, xx, xx, xx, xx, xx, xx, xx, xx, },
/* ,    */ { LT, LT, LT, LT, LT, LT, EQ, LT, LT, LT, LT, LT, LT, LT, LT, LT, EQ, },
};

void prilep(char *ret, char c, int *top, int *cap) {
    if (*cap <= *top + 5) {
        *cap = *top + 20;
        ret = my_realloc(ret, sizeof(char) * (*cap));
    }

    if (isspace(c)) {
        char tmp[5];
        sprintf(tmp, "\\%03i", c);
        ret[(*top)++] = tmp[0];
        ret[(*top)++] = tmp[1];
        ret[(*top)++] = tmp[2];
        ret[(*top)++] = tmp[3];

    } else {
        ret[(*top)++] = c;
    }
    ret[(*top)] = 0;
}

char *token2operand(t_token *token) {
    if (token == NULL) {
        return "";
    }
    int size = 130;
    char *result = my_malloc(sizeof(char) * size);
    unsigned i = 0;
    unsigned j = i;
    switch (token->token_type) {
        case ID:
            result = my_strcpy(token->data.s);
            break;
        case INT:
            sprintf(result, "int@%i", token->data.i);
            break;
        case DOUBLE:
            sprintf(result, "float@%f", token->data.d);
            break;
        case STR:
            while (token->data.s[i] != 0) {
                prilep(result, token->data.s[i], &j, &size);
                i++;
            }
            char *tmp = my_strcpy(result);
            sprintf(result, "str@%s", tmp);
            break;
        default:
            result[0] = 0;

    }
    return result;
}


int code_type(int *dollar_source, t_token *input, TTable *table){
//    t_token * input = get_token();
//    ret_sym = input;
    static int was_funct = 0;
    int i = input->token_type;

//    if (was_funct && i != LPAR){
//        clear_all();
//        exit(ERR_SEM_P);
//    }

    switch(i) {
        case PLUS:
            return E_PLUS;
        case MINUS:
            return E_MINUS;
        case KRAT:
            return E_MUL;
        case DELENO:
            return E_DIV;
        case MOD:
            return E_MOD;
        case LPAR:
            return E_LPAR;
        case RPAR:
            return E_RPAR;
        case ID: //nutno rozlisit ID funkce a ID promenne, ted neprochazi vyrazy jako ID = ID(ID)   //kontrolujem v rule !!!
        {
            TElement* found = Tbl_GetDirect(table, input->data.s); //odkomentovat, až bude globální table a budou se do ní plnit ID
            if(found != NULL)
            {
                if(found->data->type == ST_Function && found->data->isDefined) {
                    was_funct = 1;
                    return E_FUNC;
                } else if(found->data->type == ST_Variable && found->data->isDefined){
                    return E_ID;
                } else {
                    return -1; //ERR_SEMANTIC
                }
            }
        }
            //pozreme do symtable
            semerror(ERR_SEM_P);
        case INT: //mozna budeme muset mapovat jinak kvuli semanticke
            return E_ID;
        case DOUBLE:
            return E_ID;
        case STR:
            return E_ID;
        case LT:
            return E_LT;
        case LE:
            return E_LE;
        case GT:
            return E_GT;
        case GE:
            return E_GE;
        case EQ:
            return E_EQ;
        case NEQ:
            return E_NEQ;
        case COMMA:
            return E_COMMA;
        case EOL:
            *dollar_source = EOL;
            return E_DOLLAR;
        case SEMICOLLON:
            *dollar_source = SEMICOLLON;
            return E_DOLLAR;
        case KEY_WORD:
            *dollar_source = input->data.i;
            return E_DOLLAR;
        default:
            return -1;
    }
}


/* Vraci kod tokenu, ktery ukoncil vyraz.
 * Pro klicova slova vraci data.i + 100, abychom se vyhli kolizi
 * navrat musi hlidat volajici funkce v parseru
 * Priklady takovych tokenu - EOL, Then, Semicolon
 * Pro Then je navratova hodnota tedy 120
 * */

int expression(TTable *local, int typ){
    int id_of_ID = 0;

    Stack stack;
    Stack_init(&stack);
    Stack_push(&stack, E_DOLLAR, NULL, 0);
    int a;
    int b;
    int dollar_source = 0;
    t_token *my_token = get_token();
    if (my_token->token_type == EOL && typ == -2) {
        return EOL;
    }

    b = code_type(&dollar_source, my_token, local); //prekodovani typu tokenu na index do tabulky
    if(b==E_DOLLAR && dollar_source == EOL) {
        Stack_dispose(&stack);
        return dollar_source;
    }
    do{
        a = Stack_top(&stack)->type;
        int ruleNumber = 0;
        int new_id = -1;
        int token_type = -1;
        if (my_token != NULL){
             if (my_token->token_type == INT){
                token_type = k_integer;
            } else if (my_token->token_type == DOUBLE){
                 token_type = k_double;
             } else if (my_token->token_type == STR){
                 token_type = k_string;
             } else if (my_token->token_type == ID) {
                TElement *el = Tbl_GetDirect(local,my_token->data.s);
                if (el == NULL){
                    clear_all();
                    exit(ERR_SEM_P); //nebolo definovane
                }
                if (el->data->type == ST_Variable) {
                    token_type = el->data->data->var->type;
                } else {
                    token_type = el->data->data->func->return_type;
                }
            } else {
                token_type = my_token->token_type;
            }
        }
        if (b == E_ID || b == E_FUNC) {
            new_id = id_of_ID++;
        } else {
            new_id = -1;
//            my_token = NULL;
        }

        switch(precedence_table[a][b]){
            case EQ:
                Stack_push(&stack,b, token2operand(my_token), token_type);
                my_token = get_token();
                b = code_type(&dollar_source, my_token, local);
                break;
            case LT:
                Stack_expand(&stack);
                Stack_push(&stack, b, token2operand(my_token), token_type);
                my_token = get_token();
                b = code_type(&dollar_source, my_token, local);
                break;
            case GT:
                ruleNumber = rule(&stack, local);
                if (ruleNumber != 0) {
                    //printf("Rule %d\n", ruleNumber);
                } else {
                    error(ERR_SYNTA);
                }
                break;
            default: error(ERR_SYNTA);

        }

    } while (!(b == E_DOLLAR && Stack_top(&stack)->type == E_DOLLAR));

    Stack_dispose(&stack);
    char last[130];
    sprintf(last, "$E_E%i", get_id() - 1);
    create_3ac(I_PUSHS, NULL, NULL, last);  //vytvorenie operacii


    return dollar_source;
}