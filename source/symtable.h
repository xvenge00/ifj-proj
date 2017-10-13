#ifndef IFJ_PROJ_SYMTABLE_H
#define IFJ_PROJ_SYMTABLE_H

typedef struct {
    char *letters;
    int length;
} TString;

typedef enum {
    T_integer = 0,
    T_double,
    T_String
} TType;

typedef union {
    int i;
    double d;
    TString s;
} TValue;

typedef struct {
    TValue value;
    TType type;
    TString name;
} Variable;

typedef struct {
    TValue return_value;
    TType return_type;
    int data_address; //TODO: change to link to function attributes
    TString name;
} Function;

typedef union {
    Variable var;
    Function func;
} TData;

typedef enum {
    ST_Variable = 0,
    ST_Function
} Symbol_type;

typedef struct {
    Symbol_type type;
    TData data;
} TSymbol;

typedef struct {
    unsigned int key;
    TElement* next;
    TSymbol* data;
} TElement;

typedef struct {
    unsigned int size;
    unsigned int count;
    TElement* list_first[];
} TTable;
#endif //IFJ_PROJ_SYMTABLE_H
