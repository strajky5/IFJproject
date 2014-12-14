/* 
* Názov projektu: Implementace interpretu imperativního jazyka IFJ14 
* Dátum: 12/2014
* Autori: Matúš Cimerman, xcimer00
*         Tomáš Hynek ,   xhynek09
*         David Novák ,   xnovak0r
*         Michal Sedlák , xsedla0m
*         Jan Jořenek  ,  xjoren01
*/
#ifndef _ERRORS_H
#define _ERRORS_H
typedef enum
{ 
    E_OK      = 0,      
    E_LEX     = 1,      //chyba v programu v ramci lexikalni analyzy
    E_SYN     = 2,      //chyba v programu v ramci syntakticke analyzy
    E_SEMA    = 3,      //semanticka chyba v programu – nedefinovana funkce/promenna, pokus o redefinici funkce/promenne, atd.
    E_SEMB    = 4,      //semanticka chyba typove kompatibility v aritmetickych, retezcovych a relacnich vyrazech, prip. spatny pocet ci typ parametru u volani funkce
    E_SEMC    = 5,      //ostatni semanticke chyby
    E_RUNSTD  = 6,      //behova chyba pri nacitani ciselne hodnoty ze vstupu
    E_RUNVAR  = 7,      //behova chyba pri praci s neinicializovanou promennou
    E_RUNDIVZ = 8,      //behova chyba deleni nulou
	E_RUNX    = 9,      //ostatni behove chyby
    E_INTERN  = 99      //interni chyba interpretu tj. neovlivnena vstupnim programem
} tErrors;
#endif