#ifndef IFJ_PROJ_SCANNER_H
#define IFJ_PROJ_SCANNER_H

#include <stdio.h>

#define empty_token 0

FILE *f;


char key_worlds[35][10];
unsigned key_size;
unsigned min_key;



//nemenit !!!
// toto su stavy automatu
// kazdy stav musi mat ine cislo !!!
typedef enum {
    s_START = 0,
    s_ID,
    s_block_coment_0,
    s_block_coment_1,
    s_block_coment_2,
    s_line_comment,
    s_INT,
    s_double_0,
    s_double_1,
    s_double_2,
    s_double_3,
    s_str0,
    s_strL,
    s_str_spec,
    s_str_spec_hexa0,
    s_str_spec_hexa1,
    s_LT,
    s_GT,
} tstate;


//zaheslovane typ tokenu    //todo zmenit tak aby sedeli s ostatnymy
//todo skonrolovat ci su to vsetky mozne typy
typedef enum {
    EMPTY = 0,
    ID,
    INT,
    DOUBLE,
    STR,
    PLUS,
    MINUS,
    KRAT,
    DELENO,
    EQ,
    NEQ,
    LT,
    LE,
    GT,
    GE,
    MOD,
    EOL,
    END,
    LPAR,
    RPAR,
    comma,
    MIN_KEY_WORLD
} ttype;


typedef union {
    int i;
    double d;
    char *s;
} tdata;

/**
 * klucove slova jazyka
 */
//const char *key_worlds[]; //todo vyhadzuje warning
/**
 * struktura tokeny prve je typ na ktore su definovane vyzsie makra a druhe je ukazovatel na data ktoreho typ urcime z cisla
 * napriklad ak je typ int tak tam bude int ak bude double tak desatinne cislo ak retazec tak to bude pointer na char atd
 */
typedef struct {
    int token_type;      // typ tokenu
    tdata data;
} t_token;

/**
 * vrati token tvaru predchadzajucej struktury nacitava zo stdin
 * @return
 */
t_token get_token();

#endif //IFJ_PROJ_SCANNER_H
