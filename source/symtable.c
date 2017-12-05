//Author: xrandy00 14.10.2017

#include "symtable.h"
#include "memwork.h"
#include <string.h>
#include "err.h"

//konstruktor funkce
TData *Func_Create(int return_type, unsigned int attributes_count, int *attributes_values) {
    TFunction *function = NULL;
    function = my_malloc(sizeof(TFunction));

    function->return_type = return_type;
    function->attr_count = attributes_count;
    if (attributes_count == 0) {
        if (attributes_values == NULL) {
            function->attributes = NULL;
        } else {
            my_free(function);
            return NULL;
        }
    } else {
        if (attributes_values != NULL) {
            function->attributes = attributes_values;
        } else {
            my_free(function);
            return NULL;
        }
    }

    TData *data = my_malloc(sizeof(TData));
    data->func = function;
    return data;
}

//konstruktor promenne
TData *Var_Create(TValue value, int type) {
    TVariable *var = NULL;
    var = my_malloc(sizeof(TVariable));
    if (var != NULL) {
        var->value = value;
        var->type = type;
    }

    TData *data = my_malloc(sizeof(TData));
    data->var = var;
    return data;
}

//konstruktor symbolu
TSymbol *Sym_Create(Symbol_type type, TData *data, char *name) {
    if (data == NULL || name == NULL || (type != ST_Function && type != ST_Variable)){
        internall_err(__LINE__, __FILE__);
    }
    TSymbol *symbol = NULL;
    symbol = my_malloc(sizeof(TSymbol));
    if (symbol != NULL) {
        symbol->data = data;
        symbol->type = type;
        symbol->name = name;
    }
    return symbol;
}

//konstruktor elementu
TElement *El_Create(TSymbol *data) {
    TElement *element = NULL;
    element = my_malloc(sizeof(TElement));
    if (element != NULL) {
        element->data = data;
        element->next = NULL;
        element->key = data->name;
    }
    return element;
}

//konstruktor tabulky na defaultni velikost
TTable *Tbl_Create(unsigned int size) {
    //alokace samotne tabulky
    TTable *tbl = NULL;
    size_t size_table = sizeof(TTable);
    tbl = my_malloc(size_table);

    if (tbl != NULL) {
        tbl->size = size;
        tbl->isScope = false;
        tbl->count = 0;
        tbl->list_firsts = NULL;

        //alokace pole ukazatelu na prvky
        size_t size_list = sizeof(TElement) * tbl->size;
        tbl->list_firsts = my_malloc(size_list);

        //inicializace ukazatelu na NULL
        if (tbl->list_firsts != NULL) {
            for (unsigned i = 0; i < tbl->size; i++) {
                tbl->list_firsts[i] = NULL;
            }
        }
    }
    return tbl;
}

void Tbl_Resize(TTable *tbl) {

    TTable *newTbl = NULL;

    if (tbl != NULL) {
        newTbl = Tbl_Create(tbl->size * 2);
    }

    if (newTbl != NULL) {
        for (unsigned i = 0; i < tbl->size; i++) {
            TElement *tmp = tbl->list_firsts[i];
            TElement *temp = NULL;

            while (tmp != NULL) {
                temp = tmp->next;
                Tbl_Insert(newTbl, tmp);
                tmp = temp;
            }

        }
        my_free(tbl->list_firsts);
        *tbl = *newTbl;
        my_free(newTbl);
    }
}

/* Inkrementuje velikost tabulky, popripade vola resize. */
void Tbl_Increment(TTable *tbl) {
    tbl->count++;
    if (tbl->count >= tbl->size * 0.75) {
        Tbl_Resize(tbl);
    }
}

/*Vložení položky se složkami K a Data do tabulky T. Pokud tabulka T již
obsahuje položku s klíčem K dojde k přepisu datové složky Data novou
hodnotou. Tato vlastnost se podobá činnosti v kartotéce, kdy při existenci
staré karty se shodným klíčem se stará karta zahodí a vloží se nová (aktualizační
 sémantika operace TInsert).*/
