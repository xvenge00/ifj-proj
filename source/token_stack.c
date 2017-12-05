/*  * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *
 *  Implementace prekladace imperativniho jazyka IFJ17
 *
 *  Autori:
 *      xvenge00 - Adam Venger
 *      xbabka01 - Peter Babka
 *      xrandy00 - Vojtech Randysek
 *      xdosed08 - Ondrej Dosedel
 *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#include "token_stack.h"
#include "memwork.h"

typedef struct s_stack_node {
    t_token * token;
    struct s_stack_node *next;
}t_stack_node;

t_stack_node *token_stack_head = NULL;

void token_push(t_token *token){
    t_stack_node *new = my_malloc(sizeof(t_stack_node));
    new->token = token;
    new->next = NULL;
    if (token_stack_head == NULL){
        token_stack_head = new;
    } else {
        new->next = token_stack_head;

        token_stack_head = new;
    }
}

t_token *token_pop(){
    t_token *result = token_stack_head->token;

    if (token_stack_head == NULL){
        return NULL;
    } else {
        token_stack_head = token_stack_head->next;
        return result;
    }
}

typedef struct s_char_node {
    char * token;
    struct s_char_node *next;
}t_char_node;

t_char_node *char_stack_head = NULL;

void str_push(char *token){
    t_char_node *new = my_malloc(sizeof(t_char_node));
    new->token = token;
    new->next = NULL;
    if (char_stack_head == NULL){
        char_stack_head = new;
    } else {
        new->next = char_stack_head;

        char_stack_head = new;
    }
}

char *str_pop(){
    char *result = char_stack_head->token;

    if (char_stack_head == NULL){
        return NULL;
    } else
        char_stack_head = char_stack_head->next;
    return result;

}
