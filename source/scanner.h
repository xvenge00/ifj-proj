/*
 * t_token =
 * {EMPTY, NULL}    -> konec suboru
 * {ID, char *}     -> ID a v tdata je ako bol pomenovany v subore              -> pristup k hodnote data.s
 * {INT, int}       -> INT a v data je cislo ktore bolo nacitane uz v tvare int -> pristup k hodnote data.i
 * {DOUBLE, double} -> DOUBLE a v data je nacitane desatiine cislo              -> pristup k hodnote data.d7
 * {MIN_KEY_WORLD, int}
 *                  -> nacitalo sa klucove slovo a v data je cislo a podla tohto cisla a enum keyWorld ide zistit ake
 *                  klucove slovo bolo najdene
 * {ELSE, NULL}     -> pre ostatne navratove typy sa do data neaklada ziadna hodnota
 */


#ifndef IFJ_PROJ_SCANNER_H
#define IFJ_PROJ_SCANNER_H

#include <stdio.h>

FILE *f;
///klucove slova
enum keyWords{
    k_as = 0,       //0
    k_asc,
    k_declare,
    k_dim,
    k_do,
    k_double,   //5
    k_else,
    k_end,
    k_chr,
    k_function,
    k_if,       //10
    k_input,
    k_integer,
    k_length,
    k_loop,
    k_print,    //15
    k_return,
    k_scope,
    k_string,
    k_subStr,
    k_then,     //20
    k_while,
    k_and,
    k_boolean,
    k_continue,
    k_elseif,   //25
    k_exit,
    k_false,
    k_for,
    k_next,
    k_not,      //30
    k_or,
    k_shared,
    k_static,
    k_true      //34

};


//const char key_word_str[35][20];
//unsigned key_size;
//unsigned min_key;



//nemenit !!!
// toto su stavy automatu
// kazdy stav musi mat ine cislo !!!
///stavy automatu
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
    s_OP
} tstate;


//zaheslovane typ tokenu    //todo zmenit tak aby sedeli s ostatnymy
/// navratove typy
typedef enum {
    EMPTY = 0,      //0
    ID,
    INT,
    DOUBLE,
    STR,
    PLUS,           //5
    MINUS,
    KRAT,
    DELENO,
    EQ,
    NEQ,            //10
    LT,
    LE,
    GT,
    GE,
    MOD,            //15
    EOL,
    LPAR,
    RPAR,
    COMMA,
    SEMICOLLON,          //20
    KEY_WORD
} ttype;

typedef union {
    int i;
    double d;
    char *s;
} tdata;

/**
 * N -> cele cislo
 * R -> realne cislo
 */
typedef struct {
    int token_type;      ///typ tokenu ktory bol nacitany
    tdata data;         /// hodnota tokenu ak typ INT = N | DOUBLE = R | STR = retazec | KEY_WORLD = N  (toto N urcuje typ a konkretne sa da urcit z enum keyWorld)| ostatne NULL

    unsigned line; ///riadok z koteho sa token nacital
} t_token;

typedef struct s_scanner_node {
    t_token * token;
    struct s_scanner_node *next;
}t_scanner_node;



/**
 * uvolni pamet ktora bola naalokovana pre token
 * @param token
 */
void discard_token(t_token *token);
void load_all_token();
t_token *load_token();




/**
 * cita z f
 * vrati token tvaru t_token
 * @return
 */
t_token *get_token();

#endif //IFJ_PROJ_SCANNER_H
