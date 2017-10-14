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
const unsigned key_size = 22;
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
/**
 * skusa ci ret je klucove slovo
 * @param ret porovnacany retazec
 * @return index +1 klucoveho slova v premennej key_worlds alebo 0 pokial to nieje klucove slovo
 */
int is_key(char *ret){
    for (unsigned i=0; i<key_size; i++){
        if (strcmp(ret, key_worlds[i])){
            return i+1;
        }
    }
    return 0;
}

t_str_buff *scanner_buff = NULL;


t_token tget_token(char *lex){
    //priprava bufferu
    //zisti ci buffer uz existuje alebo nie
    if (scanner_buff == NULL){
        scanner_buff = init_buff(scanner_buff);
    } else {
        null_buffer(scanner_buff);
    }
    //inicializacia znaku
    int loaded = 0;
    //riadok ktory je spracovavany
    static unsigned line = 0;
    //zistenie ci neostalo po predchodzom hladany znak
    if (old != 0){
        loaded = old;
        old = 0;
    } else {
        loaded = getchar();
    }
    //inicializovanie stavu na stav STRAT
    int state = s_START;
    //pomocna premmena pre stavy
    int pom = 0;

    //vykonava cyklus pokial neskoncil subor alebo pokial sa nenasiel token
    do {
        line += (loaded == '\n')? 1 : 0;    //ak je znak \n tak sa je dalsi riadok
        switch (state){
            case s_START: //stav START
                if (isspace(loaded)){   //nic sa nemeni
                    state = s_START;
                } else if (isalpha(loaded) || (loaded == '_')){ // nasiel sa zaciatok ID
                    append_buff(scanner_buff,loaded);
                    state = s_ID;
                } else if (loaded == '\''){ // riadkovy komment
                    state = s_line_comment;
                } else if (loaded == '/'){  //blokovy koment alebo deleno
                    state = s_block_coment_0;
                } else if (loaded == ':'){  //mozne priradenie
                    state = s_asign_0;
                } else if (isdigit(loaded)){    // zaciatok INT
                    append_buff(scanner_buff,loaded);
                    state = s_INT;
                } else if (loaded == '!'){  // mozny retazec
                    state = s_str0;
                } else if (loaded == '+'){  // operacia plus
                    t_token token = {PLUS, NULL};
                    return token;
                } else if (loaded == '-'){  // operacia minus
                    t_token token = {MINUS, NULL};
                    return token;
                } else if (loaded == '*'){  // operacia krat
                    t_token token = {KRAT, NULL};
                    return token;
                } else if (loaded == '\\'){ // operacia modulo
                    t_token token = {MOD,NULL};
                    return token;
                } else if (loaded == '='){  // operacia zhodne
                    t_token token = {EQ,NULL};
                    return token;
                } else if (loaded == '>'){  // operacia porovnania moyne vysledky >= >
                    state = s_QT;
                } else if (loaded == '<'){// operacia porovnania moyne vysledky <> <= <
                    state = s_LT;
                } else {    // bol najdeny znak co tam nema hladat
                    clear_all();
                    fprintf(stderr, "%i\n",line);
                    exit(ERR_LEXIK);
                }//todo pre ine
                break;
            case s_line_comment:    // stav sa nemeni pokial neskonci riadok
                if (loaded == '\n'){
                    state = s_START;
                } else {
                    state = s_line_comment;
                }
                break;
            case s_block_coment_0:  // ked je dalsi znak ' tak sa jedna o blok koment inak je to deleno
                if (loaded == '\''){
                    state = s_block_coment_1;
                } else {
                    old = loaded;
                    null_buffer(scanner_buff);
                    t_token token = {DELENO, NULL};
                    append_buff(scanner_buff,loaded);
                    return token;
                }
                break;
            case s_block_coment_1: //blokovy koment pokial nenajde ' potom sa mozno jedna o konec komentu takze dalsi stav
                if (loaded == '\''){
                    state = s_block_coment_2;
                } else {
                    state = s_block_coment_1;
                }
                break;
            case s_block_coment_2:  // pokial sa naslo / tak konci blokovy koment inak sa vrati do predosleho stavu
                if (loaded == '/'){
                    state = s_START;
                } else {
                    state = s_block_coment_1;
                }
                break;
            case s_ID:  // ostava pokial nenajde iny znak ako 0..9 a..z A..Z ak ano vrati token s menom
                if (isalnum(loaded)||(loaded == '_')){
                    append_buff(scanner_buff,loaded);
                } else {
                    // generovanie tokenu
                    char *name;
                    char *buff = get_buff(scanner_buff);
                    name = my_malloc(sizeof(char)*(buff_size(scanner_buff)+1));
                    //prekopirovanie str
                    for (unsigned i=0; i<buff_size(scanner_buff)+1;i++){
                        name[i] = tolower(buff[i]);
                    }

                    old = loaded;

                    pom = is_key(name);
                    if (pom){
                        t_token result = {MIN_KEY_WORLD + pom - 1,NULL};
                        return result;
                    } else {
                        t_token result = {ID, (void *) name};
                        return result;
                    }
                }
                break;
            case s_INT: // bud je int alebo double nacitava pokial je 0..9 ak je . tak prechadza na desatinne ak e na exponent
                if (isdigit(loaded)){
                    append_buff(scanner_buff,loaded);
                } else if (isspace(loaded)){
                    state = s_START;
                }else if (loaded == '.'){
                    state = s_double_0;
                    append_buff(scanner_buff,loaded);
                } else if ((loaded == 'e') || (loaded == 'E')){
                    state = s_double_1;
                } else {
                    //vygeneruj token
                    int *tmp;
                    tmp = my_malloc(sizeof(int));
                    *tmp = strtol(get_buff(scanner_buff),NULL, 10);
                    t_token token = {INT, tmp};
                    null_buffer(scanner_buff);
                    old = loaded;
                    return token;
                }
                break;
            case s_double_0:    // desatinna cast realneho cisla
                if (isdigit(loaded)){
                    append_buff(scanner_buff,loaded);
                } else  if ((loaded == 'e')||(loaded == 'E')){
                    state = s_double_1;
                    append_buff(scanner_buff,loaded);
                } else {
                    old = loaded;
                    double  *tmp;
                    tmp = my_malloc(sizeof(double));
                    *tmp = strtod(get_buff(scanner_buff),NULL);
                    t_token token = {DOUBLE, tmp};
                    return token;
                }
                break;
            case s_double_1:    // exponent prvy znak
                append_buff(scanner_buff,loaded);
                if ((loaded == '+')||(loaded == '-')){
                    state = s_double_2;
                } else if ( isdigit(loaded)){
                    state = s_double_3;
                } else {
                    fprintf(stderr,"CIslo nema exponenta %s na riadku %i\n", get_buff(scanner_buff), line);
                    clear_all();
                    exit(ERR_LEXIK);
                }
                break;
            case s_double_2: // exponent 2. znak
                if (isdigit(loaded)){
                    append_buff(scanner_buff,loaded);
                    state = s_double_3;
                } else {
                    fprintf(stderr,"CIslo nema exponenta %s na riadku %i\n", get_buff(scanner_buff), line);
                    clear_all();
                    exit(ERR_LEXIK);
                }
                break;
            case s_double_3:// ostavajuce cisla exponentu
                if (isdigit(loaded)){
                    append_buff(scanner_buff,loaded);
                } else {
                    old =loaded;
                    double  *tmp;
                    tmp = my_malloc(sizeof(double));
                    *tmp = strtod(get_buff(scanner_buff),NULL);
                    t_token token = {DOUBLE, tmp};
                    return token;
                }
                break;
            case s_str0: // overuje ci sa jedna o string ci su "
                if (loaded == '"'){
                    state = s_strL;
                } else {
                    fprintf(stderr,"bol zle definovany retazec na riadku %i\n",line);
                    clear_all();
                    exit(ERR_LEXIK);
                }
                break;
            case s_strL:     // naxitava string ak najde \ dekoduje
                if (loaded == '"'){
                    char *ret;
                    char *buff = get_buff(scanner_buff);
                    ret = my_malloc(sizeof(char) * (buff_size(scanner_buff) + 1));
                    for (unsigned i=0; i<=buff_size(scanner_buff);i++){
                        ret[i] = buff[i];
                    }
                    t_token token = {STR, ret};
                    return token;
                } else if (loaded == '\\'){
                    state = s_str_spec;
                } else if (loaded < 256){
                    append_buff(scanner_buff,loaded);

                } else {
                    fprintf(stderr,"v retazci \"%s\" na riadku %i bol pouzity znak mimo aci\n",get_buff(scanner_buff),line);
                    clear_all();
                    exit(ERR_LEXIK);
                }
                break;
            case s_str_spec:    /* zistovanie o typu kodovania znaku za \ */
                if (isdigit(loaded)){
                    pom = (loaded - '0')*10;
                    state = s_str_spec_hexa0;
                } else if (loaded == 'n'){
                    append_buff(scanner_buff,'\n');
                    state = s_strL;
                } else if (loaded == '"'){
                    append_buff(scanner_buff,'"');
                    state = s_strL;
                } else if (loaded == 't'){
                    append_buff(scanner_buff,'\t');
                    state = s_strL;
                } else if (loaded == '\\'){
                    append_buff(scanner_buff,'\\');
                } else {
                    fprintf(stderr, "Vretazci bol zadany zla esc sekvencia bud 3 cisla alebo n t \\ na riadku %i\n",line );
                    clear_all();
                    exit(ERR_LEXIK);
                }
                break;
            case s_str_spec_hexa0:  // druhy znak cislenho kodovania
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
            case s_str_spec_hexa1:  // treti znak ciselneho kodovania
                if (isdigit(loaded)){
                    pom += (loaded - '0') * 10;
                    if (pom >255){
                        fprintf(stderr, "zla escape sekvencia na riadku %i\n sprvne su 3 cisla s hodnotou do 255\n",     line);
                        clear_all();
                        exit(ERR_LEXIK);
                    }
                    append_buff(scanner_buff,pom);
                    state = s_strL;
                } else {
                    fprintf(stderr, "Vretazci bol zadany zla esc sekvencia bud 3 cisla alebo n t \\ na riadku %i\n",
                            line);
                    clear_all();
                    exit(ERR_LEXIK);
                }
                break;
            case s_LT:  // stav mensi nez
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
            case s_QT:  // stav vasci nez
                if (loaded == '='){
                    t_token token = {QE, NULL};
                    return token;
                } else {
                    old = loaded;
                    t_token token = {QT, NULL};
                    return token;
                }
                break;
            case s_asign_0: // stav priradenia
                if (loaded == '='){
                    t_token token = {ASSIGN, NULL};
                    return  token;
                } else {
                    fprintf(stderr,"zly znak priradenia chyba = na riadku %i\n",line);
                    clear_all();
                    exit(ERR_LEXIK);
                }

                //todo dorobit ostatne typy a doplnit
        }
        loaded = getchar();
    }while (loaded != EOF);

    t_token result = {EMPTY, NULL};
    return result;
}