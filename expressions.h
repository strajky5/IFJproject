/* 
* Názov projektu: Implementace interpretu imperativního jazyka IFJ14 
* Dátum: 12/2014
* Autori: Matúš Cimerman, xcimer00
*         Tomáš Hynek ,   xhynek09
*         David Novák ,   xnovak0r
*         Michal Sedlák , xsedla0m
*         Jan Jořenek  ,  xjoren01
*/
#ifndef TYPES
#define TYPES

#include "scaner.h"
#include "errors.h"
#include "list.h"
#include "ial.h"
#define FALSE 0
#define TRUE 1
#define TABSIZE 8

extern FILE *f;
extern tToken T;
extern tTape* Tape;
extern tBSTNodePtr TempTree;
extern tBSTNodePtr TempTreeL;
extern tVariable *TempVar;


typedef enum
{
    L,              // 0 LESS  - nizsi priorita
    M,              // 1 MORE  - vyssi priorita
    Q,              // 2 EQUAL - rovna priorita
    E               // 3 ERROR - chyba

} tTabSigns;

typedef enum
{
    MUL_DIV,		      // 0 * /
    ADD_SUB,              // 1 + -
    REL_OP,	              // 2 < > <= >= <> =
    LB,				      // 3 (
    RB,				      // 4 )
    ID,				      // 5 id
    DOLAR,                // 6 $
    KEYWORD,              // 7 klicove slovo - 'IF','WHILE'
    NONTERM,              // 8 neterminal
    COMMA,                // 9 ,
    ASSIGNED              // 10 :=
} tExpType;


typedef struct tElem
{
        tExpType data;                  //typ tokenu
        tInstruction instruction;       //instrukce (ADD,SUB,MUL.DIV,...)
        tVariable *tempVarPtr;          //ukazatel na prvek, kde je ulozena hodnota, typ a naze tokenu
        struct tElem *rptr;             //ukazatel na nasledujici prvek zasobniku
} *tElemPtr;

typedef struct
{
    tElemPtr Top;                       //ukazuje na vrchol zasobniku
} tExpStack;

void ExpStackInit (tExpStack *L);                         //inicializace zasobniku
void ExpStackDispose (tExpStack *L);                      //zrusen i celeho zasobniku
tErrors ExpStackPush (tExpStack *L, tExpType val, tVariable *item);        //vlozeni prvku na zacatek zasobniku
void ExpStackTop (tExpStack *L, tExpType *val);           //vrati hodnotu prvku na vrcholu asobniku
void ExpStackSecTop (tExpStack *L, tExpType *val);        //vrati hodnotu druheho prvku na zasobniku
void ExpStackTopTerminal (tExpStack *L, tExpType *val);   //hleda na zasobniku prvni terminal a jeho hodnotu ulozi do val
void ExpStackPop (tExpStack *L);                          //vymaze prvek na vrcholu zasobniku
void ExpStackSecPop (tExpStack *L);                       //vymaze druhy prvek na zasobniku
tErrors ExpStackReduct(tExpStack *S, tTabSigns sign);     //zredukuje zasobnik v pripade sign= '=' nebo '>'
void ExpStackTopPoint (tExpStack *L, tElemPtr *val);
void ExpStackSecTopPoint (tExpStack *L, tElemPtr *val);
void ExpStackInstructionTopPoint (tExpStack *L, tElemPtr *val);

tErrors ExpMainFunction();                                //hlavni funkce
tTabSigns SearchSign(tExpType row, tExpType col);         //vraci index z precedencni tabulky
tErrors ConverToken(tExpType *type);                      //identifikace tokenu
tVariable *SearchDataType(tBSTNodePtr TTree, tBSTNodePtr TTreeL);   //
tErrors SearchFun();                                      //pro zpracovani funkce ve vyrazu
tErrors ExpParserSem(tOperand *tOp1, tOperand *tOp2, tInstruction *tInstr); // semanticke pravidla pro vyrazy 

#endif
