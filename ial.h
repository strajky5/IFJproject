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
	struct tBSTNode * LPtr;                                    /* levý podstrom */
	struct tBSTNode * RPtr;                                   /* pravý podstrom */
} *tBSTNodePtr;


tErrors BSTInsert (tBSTNodePtr* RootPtr, string name, tVariable* var);
tVariable* BSTSearch (tBSTNodePtr RootPtr, string name);
void BSTInit (tBSTNodePtr *RootPtr);
void BSTDispose (tBSTNodePtr *RootPtr);

void swap(char *a, char *b);
int partition (char *s, int left, int right);
void quickSort ( char *string, int left, int right);

#endif
