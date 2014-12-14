/* 
* Názov projektu: Implementace interpretu imperativního jazyka IFJ14 
* Dátum: 12/2014
* Autori: Matúš Cimerman, xcimer00
*         Tomáš Hynek ,   xhynek09
*         David Novák ,   xnovak0r
*         Michal Sedlák , xsedla0m
*         Jan Jořenek  ,  xjoren01
*/
#ifndef INT
#define INT

#include "list.h"
#include "str.h"
#include "ial.h"
#include "errors.h"

typedef struct Stack TStack;
typedef struct StackItem TStackItem;


struct Stack{   				// struktura zasobnika
	TStackItem *top;				// ukazatel na vrchol zasobniku
};

struct StackItem{   			// struktura pro prvek zasobnika
	tParamItem *op1;					// ukazatel na operand
	TStackItem *pdown;					// ukazatel na prvek pod nim
};
string Readstring();
void stackinit(TStack *stack);								//funkce na inicializaci zasobniku
tErrors stackPush(TStack *stack,tParamItem *op);			// pro pushnuti na zasobnik
tParamItem* TStackTopPop(TStack *stack);					// pro vziti ze zasobniku
void StackDeleteDataDelete(TStack *stack);					// pro uvolneni zasobniku z pameti
tParamItem *SearchStackName(string*Search);
string *conc(string*s1,string*s2);
tErrors interpret();
#endif

