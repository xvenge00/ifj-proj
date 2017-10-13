//
// Created by peter on 13.10.2017.
//

#ifndef IFJ_PROJ_MEMWORK_H
#define IFJ_PROJ_MEMWORK_H

void clear_all();
void *my_malloc(size_t size);
void *my_realloc(void *ptr,size_t new_size);
void my_free(void *ptr);


#endif //IFJ_PROJ_MEMWORK_H
