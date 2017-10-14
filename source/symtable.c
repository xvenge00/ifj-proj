//Author: xrandy00 14.10.2017

#include "symtable.h"
#include "memwork.c"

#define DEFAULT_TABLE_SIZE 8;

//konstruktor funkce
TFunction *Func_Create(TType return_type, unsigned int attributes_count, TType *attributes_values){
    TFunction *function = NULL;
    function = my_malloc(sizeof(TFunction));
    if(function != NULL){
        function->return_type = return_type;
        function->attr_count = attributes_count;
        function->attributes = attributes_values;
    }
    return function;
}

//konstruktor promenne
TVariable * Var_Create(TValue value, TType type){
    TVariable *var = NULL;
    var = my_malloc(sizeof(TVariable));
    if(var != NULL){
        var->value = value;
        var->type = type;
    }
    return var;
}

//konstruktor symbolu
TSymbol *Sym_Create(Symbol_type type, TData *data, t_str_buff *name){
    TSymbol *symbol = NULL;
    symbol = my_malloc(sizeof(TSymbol));
    if(symbol != NULL){
        symbol->data = data;
        symbol->type = type;
        symbol->name = name;
    }
    return symbol;
}

//konstruktor elementu
TElement * El_Create(TSymbol * data, unsigned int table_size){
    TElement  *element = NULL;
    element = my_malloc(sizeof(TElement));
    if(element != NULL){
        element->data = data;
        element->next = NULL;
        element->key = data->name;
        element->hash = hash(element->key,table_size);
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
unsigned int hash(t_str_buff *str, unsigned int table_size) {

    unsigned int hash = 5381;
    for (unsigned int i = 0; i < str->top; ++i)
        hash = 33 * hash + str->ret[i];

    if(table_size < hash){
            hash = hash % table_size;
        }

    return hash;
}