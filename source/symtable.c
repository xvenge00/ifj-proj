//Author: xrandy00 14.10.2017

#include "symtable.h"
#include "memwork.c"

#define DEFAULT_TABLE_SIZE 8;

//konstruktor funkce
TData *Func_Create(TType return_type, unsigned int attributes_count, TType *attributes_values){
    TFunction *function = NULL;
    function = my_malloc(sizeof(TFunction));
    if(function != NULL){
        function->return_type = return_type;
        function->attr_count = attributes_count;
        function->attributes = attributes_values;
    }

    TData *data = my_malloc(sizeof(TData*));
    data->func = function;
    return data;
}

//konstruktor promenne
TData * Var_Create(TValue value, TType type){
    TVariable *var = NULL;
    var = my_malloc(sizeof(TVariable));
    if(var != NULL){
        var->value = value;
        var->type = type;
    }

    TData *data = my_malloc(sizeof(TData*));
    data->var = var;
    return data;
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
        size_t size_list = sizeof(struct TElement *) * DEFAULT_TABLE_SIZE;
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


/*Vložení položky se složkami K a Data do tabulky T. Pokud tabulka T již
obsahuje položku s klíčem K dojde k přepisu datové složky Data novou
hodnotou. Tato vlastnost se podobá činnosti v kartotéce, kdy při existenci
staré karty se shodným klíčem se stará karta zahodí a vloží se nová (aktualizační
 sémantika operace TInsert).*/
int Tbl_Insert(TTable* tbl, TElement* el){
    //kontrola ukazatelu
    if(tbl == NULL || el == NULL){
        return ERR_INTER;
    }
    //vkladame na prvni misto
    if(tbl->list_firsts[el->hash] == NULL){
        tbl->list_firsts[el->hash] = el;
        tbl->count++;
    }
    else{
        TElement *active = tbl->list_firsts[el->hash];
        //cyklime pres prvky se stejnym hashem
        while(active->next != NULL){
            //stejny key - premazeme data
            if (active->key == el->key){
                my_free(active->data);
                active->data = el->data;

                tbl->count++;
                return 0;
            }
            active = active->next;
        }
        //k poslednimu se pres while nedostanu
        if (active->key == el->key){
            my_free(active->data);
            active->data = el->data;

            tbl->count++;
            return 0;
        }
        //pridani elementu na konec listu
        active->next=my_malloc(sizeof(TElement));
        active->next = el;
        tbl->count++;
        return 0;
    }
}

/*Predikát, který vrací hodnotu true v případě,že v tabulce T existuje položka s klíčem K
a hodnotu false v opačném případě. */
bool Tbl_Search(TTable* tbl, t_str_buff *name){
    if(tbl != NULL){
        bool found = false;
        for(unsigned int i = 0; i<tbl->size; i++){
            TElement *active = tbl->list_firsts[i];
            if(active != NULL){ //je tam prvni
                if(active->key == name){
                    found = true;
                    return found;
                }
                while (active->next != NULL){ //je jich vic
                    active = active->next;
                    if(active->key == name){
                        found = true;
                        return found;
                    }
                }
                if(active->key == name){
                    found = true;
                    return found;
                }
            }

        }
        return found;
    }
}

void El_Free(TElement* element){
    if(element->data->type == ST_Variable){
        my_free(element->data->data->var);
    }
    else {
        my_free(element->data->data->func->attributes);
        my_free(element->data->data->func);
    }

    my_free(element->data->data);
    my_free(element->data);
    my_free(element->key);
    my_free(element);
}

/*Operace rušení prvku s klíčem K v tabulce T.
V případě, že prvek neexistuje, má operace
sémantiku prázdné operace.*/
void Tbl_Delete(TTable* tbl, t_str_buff *name){
    if(tbl != NULL){
        for(unsigned int i = 0; i<tbl->size; i++){
            TElement *active = tbl->list_firsts[i];
            if(active != NULL){
                if(active->key == name){ //mazeme prvni
                    if(active->next!=NULL){
                        tbl->list_firsts[i]->next = active->next;
                        El_Free(active);
                        return;
                    }
                    El_Free(tbl->list_firsts[i]);
                    tbl->list_firsts[i] = NULL; //opetovny init pro dalsi pouziti
                    return;
                }

                while(active->next != NULL){

                    if(active->next->key == name){ //uprostred
                        TElement * tmp = active->next;
                        active->next = active->next->next;
                        El_Free(tmp);
                        return;
                    }
                    active = active->next;
                }

                if(active->key == name){ //mazeme posledni
                    tbl->list_firsts[i]->next = active->next;
                    El_Free(active);
                    return;
                }
            }


        }
    }
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