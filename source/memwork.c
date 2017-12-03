/**
 * bytvara mem_pole ukazovatelov ktore boly naalokovane
 */
#include "scanner.h"
#include <stdlib.h>
#include <stdio.h>

#include "memwork.h"
#include "err.h"

void **mem_pole = NULL;
unsigned mem_max_size = 0;
unsigned mem_size = 0;

void ERR_ALLOC(){
    clear_all();
    fprintf(stderr, "ERR 99 -- chyba v alokacii pamety\n");
    exit(ERR_INTER);

}

void clear_all(){
    for (unsigned i=0; i<mem_size; i++){
        free(mem_pole[i]);
    }
    free(mem_pole);
}

void change_size(unsigned new){
    if (mem_size < new){
        void **tmp;
        tmp = malloc(sizeof(void *)*new);
        if (tmp == NULL){
            ERR_ALLOC();
        }
        for (unsigned i=0;i<mem_size;i++){
            tmp[i] = mem_pole[i];
        }
        if (mem_pole != NULL){
            free(mem_pole);
        }
        mem_pole = tmp;
        mem_max_size = new;
    }
}

void append(void *ptr){
    if(mem_size >= mem_max_size){
        change_size(mem_max_size + 10);
    }
    mem_pole[mem_size] = ptr;
    mem_size += 1;
}

void replace(void *ptr, void *new){
    unsigned i = 0;
    for (; i<mem_size; i++){
        if (mem_pole[i] == ptr){
            mem_pole[i] = new;
            break;
        }
    }
    if (i == mem_size){
        append(new);
    }
}

void delete(void *ptr){
    for (unsigned i=0; i<mem_size; i++){
        if (mem_pole[i] == ptr){
            free(mem_pole[i]);
            mem_size--;
            mem_pole[i] = mem_pole[mem_size];
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
    void *new_ptr;
    new_ptr = realloc(ptr, new_size);
    if (new_ptr == NULL){
        ERR_ALLOC();
    }
    replace(ptr, new_ptr);
    return new_ptr;
}

void my_free(void *ptr){
    delete(ptr);
}
