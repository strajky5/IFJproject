
#ifndef _IAL_
#define _IAL_

#include "errors.h"
#include "strings.h"
#include "types.h"
#include "interpret.h"

#define TRUE 1
#define max(a, b) ((a < b) ? b : a)
#define INIT_V -1
#define ASCII_SIZE 256



typedef struct tBSTNode {
	tVariable * tree_data;                                
	struct tBSTNode * LPtr;                                    /* levý podstrom */
	struct tBSTNode * RPtr;                                   /* pravý podstrom */
} *tBSTNodePtr;	


//deklaracie funkcii:
tErrors internfun_param_chceck(tBSTNodePtr* RootPtr,tParamItem *param,tVariable* tmp_param);

//tabulka symbolov implementovana ako binarny vyhladavaci strom
void Print_tree2(tBSTNodePtr TempTree, char* sufix, char fromdir);

void BSTInit (tBSTNodePtr *);
tVariable* BSTSearch (tBSTNodePtr RootPtr,string name);
tErrors BSTInsert (tBSTNodePtr* RootPtr,string name,tVariable* var);
void BSTDispose(tBSTNodePtr *);

//prevody
void boolval(tVariable *term);
tErrors strval(tVariable *term);
void intval(tVariable *term);
void doubleval(tVariable *term);

string get_string();
tErrors put_string(tBSTNodePtr* RootPtr,tVariable* result,tParamItem *param,tVariable* tmp_param);
int strLen(char *s);
tErrors get_substring(tBSTNodePtr* RootPtr, tVariable* result,tParamItem *param,tVariable* tmp_param);
tErrors find_string(tBSTNodePtr* RootPtr,tVariable* result,tParamItem *param,tVariable* tmp_param);
void sort_string(char *s);

//pomocne funkcie:
void swap(char *a, char *b);
void quickSort( char *s, int left, int right);
int partition(char *s, int left, int right);

#endif

