#ifndef IFJ_PROJ_SCANNER_H
#define IFJ_PROJ_SCANNER_H

#define empty_token 0

//stavy automatu
#define s_START 0
#define s_ID 1
#define s_block_coment_0 2
#define s_block_coment_1 3
#define s_block_coment_2 4
#define s_line_comment 5
#define s_asign_0 6
#define s_asign_1 7
#define s_INT 8
#define s_double_0 9
#define s_double_1 10
#define s_double_2 11
#define s_double_3 20
#define s_str0 12
#define s_strL 13
#define s_str_spec 14
#define s_str_spec_hexa0 15
#define s_str_spec_hexa1 19

#define s_EQ 18
#define s_LT 17
#define s_QT 21



//zaheslovane typ tokenu
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




typedef struct {
    int token_type;      // typ tokenu
    void *data;
} t_token;

t_token get_token();
char *get_lexema();

#endif //IFJ_PROJ_SCANNER_H
