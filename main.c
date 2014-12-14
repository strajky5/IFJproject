/* 
* Názov projektu: Implementace interpretu imperativního jazyka IFJ14 
* Dátum: 12/2014
* Autori:  Matúš Cimerman, xcimer00
*          Tomáš Hynek ,   xhynek09
*          David Novák ,   xnovak0r
*          Michal Sedlák , xsedla0m
*          Jan Jořenek  ,  xjoren01
*/
#ifndef maink
#define maink
#include <stdio.h>
#include <ctype.h>
#include "scaner.h"
#include "parser.h"
#include "errors.h"
#include "ial.h"
#include "list.h"
#include "interpret.h"
tErrors error;
FILE *f;
tToken T;

int main(int argc, char *argv[])
{
	
    if (argc != 2)
    {
      printf("Zle zadan vstupni soubor\n");
      return E_INTERN;
    }
    if ((f = fopen(argv[1], "r")) == NULL)
    {
      printf("Soubor se nepodarilo otevrit\n");
      return E_INTERN;
    }  	
    //FILE *source;
	error =0;
    //source = f;


    if((strInit(&(T.s))) == STR_ERROR)
        error=E_INTERN;
	if(error==E_OK)
		error = parser();
	

	if(error==E_OK){
	 error= interpret();
	 }
	 /*if(error!=E_OK)
		error = E_RUNX;*/
	 freeAlloc(); 
	 BSTDispose(&TempTree);
	 if((fclose(f))!= 0)
        error=E_INTERN;
    switch(error){
        case E_LEX:
            fprintf(stderr,"Lexikalna chyba\n");
            break;
        case E_SYN:
            fprintf(stderr,"Syntakticka chyba\n");
            break;
        case E_SEMA:
           fprintf(stderr,"Semanticka chyba: Nedefinovana funkcia\n");
           break;
        case E_SEMB:
            fprintf(stderr,"Semanticka chyba: Chybajuci parameter\n");
            break;
        case E_SEMC:
            fprintf(stderr,"Semanticka chyba: Nedeklarovana premenna\n");
            break;
        case E_RUNSTD:
            fprintf(stderr,"Semanticka chyba: Delenie nulou\n");
            break;
        case E_RUNVAR:
            fprintf(stderr,"Semanticka chyba: Chyba pri pretypovani\n");
            break;
        case E_RUNDIVZ:
            fprintf(stderr,"Semanticka chyba: Chyba typovej kombatibility\n");
            break;
        case E_RUNX:
            fprintf(stderr,"Semanticka chyba\n");
            break;
        case E_INTERN:
            fprintf(stderr,"Interna chyba\n");
            break;
        default:
            break;
	}
    return error;
}
#endif