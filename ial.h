/* 
* N�zov projektu: Implementace interpretu imperativn�ho jazyka IFJ14 
* D�tum: 12/2014
* Autori: Mat�� Cimerman, xcimer00
*         Tom� Hynek ,   xhynek09
*         David Nov�k ,   xnovak0r
*         Michal Sedl�k , xsedla0m
*         Jan Jorenek  ,  xjoren01
*/
#ifndef IAL
#define IAL

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <stdbool.h>
#include "errors.h"
#include "str.h"
#include "list.h"

#define STR_ERR 78

extern tVariable *TempVar;


typedef struct tBSTNode {
	tVariable * tree_data;                                
	struct tBSTNode * LPtr;                                    /* lev� podstrom */
	struct tBSTNode * RPtr;                                   /* prav� podstrom */
} *tBSTNodePtr;


tErrors BSTInsert (tBSTNodePtr* RootPtr, string name, tVariable* var);
tVariable* BSTSearch (tBSTNodePtr RootPtr, string name);
void BSTInit (tBSTNodePtr *RootPtr);
void BSTDispose (tBSTNodePtr *RootPtr);

void swap(char *a, char *b);
int partition (char *s, int left, int right);
void quickSort ( char *string, int left, int right);

//int findBM(char *pattern_str, int length_p, char *find_str, int length_f);
//void prefixes_occurence(char *pattern_str, int length, int Array_Pattern[]);
//void suffixes(char *pattern_str, int length, int *suff);
//void prefixes_pattern(char *pattern_str, int length, int Array_Occurence[]);

#endif
