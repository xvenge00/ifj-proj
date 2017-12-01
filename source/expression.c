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


/*
 * spaja 2 stringy do 1
 * funkciu pouzivam na spojenie frame a mena
 * priklad name = cat_string("LF@", name)
 * ked je variable == NULL vrati NULL
 */
char *cat_string(char *frame, char *variable) {
    check_null(frame);
    if (variable == NULL) {
        return NULL;
    }

    char *buf = my_malloc(sizeof(char) * BUFFSIZE);
    buf[0] = 0;
    buf = strncat(buf, frame, 4);
    buf = strncat(buf, variable, BUFFSIZE - 4);   //asi by to mohlo byt 3

    return buf;
}

char *my_strcpy(char *src) {
    char *dest;
    if (src == NULL) {
        dest = NULL;
    } else {
        size_t size = strlen(src) + sizeof(char);
        dest = my_malloc(size);

        int i = 0;
        while (src[i] != 0) {
            dest[i] = src[i];
            i++;
        }
        dest[i] = 0;

    }
    return dest;
}

int get_id() {
    static int id = 0;
    return id++;
}

void Stack_init(Stack *stack) {
    stack->top = NULL;
    stack->active = NULL;
}

//smaze zasobnik, uvolni pamet
int Stack_dispose(Stack *stack) {
    check_pointer(stack);
    Element *tmp;

    while (stack->top != NULL) {
        tmp = stack->top;
        stack->top = tmp->next;
        my_free(tmp);
    }
    return SUCCESS;
}

//vlozi na vrchol zasobniku element s type
int Stack_push(Stack *stack, int type, char *operand, int t_dat) {
    Element *new = my_malloc(sizeof(Element));
    check_pointer(new);

    new->type = type;
    new->next = stack->top;
    new->operand = my_strcpy(operand);
    new->typ_konkretne = t_dat;

    stack->top = new;

    if (new->type != E_E) { //isTerminal
        stack->active = new;
    }
    return SUCCESS;

}

//po zavolani tehle funkce je potreba uvolnit ten ukazatel
Element *Stack_pop(Stack *stack) {
    Element *tmp = stack->top;
    stack->top = tmp->next;
    if (stack->active == tmp) { //pokud byl prvni aktivni, posuneme aktivitu na dalsi neterminal
        while (stack->active->type == E_E) {
            stack->active = stack->active->next;
        }
    }
    return tmp;
}

Element *Stack_top(Stack *stack) {
    Element *tmp = stack->top;
    while (tmp->type == E_E) {
        tmp = tmp->next;
    }
    return tmp;
}

//nad nejvrchnejsi terminal (stack.active) vlozi novy element s "<"
//implementace - vlozi a prekopiruje aktivni pod aktivni a vrchni premaze na <
int Stack_expand(Stack *stack) {
    Element *tmp = stack->top;
    while (tmp->type == E_E) {
        tmp = tmp->next;
    }

    Element *new = my_malloc(sizeof(Element));
    check_pointer(new);

    new->type = tmp->type;
    new->next = tmp->next;
    tmp->next = new;
    tmp->type = E_LT;

    return SUCCESS;
}

Element *check_next_element_type(int type, Stack *stack) {
    Element *input = Stack_pop(stack);
    check_pointer(input);
    if (input->type == type) {
        return input;
    }
    syntax_error(ERR_SYNTA, line);
    return NULL;
}

/*
 * @brief Rozhoduje ci treba pridat FT@ pred premennu
 * pozera sa ci je to hodnota alebo premenna
 */
bool add_FT(char *value) {
    int res1 = strncmp("int@", value, 4);
    int res3 = strncmp("float@", value, 6);
    int res2 = strncmp("string@", value, 7);

    return res1 && res2 && res3;    //strncmp vracia 0 ked sa zhoduju
}

