#include <stdio.h>
#include "parser.h"
#include "main.h"

enum{
    xx
}table_values;

enum{
    E_Dollar
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
void Stack_dispose(Stack * stack){}

//vlozi na vrchol zasobniku element s type
void Stack_push(Stack * stack, int type){}

//vraci ukazatel na nejvrchnejsi terminal - funkce je tu jen pro prehlednost, je jednoradkova, takze vlastne neni potreba
Element* Stack_top(Stack *stack){
    return stack->top;
}

//nad nejvrchnejsi terminal (stack.active) vlozi novy element s "<"
void Stack_expand(Stack *stack){}

/*cte elementy zeshora zasobniku dokud nenajde "<", musi overit, jestli je precteny retezec pravidlo
 * pokud je pravidlo, vymeni pravidlo z vrcholu zasobniku za jeho levou stranu
 * priklad: pravidlo: E -> E + E
 *          zasobnik:  $E+id+<E+E
 *          vysledek na zasobniku: $E+id+E  */
int rule(Stack *s){
}
// Precedencni tabulka
const int precedence_table[15][15] = {
        /*    +   -   *   /   \   (   )  id   <  <=   >  >=   =  <>   $  */
/* +    */ { GT, GT, LT, LT, LT, LT, GT, LT, GT, GT, GT, GT, GT, GT, GT, },
/* -    */ { GT, GT, LT, LT, LT, LT, GT, LT, GT, GT, GT, GT, GT, GT, GT, },
/* *    */ { GT, GT, GT, GT, GT, LT, GT, LT, GT, GT, GT, GT, GT, GT, GT, },
/* /    */ { GT, GT, GT, GT, GT, LT, GT, LT, GT, GT, GT, GT, GT, GT, GT, },
/* \    */ { GT, GT, LT, LT, GT, LT, GT, LT, GT, GT, GT, GT, GT, GT, GT, },
/* (    */ { LT, LT, LT, LT, LT, LT, EQ, LT, LT, LT, LT, LT, LT, LT, xx, },
/* )    */ { GT, GT, GT, GT, GT, xx, GT, xx, GT, GT, GT, GT, GT, GT, GT, },
/* id   */ { GT, GT, GT, GT, GT, xx, GT, xx, GT, GT, GT, GT, GT, GT, GT, },
/* <    */ { LT, LT, LT, LT, LT, LT, GT, LT, xx, xx, xx, xx, xx, xx, GT, },
/* <=   */ { LT, LT, LT, LT, LT, LT, GT, LT, xx, xx, xx, xx, xx, xx, GT, },
/* >    */ { LT, LT, LT, LT, LT, LT, GT, LT, xx, xx, xx, xx, xx, xx, GT, },
/* >=   */ { LT, LT, LT, LT, LT, LT, GT, LT, xx, xx, xx, xx, xx, xx, GT, },
/* =    */ { LT, LT, LT, LT, LT, LT, GT, LT, xx, xx, xx, xx, xx, xx, GT, },
/* <>   */ { LT, LT, LT, LT, LT, LT, GT, LT, xx, xx, xx, xx, xx, xx, GT, },
/* $    */ { LT, LT, LT, LT, LT, LT, xx, LT, LT, LT, LT, LT, LT, LT, xx, },
};

int expression(){
    Stack* stack;
    Stack_init(stack);
    Stack_push(stack, E_Dollar);
    int a;
    int b;
    do{
        a = Stack_top(stack)->type;
        b = get_token()->token_type; //typ tokenu, nejspis bude chtit prekodovat
        int rulenumber = 0;
        switch(precedence_table[a][b]){
            case EQ:
                Stack_push(stack,b);
                b = get_token()->token_type;
            case LT:
                Stack_expand(stack);
                Stack_push(stack, b);
                b = get_token()->token_type;
            case GT:
                rulenumber = rule(stack);
                if(rulenumber != 0){
                    printf("pravidlo %d\n", rulenumber);
                } else {
                     error(ERR_SYNTA);
                }
            default: error(ERR_SYNTA);

        }

    }while(b != E_Dollar && Stack_top(stack)->type != E_Dollar);
    return 1;
}