//Author: xrandy00 14.10.2017

#include <stdbool.h>
#include "str_buff.h"

#ifndef IFJ_PROJ_SYMTABLE_H
#define IFJ_PROJ_SYMTABLE_H

/*typedef struct {
    char *letters;
    int length; //aktualni delka
    int capacity; //maximalni naalokovana delka
} TString; //vlastni string - array charu
 */

typedef enum {
    E_integer = 0,
    E_double,
    E_string,
    E_void
} TType;

typedef union {
    int i;
    double d;
    t_str_buff s;
} TValue; //v zadani jsou jen tyhle tri

typedef struct {
    TValue value; //obsah
    TType type; //datovy typ - enum
} TVariable;

typedef struct {
    TType return_type; //to co vraci ma nejaky typ
    unsigned int attr_count;
    TType *attributes;
} TFunction;

typedef union {
    TVariable *var; //var ma jine atributy nez function
    TFunction *func;

} TData;

typedef enum {
    ST_Variable = 0,
    ST_Function
} Symbol_type;

typedef struct {
    Symbol_type type; //enum var/func
    TData *data;
    t_str_buff *name;
} TSymbol;

typedef struct TElement{
    t_str_buff *key; //identifikator
    unsigned int hash; //hash identifikatoru
    TSymbol* data; //pointer na symbol obsahujici data a datovy typ
    struct TElement * next; // link na dalsi prvek seznamu
} TElement; //prvek seznamu elementu se stejnym key (hashle jmeno)

typedef struct {
    unsigned int size; //na kolik je naalokovano
    unsigned int count; //obsazenych ukazatelu
    TElement **list_firsts; //pole ukazatelu na prvni TElement v seznamu
} TTable; //samotna tabulka

//konstruktory vsech struktur nadeklarovanych vyse
TData *Var_Create(TValue value, TType type);
TData *Func_Create(TType return_typ, unsigned int attributes_count, TType * attributes_values);
TSymbol *Sym_Create(Symbol_type type, TData *data,t_str_buff *name);
TElement *El_Create(TSymbol *data, unsigned int table_size); //element init
TTable *Tbl_Create(unsigned int size); //table constructor

//operace nad tabulkou viz IA

void El_Free(TElement* element); //uvolni postupne vsechny soucasti elementu az do nejnizsi urovne
int Tbl_Insert(TTable* tbl, TElement* el); //vlozi do tabulky tbl element el
bool Tbl_Search(TTable* tbl, t_str_buff *name); //vraci true, pokud v tbl existuje element s name
void El_Delete(TTable* tbl, t_str_buff *name); //smaze z tabulky element s name
void Tbl_Copy(TTable* tbl, t_str_buff *name, TElement* el); //vraci v el to co je v tbl s name
void Tbl_Resize(TTable* tbl);
void Tbl_Delete(TTable *tbl);

unsigned int hash(t_str_buff* str, unsigned int table_size);
#endif //IFJ_PROJ_SYMTABLE_H
