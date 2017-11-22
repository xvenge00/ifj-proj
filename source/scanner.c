#include <stdio.h>
#include "scanner.h"
#include <string.h>
#include <ctype.h>
#include <stdlib.h>
#include "memwork.h"
#include "str_buff.h"
#include "err.h"

#define key_size 35

t_scanner_node *scanner_head = NULL;
t_scanner_node *scanner_tail = NULL;

void scanner_append(t_token *token){
    t_scanner_node *result = my_malloc(sizeof(t_scanner_node));
    result->token = token;
    result->next = NULL;
    if (scanner_head == NULL){
        scanner_head = scanner_tail = result;
            } else {
        scanner_tail->next = result;
        scanner_tail = result;
    }
}

t_token * get_token(){
    t_token *result = NULL;
    if (scanner_head == NULL){
        result = my_malloc(sizeof(t_token));
        result->token_type = EMPTY;
        return result;
    }
    result = scanner_head->token;
    t_scanner_node *temp = scanner_head;
    scanner_head = scanner_head->next;
    my_free(temp);
    return result;
}

void return_token(t_token *token){
    t_scanner_node *result = my_malloc(sizeof(t_scanner_node));
    result->token = token;
    result->next = scanner_head;
    scanner_head = result;
}

void discard_token(t_token *token){
    if (token->token_type == ID || token->token_type == STR) {
        my_free(token->data.s);
    }
    my_free(token);
}

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
    //TODO chybove hlasenia

    fprintf(stderr,"ERR_LEX : neplatna lexema na riadku :%i  -- %s\n", line, loaded);
    if (state == s_block_coment_1 || state == s_block_coment_2){
        fprintf(stderr, "neukonceny blokovy koment\n");
    } else if (state == s_START){
        fprintf(stderr, "neplatny znak\n");
    } else if (state == s_double_1){
        fprintf(stderr, "po e musi nasledovat cislo alebo +/-\n");
    }else if (state == s_double_2){
        fprintf(stderr, "po e+/- musi nasledovat cislo\n");
    } else if (state == s_str0){
        fprintf(stderr, "mozno ste mysleli !\"\"\n");
    } else if (state == s_strL){
        fprintf(stderr, "retazec moze obsahovat znaky z ASCII vysie ako 31 !\"\"\n");
    } else if (state == s_str_spec){
        fprintf(stderr, "po znaku \\ moye nasledovat cislo 000 - 255 n t ...\n");
    }else if (state == s_str_spec_hexa0 || state == s_str_spec_hexa1){
        fprintf(stderr, "escape sekvenia potrebuje 3 cisla\n");
    }  else {
        fprintf(stderr, " mozno ste mysleli ");
        for (unsigned i = 0; i < strlen(loaded) - 1; ++i) {
            fputc(loaded[i], stderr);
        }
        putchar('\n');
    }

    clear_all();
    exit(ERR_LEXIK);
}


/**
 * skusa ci ret je klucove slovo
 * @param ret porovnacany retazec
 * @return index +1 klucoveho slova v premennej key_word_strs alebo 0 pokial to nieje klucove slovo
 */
int is_keyword(char *ret){
    for (unsigned i=0; i<key_size; i++){
        if (strcmp(ret, key_word_str[i]) == 0){
            return i+1;
        }
    }

    return 0;
}

//int isOperatorOrSpace(int loaded){
//    const int size = 12;
//    const char oper[] = {
//            '>',
//            '<',
//            '=',
//            '*',
//            '/',
//            '\\',
//            '+',
//            '-',
//            '(',
//            ')',
//            ',',
//            ';'
//    };
//
//    if (isspace(loaded)|| loaded == EOF){
//        return 1;
//    }
//
//    for (int i = 0; i < size; ++i) {
//        if (oper[i] == loaded){
//            return 1;
//        }
//    }
//
//    return 0;
//}
//
//int isValueOrSpace(int loaded){
//    return (isspace(loaded) || isalnum(loaded)|| loaded == EOF || loaded == '_' || loaded == '!' || loaded == '(' || loaded == ')');
//}

t_str_buff *scanner_buff = NULL;
int beginning = 1;

