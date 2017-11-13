#include <stdio.h>
#include "parser.h"
#include "main.h"
#include "memwork.h"
#include "symtable.h"
#include "scanner.h"
#include "expression.h"
#include "symtable.h"
//#include "stack_token.h"

#define urcite 1
#define s_konverziou 2
#define nie 0

TTable *Table;

int are_comparable(t_token *a, t_token *b){
    if (a->token_type == STR && b->token_type == STR){
        return urcite;
    } else if (a->token_type == INT && b->token_type == INT){
        return urcite;
    } else if (a->token_type == DOUBLE && b->token_type == DOUBLE){
        return urcite;
    } else if ((a->token_type == INT && b->token_type==DOUBLE) || (a->token_type == DOUBLE && b->token_type == INT)){
        return s_konverziou;
    } else {
        fprintf(stderr,"ERR - sem - na riasku %i sa pracuje s nekompatibilymi typmi\n", a->line);
        clear_all();
        exit(ERR_SEM_T);

        return nie;
    }
}

int param_check(){ //todo
    return 1;
}


int get_id(){
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
int Stack_push(Stack * stack, int type, int id, t_token *token){
    Element * new = my_malloc(sizeof(Element));
    check_pointer(new);

    new->type = type;
    new->next = stack->top;
    new->id=id;
    new->token = token;
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
    int new_id = get_id();


    Element *tmp1 = input;
    Element *tmp2 = NULL;


    Element *arr_el[100] = {NULL, };


    switch(input->type){
        case E_E:
            input = Stack_pop(stack);
            switch(input->type){
                case E_PLUS:
                    tmp2 = check_next_element_type(E_E,stack);
                    check_next_element_type(E_LT,stack);
                    are_comparable(tmp1->token,tmp2->token);
                    printf("defvar E_E%i\n",new_id);
                    printf("ADD E_E%i E_E%i E_E%i\n",new_id, tmp1->id, tmp2->id);
                    Stack_push(stack, E_E, new_id,NULL);
                    return 1;
                case E_MINUS:
                    tmp2 = check_next_element_type(E_E,stack);
                    check_next_element_type(E_LT,stack);
                    are_comparable(tmp1->token,tmp2->token);
                    printf("defvar E_E%i\n",new_id);
                    printf("SUB E_E%i E_E%i E_E%i\n",new_id, tmp1->id, tmp2->id);
                    Stack_push(stack, E_E, new_id, NULL);
                    return 2;
                case E_MUL:
                    tmp2 =check_next_element_type(E_E,stack);
                    check_next_element_type(E_LT,stack);
                    are_comparable(tmp1->token,tmp2->token);
                    printf("defvar E_E%i\n",new_id);
                    printf("MUL E_E%i E_E%i E_E%i\n",new_id, tmp1->id, tmp2->id);
                    Stack_push(stack, E_E, new_id, NULL);
                    return 3;
                case E_DIV:
                    tmp2 = check_next_element_type(E_E,stack);
                    check_next_element_type(E_LT,stack);
                    are_comparable(tmp1->token,tmp2->token);
                    printf("defvar E_E%i\n",new_id);
                    printf("DIV E_E%i E_E%i E_E%i\n",new_id, tmp1->id, tmp2->id);
                    Stack_push(stack, E_E, new_id, NULL);
                    return 4;
                case E_MOD:
                    tmp2 = check_next_element_type(E_E,stack);
                    check_next_element_type(E_LT,stack);
                    are_comparable(tmp1->token,tmp2->token);
                    printf("defvar E_E%i\n",new_id);
                    printf("MOD E_E%i E_E%i E_E%i\n",new_id, tmp1->id, tmp2->id);
                    Stack_push(stack, E_E, new_id, NULL);
                    return 5;
                case E_LT:
                    tmp2 = check_next_element_type(E_E,stack);
                    check_next_element_type(E_LT,stack);
                    are_comparable(tmp1->token,tmp2->token);
                    printf("defvar E_E%i\n",new_id);
                    printf("LT E_E%i E_E%i E_E%i\n",new_id, tmp1->id, tmp2->id);
                    Stack_push(stack, E_E, new_id, NULL);
                    return 6;
                case E_LE:
                    tmp2 = check_next_element_type(E_E,stack);
                    check_next_element_type(E_LT,stack);
                    are_comparable(tmp1->token,tmp2->token);
                    printf("defvar E_E%i\n",new_id);
                    printf("LT E_E%i E_E%i E_E%i\n",new_id, tmp1->id, tmp2->id);
                    int old_id[2] = {new_id,get_id()};
                    new_id = get_id();
                    printf("defvar E_E%i\n",new_id);
                    printf("LT E_E%i E_E%i E_E%i\n",old_id[1], tmp1->id, tmp2->id);
                    Stack_push(stack, E_E, new_id, NULL);
                    return 7;
                case E_GT:
                    tmp2 = check_next_element_type(E_E,stack);
                    check_next_element_type(E_LT,stack);
                    are_comparable(tmp1->token,tmp2->token);
                    printf("E_E%i = E_E%i > E_E%i\n",new_id, tmp1->id, tmp2->id);
                    Stack_push(stack, E_E, new_id, NULL);
                    return 8;
                case E_GE:
                    check_next_element_type(E_E,stack);
                    check_next_element_type(E_LT,stack);
                    are_comparable(tmp1->token,tmp2->token);
                    printf("E_E%i = E_E%i >= E_E%i\n",new_id, tmp1->id, tmp2->id);
                    Stack_push(stack, E_E, new_id, NULL);
                    return 9;
                case E_EQ:
                    tmp2 = check_next_element_type(E_E,stack);
                    check_next_element_type(E_LT,stack);
                    are_comparable(tmp1->token,tmp2->token);
                    printf("E_E%i = E_E%i == E_E%i\n",new_id, tmp1->id, tmp2->id);
                    Stack_push(stack, E_E, new_id, NULL);
                    return 10;
                case E_NEQ:
                    tmp2 = check_next_element_type(E_E,stack);
                    check_next_element_type(E_LT,stack);
                    are_comparable(tmp1->token,tmp2->token);
                    printf("E_E%i = E_E%i <> E_E%i\n",new_id, tmp1->id, tmp2->id);
                    Stack_push(stack, E_E, new_id, NULL);
                    return 11;
                default:
                    error(ERR_SYNTA);

            }
        case E_RPAR:

            tmp1 = check_next_element_type(E_E,stack);

            input = Stack_pop(stack);
            check_pointer(input);


            int i = 0;
            switch(input->type){
                case E_LPAR:
                    input = Stack_pop(stack);
                    check_pointer(input);

                    switch(input->type){
                        case E_LT:
                            printf("E_E%i <== E_E%i\n",new_id, tmp1->id);
                            Stack_push(stack,E_E, new_id, NULL);
                            return 12;
                        case E_ID: //zmenit pak nejspis na E_FUNCT
                            printf("E_E%i = return z E_ID%i()\n",new_id, tmp1->id);
                            check_next_element_type(E_LT,stack);
                            Stack_push(stack,E_E, new_id, NULL);
                            return 13;
                        default:
                            error(ERR_SYNTA);
                    }
                case COMMA:

                    arr_el[i++] = tmp1;
                    arr_el[i++] = check_next_element_type(E_E,stack);
                    input = Stack_pop(stack);
                    check_pointer(input);
                    while(input->type == COMMA){
                        arr_el[i++] = check_next_element_type(E_E,stack);
                        input = Stack_pop(stack);
                    }
                    if(input->type == LPAR){
                        arr_el[i++] = check_next_element_type(E_ID,stack);
                        check_next_element_type(E_LT,stack);

                        int j = i;

                        t_token *func_t =arr_el[--j]->token;
                        if (func_t->token_type != ID){
                            clear_all();
                            exit(ERR_SYNTA);
                        }

                        printf("E_E%i = return z E_ID%i(",new_id, arr_el[--i]->id);
                        while (i){
                            Element *tmp = arr_el[--i];
                            printf("E_ID%i,", tmp->id);   //TODO tu moze byt tez E_E ale kontorluje syntakt iba e_ID
                        }
                        printf(")\n");


                        Stack_push(stack,E_E, new_id, NULL);
                        return 14;
                    }
                    error(ERR_SYNTA);

                default:
                    error(ERR_SYNTA);
            }

        case E_ID:
            check_next_element_type(E_LT,stack);
            printf("E_E%i <== ID%i\n",new_id, tmp1->id);
            Stack_push(stack,E_E, new_id, NULL);
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

int code_type(int *dollar_source, t_token *input){
//    t_token * input = get_token();
//    ret_sym = input;
    static int was_funct = 0;
    int i = input->token_type;

    if (was_funct && i != LPAR){
        clear_all();
        exit(ERR_SEM_P);
    }

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
//        {
//            TElement* found = Tbl_GetDirect(Table, input->data.s); //odkomentovat, až bude globální table a budou se do ní plnit ID
//            if(found != NULL)
//            {
//                if(found->data->type == ST_Function && found->data->isDefined) {
//                    was_funct = 1;
//                    return E_FUNC;
//                } else if(found->data->type == ST_Variable && found->data->isDefined){
//                    return ID;
//                } else {
//                    return -1; //ERR_SEMANTIC
//                }
//            }
//        }
            //pozreme do symtable
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
            return -1;
    }
}


/* Vraci kod tokenu, ktery ukoncil vyraz.
 * Pro klicova slova vraci data.i + 100, abychom se vyhli kolizi
 * navrat musi hlidat volajici funkce v parseru
 * Priklady takovych tokenu - EOL, Then, Semicolon
 * Pro Then je navratova hodnota tedy 120
 * */

int expression(TTable *tTable, t_token *to_what){
    Table = tTable;
    int id_of_ID = 0;

    Stack stack;
    Stack_init(&stack);
    Stack_push(&stack, E_DOLLAR, -1, NULL);
    int a;
    int b;
    int dollar_source = 0;
    t_token *my_token = get_token();

    b = code_type(&dollar_source, my_token); //prekodovani typu tokenu na index do tabulky
    if(b==E_DOLLAR && dollar_source == EOL) {
        Stack_dispose(&stack);
        return dollar_source;
    }
    do{
        a = Stack_top(&stack)->type;
        int ruleNumber = 0;
        int new_id = -1;

        if (b == E_ID){
            new_id = id_of_ID++;
        } else {
            new_id = -1;
            my_token = NULL;
        }

        switch(precedence_table[a][b]){
            case EQ:

                Stack_push(&stack,b, new_id, my_token);
                my_token = get_token();
                b = code_type(&dollar_source, my_token);
                break;
            case LT:
                Stack_expand(&stack);
                Stack_push(&stack, b, new_id, my_token);
                my_token = get_token();
                b = code_type(&dollar_source, my_token);
                break;
            case GT:
                ruleNumber = rule(&stack);
                if(ruleNumber != 0){
                    //printf("Rule %d\n", ruleNumber);
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