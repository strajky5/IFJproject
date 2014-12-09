#ifndef STRINGS
#define STRINGS
//hlavickovy soubor pro praci s nekonecne dlouhymi retezci
#define STR_ERROR   1
#define STR_SUCCESS 0
#define STR_LEN_INC 8
typedef struct
{
  char* str;		// misto pro dany retezec ukonceny znakem '\0'
  int length;		// skutecna delka retezce
  int allocSize;	// velikost alokovane pameti
} string;


int strInit(string *s);
void strFree(string *s);

void strClear(string *s);
int strAddChar(string *s1, char c);
int strCopystring(string *s1, string *s2);
int strCmpstring(string *s1, string *s2);
int strCmpConstStr(string *s1, char *s2);
int strFromChar(string *S1, char *S2);

char *strGetStr(string *s);
int strGetLength(string *s);
#endif