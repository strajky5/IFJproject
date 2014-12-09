#include "interpret.h"
#include <expressions.h>
#include "parser.h"
#include "ial.h"
#include <math.h>
#include <errno.h>
#include <stdlib.h>
#include <stdio.h>
#include "str.h"
#include "stdbool.h"
#include "errors.h"
#include "list.h"

extern tTape *Tape;

votype stackinit(TStack *stack);								//funkce na inicializaci zasobniku
tErrors stackPush(TStack *stack,tParamItem *op1);			// pro pushnuti na zasobnik
tParamItem* TStackTopPop(TStack *stack);					// pro vziti ze zasobniku
votype StackDeleteDataDelete(TStack *stack);					// pro uvolneni zasobniku z pameti
quickSort( int a[], int l, int r);							// pro serazeni prvku pole
int partition( int a[], int l, int r);						// pomovna funkce pro quicksort

tErrors interpret()											// interpret
{
	
	stackinit(&stack);																		// inicializace pole
	while(Tape->active!=Tape->last){															// dokud nebudu na konci pasky pracuj
////////////////////////////Funkce///////////////////////////
		
		if(Tape->active->op1->type==FUNCTION)													// pokud je je typ funkce tak jdi do vetve pro funkce
		{

			stackPush(&stack,Tape->active->op1);												// pushnu si na zasobnik parametry funkce
			////////////////////////////definovane///////////////////////						// nini jdu do funci ktere jsou vestavene
			if (strCmpString(Tape->active->op1->name, "write"))									// pokud je vestavena funce write delej
				{
					while(stack->top->op1=NULL)													// cykli dokud nejsi za poslednim parametrem
					{	
						switch stack->top->op1->type {											// budem rozdelovat podle typu
							case O_INT: printf("%d",stack->top->op1->value.ival);break;			// jde o int tak tiskni int
							case O_REAL: printf("%f",stack->top->op1->value.rval);break;		// jde o real tak tiskni real
							case O_BOOL: if(stack->top->op1->value->bval==TRUE)					// jde o bool tiskni bool
											printf("TRUE");
										else printf("FALSE");
										break;
							case O_STRING: printf("%s",stack->top->op1->value.sval);break;		// jde o O_STRING tisku O_STRING
							default return E_RUNX;												// pokud neni ani jedna ztechto moznosti tak chyba
							}

						stack->top->op1=stack->top->op1->next;									// posunuse na dalsi parametr funkce
					}

				}
			else if(strCmpString(Tape->active->op1->name, "length"))							// pokud jde o length tak
			{
				if(stack->top->op1->type!=O_STRING) return E_RUNX;								// pokud neni typ O_STRING vrat chybu
				Tape->active->op2->value.ival=strGetLength(stack->top->op1->value.sval)			// do operandu dva nahrani vysledek
				Tape->active->op2->type=O_INT;													// a typ

			}
			else if(strCmpString(Tape->active->op1->name, "copy")) //// nevim jstly funguje		// pokud je copy
			{	
				int pozice=0;																	// vytvorim si pomocne promene pro prehlednost
				int pocet=0;
				char *ukaz;
				O_STRING pomocny;
				if(stack->top->op1->type!=O_STRING) return E_RUNX;								// pokud parametr funkce neni O_STRING error

				strCopyString(&pomocny, stack->top->op1->value.sval);							// zkopiruji si O_STRING do pomocne promene
				stack->top->op1=stack->top->next;												// posunuse na dalsi parametr funkce
				pozice=stack->top->op1->value.ival;												// nahraji si do promene pozici pro pod O_STRING
				stack->top->op1=stack->top->next;												// dalsi parametr funkce
				pocet=stack->top->op1->value.ival;												// pocet charu na zkopirovani

				ukaz= pomocny.str+pozice-1;														// nahraji si O_STRING ktery hledam do pole(predam si ukazatel)
				ukaz[pocet]='/0';																// na jeho konec nahraji /0
				pomocny.str=ukaz;																// nini nahraji ukazatel do O_STRINGu
				pomocny.length=pocet;															// dom do jeho parametru delku
				pomocny.allocSize=sizeof(char)*pocet;											// a jakou velikost pameti potrebuje
				strCopyString(pomocny, Tape->active->op2->value.sval);							// nini ho nahraji do op2
				Tape->active->op2->type=O_STRING;												// atyp prepisi na O_STRING


			}
			else if(strCmpString(Tape->active->op1->name, "find"))								// pokud je funkce find
			{
				int i=0;																		// tak si vytvorim pocitadlo
				char* st=stack->top->op1->value.sval;											// a ukazatel na pole (O_STRING)
				char* serch=stack->top->op1->next->value.sval;									// a na O_STRING hledany
				if(stack->top->op1->type!=O_STRING) return E_RUNX;								// pokud parametr neni sting error
				if(stack->top->op1->next->type!=O_STRING) return E_RUNX;						// pokud neni druchu paramter O_STRING error
				while (st[i]!=st[strGetLength(stack->->op1->value.sval)])						// dokud nejsi na konci O_STRINGu tak cykli
				{
					if(st[i]==serch[0])															// pokud se schoduje s charem prvnim hledaneho O_STRINGu 
					{
						int i1=0;																// tak vytvorim si druhe pocitadlo
						while(st[i+i1]==serch[i1]) 1i++;										// dokud se schoduji posunuj se a pocitej 
						if(i1==strGetLength(stack->->op1->value.sval)) {Tape->active->op2->value.ival=i; Tape->active->op2->type->O_INT;break;}
					}																			// pokud se schoduji velikosti O_STRINGu tak vrat pozici do op 2
					if(st[i+strGetLength(stack->op1->next->value.sval)-1]=='/0') break;				// pokud uz je mensi nez delka hledaneho O_STRINGu tak vyskoc
					i++;																		// navys pocitadlo
				}

			}
			else if(strCmpString(Tape->active->op1->name, "sort"))								// pokud je sort
			{
				char*pole=stack->top->op1->value.sval;											// tak si vytvorim pole znaku se srtingem
				if(stack->top->op1->type!=O_STRING) return E_RUNX;								// pokud neni typ sting error 
				quickSort( (int *)pole[], 0, strGetLength(stack->->op1->value.sval));			// propehne serazeni str get leng da velikost - \0
				Tape->active->op2->value.sval=(char*)pole;										// nini nahraji toto pole do op 2
				Tape->active->op2->type=O_STRING;												// a nastavim typ na O_STRING

			}
			else if(strCmpString(Tape->active->op1->name, "readln"))							// pokud je readln
			{
				switch stack->top->op1->type {													// kontroluji podle typu
					case O_INT: scanf("%d",&Tape->active->op2->value.ival);						// jde o inttak ho naskenuji a nahraji do op2 
								Tape->active->op2->type=O_INT;									// nastavim typ na int
								break;
					case O_REAL:scanf("%f",&Tape->active->op2->value.rval);						// pokud jde o real tak ho naskenuji do op2 
								Tape->active->op2->type=O_REAL;									// nasatavim typ
								break;
					case O_O_STRING:scanf("%s",&Tape->active->op2->value.sval);					// pokud O_STRING...
								Tape->active->op2->type=O_STRING;
								break;
					default : return E_RUNX;
				}

			}
			else if(Tape->active->instruction==CALL)											// kdyz je to funkce vlastni tak je tam call 
			{
				Tape->active=Tape->active->result->value->Tape_pointer;							// tak ze posunu se do ukazovaneho mista 
				continue;																		// vratim se na zacatek cyklu
			}
		}
		else if (Tape->active->op1->type==)
		{

		}
		else if (Tape->active->op1->instruction==JUMP)											// pokud jde o if neho while tak kontroluji na jump
		{
			if(Tape->active->previous->result->value.bval==FALSE && Tape->active->previous->result->type==O_BOOL){	// kdyz je predchozi vysledek false tak skacu
			Tape->active=Tape->active->result->value->Tape_pointer;								// a skacu tam kam ukazuje pointer
			continue;																			// a na zacatek cyklu
			}
			else ;																				// pokud neni false tak nic nedelej

		}
<<<<<<< .mine
		Tape->active=Tape->active->next;														// posunuti na pasce na dasi instructionukci
=======
		else if (Tape->active->op1->instruction==NOP)
		{
			// co returnout ??????????????????????????
			TStackTopPop(&stack);
			
		}
		Tape->active=Tape->active->next;														// posunuti na pasce na dasi instrukci
>>>>>>> .r271
		
	}
	


votype stackinit(TStack *stack)       // inicializace zaodbnika
{
    stack->top=NULL;                // top vyprazdnim
    
    return;
}

tErrors stackPush(TStack *stack,tParamItem *op1)                                    // funkce pro nahrati na zasobnik 
{
    TStackItem *phelp                                                           // pomocna promena pro svazani na zasobniku se spodnim prvkem

    if(stack->top==NULL )                                                       // kdyz je zadobnik prazdny tak
    {
        if((stack->top=malloc(sizeof(TStackItem)))==NULL) return E_INTERN;      // alokuj misto ptvku a nahraj ho na vrchol zasobniku
        stack->top->op1=op1;                                                    // a do tohoto prvku vloz operand 
        stack->pdown=NULL;                                                      // pro jistotu vloz do ukazatele na spodni prvek null
        return E_OK;                                                            

    }
    else                                                                        // kdyz neni prazdny
    {       
        if((phelp=malloc(sizeof(TStackItem)))==NULL) return E_INTERN;           // tak alokuj misto pro prvek a vloz ho do pomocneho ukazatele 
        phelp->pdown=stack->top;                                                // nini v pomocnem prvku nahraj vazbu s top vrcholem
        stack->top=phelp;                                                       // a nahraj pomocny prvek jako top
        stack->top->op1=op1;                                                    // nini na top vloz operand 
        return E_OK;
    }
    
}

tParamItem* TStackTopPop(TStack *stack)                                         // vyjme prvek ze zasobniku a vrati ukazatel na neho
{
    tParamItem *pom=stack->top->op1;                                                // do pomocneho prvku nahraj pozadovany operand s vrcholu
    TStackItem *toppom;                                                         // tvorba pomocneho prvku (kvuji uvolneni z pameti)

    if(stack->top==NULL) return NULL;                                           // pokud je seznam prazdny vracim null

    toppom=stack->top;                                                          // do pomocne nahraji vrchol zasobniku
    stack->top=stack->top->pdown;                                               // nini nastavym vrchol zasobnika o jedno nize
    free(toppom);                                                               // uvolnim pamet pro prvek zasobniku

    return pom;
}

votype StackDeleteDataDelete(TStack *stack)                                       // fuknce pro uvolneni zasobnika
{
    if (stack->top==NULL) return;                                                       // pokud je prazdny nic nedelej
    while(TStackTopPop(&stack)!=NULL)                                                   // dokud zasobnik neni prazdny tak pop
        ;
    return;
}

votype quickSort( int a[], int l, int r)
{
   int j;

   if( l < r ) 
   {
   	// divtypee and conquer
        j = partition( a, l, r);
       quickSort( a, l, j-1);
       quickSort( a, j+1, r);
   }
	
}



int partition( int a[], int l, int r) {
   int pivot, i, j, t;
   pivot = a[l];
   i = l; j = r+1;
		
   while( 1)
   {
   	do ++i; while( a[i] <= pivot && i <= r );
   	do --j; while( a[j] > pivot );
   	if( i >= j ) break;
   	t = a[i]; a[i] = a[j]; a[j] = t;
   }
   t = a[l]; a[l] = a[j]; a[j] = t;
   return j;
}

/****************************************ARITMETICKÉ INSTRUKCIE MIŠO*******************************/


/*********************************************ADD*********************************************************/

	   else if(strCmpString(Tape->active->instruction, "ADD"))
	   {
	   		if(Tape->active->op1->type == O_STRING)
					{
					if(strCopyString(&(instruction->result->value.sval),&(instruction->op1->value.sval))==1)
						return E_RUNX;
					else if((inst->op1->type == O_INT) && (Tape->active->op2->type == O_INT))
					{
						if(instruction->result->type == O_INT)
							instruction->result->value.ival=instruction->op1->value.ival+instruction->op2->value.ival;
						else 
							instruction->result->value.rval=instruction->op1->value.ival+instruction->op2->value.ival;
					}
					else if((Tape->active->op1->type == O_INT) && (Tape->active->op2->type == O_REAL))
						instruction->result->value.rval=instruction->op1->value.ival+instruction->op2->value.rval;
					else if((Tape->active->op1->type == O_REAL) && (Tape->active->op2->type == O_INT))
						instruction->result->value.rval=instruction->op1->value.rval+instruction->op2->value.ival;
					else if((Tape->active->op1->type == O_REAL) && (Tape->active->op2->type == O_REAL))
						instruction->result->value.rval=instruction->op1->value.rval+instruction->op2->value.rval;	
					else return E_INTERN;				
		}

/*********************************************SUB*********************************************************/
		else if(strCmpString(Tape->active->instruction, "SUB"))
		{
				if(Tape->active->op1->type == O_INT && Tape->active->op2->type == O_INT)
					{
					if(instruction->result->type == O_INT)
						instruction->result->value.ival=instruction->op1->value.ival-instruction->op2->value.ival;
					else instruction->result->value.dval=instruction->op1->value.ival-instruction->op2->value.ival;
					}
				else if(Tape->active->op1->type == O_INT && Tape->active->op2->type == O_REAL)
					instruction->result->value.dval=instruction->op1->value.ival-instruction->op2->value.dval;
				else if(Tape->active->op1->type == O_REAL && Tape->active->op2->type == O_INT)
					instruction->result->value.dval=instruction->op1->value.dval-instruction->op2->value.ival;
				else if(Tape->active->op1->type == O_REAL && Tape->active->op2->type == O_REAL)
					instruction->result->value.dval=instruction->op1->value.dval-instruction->op2->value.dval;	
				else return E_INTERN;	
		}		

/*********************************************MUL*********************************************************/
		else if(strCmpString(Tape->active->instruction, "MUL"))
		{
				if(Tape->active->op1->type == O_INT && Tape->active->op2->type == O_INT)
					{
					if(instruction->result->type == O_INT)
						instruction->result->value.ival=instruction->op1->value.ival*instruction->op2->value.ival;
					else instruction->result->value.dval=instruction->op1->value.ival*instruction->op2->value.ival;
					}
				else if(Tape->active->op1->type == O_INT && Tape->active->op2->type == O_REAL)
					instruction->result->value.dval=instruction->op1->value.ival*instruction->op2->value.dval;
				else if(Tape->active->op1->type == O_REAL && Tape->active->op2->type == O_INT)
					instruction->result->value.dval=instruction->op1->value.dval*instruction->op2->value.ival;
				else if(Tape->active->op1->type == O_REAL && Tape->active->op2->type == O_REAL)
					instruction->result->value.dval=instruction->op1->value.dval*instruction->op2->value.dval;	
				else return E_INTERN;	
		}

/*********************************************DIV*********************************************************/
		else if(strCmpString(Tape->active->instruction, "DIV"))
		{
			if(Tape->active->op1->type == O_INT && Tape->active->op2->type == O_INT)
					{
					if(instruction->result->type == O_INT && instruction->op2->value.ival!= 0)
						instruction->result->value.ival=instruction->op1->value.ival/instruction->op2->value.ival;
					else return E_INTERN;
					}
				else return E_INTERN;	
		}

/*********************************************BIGGER******************************************************/
		else if(strCmpString(Tape->active->instruction, "BIGGER"))
		{
			if(Tape->active->op1->type == O_INT && Tape->active->op2->type == O_INT)
					instruction->result->value.ival = (int)(instruction->op1->value.ival > instruction->op2->value.ival);
				else if(Tape->active->op1->type == O_REAL && Tape->active->op2->type == O_REAL)
					instruction->result->value.ival = (int)(instruction->op1->value.rval > instruction->op2->value.rval);
				else if(Tape->active->op1->type == O_STRING && Tape->active->op2->type == O_STRING)
					{
					if(strCmpString(&(instruction->op1->value.sval),&(instruction->op2->value.sval)) > 0)
						instruction->result->value.ival=1;
					else instruction->result->value.ival=0;
					}
				else return E_INTERN;
		}

/*********************************************SMALLER*****************************************************/
		else if(strCmpString(Tape->active->instruction, "SMALLER"))
		{
				if(Tape->active->op1->type == O_INT && Tape->active->op2->type == O_INT)
					instruction->result->value.ival = (int)(instruction->op1->value.ival<instruction->op2->value.ival);
				else if(Tape->active->op1->type == O_REAL && Tape->active->op2->type == O_REAL)
					instruction->result->value.ival = (int)(instruction->op1->value.rval<instruction->op2->value.rval);
				else if(Tape->active->op1->type == O_STRING && Tape->active->op2->type == O_STRING)
					{
					if(strCmpString(&(instruction->op1->value.sval),&(instruction->op2->value.sval))<0)
						instruction->result->value.ival=1;
					else instruction->result->value.ival=0;
					}
				else return E_INTERN;
		}
	
/*********************************************EBIGGER*****************************************************/
		else if(strCmpString(Tape->active->instruction, "EBIGGER"))
		{
				if(Tape->active->op1->type == O_INT && Tape->active->op2->type == O_INT)
					instruction->result->value.ival = (int)(instruction->op1->value.ival>=instruction->op2->value.ival);
				else if(Tape->active->op1->type == O_REAL && Tape->active->op2->type == O_REAL)
					instruction->result->value.ival = (int)(instruction->op1->value.rval>=instruction->op2->value.rval);
				else if(Tape->active->op1->type == O_STRING && Tape->active->op2->type == O_STRING)
					{
					if(strCmpString(&(instruction->op1->value.sval),&(instruction->op2->value.sval))>=0)
						instruction->result->value.ival=1;
					else instruction->result->value.ival=0;
					}
				else return E_INTERN;
		}
/*********************************************ESMALLER****************************************************/
		else if(strCmpString(Tape->active->instruction, "ESMALLER"))
		{
				if(Tape->active->op1->type == O_INT && Tape->active->op2->type == O_INT)
					instruction->result->value.ival = (int)(instruction->op1->value.ival<=instruction->op2->value.ival);
				else if(Tape->active->op1->type == O_REAL && Tape->active->op2->type == O_REAL)
					instruction->result->value.ival = (int)(instruction->op1->value.rval<=instruction->op2->value.rval);
				else if(Tape->active->op1->type == O_STRING && Tape->active->op2->type == O_STRING)
					{
					if(strCmpString(&(instruction->op1->value.sval),&(instruction->op2->value.sval))<=0)
						instruction->result->value.ival=1;
					else instruction->result->value.ival=0;
					}
				else return E_INTERN;
		}
/*********************************************EQUAL*******************************************************/
		else if(strCmpString(Tape->active->instruction, "EQUAL"))
		{
				if(Tape->active->op1->type == O_INT && Tape->active->op2->type == O_INT)
					instruction->result->value.ival = (int)(instruction->op1->value.ival==instruction->op2->value.ival);
				else if(Tape->active->op1->type == O_REAL && Tape->active->op2->type == O_REAL)
					instruction->result->value.ival = (int)(instruction->op1->value.rval==instruction->op2->value.rval);
				else if(Tape->active->op1->type == O_STRING && Tape->active->op2->type == O_STRING)
					{
					if(strCmpString(&(instruction->op1->value.sval),&(instruction->op2->value.sval))==0)
						instruction->result->value.ival=1;
					else instruction->result->value.ival=0;
					}
				else return E_INTERN;
		}
/*********************************************NEQUAL******************************************************/
		else if(strCmpString(Tape->active->instruction, "NEQUAL"))
		{
				if(Tape->active->op1->type == O_INT && Tape->active->op2->type == O_INT)
					instruction->result->value.ival = (int)(instruction->op1->value.ival!=instruction->op2->value.ival);
				else if(Tape->active->op1->type == O_REAL && Tape->active->op2->type == O_REAL)
					instruction->result->value.ival = (int)(instruction->op1->value.rval!=instruction->op2->value.rval);
				else if(Tape->active->op1->type == O_STRING && Tape->active->op2->type == O_STRING)
					{
					if(strCmpstring(&(instruction->op1->value.sval),&(instruction->op2->value.sval))!=0)
						instruction->result->value.ival=1;
					else instruction->result->value.ival=0;
					}
				else return E_INTERN;
		}
/*********************************************ASSIG*******************************************************/
		else if(strCmpString(Tape->active->instruction, "ASSIG"))
		{
				if(Tape->active->op1->type == O_INT && instruction->result->type == O_INT)
					instruction->result->value.ival = instruction->op1->value.ival;
				else if(Tape->active->op1->type == O_REAL && instruction->result->type == O_REAL)
					instruction->result->value.ival = instruction->op1->value.rval;
				else if(Tape->active->op1->type == O_STRING && instruction->result->type == O_STRING)
					{
					if(strCopystring(&(instruction->result->value.sval),&(instruction->op1->value.sval))== 1)
						return E_RUNX;
					}
				else return E_INTERN;
		}
}
/******************************KONIEC ARITMETICKÝCH INSTRKCÍ*************************************/


  

            
/*votype EditPar(tVariable *edit)                                                   // kdyz promena nema hodnotu je poslana do funkce a ja ze zasobniku pokud tam je tak je zamenim 
{
    tParamItem *pom=stack->top;                                                 // pomocna pro prohledani na zasobniku

    while(pom!=NULL)                                                            // cykli do konce zasobniku (parametry jsou spojene tak projizdim do konce)
    {
        if(strCmpString(edit->name,pom->name))                                  // porovnej jestli je schoda vyskoc
            break;
        pom=pom->next;                                                          // pomocnou posun dal
    }

    if(pom==NULL) return;                                                       // pokud je pom null tak neni na zasobniku

    switch pom->TYPE
        case O_INT: edit->value.ival=pom->value.ival; break;                    // kontroluji hledam typ a podle to ho pak kopiruji 
        case O_REAL: edit->value.rval=pom->value.rval; break;
        case O_O_STRING: strCopyString(edit->value.sval,pom->value.sval); break;
        case O_BOOL: edit->value.bval=pom->value.bval break;
        case NONTRM: break;
    return;
}*/