int ruleE_E(Stack *stack, Element *tmp2, char **ret_var,int *last_type) {
    Element *input = Stack_pop(stack);
    Element *tmp1 = NULL;
    char *dest = NULL;

    tmp1 = check_next_element_type(E_E, stack);
    check_next_element_type(E_LT, stack);

    switch (input->type) {
        case E_PLUS:
            dest = gen_and_convert(E_PLUS, tmp1, tmp2);
            Stack_push(stack, E_E, dest, tmp1->typ_konkretne);
            *last_type = tmp1->typ_konkretne;
            *ret_var = dest;
            return 1;
        case E_MINUS:
            dest = gen_and_convert(E_MINUS, tmp1, tmp2);
            Stack_push(stack, E_E, dest, tmp1->typ_konkretne);
            *last_type = tmp1->typ_konkretne;
            *ret_var = dest;
            return 2;
        case E_MUL:
            dest = gen_and_convert(E_MUL, tmp1, tmp2);
            Stack_push(stack, E_E, dest, tmp1->typ_konkretne);
            *last_type = tmp1->typ_konkretne;
            *ret_var = dest;
            return 3;
        case E_DIV:
            dest = gen_and_convert(E_DIV, tmp1, tmp2);
            Stack_push(stack, E_E, dest, k_double);
            *ret_var = dest;
            *last_type = k_double;

            return 4;
        case E_MOD:
            dest = gen_and_convert(E_MOD, tmp1, tmp2);
            Stack_push(stack, E_E, dest, k_integer);
            *ret_var = dest;
            *last_type = k_integer;
            return 5;
        case E_LT:
            dest = gen_and_convert(E_LT, tmp1, tmp2);
            Stack_push(stack, E_E, dest, k_boolean);
            *ret_var = dest;
            *last_type = k_boolean;

            return 6;
        case E_LE:
            dest = gen_and_convert(E_LE, tmp1, tmp2);
            Stack_push(stack, E_E, dest, k_boolean);
            *ret_var = dest;
            *last_type = k_boolean;
            return 7;
        case E_GT:
            dest = gen_and_convert(E_GT, tmp1, tmp2);
            Stack_push(stack, E_E, dest, k_boolean);
            *last_type = k_boolean;
            *ret_var = dest;
            return 8;
        case E_GE:
            dest = gen_and_convert(E_GE, tmp1, tmp2);
            Stack_push(stack, E_E, dest, k_boolean);
            *last_type = k_boolean;
            *ret_var = dest;
            return 9;
        case E_EQ:
            dest = gen_and_convert(E_EQ, tmp1, tmp2);
            Stack_push(stack, E_E, dest, k_boolean);
            *last_type = k_boolean;
            *ret_var = dest;
            return 10;
        case E_NEQ:
            dest = gen_and_convert(E_NEQ, tmp1, tmp2);
            Stack_push(stack, E_E, dest, k_boolean);
            *last_type = k_boolean;
            *ret_var = dest;
            return 11;
        default:
            syntax_error(ERR_SYNTA, line);
    }
    return -1;
}

