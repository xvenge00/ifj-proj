#include <stdio.h>
#include "../source/scanner.h"
#include "../source/memwork.h"

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
            "COMMA",
            "SEMICOLLON",
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

    int loaded = 0;
    t_token *tmp = NULL;
    f = fopen("../tests/test.txt","r");
    load_all_token();
    do  {
        tmp = get_token();
        printf("type :%-10s |",op[tmp->token_type]);
        printf(" line: %3i |", tmp->line);
        if (tmp->token_type == ID || tmp->token_type == STR) {
            printf(" %s",tmp->data.s);
        } else if (tmp->token_type == INT ) {
                printf(" %i", tmp->data.i);
        }else if (tmp->token_type == DOUBLE) {
            printf(" %f", tmp->data.d);
        } else if (tmp->token_type == KEY_WORD){
            printf(" %s",key_word_str[tmp->data.i]);
        }
        printf("\n");
        if (tmp->token_type == EOL){
            printf("---------------------------------------------\n");
        }


        loaded = tmp->token_type;
        discard_token(tmp);
    }while (loaded != EMPTY);
    fclose(f);
    clear_all();
    return 0;

}