t_token *create_token(ttype typ, tdata data, unsigned *line){
    t_token *tmp = my_malloc(sizeof(t_token));


    tmp->token_type = typ;
    tmp->data = data;
    tmp->line = *line;
    //vymaz buffer
    my_free(scanner_buff->ret);
    my_free(scanner_buff);
    scanner_buff->top = 0;
    scanner_buff->max = 0;
    if (typ == EOL){
        (*line)++;
    }
    if (typ == EMPTY){
        beginning = 1;
    }

    return tmp;
}

static int old = 0;

t_token *load_token(){
//    t_token *result = NULL;
    tdata data;
    data.s = NULL;

    scanner_buff = init_buff();     //priprava bufferu
    static int loaded = 0;                      //inicializacia znaku
    static unsigned line = 1;                   //riadok ktory je spracovavany
    //zistenie ci neostalo po predchodzom hladany znak
    if (beginning){
        beginning = 0;
        line = 1;
    }

    tstate state = s_START;                     //inicializovanie stavu na stav STRAT

    int pom = 0;                                //pomocna premmena pre stavy
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

        switch (state){
            case s_START: //stav START
                if (isspace(loaded) && (loaded != '\n')){
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
                    return create_token(PLUS, data,&line);
//                    state = s_OP;
                } else if (loaded == '-'){  // operacia minus
                    return create_token(MINUS, data,&line);
//                    state = s_OP;
                } else if (loaded == '*'){  // operacia krat
                    return create_token(KRAT, data,&line);
//                    state = s_OP;
                } else if (loaded == '\\'){ // operacia modulo
                    return  create_token(MOD, data,&line);
//                    state = s_OP;
                } else if (loaded == '='){  // operacia zhodne
                    return create_token(EQ, data,&line);
//                    state = s_OP;
                } else if (loaded == '>'){  // operacia porovnania moyne vysledky >= >
                    state = s_GT;
                } else if (loaded == '<'){// operacia porovnania moyne vysledky <> <= <
                    state = s_LT;
                } else if (loaded == '\n'){ //EOL
                    return create_token(EOL, data,&line);
                } else if (loaded == '('){
                    return create_token(LPAR, data,&line);
//                    state = s_OP;
                } else if (loaded == ')'){
                    return create_token(RPAR, data,&line);
//                    state = s_OP;
                }else if (loaded == ','){
                    return create_token(COMMA, data,&line);
                }else if (loaded == ';'){
                    return create_token(SEMICOLLON, data,&line);
                } else if (loaded == EOF){
                    return create_token(EMPTY, data,&line);
                }  else {    //narazil na necakany znak
                    append_buff(scanner_buff, loaded);
                    append_buff(scanner_buff,0);
                    ERR_LEX(state, get_buff(scanner_buff), line);
                }
                break;
            case s_line_comment:    // stav sa nemeni pokial neskonci riadok
                if (loaded == '\n'){
                    line++;
                    return create_token(EOL, data,&line);
                } else if (loaded == EOF){
                    return create_token(EMPTY, data,&line);
                }
                break;
            case s_block_coment_0:  // ked je dalsi znak ' tak sa jedna o blok koment, inak je to deleno
                if (loaded == '\''){
                    state = s_block_coment_1;
                } else {
                    old = loaded;
                    return create_token(DELENO,data, &line);
//                    if (isValueOrSpace(loaded)){
//                        return create_token(DELENO, data,&line);
//                    } else {
//                        append_buff(scanner_buff,loaded);
//                        append_buff(scanner_buff, 0);
//                        ERR_LEX(state, get_buff(scanner_buff),line);
//                    }
                }
                break;
            case s_block_coment_1: //blokovy koment pokial nenajde ' potom sa mozno jedna o konec komentu takze dalsi stav
                if (loaded == '\''){
                    state = s_block_coment_2;
                } else if (loaded == EOF) {
                    ERR_LEX(state, get_buff(scanner_buff),line);
                } else {
                    state = s_block_coment_1;
                }
                break;
            case s_block_coment_2:  // pokial sa naslo / tak konci blokovy koment inak sa vrati do predosleho stavu
                if (loaded == '/'){
                    state = s_START;
                } else if (loaded == EOF){
                    ERR_LEX(state, get_buff(scanner_buff),line);
                } else {
                    state = s_block_coment_1;
                }
                break;
            case s_ID:  // ostava pokial nenajde iny znak ako 0..9, a..z, A..Z, _ ak ano vrati token s menom
                if (isalnum(loaded)||(loaded == '_')){
                    append_buff(scanner_buff,loaded);
                } else {
//                    if (!isOperatorOrSpace(loaded)){
//                        append_buff(scanner_buff,loaded);
//                        append_buff(scanner_buff, 0);
//                        ERR_LEX(state, get_buff(scanner_buff),line);
//                    }
                    // generovanie tokenu
                    append_buff(scanner_buff,0);
                    char *buff = get_buff(scanner_buff);
                    data.s = my_malloc(sizeof(char)*(buff_size(scanner_buff)));
                    //prekopirovanie str
                    for (int i=0; i<buff_size(scanner_buff);i++){
                        data.s[i] = (char)tolower(buff[i]);
                    }

                    old = loaded;


                    pom = is_keyword(data.s);
                    if (pom){
                        my_free(data.s);
                        data.i = pom - 1;
                        return create_token(KEY_WORD, data,&line);
                    } else {
                        return create_token(ID, data,&line);
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
//                    if (!isValueOrSpace(old)){
//                        append_buff(scanner_buff,loaded);
//                        append_buff(scanner_buff, 0);
//                        ERR_LEX(state, get_buff(scanner_buff), line);
//                    }
                    append_buff(scanner_buff,0);
                    data.i = (int)strtol(get_buff(scanner_buff), NULL, 10);
                    return create_token(INT, data,&line);
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
//                    if (!isValueOrSpace(old)){
//                        append_buff(scanner_buff,loaded);
//                        append_buff(scanner_buff, 0);
//                        ERR_LEX(state, get_buff(scanner_buff), line);
//                    }
                    append_buff(scanner_buff,0);
                    data.d = strtod(get_buff(scanner_buff),NULL);
                    return create_token(DOUBLE, data,&line);
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
//                    if (!isValueOrSpace(old)){
//                        append_buff(scanner_buff,old);
//                        append_buff(scanner_buff,0);
//
//                        ERR_LEX(state, get_buff(scanner_buff), line);
//                    }
                    append_buff(scanner_buff,0);
                    data.d = strtod(get_buff(scanner_buff), NULL);
                    old =loaded;
                    return create_token(DOUBLE, data,&line);
                }
                break;
            case s_str0: // overuje ci sa jedna o string ci su "
                if (loaded == '"'){
                    state = s_strL;
                } else {
                    append_buff(scanner_buff,'!');
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
                    return create_token(STR, data,&line);
                } else if (loaded == '\\'){
                    state = s_str_spec;
                } else if (loaded > 31 && isascii(loaded)){
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
                    return create_token(NEQ, data,&line);
//                    state = s_OP;
                } else if (loaded == '='){
                    return create_token(LE, data,&line);
//                    state = s_OP;
                } else {
                    old = loaded;
                    return create_token(LT, data,&line);
//                    state = s_OP;

                }
                break;
            case s_GT:  // stav vasci nez
                if (loaded == '='){
                    return  create_token(GE,data,&line);
//                    state = s_OP;
                } else {
                    old = loaded;
                    return   create_token(GT,data,&line);
//                    state = s_OP;
                }
                break;
            default:
                old = loaded;
                state = s_START;
//                break;
//            case s_OP:
//                if (isValueOrSpace(loaded)){
//                    old = loaded;
//                    return result;
//                } else{
//                   ERR_LEX(state, "operacia", line);
//                }
        }
    }while (loaded != EOF);
    //sem by sa nikdy nemal dostat ak ano niekde je chyba
    my_free(scanner_buff->ret);
    my_free(scanner_buff);
    fprintf(stderr,"ERROR -- lexikalna analyza skoncila zle\n");
    return NULL;
}

void load_all_token(){
    scanner_head = NULL;
    scanner_tail = NULL;
    t_token *input = NULL;
    do {
        input = load_token();
        scanner_append(input);
    }while (input->token_type != EMPTY);

}