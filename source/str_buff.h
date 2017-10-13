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

void null_buffer();
void append_buff(char c);
char *get_buff();
int buff_size();
void init_buff();


#endif //IFJ_PROJ_STR_BUFF_H
