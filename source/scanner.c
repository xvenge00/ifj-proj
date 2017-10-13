#include <stdio.h>
#include "scanner.h"
#include <string.h>
#include <ctype.h>
#include <stdlib.h>
#include "memwork.h"
#include "str_buff.h"
#include "main.h"



const char *key_worlds[] = {
        "As",
        "Asc",
        "Declare",
        "Dim",
        "Do",
        "Double",
        "Else",
        "End",
        "Chr",
        "Function",
        "If",
        "Input",
        "Integer",
        "Length",
        "Loop",
        "Print",
        "Return",
        "Scope",
        "String",
        "SubStr",
        "Then",
        "While"
};

const char *rez_key_worlds[] = {
        "And",
        "Boolean",
        "Continue",
        "Elseif",
        "Exit",
        "False",
        "For",
        "Next",
        "Not",
        "Or",
        "Shared",
        "Static",
        "True"
};

int old = 0;


t_token tget_token(char *lex){
    //priprava bufferu
    init_buff();
    int loaded = 0;
    static unsigned line = 0;
    if (old != 0){
        loaded = old;
        old = 0;
    } else {
        loaded = getchar();
    }
    int state = s_START;
    int pom = 0;


    while (loaded != EOF){
        line += (loaded == '\n')? 1 : 0;
        switch (state){
            case s_START:
                null_buffer();
                if (isspace(loaded)){
                    state = s_START;
                } else if (isalpha(loaded) || (loaded == '_')){
                    append_buff(loaded);
                    state = s_ID;
                } else if (loaded == '\''){
                    state = s_line_comment;
                } else if (loaded == '/'){
                    state = s_block_coment_0;
                } else if (loaded == ':'){
                    state = s_asign_0;
                } else if (isdigit(loaded)){
                    append_buff(loaded);
                    state = s_INT;
                } else if (loaded == '!'){
                    state = s_str0;
                } else if (loaded == '+'){
                    t_token token = {PLUS, NULL};
                    return token;
                } else if (loaded == '-'){
                    t_token token = {MINUS, NULL};
                    return token;
                } else if (loaded == '*'){
                    t_token token = {KRAT, NULL};
                    return token;
                } else if (loaded == '\\'){
                    t_token token = {MOD,NULL};
                    return token;
                } else if (loaded == '='){
                    t_token token = {EQ,NULL};
                    return token;
                } else if (loaded == '>'){
                    state = s_QT;
                } else if (loaded == '<'){
                    state = s_LT;
                } else {
                    clear_all();
                    exit(ERR_LEXIK);
                }//todo pre ine
                break;
            case s_line_comment:
                if (loaded == '\n'){
                    state = s_START;
                } else {
                    state = s_line_comment;
                }
                break;
            case s_block_coment_0:
                if (loaded == '\''){
                    state = s_block_coment_1;
                } else if (isalnum(loaded) || (loaded == '_')){
                    old = loaded;
                    null_buffer();
                    t_token token = {DELENO, NULL};
                    append_buff(loaded);
                    return token;
                }else {
                    clear_all();
                    exit(ERR_INTER);
                }
                break;
            case s_block_coment_1:
                if (loaded == '\''){
                    state = s_block_coment_2;
                } else {
                    state = s_block_coment_1;
                }
                break;
            case s_block_coment_2:
                if (loaded == '/'){
                    state = s_START;
                } else {
                    state = s_block_coment_1;
                }
                break;
            case s_ID:
                if (isalnum(loaded)||(loaded == '_')){
                    append_buff(loaded);
                } else {
                    // generovanie tokenu
                    char *name;
                    char *buff = get_buff();
                    name = my_malloc(sizeof(char)*(buff_size()+1));
                    //prekopirovanie str
                    for (unsigned i=0; i<buff_size()+1;i++){
                        name[i] = buff[i];
                    }
                    old = loaded;
                    //vytvorenie tokenu
                    t_token result = {ID,(void *)name};
                    return result;

                }
                break;
            case s_INT:
                if (isdigit(loaded)){
                    append_buff(loaded);
                } else if (isspace(loaded)){
                    state = s_START;
                }else if (loaded == '.'){
                    state = s_double_0;
                    append_buff(loaded);
                } else if ((loaded == 'e') || (loaded == 'E')){
                    state = s_double_1;
                } else {
                    //vygeneruj token
                    int *tmp;
                    tmp = my_malloc(sizeof(int));
                    *tmp = strtol(get_buff(),NULL, 10);
                    t_token token = {INT, tmp};
                    null_buffer();
                    old = loaded;
                    return token;
                }
                break;
            case s_double_0:
                if (isdigit(loaded)){
                    append_buff(loaded);
                } else  if ((loaded == 'e')||(loaded == 'E')){
                    state = s_double_1;
                    append_buff(loaded);
                } else {
                    old = loaded;
                    double  *tmp;
                    tmp = my_malloc(sizeof(double));
                    *tmp = strtod(get_buff(),NULL);
                    t_token token = {DOUBLE, tmp};
                    return token;
                }
                break;
            case s_double_1:
                append_buff(loaded);
                if ((loaded == '+')||(loaded == '-')){
                    state = s_double_2;
                } else if ( isdigit(loaded)){
                    state = s_double_3;
                } else {
                    fprintf(stderr,"CIslo nema exponenta %s na riadku %i\n", get_buff(), line);
                    clear_all();
                    exit(ERR_LEXIK);
                }
                break;
            case s_double_2:
                if (isdigit(loaded)){
                    append_buff(loaded);
                    state = s_double_3;
                } else {
                    fprintf(stderr,"CIslo nema exponenta %s na riadku %i\n", get_buff(), line);
                    clear_all();
                    exit(ERR_LEXIK);
                }
                break;
            case s_double_3:
                if (isdigit(loaded)){
                    append_buff(loaded);
                } else {
                    old =loaded;
                    double  *tmp;
                    tmp = my_malloc(sizeof(double));
                    *tmp = strtod(get_buff(),NULL);
                    t_token token = {DOUBLE, tmp};
                    return token;
                }
                break;
            case s_str0:
                if (loaded == '"'){
                    state = s_strL;
                } else {
                    fprintf(stderr,"bol zle definovany retazec na riadku %i\n",line);
                    clear_all();
                    exit(ERR_LEXIK);
                }
                break;
            case s_strL:
                if (loaded == '"'){
                    char *ret;
                    char *buff = get_buff();
                    ret = my_malloc(sizeof(char) * (buff_size() + 1));
                    for (unsigned i=0; i<=buff_size();i++){
                        ret[i] = buff[i];
                    }
                    t_token token = {STR, ret};
                    return token;
                } else if (loaded == '\\'){
                    state = s_str_spec;
                } else if (loaded < 256){
                    append_buff(loaded);

                } else {
                    fprintf(stderr,"v retazci \"%s\" na riadku %i bol pouzity znak mimo aci\n",get_buff(),line);
                    clear_all();
                    exit(ERR_LEXIK);
                }
                break;
            case s_str_spec:
                if (isdigit(loaded)){
                    pom = (loaded - '0')*10;
                    state = s_str_spec_hexa0;
                } else if (loaded == 'n'){
                    append_buff('\n');
                    state = s_strL;
                } else if (loaded == '"'){
                    append_buff('"');
                    state = s_strL;
                } else if (loaded == 't'){
                    append_buff('\t');
                    state = s_strL;
                } else if (loaded == '\\'){
                    append_buff('\\');
                } else {
                    fprintf(stderr, "Vretazci bol zadany zla esc sekvencia bud 3 cisla alebo n t \\ na riadku %i\n",line );
                    clear_all();
                    exit(ERR_LEXIK);
                }
                break;
            case s_str_spec_hexa0:
                if (isdigit(loaded)){
                    pom += (loaded - '0') * 10;
                    state = s_str_spec_hexa1;
                } else {
                    fprintf(stderr, "Vretazci bol zadany zla esc sekvencia bud 3 cisla do 255 alebo n t \\ na riadku %i\n",
                            line);
                    clear_all();
                    exit(ERR_LEXIK);
                }
                break;
            case s_str_spec_hexa1:
                if (isdigit(loaded)){
                    pom += (loaded - '0') * 10;
                    if (pom >255){
                        fprintf(stderr, "zla escape sekvencia na riadku %i\n sprvne su 3 cisla s hodnotou do 255\n",     line);
                        clear_all();
                        exit(ERR_LEXIK);
                    }
                    append_buff(pom);
                    state = s_strL;
                } else {
                    fprintf(stderr, "Vretazci bol zadany zla esc sekvencia bud 3 cisla alebo n t \\ na riadku %i\n",
                            line);
                    clear_all();
                    exit(ERR_LEXIK);
                }
                break;
            case s_LT:
                if (loaded == '>'){
                    t_token token = {NEQ, NULL};
                    return token;
                } else if (loaded == '='){
                    t_token token = {LE, NULL};
                    return token;
                } else {
                    old = loaded;
                    t_token token = {LT, NULL};
                    return token;
                }
                break;
            case s_QT:
                if (loaded == '='){
                    t_token token = {QE, NULL};
                    return token;
                } else {
                    old = loaded;
                    t_token token = {QT, NULL};
                    return token;
                }

                //todo dorobit ostatne typy a doplnit
        }
        loaded = getchar();
    }

    t_token result = {EMPTY, NULL};
    return result;
}