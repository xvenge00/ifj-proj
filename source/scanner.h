#ifndef IFJ_PROJ_SCANNER_H
#define IFJ_PROJ_SCANNER_H

#include <stdio.h>

#define empty_token 0

FILE *f;

//nemenit !!!
// toto su stavy automatu
// kazdy stav musi mat ine cislo !!!
#define s_START 0
#define s_ID 1
#define s_block_coment_0 2
#define s_block_coment_1 3
#define s_block_coment_2 4
#define s_line_comment 5
#define s_asign_0 6
#define s_INT 7
#define s_double_0 8
#define s_double_1 9
#define s_double_2 10
#define s_double_3 11
#define s_str0 12
#define s_strL 13
#define s_str_spec 14
#define s_str_spec_hexa0 15
#define s_str_spec_hexa1 16
#define s_LT 17
#define s_QT 18



//zaheslovane typ tokenu    //todo zmenit tak aby sedeli s ostatnymy
//todo skonrolovat ci su to vsetky mozne typy
#define EMPTY 0
#define ID 1
#define INT 2
#define DOUBLE 3
#define STR 4
#define ASSIGN 5
#define PLUS 6
#define MINUS 7
#define KRAT 8
#define DELENO 9
#define EQ 6
#define NEQ 7
#define LT 8
#define LE 9
#define QT 10
#define QE 11
#define MOD 12
//typ klucoveho slova  ked od neho odcitame 1000 tak dostaneme pointer na klucove slovo v premennej key_worlds
#define MIN_KEY_WORLD 1000


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
    void *data;
} t_token;

/**
 * vrati token tvaru predchadzajucej struktury nacitava zo stdin
 * @return
 */
t_token get_token();

#endif //IFJ_PROJ_SCANNER_H
