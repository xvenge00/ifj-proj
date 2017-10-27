//Author: xrandy00 14.10.2017

#include "symtable.h"
#include "memwork.h"
#include <stdio.h>

#define DEFAULT_TABLE_SIZE 8;

TFunction *Func_Create(TType return_type, t_str_buff *name, unsigned int attributes_count, TType *attributes_values){
    TFunction *function = NULL;
    function = my_malloc(sizeof(TFunction));
    if(function != NULL){
        function->return_type = return_type;
        function->name = name;
        function->attr_count = attributes_count;
        function->attributes = attributes_values;
    }
    return function;
}


TVariable * Var_Create(TValue value, TType type, t_str_buff *name){
    TVariable *var = NULL;
    var = my_malloc(sizeof(TVariable));
    if(var != NULL){
        var->value = value;
        var->type = type;
        var->name = name;
    }
    return var;
}
//konstruktor symbolu
TSymbol *Sym_Create(Symbol_type type, TData *data){
    TSymbol *symbol = NULL;
    symbol = my_malloc(sizeof(TSymbol));
    if(symbol != NULL){
        symbol->data = data;
        symbol->type = type;
    }
    return symbol;
}

//konstruktor elementu
TElement * El_Create(TSymbol * data, unsigned int key){
    TElement  *element = NULL;
    element = my_malloc(sizeof(TElement));
    if(element != NULL){
        element->data = data;
        element->next = NULL;
        element->key = key;
    }
    return element;
}

//konstruktor tabulky na defaultni velikost
TTable * Tbl_Create(){
    //alokace samotne tabulky
    TTable *tbl = NULL;
    size_t size_table = sizeof(TTable);
    tbl = my_malloc(size_table);

    if(tbl!=NULL){
        tbl->size = DEFAULT_TABLE_SIZE;
        tbl->count = 0;
        tbl->list_firsts = NULL;

        //alokace pole ukazatelu na prvky
        size_t size_list = sizeof(TElement *) * DEFAULT_TABLE_SIZE;
        tbl->list_firsts = my_malloc(size_list);

        //inicializace ukazatelu na NULL
        if(tbl->list_firsts != NULL){
            for(unsigned i = 0; i < tbl->size; i++){
                tbl->list_firsts[i] = NULL;
            }
        }
    }

    return tbl;
}



//Bernsteinova funkce, source https://www.strchr.com/hash_functions
unsigned int hash(t_str_buff *str, TTable *tbl) {

    unsigned int hash = 5381;
    for (unsigned int i = 0; i < str->top; ++i)
        hash = 33 * hash + str->ret[i];

    if(tbl){
        if(tbl->size < hash){
            hash = hash % tbl->size;
        }
    }

    return hash;
}