int ruleE_RPAR(Stack *stack, TTable *func_table, TTable *local, char **ret_var,int *last_type) {
    Element *input = Stack_pop(stack);
    Element **arr_el = NULL;
    Element *tmp1 = NULL;
    char *dest = NULL;

    TElement *found = NULL;

    unsigned i = 0;
    switch (input->type) {
        case E_LPAR:
            input = check_next_element_type(E_FUNC, stack);
            char *name = input->operand;

            //semanticky skontrolovat ze tato funkcie ma 0 parametrou jej meno je name
            found = Tbl_GetDirect(func_table, name);
            if (found == NULL) {
                undefined_err(name, line);
                return -1;
            }
            if (found->data->data->func->attr_count != 0) {
                error("Nespravny pocet parametrov", ERR_SEM_TYPE, line);
            }

            dest = call_function(name, NULL, 0);

            check_next_element_type(E_LT, stack);
            Stack_push(stack, E_E, dest, input->typ_konkretne);
            *last_type = input->typ_konkretne;
            *ret_var = dest;
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
                            *last_type = tmp1->typ_konkretne;
                            return 12;
                        case E_FUNC: { //zmenit pak nejspis na E_FUNCT
                            //skontrolovat sematiku ze tato funkcia sa ma volat s jednym parametrom a to typ tmp1->typ_konktretne ci implicitne prekonvertovatelny typ
                            found = Tbl_GetDirect(func_table, input->operand);
                            if (found == NULL) {
                                semerror(ERR_SEM_TYPE, line);
                            }
                            if (found->data->data->func->attr_count == 1) {
                                int paramReturn = found->data->data->func->attributes[0];
                                if (!((paramReturn == tmp1->typ_konkretne) ||
                                      (paramReturn == k_integer && tmp1->typ_konkretne == k_double)
                                      || (paramReturn == k_double && tmp1->typ_konkretne == k_integer))) {
                                    semerror(ERR_SEM_TYPE, line);
                                    //todo konverzia
                                }
                            } else {
                                semerror(ERR_SEM_TYPE, line);
                            }
                            //gen vnutorneho kodu
                            dest = call_function(input->operand, &tmp1, 1);

                            check_next_element_type(E_LT, stack);
                            Stack_push(stack, E_E, dest, input->typ_konkretne);
                            *last_type = input->typ_konkretne;
                            *ret_var = dest;
                            return 13;
                            default:
                                syntax_error(ERR_SYNTA, line);
                        }
                    }
                case E_COMMA:
                    arr_el = my_realloc(arr_el, sizeof(Element *) * (i + 1));
                    arr_el[i++] = tmp1;
                    arr_el[i++] = check_next_element_type(E_E, stack);

                    input = Stack_pop(stack);
                    while (input->type == E_COMMA) {
                        arr_el = my_realloc(arr_el, sizeof(Element) * (i + 1));
                        arr_el[i++] = check_next_element_type(E_E, stack);
                        input = Stack_pop(stack);
                    }
                    if (input->type == E_LPAR) {
                        input = check_next_element_type(E_FUNC, stack);
                        check_next_element_type(E_LT, stack);

                        char *name = input->operand;
                        found = Tbl_GetDirect(local, name);
                        if (found == NULL) {
                            found = Tbl_GetDirect(func_table, name);
                            if (found == NULL) {
                                semerror(ERR_SEM_TYPE, line);
                            }
                        }
                        if (found->data->data->func->attr_count != i) {
                            semerror(ERR_SEM_TYPE, line);
                        }
                        for (unsigned j = 0; j < i; ++j) {
                            //parametre su nacitane v arr_el a su su nacitane od konca
                            // semantika skonrolovat ci funckia pod menom name ma prave i parametrov a ci sa tieto parametre zhoduju ci su prekonvergovatelne implicitne
                            int paramReturn = found->data->data->func->attributes[i - j - 1];
                            if (paramReturn == arr_el[j]->typ_konkretne) {

                            } else if (paramReturn == k_integer && arr_el[j]->typ_konkretne == k_double) {
                                create_3ac(I_FLOAT2R2EINT, arr_el[j]->operand, NULL, arr_el[j]->operand);
                            } else if (paramReturn == k_double && arr_el[j]->typ_konkretne == k_integer) {
                                create_3ac(I_INT2FLOAT, arr_el[j]->operand, NULL, arr_el[j]->operand);
                            } else {
                                semerror(ERR_SEM_TYPE, line);
                            }
                            // overenie typu parametru rob tu ja tu potom do toho doplnim premeni urob to obdobne ako to je urobene pri e_plus ...

                        }

                        dest = call_function(name, arr_el, i);

                        Stack_push(stack, E_E, dest, input->typ_konkretne);
                        *last_type = input->typ_konkretne;
                        *ret_var = dest;
                        return 13;
                    } else {
                        syntax_error(ERR_SYNTA, line);
                    }
            }
            syntax_error(ERR_SYNTA, line);

        default:
            syntax_error(ERR_SYNTA, line);
            return -1;
    }
}

int ruleID(Stack *stack, Element *input, char **ret_var,int *last_type){
    check_next_element_type(E_LT, stack);
    char *dest = NULL;
    if (add_FT(input->operand)) {
        dest = cat_string("TF@", input->operand);
    } else {
        dest= input->operand;
    }
    Stack_push(stack, E_E, dest, input->typ_konkretne);
    *last_type = input->typ_konkretne;
    *ret_var = dest;
    return 15;
}

/*cte elementy zeshora zasobniku dokud nenajde "<", musi overit, jestli je precteny retezec pravidlo
 * pokud je pravidlo, vymeni pravidlo z vrcholu zasobniku za jeho levou stranu
 * priklad: pravidlo: E -> E + E
 *          zasobnik:  $E+id+<E+E
 *          vysledek na zasobniku: $E+id+E  */
int rule(Stack *stack, TTable *local, TTable *func_table, char **ret_var,int *last_type) {
    Element *input = Stack_pop(stack);
    check_pointer(input);

    switch (input->type) {
        case E_E:
            return ruleE_E(stack, input, ret_var, last_type);
        case E_RPAR:
            return ruleE_RPAR(stack, func_table, local,ret_var, last_type);
        case E_ID:
            return ruleID(stack, input,ret_var, last_type);
        default:
            syntax_error(ERR_SYNTA, line);
    }
    return -1;
}

