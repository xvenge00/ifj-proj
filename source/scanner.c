#include <stdio.h>
#include "scanner.h"
#include <string.h>
#include <ctype.h>
#include <stdlib.h>
#include "memwork.h"
#include "str_buff.h"
#include "main.h"

#define key_size 35

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



/**
 * funkcia ktora navrhne mozne riesenie lexikalnej chyby a ukonci program
 * @param state
 * @param loaded
 * @param line
 */
void ERR_LEX(tstate state, char *loaded, int line){
    fprintf(stderr,"ERR_LEX : na riadku %d je nespravne dany prvok %s\n", line, loaded);
    if (state == s_START){
        printf("pouzivate znak mimo asci\n");
    } else {
        fprintf(stderr, "neznama chyba\n"); // todo dorobit pre ostatne stavy
    }

    clear_all();
    exit(ERR_LEXIK);
}


/**
 * skusa ci ret je klucove slovo
 * @param ret porovnacany retazec
 * @return index +1 klucoveho slova v premennej key_word_strs alebo 0 pokial to nieje klucove slovo
 */
int is_key(char *ret){
    for (unsigned i=0; i<key_size; i++){
        if (strcmp(ret, key_word_str[i]) == 0){
            return i+1;
        }
    }
    return 0;
}

int isOpSp(int loaded){
    const char oper[] = {
            '>',
            '<',
            '=',
            '*',
            '/',
            '\\',
            '+',
            '-',
            '(',
            ')'
    };
    const int size = 10;
    if (isspace(loaded)|| loaded == EOF){
        return 1;
    }
    for (int i = 0; i < size; ++i) {
        if (oper[i] == loaded){
            return 1;
        }
    }
    return 0;
}

int isVaSp(int loaded){
    const char val[] = {
            '_',
            '!'
    }; int size = 2;
    if (isspace(loaded) || isalnum(loaded)|| loaded == EOF){
        return 1;
    }
    for (int i = 0; i < size; ++i) {
        if (loaded == val[i]){
            return 1;
        }
    }
    return 0;
}

t_str_buff *scanner_buff = NULL;