int Tbl_Insert(TTable *tbl, TElement *el) {
    //kontrola ukazatelu

    if (tbl == NULL || el == NULL) {
        internall_err(__LINE__, __FILE__);
    }
    TElement *tmp = Tbl_GetDirect(tbl, el->key);
    if (tmp != NULL) {
        tmp->data = el->data;
    } else {
        el->hash = hash(el->key, tbl->size);
        el->next = tbl->list_firsts[el->hash];
        tbl->list_firsts[el->hash] = el;
        Tbl_Increment(tbl);
    }
    return 0;

}

/*Predikát, který vrací hodnotu true v případě,že v tabulce T existuje položka s klíčem K
a hodnotu false v opačném případě. */
bool Tbl_Search(TTable *tbl, char *name) {
    bool found = false;
    if (tbl != NULL) {
        int el_hash = hash(name, tbl->size);
        TElement *tmp = tbl->list_firsts[el_hash];
        TElement *temp = NULL;
        while (tmp != NULL) {
            temp = tmp->next;
            if (strcmp(tmp->key, name) == 0) {
                return true;
            }
            tmp = temp;
        }
    }
    return found;
}

/* Vrací ukazatel na prvek tabulky, umožňuje přímý přístup k datům, funguje na stejném principu jako Search.
 * Vrací NULL pokud element s keky name není nalezen.
 * */
TElement *Tbl_GetDirect(TTable *tbl, char *name) {
    TElement *found = NULL;
    if (tbl != NULL) {

        int el_hash = hash(name, tbl->size);
        TElement *tmp = tbl->list_firsts[el_hash];
        TElement *temp = NULL;
        while (tmp != NULL) {
            temp = tmp->next;
            if (strcmp(tmp->key, name) == 0) {
                return tmp;
            }
            tmp = temp;
        }

    }

    return found;
}


void El_Free(TElement *element) {
    if (element->data->type == ST_Variable) {
        my_free(element->data->data->var);
        my_free(element->data->data);

    } else {
        my_free(element->data->data->func->attributes);
        my_free(element->data->data->func);
        my_free(element->data->data);
    }
    my_free(element->data);

    my_free(element);
}

/*Operace rušení prvku s klíčem name v tabulce tbl.
V případě, že prvek neexistuje, má operace
sémantiku prázdné operace.*/
void El_Delete(TTable *tbl, char *name) {
    if (tbl != NULL) {
        int el_hash = hash(name, tbl->size);
        TElement *tmp = tbl->list_firsts[el_hash];
        TElement *temp = NULL;
        while (tmp != NULL) {
            if (strcmp(tmp->key, name) == 0) {
                if (temp == NULL){
                    tbl->list_firsts[el_hash] = tmp->next;
                } else {
                    temp->next = tmp->next;
                }
                El_Free(tmp);
                return;
            }
            temp = tmp;
            tmp = tmp->next;
        }

    }
}

void Tbl_Delete(TTable *tbl) {
    if (tbl != NULL) {
        TElement *tmp = NULL;
        TElement *tmp2 = NULL;
        for (unsigned i = 0; i < tbl->size; i++) {
            TElement *tmp = tbl->list_firsts[i];
            TElement *temp = NULL;
            while (tmp != NULL) {
                temp = tmp->next;
                El_Free(tmp);
                tmp = temp;
            }
        }//for pres radky
        my_free(tbl->list_firsts);
        my_free(tbl);

    }
}


//Bernsteinova funkce, source https://www.strchr.com/hash_functions
unsigned int hash(char *str, unsigned int table_size) {

    unsigned int hash = 5381;
    for (unsigned int i = 0; i < strlen(str); ++i)
        hash = 33 * hash + str[i];

    if (table_size < hash) {
        hash = hash % table_size;
    }

    return hash;
}