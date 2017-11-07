//Author: xrandy00 14.10.2017

#include <stdbool.h>
#include "scanner.h"

#ifndef IFJ_PROJ_SYMTABLE_H
#define IFJ_PROJ_SYMTABLE_H

typedef enum {
    E_integer = k_integer,
    E_double = k_double,
    E_string = k_string,
    E_void = 42
} TType;

typedef union {
    int i;
    double d;
    char* s;
} TValue; //v zadani jsou jen tyhle tri

typedef struct {
    TValue value; //obsah
    int type; //viz enum TType
} TVariable;

typedef struct {
    int return_type; //to co vraci ma nejaky typ
    unsigned int attr_count;
    int *attributes; //pole integeru z TType
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
    char* name;
    bool isDeclared;
    bool isDefined;
} TSymbol;

typedef struct TElement{
    char* key; //identifikator
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
TData *Var_Create(TValue value, int type);
TData *Func_Create(int return_typ, unsigned int attributes_count, int * attribute_types);
TSymbol *Sym_Create(Symbol_type type, TData *data, char *name); //možná přidat isDeclared a isDefined i do konstruktoru?
TElement *El_Create(TSymbol *data); //element init
TTable *Tbl_Create(unsigned int size); //table constructor

//operace nad tabulkou viz IA

void El_Free(TElement* element); //uvolni postupne vsechny soucasti elementu az do nejnizsi urovne
int Tbl_Insert(TTable* tbl, TElement* el); //vlozi do tabulky tbl element el
bool Tbl_Search(TTable* tbl, char* name); //vraci true, pokud v tbl existuje element s name
TElement * Tbl_GetDirect(TTable* tbl, char* name);
void El_Delete(TTable* tbl, char* name); //smaze z tabulky element s name
void Tbl_Copy(TTable* tbl, char* name, TElement* el); //vraci v el to co je v tbl s name
void Tbl_Resize(TTable* tbl);
void Tbl_Delete(TTable *tbl);

unsigned int hash(char* str, unsigned int table_size);
#endif //IFJ_PROJ_SYMTABLE_H
