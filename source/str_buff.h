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

void null_buffer(t_str_buff *buffer);
void append_buff(t_str_buff *buffer,char c);
char *get_buff(t_str_buff *buffer);
int buff_size(t_str_buff *buffer);
t_str_buff *init_buff(t_str_buff *buffer);


#endif //IFJ_PROJ_STR_BUFF_H