const int precedence_table[17][17] = {
        /*    +   -   *   /   \   (   )  id   <  <=   >  >=   =  <>   $   f   ,  */
/* +    */ { GT, GT, LT, LT, LT, LT, GT, LT, GT, GT, GT, GT, GT, GT, GT, LT, GT, },
/* -    */ { GT, GT, LT, LT, LT, LT, GT, LT, GT, GT, GT, GT, GT, GT, GT, LT, GT, },
/* *    */ { GT, GT, GT, GT, GT, LT, GT, LT, GT, GT, GT, GT, GT, GT, GT, LT, GT, },
/* /    */ { GT, GT, GT, GT, GT, LT, GT, LT, GT, GT, GT, GT, GT, GT, GT, LT, GT, },
/* \    */ { GT, GT, LT, LT, GT, LT, GT, LT, GT, GT, GT, GT, GT, GT, GT, LT, GT, },
/* (    */ { LT, LT, LT, LT, LT, LT, EQ, LT, LT, LT, LT, LT, LT, LT, xx, LT, EQ, },
/* )    */ { GT, GT, GT, GT, GT, xx, GT, xx, GT, GT, GT, GT, GT, GT, GT, xx, GT, },
/* id   */ { GT, GT, GT, GT, GT, xx, GT, xx, GT, GT, GT, GT, GT, GT, GT, xx, GT, },
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

void prilep(char **ret, char c, unsigned *top, unsigned *cap) {  //TODO !!!!nefunguje !!!!!!!
    if (*cap <= *top + 5) {
        *cap = *top + 20;
        *ret = my_realloc(*ret, sizeof(char) * (*cap));
    }

    if (isspace(c)) {
        char tmp[5];
        snprintf(tmp, 5, "\\%03i", c);
        (*ret)[(*top)++] = tmp[0];
        (*ret)[(*top)++] = tmp[1];
        (*ret)[(*top)++] = tmp[2];
        (*ret)[(*top)++] = tmp[3];

    } else {
        (*ret)[(*top)++] = c;
    }
    (*ret)[(*top)] = 0;
}

char *token2operand(t_token *token) {
    if (token == NULL) {
        return "";
    }

    unsigned size = BUFFSIZE;
    char *result = my_malloc(sizeof(char) * size);
    unsigned i = 0;
    unsigned j = i;
    switch (token->token_type) {
        case ID:
            result = my_strcpy(token->data.s);
            break;
        case INT:
            snprintf(result, size, "int@%i", token->data.i);
            break;
        case DOUBLE:
            snprintf(result, size, "float@%f", token->data.d);
            break;
        case STR:
            while (token->data.s[i] != 0) {
                prilep(&result, token->data.s[i], &j, &size);
                i++;
            }
            char *tmp = my_strcpy(result);
            snprintf(result, size, "string@%s", tmp);
            break;
        default:
            result[0] = 0;

    }
    return result;
}

int code_type(int *dollar_source, t_token *input, TTable *local, TTable *func_table) {
    int i = input->token_type;

    switch (i) {
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
        case ID:
        {
            char *name = input->data.s;
            TElement *found = Tbl_GetDirect(local, name);
            if (found == NULL) {
                found = Tbl_GetDirect(func_table, name);
                if (found == NULL) {
                    semerror(ERR_SEM_DEF, line);
                    return -1;
                }
            }
            if (found->data->type == ST_Function && found->data->isDeclared) { //todo na konci parse overit ci vsetky f boli def
                return E_FUNC;
            } else if (found->data->type == ST_Variable && found->data->isDeclared) {
                return E_ID;
            } else {
                undefined_err(name, line);
                return -1;
            }

        }
        case INT:
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
            //rezervovane slova
            if (input->data.i == k_length){
                input->token_type = ID;
                input->data.s = "length";
                used_length = 1;
                return E_FUNC;
            } else if (input->data.i == k_subStr){
                input->token_type = ID;
                input->data.s = "substr";
                used_substr = 1;
                return E_FUNC;
            } else if (input->data.i == k_asc){
                input->token_type = ID;
                input->data.s = "asc";
                used_asc = 1;
                return E_FUNC;
            } else if (input->data.i == k_chr){
                input->token_type = ID;
                input->data.s = "chr";
                used_chr = 1;
                return E_FUNC;
            }
            *dollar_source = input->data.i;
            return E_DOLLAR;
        default:
            return -1;
    }
}

int should_convert(int first, int second) {
    if (first == second && is_data_type(first) && is_data_type(second)) {
        return 0;   //nekonvertuj
    }
    if ((first == E_integer && second == E_double) ||
            (first == E_double && second == E_integer)) {
        return 1;   //konvertuj
    }
    return -1;      //nekompatibilne
}

bool check_return_type(int expected, Element *el) {
    int actual_type = el->typ_konkretne;
    if (expected == E_void || expected < 0 ) {
        return true;
    }

    switch (should_convert(expected, actual_type)) {
        case 0:
            return true;
        case 1:
            //convert druhy //TODO generacia konverzie
            return true;
        default:
            incompatible_types_err(line);
            return false;
    }
}


/* Vraci kod tokenu, ktery ukoncil vyraz.
 * Pro klicova slova vraci data.i + 100, abychom se vyhli kolizi
 * navrat musi hlidat volajici funkce v parseru
 * Priklady takovych tokenu - EOL, Then, Semicolon
 * Pro Then je navratova hodnota tedy 120
 * TODO ked je -1 tak moze byt max 1 krat < > = != */
int expression(TTable *func_table, TTable *local, int typ, char **ret_var) {
    Stack stack;
    Stack_init(&stack);
    Stack_push(&stack, E_DOLLAR, NULL, 0);
    int a;
    int b;
    int dollar_source = 0;
    t_token *my_token = get_token();
    line = my_token->line;

    int last_type = -2;

    if (my_token->token_type == EOL && typ == -2) {
        *ret_var = NULL;
        return EOL;
    }

    b = code_type(&dollar_source, my_token, local, func_table); //prekodovani typu tokenu na index do tabulky
    if (b == E_DOLLAR && dollar_source == EOL) {
        Stack_dispose(&stack);
        return dollar_source;
    }
    do {
        a = Stack_top(&stack)->type;
        int ruleNumber = 0;
        int token_type = -1;
        if (my_token != NULL) {
            if (my_token->token_type == INT) {
                token_type = k_integer;
            } else if (my_token->token_type == DOUBLE) {
                token_type = k_double;
            } else if (my_token->token_type == STR) {
                token_type = k_string;
            } else if (my_token->token_type == ID) {
                TElement *el = Tbl_GetDirect(local, my_token->data.s);
                if (el == NULL) {
                    el = Tbl_GetDirect(func_table, my_token->data.s);
                    if (el == NULL) {
                        undefined_err(my_token->data.s, line);
                        return -1;
                    }
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

        switch (precedence_table[a][b]) {
            case EQ:
                Stack_push(&stack, b, token2operand(my_token), token_type);
                my_token = get_token();
                line = my_token->line;
                b = code_type(&dollar_source, my_token, local, func_table);
                break;
            case LT:
                Stack_expand(&stack);
                Stack_push(&stack, b, token2operand(my_token), token_type);
                my_token = get_token();
                line = my_token->line;
                b = code_type(&dollar_source, my_token, local, func_table);
                break;
            case GT:
                ruleNumber = rule(&stack, local, func_table, ret_var, &last_type);
                if (ruleNumber != 0) {
                    //printf("Rule %d\n", ruleNumber);
                } else {
                    syntax_error(ERR_SYNTA, line);
                }
                break;
            default:
                syntax_error(ERR_SYNTA, line);

        }

    } while (!(b == E_DOLLAR && Stack_top(&stack)->type == E_DOLLAR));

    Element *last_el = Stack_pop(&stack);
    if (last_el != NULL){               //asi zbytocne malo by to platit vzdy
        check_return_type(typ, last_el);
    } else {
        internall_err(__LINE__);
    }


    Stack_dispose(&stack);
    if (typ == -1){
        typ = k_boolean;
    }
    if (typ != -2){
        if (typ != last_type){
            char *result = my_strcpy(*ret_var);
            if (result[0]!= 'T' || result[1]!= 'F' ||result[2]!= '@' || result[3]!= '$'){
                *ret_var = gen_temp_var();
            }
            if (typ == k_double && last_type == k_integer){
                create_3ac(I_INT2FLOAT, result, NULL, *ret_var);
            } else if (typ == k_integer && last_type == k_double){
                create_3ac(I_FLOAT2R2EINT, result, NULL, *ret_var);
            } else {
                semerror(ERR_SEM_TYPE, line );
            }
        }
    }

    return dollar_source;
}
