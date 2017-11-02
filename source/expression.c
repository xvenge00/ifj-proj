#include <stdio.h>
#include "parser.h"
#include "main.h"
#include "memwork.h"

enum{
    xx
}table_values;

enum{
    E_PLUS = 0,
    E_MINUS,
    E_MUL,
    E_DIV,
    E_MOD,
    E_LPAR, //5
    E_RPAR,
    E_ID,
    E_LT,
    E_LE,
    E_GT, //10
    E_GE,
    E_EQ,
    E_NEQ,
    E_DOLLAR, //15
    E_FUNC,
    E_COMMA,
    E_E //18
}types;

typedef struct element{
    int type;
    struct element *next;
} Element; //prvek zásobníku - type bude zakodovany typ symbolu, next ukazatel na dalsi

typedef struct stack{
    Element *active; //ukazuje na nejvrchnejsi terminal
    Element *top; //ukazuje na vrchol zasobniku
}Stack;


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
int Stack_push(Stack * stack, int type){
    Element * new = my_malloc(sizeof(Element));
    check_pointer(new);

    new->type = type;
    new->next = stack->top;
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
int rule(Stack *stack){
    Element * input = Stack_pop(stack);
    check_pointer(input);

    switch(input->type){
        case E_E:
            input = Stack_pop(stack);
            switch(input->type){
                case E_PLUS:
                    check_next_element_type(E_E,stack);
                    check_next_element_type(E_LT,stack);
                    Stack_push(stack, E_E);
                    return 1;
                case E_MINUS:
                    check_next_element_type(E_E,stack);
                    check_next_element_type(E_LT,stack);
                    Stack_push(stack, E_E);
                    return 2;
                case E_MUL:
                    check_next_element_type(E_E,stack);
                    check_next_element_type(E_LT,stack);
                    Stack_push(stack, E_E);
                    return 3;
                case E_DIV:
                    check_next_element_type(E_E,stack);
                    check_next_element_type(E_LT,stack);
                    Stack_push(stack, E_E);
                    return 4;
                case E_MOD:
                    check_next_element_type(E_E,stack);
                    check_next_element_type(E_LT,stack);
                    Stack_push(stack, E_E);
                    return 5;
                case E_LT:
                    check_next_element_type(E_E,stack);
                    check_next_element_type(E_LT,stack);
                    Stack_push(stack, E_E);
                    return 6;
                case E_LE:
                    check_next_element_type(E_E,stack);
                    check_next_element_type(E_LT,stack);
                    Stack_push(stack, E_E);
                    return 7;
                case E_GT:
                    check_next_element_type(E_E,stack);
                    check_next_element_type(E_LT,stack);
                    Stack_push(stack, E_E);
                    return 8;
                case E_GE:
                    check_next_element_type(E_E,stack);
                    check_next_element_type(E_LT,stack);
                    Stack_push(stack, E_E);
                    return 9;
                case E_EQ:
                    check_next_element_type(E_E,stack);
                    check_next_element_type(E_LT,stack);
                    Stack_push(stack, E_E);
                    return 10;
                case E_NEQ:
                    check_next_element_type(E_E,stack);
                    check_next_element_type(E_LT,stack);
                    Stack_push(stack, E_E);
                    return 11;
                default:
                    error(ERR_SYNTA);

            }
        case E_RPAR:
            check_next_element_type(E_E,stack);

            input = Stack_pop(stack);
            check_pointer(input);

            switch(input->type){
                case E_LPAR:
                    input = Stack_pop(stack);
                    check_pointer(input);

                    switch(input->type){
                        case E_LT:
                            Stack_push(stack,E_E);
                            return 12;
                        case E_ID: //zmenit pak nejspis na E_FUNCT
                            check_next_element_type(E_LT,stack);
                            Stack_push(stack,E_E);
                            return 13;
                        default:
                            error(ERR_SYNTA);
                    }
                case COMMA:
                    check_next_element_type(E_E,stack);
                    input = Stack_pop(stack);
                    check_pointer(input);
                    while(input->type == COMMA){
                        check_next_element_type(E_E,stack);
                        input = Stack_pop(stack);
                    }
                    if(input->type == LPAR){
                        check_next_element_type(E_ID,stack);
                        check_next_element_type(E_LT,stack);
                        Stack_push(stack,E_E);
                        return 14;
                    }
                    error(ERR_SYNTA);

                default:
                    error(ERR_SYNTA);
            }

        case E_ID:
            check_next_element_type(E_LT,stack);
            Stack_push(stack,E_E);
            return 15;
        default: error(ERR_SYNTA);
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
/* id   */ { GT, GT, GT, GT, GT, EQ, GT, xx, GT, GT, GT, GT, GT, GT, GT, xx, GT, }, //ID x ( ma byt xx, tohle je kvuli debugu funkci
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

int code_type(int *dollar_source){
    t_token * input = get_token();
    int i = input->token_type;
    int result = -1;
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
        case ID: //nutno rozlisit ID funkce a ID promenne, ale zatim je to jedno, ID i F maji stejne hodnoty v tabulce
            return E_ID;
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
            return result;
    }
}


/* Vraci kod tokenu, ktery ukoncil vyraz.
 * Pro klicova slova vraci data.i + 100, abychom se vyhli kolizi
 * navrat musi hlidat volajici funkce v parseru
 * Priklady takovych tokenu - EOL, Then, Semicolon
 * Pro Then je navratova hodnota tedy 120
 * */
int expression(){
    Stack stack;
    Stack_init(&stack);
    Stack_push(&stack, E_DOLLAR);
    int a;
    int b;
    int dollar_source = 0;
    b = code_type(&dollar_source); //prekodovani typu tokenu na index do tabulky
    if(b==E_DOLLAR && dollar_source == EOL) {
        Stack_dispose(&stack);
        return dollar_source;
    }
    do{
        a = Stack_top(&stack)->type;
        int ruleNumber = 0;
        switch(precedence_table[a][b]){
            case EQ:
                Stack_push(&stack,b);
                b = code_type(&dollar_source);
                break;
            case LT:
                Stack_expand(&stack);
                Stack_push(&stack, b);
                b = code_type(&dollar_source);
                break;
            case GT:
                ruleNumber = rule(&stack);
                if(ruleNumber != 0){
                    printf("Rule %d\n", ruleNumber);
                } else {
                     error(ERR_SYNTA);
                }
                break;
            default: error(ERR_SYNTA);

        }

    }while(!(b == E_DOLLAR && Stack_top(&stack)->type == E_DOLLAR));

    Stack_dispose(&stack);
    return dollar_source;
}