
#ifndef _IFJTYPES_
#define _IFJTYPES_

/* Popis cinnosti:                                                                    */ 
/* - syntakticky analyzuje vyraz a vracia E_OK pri syntakticky spravnom vyraze,       */
/*   E_INTERN pri chybe alokacie a E_SYN pri syntakticky chybnom vyraze               */
/* - zacina prvym tokenom z vyrazu a konci tak ze nechava globalny token ) alebo ;    */
/* - generuje pasku instrukcii, tak ze vychadza z Tape->last vytvori seriu instrukcii */
/*   a konci vzdy operaciou NOP z ktorej sa bere iba result, ktory je vzdy vysledkom  */
/*   celeho vyrazu                                                                    */
/* - NONTERMINALY su previazane tak, ze ich staci menit na paske, su nepomenovane a   */
/*   neukladaju sa do tabulky symbolov                                                */
/* - true je ulozene ako int 1, false ako int 0                                       */
/* - hodnoty su prevedene zo stringu na int alebo double a popisane ako O_INT         */
/*   O_DOUBLE                                                                         */
/* - meno premennych je ulozene ako string name, ked je meno prazdny string nejedna   */
/*   sa o premennu, premenne nemaju priradeny ziadny typ moze sa stat ze bude         */
/*   implicitne nastaveny ako 0, co je v nasom pripade O_BOOL                         */
/* - na vytvorenie novej instrukcie na konci pasky pouziva funkciu                    */
/*   TapeNewVoidLastItem()                                                            */

#define TABLESIZE 8

#include "scanner.h"
#include "errors.h"
#include "types.h"

extern tTape *Tape;
extern tToken T;

typedef enum
{ 
    L, //LESS nizsia priorita
    M, //MORE vyssia priorita
    Q, //EQUAL rovna sa pri zatvorkach
    E, //ERROR chyba
    F  //FINISH koniec pri prvej nesparovanej pravej zatvorke
} tTabSigns;

typedef enum
{
    AD_SU_CO,		// 0 + - .
    MU_DI,			// 1 * /
    EQ_NE,			// 2 === !==
    LS_MO_LE_LM,	// 3 < > <= >=
    LP,				// 4 (
    RP,				// 5 )
    ID,				// 6 id
    SC,				// 7 ;
    NONTERM        // 8 neterminal
} tExpType;

typedef struct tElem 
{
        tExpType data;
        tInstruction instruction;
        tVariable *tempVarPtr;
        struct tElem *rptr;
} *tElemPtr;

typedef struct 
{ 
    tElemPtr Top;
} tExpStack;

void ExpStackInit (tExpStack *L);
void ExpStackDispose (tExpStack *L);
tErrors ExpStackPush (tExpStack *L, tExpType val);
void ExpStackTop (tExpStack *L, tExpType *val);
void ExpStackTop (tExpStack *L, tExpType *val);
void ExpStackTopTerminal (tExpStack *L, tExpType *val);
void ExpStackPop (tExpStack *L);
void ExpStackSecPop (tExpStack *L);
int ExpStackEmpty (tExpStack *L);

tTabSigns SearchSign(tExpType row, tExpType col);
tErrors ConvertToken(tExpType *type);
tErrors ExpStackReduct(tExpStack *S, tTabSigns sign);
tErrors ExpParser();


#endif

