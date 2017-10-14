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
    E_String
} TType;

typedef union {
    int i;
    double d;
    t_str_buff s;
} TValue; //v zadani jsou jen tyhle tri

typedef struct {
    TValue value; //obsah
    TType type; //datovy typ - enum
    t_str_buff *name;
} TVariable;

typedef struct {
    //TValue return_value; //funkce neco vraci
    TType return_type; //to co vraci ma nejaky typ
    unsigned int attr_count;
    TType *attributes;
    t_str_buff *name;
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
} TSymbol;

typedef struct {
    unsigned int key; //hash jmena
    TSymbol* data; //pointer na symbol obsahujici data a datovy typ
    struct TElement* next; // link na dalsi prvek seznamu
} TElement; //prvek seznamu elementu se stejnym key (hashle jmeno)

typedef struct {
    unsigned int size;
    unsigned int count;
    TElement **list_firsts; //pole ukazatelu na prvni TElement v seznamu
} TTable; //samotna tabulka

//konstruktory vsech struktur nadeklarovanych vyse
TVariable *Var_Create(TValue value, TType type, t_str_buff *name);
TFunction *Func_Create(TType return_type, t_str_buff *name,unsigned int attributes_count, TType * attributes_values);
TSymbol *Sym_Create(Symbol_type type, TData *data);
TElement *El_Create(TSymbol *data, unsigned int key); //element init
TTable *Tbl_Create(); //table constructor

//operace nad tabulkou viz IAL prednaska

int Tbl_Insert(TTable* tbl, TElement* el); //vlozi do tabulky tbl element el
bool Tbl_Search(TTable* tbl, t_str_buff name); //vraci true, pokud v tbl existuje element s name
int Tbl_Delete(TTable* tbl, t_str_buff name); //smaze z tabulky element s name
void Tbl_Copy(TTable* tbl, t_str_buff name, TElement* el); //vraci v el to co je v tbl s name

unsigned int hash(t_str_buff* str, TTable* tbl);
#endif //IFJ_PROJ_SYMTABLE_H
