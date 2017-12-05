/*  * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *
 *  Implementace prekladace imperativniho jazyka IFJ17
 *
 *  Autori:
 *      xvenge00 - Adam Venger
 *      xbabka01 - Peter Babka
 *      xrandy00 - Vojtech Randysek
 *      xdosed08 - Ondrej Dosedel
 *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#include <stdio.h>
#include "scanner.h"
#include "parser.h"
#include "symtable.h"
#include "codegen.h"
#include "err.h"
#include "memwork.h"

void declare_built_in(TTable* func_table){
    /**
     * deklaracia vstavanych f
     */
    int *type_att = NULL;
    TData *func = NULL;
    TSymbol *symb = NULL;
    TElement *ele = NULL;

    //Length(s As String) As Integer
    type_att = my_malloc(sizeof(int));
    *type_att = k_string;
    func = Func_Create(k_integer,1,type_att);
    symb = Sym_Create(ST_Function, func, "length");
    symb->isDefined = true;
    symb->isDeclared = true;
    ele = El_Create(symb);
    Tbl_Insert(func_table, ele);

    //SubStr(s As String, i As Integer, n As Integer) As String
    type_att = my_malloc(sizeof(int)*3);
    type_att[0] = k_string;
    type_att[1] = k_integer;
    type_att[2] = k_integer;
    func = Func_Create(k_string,3,type_att);
    symb = Sym_Create(ST_Function, func, "substr");
    symb->isDefined = true;
    symb->isDeclared = true;
    ele = El_Create(symb);
    Tbl_Insert(func_table, ele);


    //Asc(s As String, i As Integer) As Integer
    type_att = my_malloc(sizeof(int)*2);
    type_att[0] = k_string;
    type_att[1] = k_integer;
    func = Func_Create(k_integer,2,type_att);
    symb = Sym_Create(ST_Function, func, "asc");
    symb->isDefined = true;
    symb->isDeclared = true;
    ele = El_Create(symb);
    Tbl_Insert(func_table, ele);

    //Chr(i As Integer) As String
    type_att = my_malloc(sizeof(int));
    *type_att = k_integer;
    func = Func_Create(k_string,1,type_att);
    symb = Sym_Create(ST_Function, func, "chr");
    symb->isDefined = true;
    symb->isDeclared = true;
    ele = El_Create(symb);
    Tbl_Insert(func_table, ele);
}


int main(int argc, char** argv) {

    /* z dovodov testovania */
    /* neviem automaticky testovat vstup zo stdin */
    /* namiesto getchar pouzivajte teda fgetc(FILE *stream) */
    if (argc == 2){
        f = fopen(argv[1],"r");
        if (f == NULL) {
            fprintf(stderr, "Subor sa nepodarilo otvorit\n");
        }
    } else {                        //ked neni zadany argument, cita vstup zo stdin
        f = stdin;
    }

    TTable* func_table = Tbl_Create(8);

    declare_built_in(func_table);

    parse(func_table);
    all_defined(func_table);
    generate_code();

    clear_all();
    if (argc == 2) {    //ak bol otvoreny subor, zatvor ho
        fclose(f);
    }

    return 0;
}
