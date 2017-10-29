/**
 * bytvara pole ukazovatelov ktore boly naalokovane
 */
#include "scanner.h"
#include <stdlib.h>
#include <stdio.h>

#include "memwork.h"
#include "main.h"

void **pole = NULL;
unsigned max = 0;
unsigned top = 0;

void ERR_ALLOC(){
    clear_all();
    fprintf(stderr, "ERR 99 -- chyba v alokacii pamety\n");
    exit(ERR_INTER);

}

void clear_all(){
    for (unsigned i=0; i<top; i++){
        free(pole[i]);
    }
    free(pole);
}

void change_size(unsigned new){
    if (top < new){
        void **tmp;
        tmp = malloc(sizeof(void *)*new);
        if (tmp == NULL){
            ERR_ALLOC();
        }
        for (unsigned i=0;i<top;i++){
            tmp[i] = pole[i];
        }
        free(pole);
        pole = tmp;
        max = new;
    }
}

void append(void *ptr){
    if(top >= max){
        change_size(max + 10);
    }
    pole[top] = ptr;
    top += 1;
}

void replace(void *ptr, void *new){
    unsigned i = 0;
    for (; i<top; i++){
        if (pole[i] == ptr){
            pole[i] = new;
            break;
        }
    }
    if (i == top){
        append(new);
    }
}

void delete(void *ptr){
    for (unsigned i=0; i<top; i++){
        if (pole[i] == ptr){
            free(pole[i]);
            top--;
            pole[i] = pole[top];
            break;
        }
    }
}

void *my_malloc(size_t size){
    void *tmp = malloc(size);
    if (tmp == NULL){
        ERR_ALLOC();
    }
    append(tmp);
    return tmp;
}

void *my_realloc(void *ptr,size_t new_size){
    void *tmp;
    tmp = realloc(ptr, new_size);
    if (tmp == NULL){
        ERR_ALLOC();
    }
    replace(ptr, tmp);
    return tmp;
}

void my_free(void *ptr){
    delete(ptr);
}