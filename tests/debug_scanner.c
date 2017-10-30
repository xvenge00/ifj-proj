#include <stdio.h>
#include "../source/scanner.h"

/*
 * DEBUGOVANIE SCANNER
 */


int main() {

    char op[23][30] = {
            "EMPTY",
            "ID",
            "INT",
            "DOUBLE",
            "STR",
            "PLUS",
            "MINUS",
            "KRAT",
            "DELENO",
            "EQ",
            "NEQ",
            "LT",
            "LE",
            "GT",
            "GE",
            "MOD",
            "EOL",
            "LPAR",
            "RPAR",
            "comma",
            "COMMA",
            "KEY_WORLD"
    };


    const char key_word_str[35][20] = {
            "as",       //0
            "asc",
            "declare",
            "dim",
            "do",
            "double",   //5
            "else",
            "end",
            "chr",
            "function",
            "if",       //10
            "input",
            "integer",
            "length",
            "loop",
            "print",    //15
            "return",
            "scope",
            "string",
            "subStr",
            "then",     //20
            "while",
            "and",
            "boolean",
            "continue",
            "elseif",   //25
            "exit",
            "false",
            "for",
            "next",
            "not",      //30
            "or",
            "shared",
            "static",
            "true"      //34
    };

    const int load_tok = 50; //konsatnta kolko tokenov sa ma nacitat
    t_token *tmp = NULL;
    f = fopen("../tests/test.txt","r");

    for (int i = 0; i < load_tok; ++i) {

        tmp = get_token();
        printf("%-15s :",op[tmp->token_type]);
        if (tmp->token_type == ID || tmp->token_type == STR) {
            printf(" %s\n",tmp->data.s);
        } else if (tmp->token_type == INT ) {
                printf(" %i\n", tmp->data.i);
        }else if (tmp->token_type == DOUBLE) {
            printf(" %f\n", tmp->data.d);
        } else if (tmp->token_type == KEY_WORD){
            printf(" %s\n",key_word_str[tmp->data.i]);
        } else {
            printf("\n");
        }

    }
    return 0;

}