t_token *create_token(ttype typ, tdata data){
    t_token *tmp = my_malloc(sizeof(t_token));
    tmp->token_type = typ;
    tmp->data = data;
    //vymaz buffer
    my_free(scanner_buff->ret);
    my_free(scanner_buff);
    return tmp;
}
static int old = 0;
t_token *get_token(){
    t_token *result = NULL;
    tdata data;
    data.s = NULL;
    //priprava bufferu
    scanner_buff = init_buff(scanner_buff);
    //inicializacia znaku
    static int loaded = 0;
    //riadok ktory je spracovavany
    static unsigned line = 0;
    //zistenie ci neostalo po predchodzom hladany znak

    //inicializovanie stavu na stav STRAT
    tstate state = s_START;
    //pomocna premmena pre stavy
    int pom = 0;
/**********************************************
 *  stavovy automat
 ***********************************************/

    //vykonava cyklus pokial neskoncil subor alebo pokial sa nenasiel token
    do {
        if (old != 0){
            loaded = old;
            old = 0;
        } else {
            loaded = fgetc(f);
        }
        line += (loaded == '\n')? 1 : 0;    //ak je znak \n tak sa je dalsi riadok
        switch (state){
            case s_START: //stav START
                if (isspace(loaded) && (loaded != '\n')){   //nic sa nemeni
                    //state = s_START; nerob nic
                } else if (isalpha(loaded) || (loaded == '_')){ // nasiel sa zaciatok ID
                    append_buff(scanner_buff,loaded);
                    state = s_ID;
                } else if (loaded == '\''){ // riadkovy komment
                    state = s_line_comment;
                } else if (loaded == '/'){  //blokovy koment alebo deleno
                    state = s_block_coment_0;
                } else if (isdigit(loaded)){    // zaciatok INT
                    append_buff(scanner_buff,loaded);
                    state = s_INT;
                } else if (loaded == '!'){  // mozny retazec
                    state = s_str0;
                } else if (loaded == '+'){  // operacia plus
                    result = create_token(PLUS, data);
                    state = s_OP;
                } else if (loaded == '-'){  // operacia minus
                    result = create_token(MINUS, data);
                    state = s_OP;
                } else if (loaded == '*'){  // operacia krat
                    result = create_token(KRAT, data);
                    state = s_OP;
                } else if (loaded == '\\'){ // operacia modulo
                    result =  create_token(MOD, data);
                    state = s_OP;
                } else if (loaded == '='){  // operacia zhodne
                    result = create_token(EQ, data);
                    state = s_OP;
                } else if (loaded == '>'){  // operacia porovnania moyne vysledky >= >
                    state = s_GT;
                } else if (loaded == '<'){// operacia porovnania moyne vysledky <> <= <
                    state = s_LT;
                } else if (loaded == '\n'){ //EOL
                    return create_token(EOL, data);
                } else if (loaded == '('){
                    result = create_token(LPAR, data);
                    state = s_OP;
                } else if (loaded == ')'){
                    result = create_token(RPAR, data);
                    state = s_OP;

                }else if (loaded == ','){
                    return create_token(COMMA, data);

                }else if (loaded == ';'){
                    return create_token(SEMICOLLON, data);

                } else if (loaded == EOF){
                    return create_token(EMPTY, data);
                }  else {    //narazil na necakani znak
                    append_buff(scanner_buff, loaded);
                    append_buff(scanner_buff,0);
                    ERR_LEX(state, get_buff(scanner_buff), line);
                }
                break;
            case s_line_comment:    // stav sa nemeni pokial neskonci riadok
                if (loaded == '\n'){
                    return create_token(EOL, data);
                }
                break;
            case s_block_coment_0:  // ked je dalsi znak ' tak sa jedna o blok koment inak je to deleno
                if (loaded == '\''){
                    state = s_block_coment_1;
                } else {
                    old = loaded;
                    if (isVaSp(loaded)){
                        return create_token(DELENO, data);
                    } else {
                        append_buff(scanner_buff,loaded);
                        append_buff(scanner_buff, 0);
                        ERR_LEX(state, get_buff(scanner_buff),line);
                    }
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
                    if (!isOpSp(loaded)){
                        append_buff(scanner_buff,loaded);
                        append_buff(scanner_buff, 0);
                        ERR_LEX(state, get_buff(scanner_buff),line);
                    }
                    // generovanie tokenu
                    append_buff(scanner_buff,0);
                    char *buff = get_buff(scanner_buff);
                    data.s = my_malloc(sizeof(char)*(buff_size(scanner_buff)));
                    //prekopirovanie str
                    for (int i=0; i<buff_size(scanner_buff);i++){
                        data.s[i] = tolower(buff[i]);
                    }

                    old = loaded;


                    pom = is_key(data.s);
                    if (pom){
                        my_free(data.s);
                        data.i = pom - 1;
                        return create_token(KEY_WORD, data);
                    } else {
                        return create_token(ID, data);
                    }
                }
                break;
            case s_INT: // bud je int alebo double nacitava pokial je 0..9 ak je . tak prechadza na desatinne ak e na exponent
                if (isdigit(loaded)){
                    append_buff(scanner_buff,loaded);
                } else if (loaded == '.'){
                    state = s_double_0;
                    append_buff(scanner_buff,loaded);
                } else if ((loaded == 'e') || (loaded == 'E')){
                    state = s_double_1;
                    append_buff(scanner_buff,loaded);
                } else {
                    //vygeneruj token
                    old = loaded;
                    if (!isVaSp(old)){
                        append_buff(scanner_buff,loaded);
                        append_buff(scanner_buff, 0);
                        ERR_LEX(state, get_buff(scanner_buff), line);
                    }
                    append_buff(scanner_buff,0);
                    data.i = strtol(get_buff(scanner_buff), NULL, 10);
                    return create_token(INT, data);
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
                    if (!isVaSp(old)){
                        append_buff(scanner_buff,loaded);
                        append_buff(scanner_buff, 0);
                        ERR_LEX(state, get_buff(scanner_buff), line);
                    }
                    append_buff(scanner_buff,0);
                    data.d = strtod(get_buff(scanner_buff),NULL);
                    return create_token(DOUBLE, data);
                }
                break;
            case s_double_1:    // exponent prvy znak
                append_buff(scanner_buff,loaded);
                if ((loaded == '+')||(loaded == '-')){
                    state = s_double_2;
                } else if ( isdigit(loaded)){
                    state = s_double_3;
                } else {
                    append_buff(scanner_buff,loaded);
                    append_buff(scanner_buff, 0);
                    ERR_LEX(state, get_buff(scanner_buff), line);
                }
                break;
            case s_double_2: // exponent 2. znak
                if (isdigit(loaded)){
                    append_buff(scanner_buff,loaded);
                    state = s_double_3;
                } else {
                    append_buff(scanner_buff,loaded);

                    append_buff(scanner_buff, 0);
                    ERR_LEX(state, get_buff(scanner_buff), line);
                }
                break;
            case s_double_3:// ostavajuce cisla exponentu
                if (isdigit(loaded)){
                    append_buff(scanner_buff,loaded);
                } else {
                    if (!isVaSp(old)){
                        append_buff(scanner_buff,old);
                        append_buff(scanner_buff,0);

                        ERR_LEX(state, get_buff(scanner_buff), line);
                    }
                    append_buff(scanner_buff,0);
                    data.d = strtod(get_buff(scanner_buff), NULL);
                    old =loaded;
                    return create_token(DOUBLE, data);
                }
                break;
            case s_str0: // overuje ci sa jedna o string ci su "
                if (loaded == '"'){
                    state = s_strL;
                } else {
                    append_buff(scanner_buff,loaded);
                    append_buff(scanner_buff, 0);
                    ERR_LEX(state, get_buff(scanner_buff), line);
                }
                break;
            case s_strL:     // naxitava string ak najde \ dekoduje
                if (loaded == '"'){
                    char *buff = get_buff(scanner_buff);
                    data.s = my_malloc(sizeof(char) * (buff_size(scanner_buff) + 1));
                    for (int i=0; i<=buff_size(scanner_buff);i++){
                        data.s[i] = buff[i];
                    }
                    return create_token(STR, data);
                } else if (loaded == '\\'){
                    state = s_str_spec;
                } else if (loaded < 256){
                    append_buff(scanner_buff,loaded);
                } else {
                    append_buff(scanner_buff,loaded);
                    append_buff(scanner_buff,0);
                    ERR_LEX(state, get_buff(scanner_buff), line);
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
                    append_buff(scanner_buff, loaded);
                    append_buff(scanner_buff,0);
                    ERR_LEX(state, get_buff(scanner_buff), line);
                }
                break;
            case s_str_spec_hexa0:  // druhy znak cislenho kodovania
                if (isdigit(loaded)){
                    pom += (loaded - '0') * 10;
                    state = s_str_spec_hexa1;
                } else {
                    append_buff(scanner_buff, loaded);
                    append_buff(scanner_buff,0);
                    ERR_LEX(state, get_buff(scanner_buff), line);
                }
                break;
            case s_str_spec_hexa1:  // treti znak ciselneho kodovania
                if (isdigit(loaded)){
                    pom += (loaded - '0') * 10;
                    if (pom >255){
                        append_buff(scanner_buff, loaded);
                        append_buff(scanner_buff,0);
                        ERR_LEX(state, get_buff(scanner_buff), line);
                    }
                    append_buff(scanner_buff,pom);
                    state = s_strL;
                } else {
                    append_buff(scanner_buff, loaded);
                    append_buff(scanner_buff,0);
                    ERR_LEX(state, get_buff(scanner_buff), line);
                }
                break;
            case s_LT:  // stav mensi nez
                if (loaded == '>'){
                    result = create_token(NEQ, data);
                    state = s_OP;
                } else if (loaded == '='){
                    result = create_token(LE, data);
                    state = s_OP;
                } else {
                    old = loaded;
                    result = create_token(LT, data);
                    state = s_OP;

                }
                break;
            case s_GT:  // stav vasci nez
                if (loaded == '='){
                    result =  create_token(GE,data);
                    state = s_OP;
                } else {
                    old = loaded;
                    result =  create_token(GT,data);
                    state = s_OP;
                }
                break;
            case s_OP:
                if (isVaSp(loaded)){
                    old = loaded;
                    return result;
                } else{
                    append_buff(scanner_buff, loaded);
                    append_buff(scanner_buff, 0);

                    ERR_LEX(state, get_buff(scanner_buff), line);
                }

                //todo dorobit ostatne typy a doplnit
        }
    }while (loaded != EOF);
    //sem by sa nikdi nemal dostat ak ano niekde je chyba
    my_free(scanner_buff->ret);
    my_free(scanner_buff);
    fprintf(stderr,"ERROR -- lex skoncil zle\n");
    return NULL;
}