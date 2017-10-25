//
// Created by peter on 13.10.2017.
//

#ifndef IFJ_PROJ_STR_BUFF_H
#define IFJ_PROJ_STR_BUFF_H

typedef struct {
    char *ret;
    unsigned top;
    unsigned max;
} t_str_buff;

/**
 * funkcia ktora nuluje obsah buferra a nastavy top na 0
 * @param buffer co ca ma vyprazdnit
 */
void null_buffer(t_str_buff *buffer);
/**
 * prida na konec bufferu znak ak nieje dost miesta tak zvasci buffer
 * @param buffer do coho
 * @param c co
 */
void append_buff(t_str_buff *buffer,char c);
/**
 * vrati odkaz na retazec v buffery
 * @param buffer ktory buffer
 * @return pointer na retazec bufferu
 */
char *get_buff(t_str_buff *buffer);
/**
 * vrati pocet ynakou ulozenych do bufferu treba priratat 1 lebo retazec ma mat na konci 0
 * @param buffer 
 * @return 
 */
int buff_size(t_str_buff *buffer);
/**
 * konstruktor bufferu vytvory novy buffer 
 * @return pointer na novy buffer
 */
t_str_buff *init_buff();


#endif //IFJ_PROJ_STR_BUFF_H
