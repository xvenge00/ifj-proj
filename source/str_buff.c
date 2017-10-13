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


t_str_buff buffer = {NULL,0,0};

void null_buffer(){
    for (unsigned i=0; i<buffer.top; i++){
        buffer.ret[i] = 0;
    }
    buffer.top = 0;
}

void append_buff(char c){
    if (buffer.top >= buffer.max - 1){
        buffer.ret = my_realloc(buffer.ret,sizeof(char) * (buffer.top + 100));
    }
    buffer.ret[buffer.top] = c;
    buffer.top++;
}

char *get_buff(){
    return buffer.ret;
}

int buff_size(){
    return buffer.top;
}
