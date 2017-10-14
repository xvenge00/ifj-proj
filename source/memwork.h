//
// Created by peter on 13.10.2017.
//

#ifndef IFJ_PROJ_MEMWORK_H
#define IFJ_PROJ_MEMWORK_H

#include <stdlib.h>
/**
 * sluzi na uvolnenie vsetkej pamety ktora bola naalokovana my_malloc a my_relloc
 */
void clear_all();
/**
 * rovnake ako malloc len prida pointer aj do pola aby sme vedeli uvolnit clear_all
 * @param size kolko
 * @return pointer kde
 */
void *my_malloc(size_t size);
/**
 * rovnake ako realloc len prida pointer aj do pola aby sme vedeli uvolnit clear_all
 * @param ptr co zmenit
 * @param new_size na aku velikost
 * @return kde sa to teraz nachadza
 */
void *my_realloc(void *ptr,size_t new_size);
/**
 * funguje rovnako ako free len v poli naalokovanych najde a nahradi NULL todo mozno prerobit aby sa celkovo vymazalo
 * @param ptr co uvolnit
 */
void my_free(void *ptr);


#endif //IFJ_PROJ_MEMWORK_H
