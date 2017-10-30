//Author: xrandy00 14.10.2017

#include "symtable.h"
#include "memwork.h"
#include <string.h>
#include "main.h"

//konstruktor funkce
TData *Func_Create(TType return_type, unsigned int attributes_count, TType *attributes_values){
    TFunction *function = NULL;
    function = my_malloc(sizeof(TFunction));
    if(function != NULL){
        function->return_type = return_type;
        function->attr_count = attributes_count;
        function->attributes = attributes_values;
    }

    TData *data = my_malloc(sizeof(TData));
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

    TData *data = my_malloc(sizeof(TData));
    data->var = var;
    return data;
}

//konstruktor symbolu
TSymbol *Sym_Create(Symbol_type type, TData *data, char* name){
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
TElement * El_Create(TSymbol * data){
    TElement  *element = NULL;
    element = my_malloc(sizeof(TElement));
    if(element != NULL){
        element->data = data;
        element->next = NULL;
        element->key = data->name;
    }
    return element;
}

//konstruktor tabulky na defaultni velikost
TTable * Tbl_Create(unsigned int size){
    //alokace samotne tabulky
    TTable *tbl = NULL;
    size_t size_table = sizeof(TTable);
    tbl = my_malloc(size_table);

    if(tbl!=NULL){
        tbl->size = size;
        tbl->count = 0;
        tbl->list_firsts = NULL;

        //alokace pole ukazatelu na prvky
        size_t size_list = sizeof(TElement) * tbl->size;
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

void Tbl_Resize(TTable* tbl){

    TTable* newTbl = NULL;

    if(tbl != NULL){
        newTbl = Tbl_Create(tbl->size * 2);
    }

    if(newTbl != NULL){
        for (int i = 0; i < tbl->size; i++) {
            TElement * tmp = NULL;
            TElement * temp = NULL;

            if(tbl->list_firsts[i] != NULL){ //ve stare tabulce na prvni pozici neco je
                tbl->list_firsts[i]->hash = hash(tbl->list_firsts[i]->key,newTbl->size); //prehashujem
                tmp = tbl->list_firsts[i]->next; //naslednik v tmp
                tbl->list_firsts[i]->next = NULL; //vynulovani nextu prvniho prvku
                Tbl_Insert(newTbl, tbl->list_firsts[i]); //vlozim prvni prvek do nove table

                if(tmp != NULL){ //v nextu neco bylo
                    temp = tmp; //schovam si ukazatel dal
                    temp->hash = hash(temp->key,newTbl->size); //prehashuju temp
                }

                while(temp != NULL){ //vlozeni vsech dalsich el do nove tably
                    tmp = temp->next; //posunu ukazatel dal
                    temp->next = NULL; //vynuluju next
                    Tbl_Insert(newTbl, temp); //vlozim do nove s vynulovanym nextem
                    if(tmp != NULL){ //pokud neco bylo dal
                        temp = tmp; //aby sedela podminka cyklu
                        temp->hash = hash(temp->key, newTbl->size); //prehashovat
                    }
                    temp = tmp; //aby sedela podminka cyklu

                }
            }
        }
        my_free(tbl->list_firsts);
        *tbl = *newTbl;
        my_free(newTbl);
    }
}

/* Inkrementuje velikost tabulky, popripade vola resize. */
void Tbl_Increment(TTable* tbl){
    tbl->count++;
    if(tbl->count >= tbl->size*0.75){
        Tbl_Resize(tbl);
    }
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
    el->hash = hash(el->key,tbl->size);

    //vkladame na prvni misto
    if(tbl->list_firsts[el->hash] == NULL){
        tbl->list_firsts[el->hash] = el;
        Tbl_Increment(tbl);
        return 0;
    }
    else{
        TElement *active = tbl->list_firsts[el->hash];

        //cyklime pres prvky se stejnym hashem
        while(active->next != NULL){
            //stejny key - premazeme data
            if (active->key == el->key){
                active->data = el->data;
                active->next = el->next;
                active->hash = el->hash;
                my_free(active->data->data);
                //my_free(active->data->name);

                return 0;
            }
            active = active->next;
        }
        //k poslednimu se pres while nedostanu
        if (active->key == el->key){
            active->data = el->data;
            active->next = el->next;
            active->hash = el->hash;
            my_free(active->data->data);
            //my_free(active->data->name);
            return 0;
        }
        //pridani elementu na konec listu
        active->next = el;
        return 0;
    }
}

/*Predikát, který vrací hodnotu true v případě,že v tabulce T existuje položka s klíčem K
a hodnotu false v opačném případě. */
bool Tbl_Search(TTable* tbl, char *name){
    bool found = false;
    if(tbl != NULL){
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
    return found;
}

void El_Free(TElement* element){
    if(element->data->type == ST_Variable){
        my_free(element->data->data->var);
        my_free(element->data->data);

    }
    else {
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
void El_Delete(TTable* tbl, char* name){
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

void Tbl_Delete(TTable * tbl){
    if(tbl != NULL){
        TElement * tmp = NULL;
        TElement * tmp2 = NULL;
        for(int i = 0; i < tbl->size; i++){
            if(tbl->list_firsts[i] != NULL) { //neco v tom seznamu je
                if(tbl->list_firsts[i]->next != NULL){ //je tam vic nez jeden
                    tmp = tbl->list_firsts[i]->next;
                    El_Free(tbl->list_firsts[i]);

                    while (tmp != NULL) //cyklime do konce seznamu
                    {
                        tmp2 = tmp;
                        tmp = tmp->next;

                        El_Free(tmp2);
                    }
                }
                    //je jen jeden
                else {
                    El_Free(tbl->list_firsts[i]);
                    tbl->count--;
                }

            } //if neni radek null
        }//for pres radky
        my_free(tbl->list_firsts);
        my_free(tbl);

    }
}



//Bernsteinova funkce, source https://www.strchr.com/hash_functions
unsigned int hash(char* str, unsigned int table_size) {

    unsigned int hash = 5381;
    for (unsigned int i = 0; i < strlen(str); ++i)
        hash = 33 * hash + str[i];

    if(table_size < hash){
        hash = hash % table_size;
    }

    return hash;
}