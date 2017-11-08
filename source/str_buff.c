//
// Created by peter on 13.10.2017.
//

#include "str_buff.h"
#include <stdio.h>
#include "scanner.h"
#include <string.h>
#include <ctype.h>
#include <stdlib.h>
#include "memwork.h"
#include "main.h"


void null_buffer(t_str_buff *buffer){
    for (unsigned i=0; i<buffer->top; i++){
        buffer->ret[i] = 0;
    }
    buffer->top = 0;
}

void append_buff(t_str_buff *buffer, char c){
    // ak maly buffer zvacsi ho
    if (buffer->top >= buffer->max - 1){
        buffer->max += 50;
        buffer->ret = my_realloc(buffer->ret,sizeof(char) * buffer->max);

        for (unsigned i = buffer->top + 1; i < buffer->max; ++i) {
            buffer->ret[i] = 0;
        }
    }
    buffer->ret[buffer->top] = c;
    buffer->top++;
}

char *get_buff(t_str_buff *buffer){
    return buffer->ret;
}

int buff_size(t_str_buff *buffer){
    return buffer->top;
}

t_str_buff *init_buff(){
    //konstruktor
    t_str_buff *buffer;

    buffer = my_malloc(sizeof(t_str_buff));
    buffer->top = 0;
    buffer->ret = my_malloc(sizeof(char) * 101);
    buffer->max = 101;
    for (unsigned i=0; i<buffer->max; i++){
        buffer->ret[i] = 0;
    }
    return buffer;
}