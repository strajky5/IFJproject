#ifndef ENUM
#define ENUM
#include "str.h"
typedef enum {
    T_ERRORTOKEN,   // 0 error, zlyhanie nacitania platneho lexemu
	T_ERRORSYSTEM,  // 1
    T_EOF,      	// 2 koniec suboru
    T_KEYWORD,      // 3 klucove slovo
    T_KONST,        // 4 true,false
    T_ID,           // 5 identifikator
	T_DATATYPE,     // 6
    T_INT,          // 7 int
    T_REAL,         // 8 real
    T_STRING,       // 9 string
    T_BOOLEAN,      // 10 boolean
    T_LB,           // 11 (	
    T_RB,           // 12 )
    T_MUL,          // 13 *
    T_DIV,          // 14 /
    T_ADD,          // 15 +
    T_SUB,          // 16 -
    T_LESS,         // 17 <
    T_MORE,         // 18 >
    T_LESS_EQ,      // 19 <=
    T_MORE_EQ,      // 20 >=
    T_NOT_EQ,       // 21 <>
    T_ASSIGN,       // 22 :=
    T_EQUAL,        // 23 =
    T_COMMA,        // 24 ,
    T_SEMICOLON,    // 25 ;
	T_COLON,        // 26 :
	T_DOT           // 27 .
} tTypes;

typedef struct {    //struktura tokenu
    tTypes type;    //type = typ tokenu
    string s;       //string na atribut tokenu ak je treba
} tToken;


void gettoken();        //vracia token
int comment();        //preskakuje komentare v zdrojovom subori
int keyword(char*sid);        //detekuje keywords
#endif 