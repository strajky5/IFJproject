//#include "interpret.h"
#include "expressions.h"
#include "parser.h"
#include "scaner.h"
#include "ial.h"
#include <math.h>
#include <errno.h>
#include <stdlib.h>
#include <stdio.h>
#include "str.h"
#include "stdbool.h"
#include "errors.h"
#include "list.h"
#include "interpret.h"

extern tTape *Tape;


  //Globálna premenná  pre zásobník
typedef struct Stack TStack;
typedef struct StackItem TStackItem;
struct StackItem{   			// struktura pro prvek zasobnika
								// obsahuje:
tParamItem *op1;					// ukazatel na operand
TStack * pdown;					// ukazatel na prvek pod nim

};

struct Stack{   				// struktura zasobnika

TStackItem * top;				// ukazatel na vrchol zasobniku

};
TStack stack;
void stackinit(TStack *stack);								//funkce na inicializaci zasobniku
tErrors stackPush(TStack *stack,tParamItem *op);			// pro pushnuti na zasobnik
tParamItem* TStackTopPop(TStack *stack);					// pro vziti ze zasobniku
void StackDeleteDataDelete(TStack *stack);					// pro uvolneni zasobniku z pameti
quickSort( int a[], int l, int r);							// pro serazeni prvku pole
int partition( int a[], int l, int r);						// pomovna funkce pro quicksort
tParamItem *SearchStackName(string*Search);
string *conc(string*s1,string*s2);

tErrors interpret()											// interpret
{
    tTapeItem *savepositionCall=NULL;
    tTapeItem *savepositionJump=NULL;
    tParamItem *hodnota,*phodnota;


	stackinit(&stack);

																			// inicializace pole
	while(Tape->active!=Tape->last){
		hodnota=SearchStackName(&Tape->active->op1->name);
		phodnota=SearchStackName(&Tape->active->op2->name);
																// dokud nebudu na konci pasky pracuj
////////////////////////////Funkce///////////////////////////

		if(Tape->active->op1->type==FUNCTION)													// pokud je je typ funkce tak jdi do vetve pro funkce
		{

			stackPush(&stack,Tape->active->op1);												// pushnu si na zasobnik parametry funkce
			////////////////////////////definovane///////////////////////						// nini jdu do funci ktere jsou vestavene
			if (strCmpConstStr(&Tape->active->op1->name, "write"))									// pokud je vestavena funce write delej
				{
					while(stack.top->op1=NULL)													// cykli dokud nejsi za poslednim parametrem
					{
						switch (stack.top->op1->type) {											// budem rozdelovat podle typu
							case O_INT: printf("%d",stack.top->op1->value.ival);break;			// jde o int tak tiskni int
							case O_REAL: printf("%f",stack.top->op1->value.rval);break;		// jde o real tak tiskni real
							case O_BOOL: if(stack.top->op1->value.bval==TRUE)					// jde o bool tiskni bool
											printf("TRUE");
										else printf("FALSE");
										break;
							case O_STRING: printf("%s",stack.top->op1->value.sval);break;		// jde o string tisku string
							default: return E_RUNX;												// pokud neni ani jedna ztechto moznosti tak chyba
							}

						stack.top->op1=stack.top->op1->next;									// posunuse na dalsi parametr funkce
					}

				}
			else if(strCmpstring(&Tape->active->op1->name, "length"))							// pokud jde o length tak
			{
				if(stack.top->op1->type!=O_STRING) return E_RUNX;								// pokud neni typ string vrat chybu
				Tape->active->op1->value.ival=strGetLength(&stack.top->op1->value.sval);			// do operandu dva nahrani vysledek
				Tape->active->op1->type=O_INT;													// a typ

			}
			else if(strCmpstring(&Tape->active->op1->name, "copy")) //// nevim jstly funguje		// pokud je copy
			{
				int pozice=0;																	// vytvorim si pomocne promene pro prehlednost
				int pocet=0;
				char *ukaz;
				string pomocny;
				if(stack.top->op1->type!=O_STRING) return E_RUNX;								// pokud parametr funkce neni string error

				strCopystring(&pomocny, &stack.top->op1->value.sval);							// zkopiruji si string do pomocne promene
				stack.top->op1=stack.top->op1->next;												// posunuse na dalsi parametr funkce
				pozice=stack.top->op1->value.ival;												// nahraji si do promene pozici pro pod string
				stack.top->op1=stack.top->op1->next;												// dalsi parametr funkce
				pocet=stack.top->op1->value.ival;												// pocet charu na zkopirovani

				ukaz= pomocny.str+pozice-1;														// nahraji si string ktery hledam do pole(predam si ukazatel)
				ukaz[pocet]='/0';																// na jeho konec nahraji /0
				pomocny.str=ukaz;																// nini nahraji ukazatel do stringu
				pomocny.length=pocet;															// dom do jeho parametru delku
				pomocny.allocSize=sizeof(char)*pocet;											// a jakou velikost pameti potrebuje
				strCopystring(&pomocny, &Tape->active->op1->value.sval);							// nini ho nahraji do op2
				Tape->active->op1->type=O_STRING;												// atyp prepisi na string


			}
			else if(strCmpstring(&Tape->active->op1->name, "find"))								// pokud je funkce find
			{
				int i=0;																		// tak si vytvorim pocitadlo
				char* st=stack.top->op1->value.sval.str;											// a ukazatel na pole (string)
				char* serch=stack.top->op1->next->value.sval.str;									// a na string hledany
				if(stack.top->op1->type!=O_STRING) return E_RUNX;								// pokud parametr neni sting error
				if(stack.top->op1->next->type!=O_STRING) return E_RUNX;						// pokud neni druchu paramter string error
				while (st[i]!=st[strGetLength(&stack.top->op1->value.sval)])						// dokud nejsi na konci stringu tak cykli
				{
					if(st[i]==serch[0])															// pokud se schoduje s charem prvnim hledaneho stringu
					{
						int i1=0;																// tak vytvorim si druhe pocitadlo
						while(st[i+i1]==serch[i1]) i1++;										// dokud se schoduji posunuj se a pocitej
						if(i1==strGetLength(&stack.top->op1->value.sval))
                            {Tape->active->op1->value.ival=i;
                            Tape->active->op1->type=O_INT;
                            break;}
					}																			// pokud se schoduji velikosti stringu tak vrat pozici do op 2
					if(st[i+strGetLength(&stack.top->op1->next->value.sval)-1]=='/0') break;				// pokud uz je mensi nez delka hledaneho stringu tak vyskoc
					i++;																		// navys pocitadlo
				}

			}
			else if(strCmpstring(&Tape->active->op1->name, "sort"))								// pokud je sort
			{
				char*pole=stack.top->op1->value.sval.str;											// tak si vytvorim pole znaku se srtingem
				if(stack.top->op1->type!=O_STRING) return E_RUNX;								// pokud neni typ sting error
				quickSort( (int *)pole, 0, strGetLength(&stack.top->op1->value.sval));			// propehne serazeni str get leng da velikost - \0
				Tape->active->op1->value.sval.str=(char*)pole;										// nini nahraji toto pole do op 2
				Tape->active->op1->type=O_STRING;												// a nastavim typ na string

			}
			else if(strCmpstring(&Tape->active->op1->name, "readln"))							// pokud je readln
			{
				switch (stack.top->op1->type) {													// kontroluji podle typu
					case O_INT: scanf("%d",&Tape->active->op1->value.ival);						// jde o inttak ho naskenuji a nahraji do op2
								Tape->active->op1->type=O_INT;									// nastavim typ na int
								break;
					case O_REAL:scanf("%f",&Tape->active->op1->value.rval);						// pokud jde o real tak ho naskenuji do op2
								Tape->active->op1->type=O_REAL;									// nasatavim typ
								break;
					case O_STRING:scanf("%s",&Tape->active->op1->value.sval);					// pokud string...
								Tape->active->op1->type=O_STRING;
								break;
					default : return E_RUNX;
				}

			}
			else if(Tape->active->instruction==CALL)											// kdyz je to funkce vlastni tak je tam call
			{
			    Tape->active->op1->value.tape_pointer=savepositionCall;
			    savepositionCall=Tape->active;
			    Tape->active=Tape->active->result->value.tape_pointer->next;					// tak ze posunu se do ukazovaneho mista
				continue;																		// vratim se na zacatek cyklu
			}
		}

		else if (Tape->active->instruction==JUMP)											// pokud jde o if neho while tak kontroluji na jump
		{
		    savepositionJump=Tape->active;
		    Tape->active->op1->value.tape_pointer=savepositionJump;
			if(Tape->active->previous->result->value.bval==FALSE && Tape->active->previous->result->type==O_BOOL){	// kdyz je predchozi vysledek false tak skacu
			savepositionJump=Tape->active->op1->value.tape_pointer;
			Tape->active=Tape->active->result->value.tape_pointer;								// a skacu tam kam ukazuje pointer
			continue;																			// a na zacatek cyklu
			}
			else return E_INTERN;																				// pokud neni false tak nic nedelej

		}
		else if (Tape->active->instruction==NOP)
		{

			Tape->active=savepositionCall->next;
			TStackTopPop(&stack);

		}
		else if (Tape->active->instruction==NOPJ)
		{
			Tape->active=savepositionJump->previous;
		}
		/*********************************************ADD*********************************************************/
	   else if(Tape->active->instruction==ADD)
	   {

	   		if(Tape->active->op1->type==NONTRM)																					//ZEPTAT SE JESTLY MUZE BYT 2OP noterm
	   		{
	   			Tape->active->op1->type=Tape->active->previous->result->type;
	   		}

	   		else if(hodnota!=NULL && phodnota!=NULL)
	   		{
	   			if(hodnota->type == O_STRING && phodnota->type == O_STRING)
					{
					if(conc(&hodnota->value.sval,&phodnota->value.sval)==NULL)	/// spojeni dvou stringu
						return E_RUNX;
					}
				else if(hodnota->type == O_INT && phodnota->type == O_INT)
					{
						Tape->active->result->type=O_INT;
						Tape->active->result->value.ival=hodnota->value.ival+phodnota->value.ival;
					}
				else if(hodnota->type == O_INT && phodnota->type == O_REAL){
					Tape->active->result->value.rval=hodnota->value.ival+phodnota->value.rval;
					Tape->active->result->type=O_REAL;
				}

				else if(hodnota->type == O_REAL && phodnota->type == O_INT){
					Tape->active->result->type=O_REAL;
					Tape->active->result->value.rval=hodnota->value.rval+phodnota->value.ival;
				}

				else if(hodnota->type == O_REAL && phodnota->type == O_REAL){
					Tape->active->result->type=O_REAL;
					Tape->active->result->value.rval=hodnota->value.rval+phodnota->value.rval;
				}

				else return E_INTERN;


	   		}
	   		else if(hodnota!=NULL && phodnota==NULL)
	   		{
	   			if(hodnota->type == O_STRING && phodnota->type == O_STRING)
					{
				if(conc(&hodnota->value.sval,&Tape->active->op2->value.sval)==NULL)	/// spojeni dvou stringu
						return E_RUNX;
					}
				else if(hodnota->type == O_INT && Tape->active->op2->type == O_INT)
					{
						Tape->active->result->type=O_INT;
						Tape->active->result->value.ival=hodnota->value.ival+Tape->active->op2->value.ival;
					}
				else if(hodnota->type == O_INT && Tape->active->op2->type == O_REAL){
					Tape->active->result->value.rval=hodnota->value.ival+Tape->active->op2->value.rval;
					Tape->active->result->type=O_REAL;
				}

				else if(hodnota->type == O_REAL && Tape->active->op2->type == O_INT){
					Tape->active->result->type=O_REAL;
					Tape->active->result->value.rval=hodnota->value.rval+Tape->active->op2->value.ival;
				}

				else if(hodnota->type == O_REAL && Tape->active->op2->type == O_REAL){
					Tape->active->result->type=O_REAL;
					Tape->active->result->value.rval=hodnota->value.rval+Tape->active->op2->value.rval;
				}

				else return E_INTERN;


	   		}

	   		else if(hodnota==NULL && phodnota!=NULL)
	   		{
	   			if(Tape->active->op1->type == O_STRING && phodnota->type == O_STRING)
					{
				if(conc(&Tape->active->op1->value.sval,&phodnota->value.sval)==NULL)	/// spojeni dvou stringu
						return E_RUNX;
					}
				else if(Tape->active->op1->type == O_INT && phodnota->type == O_INT)
					{
						Tape->active->result->type=O_INT;
						Tape->active->result->value.ival=Tape->active->op1->value.ival+phodnota->value.ival;
					}

				else if(Tape->active->op1->type == O_INT && phodnota->type == O_REAL){
					Tape->active->result->value.rval=Tape->active->op1->value.ival+phodnota->value.rval;
					Tape->active->result->type=O_REAL;
				}

				else if(Tape->active->op1->type == O_REAL && phodnota->type == O_INT){
					Tape->active->result->type=O_REAL;
					Tape->active->result->value.rval=Tape->active->op1->value.rval+phodnota->value.ival;
				}

				else if(Tape->active->op1->type == O_REAL && phodnota->type == O_REAL){
					Tape->active->result->type=O_REAL;
					Tape->active->result->value.rval=Tape->active->op1->value.rval+phodnota->value.rval;
				}

				else return E_INTERN;
			}

	   		else
	   		{
	   			if(Tape->active->op1->type == O_STRING && Tape->active->op2->type == O_STRING)
					{
				if(conc(&Tape->active->op1->value.sval,&Tape->active->op2->value.sval)==NULL)	/// spojeni dvou stringu
						return E_RUNX;
					}
				else if(Tape->active->op1->type == O_INT && Tape->active->op2->type == O_INT)
					{
						Tape->active->result->type=O_INT;
						Tape->active->result->value.ival=Tape->active->op1->value.ival+Tape->active->op2->value.ival;
					}
				else if(Tape->active->op1->type == O_INT && phodnota->type == O_REAL){
					Tape->active->result->value.rval=Tape->active->op1->value.ival+Tape->active->op2->value.rval;
					Tape->active->result->type=O_REAL;
				}

				else if(Tape->active->op1->type == O_REAL && Tape->active->op2->type == O_INT){
					Tape->active->result->type=O_REAL;
					Tape->active->result->value.rval=Tape->active->op1->value.rval+Tape->active->op2->value.ival;
				}

				else if(Tape->active->op1->type == O_REAL && Tape->active->op2->type == O_REAL){
					Tape->active->result->type=O_REAL;
					Tape->active->result->value.rval=Tape->active->op1->value.rval+Tape->active->op2->value.rval;
				}

				else return E_INTERN;
			}
		}

/*********************************************SUB*********************************************************/
		 else if(Tape->active->instruction==SUB)
	   {

	   		if(Tape->active->op1->type==NONTRM)																					//ZEPTAT SE JESTLY MUZE BYT 2OP noterm
	   		{
	   			Tape->active->op1->type=Tape->active->previous->result->type;
	   		}

	   		else if(hodnota!=NULL && phodnota!=NULL)
	   		{

				if(hodnota->type == O_INT && phodnota->type == O_INT)
					{
						Tape->active->result->type=O_INT;
						Tape->active->result->value.ival=hodnota->value.ival-phodnota->value.ival;
					}
				else if(hodnota->type == O_INT && phodnota->type == O_REAL){
					Tape->active->result->value.rval=hodnota->value.ival-phodnota->value.rval;
					Tape->active->result->type=O_REAL;
				}

				else if(hodnota->type == O_REAL && phodnota->type == O_INT){
					Tape->active->result->type=O_REAL;
					Tape->active->result->value.rval=hodnota->value.rval-phodnota->value.ival;
				}

				else if(hodnota->type == O_REAL && phodnota->type == O_REAL){
					Tape->active->result->type=O_REAL;
					Tape->active->result->value.rval=hodnota->value.rval-phodnota->value.rval;
				}

				else return E_INTERN;


	   		}
	   		else if(hodnota!=NULL && phodnota==NULL)
	   		{

				if(hodnota->type == O_INT && Tape->active->op2->type == O_INT)
					{
						Tape->active->result->type=O_INT;
						Tape->active->result->value.ival=hodnota->value.ival-Tape->active->op2->value.ival;
					}
				else if(hodnota->type == O_INT && Tape->active->op2->type == O_REAL){
					Tape->active->result->value.rval=hodnota->value.ival-Tape->active->op2->value.rval;
					Tape->active->result->type=O_REAL;
				}

				else if(hodnota->type == O_REAL && Tape->active->op2->type == O_INT){
					Tape->active->result->type=O_REAL;
					Tape->active->result->value.rval=hodnota->value.rval-Tape->active->op2->value.ival;
				}

				else if(hodnota->type == O_REAL && Tape->active->op2->type == O_REAL){
					Tape->active->result->type=O_REAL;
					Tape->active->result->value.rval=hodnota->value.rval-Tape->active->op2->value.rval;
				}

				else return E_INTERN;


	   		}

	   		else if(hodnota==NULL && phodnota!=NULL)
	   		{

				if(Tape->active->op1->type == O_INT && phodnota->type == O_INT)
					{
						Tape->active->result->type=O_INT;
						Tape->active->result->value.ival=Tape->active->op1->value.ival-phodnota->value.ival;
					}

				else if(Tape->active->op1->type == O_INT && phodnota->type == O_REAL){
					Tape->active->result->value.rval=Tape->active->op1->value.ival-phodnota->value.rval;
					Tape->active->result->type=O_REAL;
				}

				else if(Tape->active->op1->type == O_REAL && phodnota->type == O_INT){
					Tape->active->result->type=O_REAL;
					Tape->active->result->value.rval=Tape->active->op1->value.rval-phodnota->value.ival;
				}

				else if(Tape->active->op1->type == O_REAL && phodnota->type == O_REAL){
					Tape->active->result->type=O_REAL;
					Tape->active->result->value.rval=Tape->active->op1->value.rval-phodnota->value.rval;
				}

				else return E_INTERN;
			}

	   		else
	   		{

				if(Tape->active->op1->type == O_INT && Tape->active->op2->type == O_INT)
					{
						Tape->active->result->type=O_INT;
						Tape->active->result->value.ival=Tape->active->op1->value.ival-Tape->active->op2->value.ival;
					}
				else if(Tape->active->op1->type == O_INT && phodnota->type == O_REAL){
					Tape->active->result->value.rval=Tape->active->op1->value.ival-Tape->active->op2->value.rval;
					Tape->active->result->type=O_REAL;
				}

				else if(Tape->active->op1->type == O_REAL && Tape->active->op2->type == O_INT){
					Tape->active->result->type=O_REAL;
					Tape->active->result->value.rval=Tape->active->op1->value.rval-Tape->active->op2->value.ival;
				}

				else if(Tape->active->op1->type == O_REAL && Tape->active->op2->type == O_REAL){
					Tape->active->result->type=O_REAL;
					Tape->active->result->value.rval=Tape->active->op1->value.rval-Tape->active->op2->value.rval;
				}

				else return E_INTERN;
			}
		}

/*********************************************MUL*********************************************************/
		 else if(Tape->active->instruction==MUL)
	   {

	   		if(Tape->active->op1->type==NONTRM)																					//ZEPTAT SE JESTLY MUZE BYT 2OP noterm
	   		{
	   			Tape->active->op1->type=Tape->active->previous->result->type;
	   		}

	   		else if(hodnota!=NULL && phodnota!=NULL)
	   		{

				if(hodnota->type == O_INT && phodnota->type == O_INT)
					{
						Tape->active->result->type=O_INT;
						Tape->active->result->value.ival=hodnota->value.ival*phodnota->value.ival;
					}
				else if(hodnota->type == O_INT && phodnota->type == O_REAL){
					Tape->active->result->value.rval=hodnota->value.ival*phodnota->value.rval;
					Tape->active->result->type=O_REAL;
				}

				else if(hodnota->type == O_REAL && phodnota->type == O_INT){
					Tape->active->result->type=O_REAL;
					Tape->active->result->value.rval=hodnota->value.rval*phodnota->value.ival;
				}

				else if(hodnota->type == O_REAL && phodnota->type == O_REAL){
					Tape->active->result->type=O_REAL;
					Tape->active->result->value.rval=hodnota->value.rval*phodnota->value.rval;
				}

				else return E_INTERN;


	   		}
	   		else if(hodnota!=NULL && phodnota==NULL)
	   		{

				if(hodnota->type == O_INT && Tape->active->op2->type == O_INT)
					{
						Tape->active->result->type=O_INT;
						Tape->active->result->value.ival=hodnota->value.ival*Tape->active->op2->value.ival;
					}
				else if(hodnota->type == O_INT && Tape->active->op2->type == O_REAL){
					Tape->active->result->value.rval=hodnota->value.ival*Tape->active->op2->value.rval;
					Tape->active->result->type=O_REAL;
				}

				else if(hodnota->type == O_REAL && Tape->active->op2->type == O_INT){
					Tape->active->result->type=O_REAL;
					Tape->active->result->value.rval=hodnota->value.rval*Tape->active->op2->value.ival;
				}

				else if(hodnota->type == O_REAL && Tape->active->op2->type == O_REAL){
					Tape->active->result->type=O_REAL;
					Tape->active->result->value.rval=hodnota->value.rval*Tape->active->op2->value.rval;
				}

				else return E_INTERN;


	   		}

	   		else if(hodnota==NULL && phodnota!=NULL)
	   		{

				if(Tape->active->op1->type == O_INT && phodnota->type == O_INT)
					{
						Tape->active->result->type=O_INT;
						Tape->active->result->value.ival=Tape->active->op1->value.ival*phodnota->value.ival;
					}

				else if(Tape->active->op1->type == O_INT && phodnota->type == O_REAL){
					Tape->active->result->value.rval=Tape->active->op1->value.ival*phodnota->value.rval;
					Tape->active->result->type=O_REAL;
				}

				else if(Tape->active->op1->type == O_REAL && phodnota->type == O_INT){
					Tape->active->result->type=O_REAL;
					Tape->active->result->value.rval=Tape->active->op1->value.rval*phodnota->value.ival;
				}

				else if(Tape->active->op1->type == O_REAL && phodnota->type == O_REAL){
					Tape->active->result->type=O_REAL;
					Tape->active->result->value.rval=Tape->active->op1->value.rval*phodnota->value.rval;
				}

				else return E_INTERN;
			}

	   		else
	   		{

				if(Tape->active->op1->type == O_INT && Tape->active->op2->type == O_INT)
					{
						Tape->active->result->type=O_INT;
						Tape->active->result->value.ival=Tape->active->op1->value.ival*Tape->active->op2->value.ival;
					}
				else if(Tape->active->op1->type == O_INT && phodnota->type == O_REAL){
					Tape->active->result->value.rval=Tape->active->op1->value.ival*Tape->active->op2->value.rval;
					Tape->active->result->type=O_REAL;
				}

				else if(Tape->active->op1->type == O_REAL && Tape->active->op2->type == O_INT){
					Tape->active->result->type=O_REAL;
					Tape->active->result->value.rval=Tape->active->op1->value.rval*Tape->active->op2->value.ival;
				}

				else if(Tape->active->op1->type == O_REAL && Tape->active->op2->type == O_REAL){
					Tape->active->result->type=O_REAL;
					Tape->active->result->value.rval=Tape->active->op1->value.rval*Tape->active->op2->value.rval;
				}

				else return E_INTERN;
			}
		}

/*********************************************DIV*********************************************************/
		else if(Tape->active->instruction==DIV)
	   {

	   		if(Tape->active->op1->type==NONTRM)																					//ZEPTAT SE JESTLY MUZE BYT 2OP noterm
	   		{
	   			Tape->active->op1->type=Tape->active->previous->result->type;
	   		}

	   		else if(hodnota!=NULL && phodnota!=NULL)
	   		{

				if(hodnota->type == O_INT && phodnota->type == O_INT)
					{
						Tape->active->result->type=O_INT;
						Tape->active->result->value.ival=hodnota->value.ival/phodnota->value.ival;
					}
				else if(hodnota->type == O_INT && phodnota->type == O_REAL){
					Tape->active->result->value.rval=hodnota->value.ival/phodnota->value.rval;
					Tape->active->result->type=O_REAL;
				}

				else if(hodnota->type == O_REAL && phodnota->type == O_INT){
					Tape->active->result->type=O_REAL;
					Tape->active->result->value.rval=hodnota->value.rval/phodnota->value.ival;
				}

				else if(hodnota->type == O_REAL && phodnota->type == O_REAL){
					Tape->active->result->type=O_REAL;
					Tape->active->result->value.rval=hodnota->value.rval/phodnota->value.rval;
				}

				else return E_INTERN;


	   		}
	   		else if(hodnota!=NULL && phodnota==NULL)
	   		{

				if(hodnota->type == O_INT && Tape->active->op2->type == O_INT)
					{
						Tape->active->result->type=O_INT;
						Tape->active->result->value.ival=hodnota->value.ival/Tape->active->op2->value.ival;
					}
				else if(hodnota->type == O_INT && Tape->active->op2->type == O_REAL){
					Tape->active->result->value.rval=hodnota->value.ival/Tape->active->op2->value.rval;
					Tape->active->result->type=O_REAL;
				}

				else if(hodnota->type == O_REAL && Tape->active->op2->type == O_INT){
					Tape->active->result->type=O_REAL;
					Tape->active->result->value.rval=hodnota->value.rval/Tape->active->op2->value.ival;
				}

				else if(hodnota->type == O_REAL && Tape->active->op2->type == O_REAL){
					Tape->active->result->type=O_REAL;
					Tape->active->result->value.rval=hodnota->value.rval/Tape->active->op2->value.rval;
				}

				else return E_INTERN;


	   		}

	   		else if(hodnota==NULL && phodnota!=NULL)
	   		{

				if(Tape->active->op1->type == O_INT && phodnota->type == O_INT)
					{
						Tape->active->result->type=O_INT;
						Tape->active->result->value.ival=Tape->active->op1->value.ival/phodnota->value.ival;
					}

				else if(Tape->active->op1->type == O_INT && phodnota->type == O_REAL){
					Tape->active->result->value.rval=Tape->active->op1->value.ival/phodnota->value.rval;
					Tape->active->result->type=O_REAL;
				}

				else if(Tape->active->op1->type == O_REAL && phodnota->type == O_INT){
					Tape->active->result->type=O_REAL;
					Tape->active->result->value.rval=Tape->active->op1->value.rval/phodnota->value.ival;
				}

				else if(Tape->active->op1->type == O_REAL && phodnota->type == O_REAL){
					Tape->active->result->type=O_REAL;
					Tape->active->result->value.rval=Tape->active->op1->value.rval/phodnota->value.rval;
				}

				else return E_INTERN;
			}

	   		else
	   		{

				if(Tape->active->op1->type == O_INT && Tape->active->op2->type == O_INT)
					{
						Tape->active->result->type=O_INT;
						Tape->active->result->value.ival=Tape->active->op1->value.ival/Tape->active->op2->value.ival;
					}
				else if(Tape->active->op1->type == O_INT && phodnota->type == O_REAL){
					Tape->active->result->value.rval=Tape->active->op1->value.ival/Tape->active->op2->value.rval;
					Tape->active->result->type=O_REAL;
				}

				else if(Tape->active->op1->type == O_REAL && Tape->active->op2->type == O_INT){
					Tape->active->result->type=O_REAL;
					Tape->active->result->value.rval=Tape->active->op1->value.rval/Tape->active->op2->value.ival;
				}

				else if(Tape->active->op1->type == O_REAL && Tape->active->op2->type == O_REAL){
					Tape->active->result->type=O_REAL;
					Tape->active->result->value.rval=Tape->active->op1->value.rval/Tape->active->op2->value.rval;
				}

				else return E_INTERN;
			}
		}

/*********************************************BIGGER******************************************************/
		else if(Tape->active->instruction==MORE)		//op1>op2
		{

	   		if(Tape->active->op1->type==NONTRM)																					//ZEPTAT SE JESTLY MUZE BYT 2OP noterm
	   		{
	   			Tape->active->op1->type=Tape->active->previous->result->type;
	   		}

	   		else if(hodnota!=NULL && phodnota!=NULL)
	   		{

				if(hodnota->type == O_INT && phodnota->type == O_INT)
					{
						Tape->active->result->type=O_BOOL;
						Tape->active->result->value.bval=hodnota->value.ival>phodnota->value.ival;
					}
				else if(hodnota->type == O_INT && phodnota->type == O_REAL){
					Tape->active->result->value.bval=hodnota->value.ival>phodnota->value.rval;
					Tape->active->result->type=O_BOOL;
				}

				else if(hodnota->type == O_REAL && phodnota->type == O_INT){
					Tape->active->result->type=O_BOOL;
					Tape->active->result->value.rval=hodnota->value.rval>phodnota->value.ival;
				}

				else if(hodnota->type == O_REAL && phodnota->type == O_REAL){
					Tape->active->result->type=O_BOOL;
					Tape->active->result->value.rval=hodnota->value.rval>phodnota->value.rval;
				}
				else if(hodnota->type == O_BOOL && phodnota->type == O_BOOL){
					Tape->active->result->type=O_BOOL;
					Tape->active->result->value.rval=hodnota->value.rval>phodnota->value.rval;
				}
				else if(hodnota->type == O_STRING && phodnota->type == O_STRING)
					{
					Tape->active->result->type=O_BOOL;
					if(strCmpstring(&hodnota->value.sval,&phodnota->value.sval) > 0)
						Tape->active->result->value.bval=1;
					else Tape->active->result->value.bval=0;
					}
				else return E_INTERN;


	   		}
	   		else if(hodnota!=NULL && phodnota==NULL)
	   		{

				if(hodnota->type == O_INT && Tape->active->op2->type == O_INT)
					{
						Tape->active->result->type=O_BOOL;
						Tape->active->result->value.ival=hodnota->value.ival>Tape->active->op2->value.ival;
					}
				else if(hodnota->type == O_INT && Tape->active->op2->type == O_REAL){
					Tape->active->result->value.rval=hodnota->value.ival>Tape->active->op2->value.rval;
					Tape->active->result->type=O_BOOL;
				}

				else if(hodnota->type == O_REAL && Tape->active->op2->type == O_INT){
					Tape->active->result->type=O_BOOL;
					Tape->active->result->value.rval=hodnota->value.rval>Tape->active->op2->value.ival;
				}

				else if(hodnota->type == O_REAL && Tape->active->op2->type == O_REAL){
					Tape->active->result->type=O_BOOL;
					Tape->active->result->value.rval=hodnota->value.rval>Tape->active->op2->value.rval;
				}
				else if(hodnota->type == O_BOOL && Tape->active->op2->type == O_BOOL){
					Tape->active->result->type=O_BOOL;
					Tape->active->result->value.rval=hodnota->value.rval>Tape->active->op2->value.rval;
				}
				else if(hodnota->type == O_STRING && Tape->active->op2->type == O_STRING)
					{
					Tape->active->result->type=O_BOOL;
					if(strCmpstring(&(hodnota->value.sval),&(Tape->active->op2->value.sval)) > 0)
						Tape->active->result->value.bval=1;
					else Tape->active->result->value.bval=0;
					}

				else return E_INTERN;


	   		}

	   		else if(hodnota==NULL && phodnota!=NULL)
	   		{

				if(Tape->active->op1->type == O_INT && phodnota->type == O_INT)
					{
						Tape->active->result->type=O_BOOL;
						Tape->active->result->value.ival=Tape->active->op1->value.ival>phodnota->value.ival;
					}

				else if(Tape->active->op1->type == O_INT && phodnota->type == O_REAL){
					Tape->active->result->value.rval=Tape->active->op1->value.ival>phodnota->value.rval;
					Tape->active->result->type=O_BOOL;
				}

				else if(Tape->active->op1->type == O_REAL && phodnota->type == O_INT){
					Tape->active->result->type=O_BOOL;
					Tape->active->result->value.rval=Tape->active->op1->value.rval>phodnota->value.ival;
				}

				else if(Tape->active->op1->type == O_REAL && phodnota->type == O_REAL){
					Tape->active->result->type=O_BOOL;
					Tape->active->result->value.rval=Tape->active->op1->value.rval>phodnota->value.rval;
				}
				else if(Tape->active->op1->type == O_BOOL && phodnota->type == O_BOOL){
					Tape->active->result->type=O_BOOL;
					Tape->active->result->value.rval=Tape->active->op1->value.rval>phodnota->value.rval;
				}
				else if(Tape->active->op1->type == O_STRING && phodnota->type == O_STRING)
					{
					Tape->active->result->type=O_BOOL;
					if(strCmpstring(&(Tape->active->op1->value.sval),&(phodnota->value.sval)) > 0)
						Tape->active->result->value.bval=1;
					else Tape->active->result->value.bval=0;
					}

				else return E_INTERN;
			}

	   		else
	   		{

				if(Tape->active->op1->type == O_INT && Tape->active->op2->type == O_INT)
					{
						Tape->active->result->type=O_BOOL;
						Tape->active->result->value.ival=Tape->active->op1->value.ival>Tape->active->op2->value.ival;
					}
				else if(Tape->active->op1->type == O_INT && phodnota->type == O_REAL){
					Tape->active->result->value.rval=Tape->active->op1->value.ival>Tape->active->op2->value.rval;
					Tape->active->result->type=O_BOOL;
				}

				else if(Tape->active->op1->type == O_REAL && Tape->active->op2->type == O_INT){
					Tape->active->result->type=O_BOOL;
					Tape->active->result->value.rval=Tape->active->op1->value.rval>Tape->active->op2->value.ival;
				}

				else if(Tape->active->op1->type == O_REAL && Tape->active->op2->type == O_REAL){
					Tape->active->result->type=O_BOOL;
					Tape->active->result->value.rval=Tape->active->op1->value.rval>Tape->active->op2->value.rval;
				}
				else if(Tape->active->op1->type == O_BOOL && Tape->active->op2->type == O_BOOL){
					Tape->active->result->type=O_BOOL;
					Tape->active->result->value.rval=Tape->active->op1->value.rval>Tape->active->op2->value.rval;
				}
				else if(Tape->active->op1->type == O_STRING && Tape->active->op2->type == O_STRING)
					{
					Tape->active->result->type=O_BOOL;
					if(strCmpstring(&(Tape->active->op1->value.sval),&(Tape->active->op2->value.sval)) > 0)
						Tape->active->result->value.bval=1;
					else Tape->active->result->value.bval=0;
					}

				else return E_INTERN;
			}
		}


/*********************************************SMALLER*****************************************************/
		else if(Tape->active->instruction==LESS)													/// op1<op2
		{
				tValue*prohozeni=Tape->active->op1;
				Tape->active->op1=Tape->active->op2;
				Tape->active->op2=prohozeni;
				Tape->active->instruction=MORE;
				continue;
		}

/*********************************************EBIGGER*****************************************************/
		else if(Tape->active->instruction==EQM)						// op1>=op2
		{

	   		if(Tape->active->op1->type==NONTRM)																					//ZEPTAT SE JESTLY MUZE BYT 2OP noterm
	   		{
	   			Tape->active->op1->type=Tape->active->previous->result->type;
	   		}

	   		else if(hodnota!=NULL && phodnota!=NULL)
	   		{

				if(hodnota->type == O_INT && phodnota->type == O_INT)
					{
						Tape->active->result->type=O_BOOL;
						Tape->active->result->value.bval=hodnota->value.ival>=phodnota->value.ival;
					}
				else if(hodnota->type == O_INT && phodnota->type == O_REAL){
					Tape->active->result->value.bval=hodnota->value.ival>=phodnota->value.rval;
					Tape->active->result->type=O_BOOL;
				}

				else if(hodnota->type == O_REAL && phodnota->type == O_INT){
					Tape->active->result->type=O_BOOL;
					Tape->active->result->value.rval=hodnota->value.rval>=phodnota->value.ival;
				}

				else if(hodnota->type == O_REAL && phodnota->type == O_REAL){
					Tape->active->result->type=O_BOOL;
					Tape->active->result->value.rval=hodnota->value.rval>=phodnota->value.rval;
				}
				else if(hodnota->type == O_BOOL && phodnota->type == O_BOOL){
					Tape->active->result->type=O_BOOL;
					Tape->active->result->value.rval=hodnota->value.rval>=phodnota->value.rval;
				}
				else if(hodnota->type == O_STRING && phodnota->type == O_STRING)
					{
					Tape->active->result->type=O_BOOL;
					if(strCmpstring(&(hodnota->value.sval),&(phodnota->value.sval)) >= 0)
						Tape->active->result->value.bval=1;
					else Tape->active->result->value.bval=0;
					}
				else return E_INTERN;


	   		}
	   		else if(hodnota!=NULL && phodnota==NULL)
	   		{

				if(hodnota->type == O_INT && Tape->active->op2->type == O_INT)
					{
						Tape->active->result->type=O_BOOL;
						Tape->active->result->value.ival=hodnota->value.ival>=Tape->active->op2->value.ival;
					}
				else if(hodnota->type == O_INT && Tape->active->op2->type == O_REAL){
					Tape->active->result->value.rval=hodnota->value.ival>=Tape->active->op2->value.rval;
					Tape->active->result->type=O_BOOL;
				}

				else if(hodnota->type == O_REAL && Tape->active->op2->type == O_INT){
					Tape->active->result->type=O_BOOL;
					Tape->active->result->value.rval=hodnota->value.rval>=Tape->active->op2->value.ival;
				}

				else if(hodnota->type == O_REAL && Tape->active->op2->type == O_REAL){
					Tape->active->result->type=O_BOOL;
					Tape->active->result->value.rval=hodnota->value.rval>=Tape->active->op2->value.rval;
				}
				else if(hodnota->type == O_BOOL && Tape->active->op2->type == O_BOOL){
					Tape->active->result->type=O_BOOL;
					Tape->active->result->value.rval=hodnota->value.rval>=Tape->active->op2->value.rval;
				}
				else if(hodnota->type == O_STRING && Tape->active->op2->type == O_STRING)
					{
					Tape->active->result->type=O_BOOL;
					if(strCmpstring(&(hodnota->value.sval),&(Tape->active->op2->value.sval)) >= 0)
						Tape->active->result->value.bval=1;
					else Tape->active->result->value.bval=0;
					}

				else return E_INTERN;


	   		}

	   		else if(hodnota==NULL && phodnota!=NULL)
	   		{

				if(Tape->active->op1->type == O_INT && phodnota->type == O_INT)
					{
						Tape->active->result->type=O_BOOL;
						Tape->active->result->value.ival=Tape->active->op1->value.ival>=phodnota->value.ival;
					}

				else if(Tape->active->op1->type == O_INT && phodnota->type == O_REAL){
					Tape->active->result->value.rval=Tape->active->op1->value.ival>=phodnota->value.rval;
					Tape->active->result->type=O_BOOL;
				}

				else if(Tape->active->op1->type == O_REAL && phodnota->type == O_INT){
					Tape->active->result->type=O_BOOL;
					Tape->active->result->value.rval=Tape->active->op1->value.rval>=phodnota->value.ival;
				}

				else if(Tape->active->op1->type == O_REAL && phodnota->type == O_REAL){
					Tape->active->result->type=O_BOOL;
					Tape->active->result->value.rval=Tape->active->op1->value.rval>=phodnota->value.rval;
				}
				else if(Tape->active->op1->type == O_BOOL && phodnota->type == O_BOOL){
					Tape->active->result->type=O_BOOL;
					Tape->active->result->value.rval=Tape->active->op1->value.rval>=phodnota->value.rval;
				}
				else if(Tape->active->op1->type == O_STRING && phodnota->type == O_STRING)
					{
					Tape->active->result->type=O_BOOL;
					if(strCmpstring(&(Tape->active->op1->value.sval),&(phodnota->value.sval)) >= 0)
						Tape->active->result->value.bval=1;
					else Tape->active->result->value.bval=0;
					}

				else return E_INTERN;
			}

	   		else
	   		{

				if(Tape->active->op1->type == O_INT && Tape->active->op2->type == O_INT)
					{
						Tape->active->result->type=O_BOOL;
						Tape->active->result->value.ival=Tape->active->op1->value.ival>=Tape->active->op2->value.ival;
					}
				else if(Tape->active->op1->type == O_INT && phodnota->type == O_REAL){
					Tape->active->result->value.rval=Tape->active->op1->value.ival>=Tape->active->op2->value.rval;
					Tape->active->result->type=O_BOOL;
				}

				else if(Tape->active->op1->type == O_REAL && Tape->active->op2->type == O_INT){
					Tape->active->result->type=O_BOOL;
					Tape->active->result->value.rval=Tape->active->op1->value.rval>=Tape->active->op2->value.ival;
				}

				else if(Tape->active->op1->type == O_REAL && Tape->active->op2->type == O_REAL){
					Tape->active->result->type=O_BOOL;
					Tape->active->result->value.rval=Tape->active->op1->value.rval>=Tape->active->op2->value.rval;
				}
				else if(Tape->active->op1->type == O_BOOL && Tape->active->op2->type == O_BOOL){
					Tape->active->result->type=O_BOOL;
					Tape->active->result->value.rval=Tape->active->op1->value.rval>=Tape->active->op2->value.rval;
				}
				else if(Tape->active->op1->type == O_STRING && Tape->active->op2->type == O_STRING)
					{
					Tape->active->result->type=O_BOOL;
					if(strCmpstring(&(Tape->active->op1->value.sval),&(Tape->active->op2->value.sval)) >= 0)
						Tape->active->result->value.bval=1;
					else Tape->active->result->value.bval=0;
					}

				else return E_INTERN;
			}
		}
/*********************************************ESMALLER****************************************************/
		else if(Tape->active->instruction==EQL)
		{
				tValue*prohozeni=Tape->active->op1;
				Tape->active->op1=Tape->active->op2;
				Tape->active->op2=prohozeni;
				Tape->active->instruction=EQM;
				continue;
		}
/*********************************************EQUAL*******************************************************/
		else if(Tape->active->instruction==EQL)
		{

	   		if(Tape->active->op1->type==NONTRM)																					//ZEPTAT SE JESTLY MUZE BYT 2OP noterm
	   		{
	   			Tape->active->op1->type=Tape->active->previous->result->type;
	   		}

	   		else if(hodnota!=NULL && phodnota!=NULL)
	   		{

				if(hodnota->type == O_INT && phodnota->type == O_INT)
					{
						Tape->active->result->type=O_BOOL;
						Tape->active->result->value.bval=hodnota->value.ival==phodnota->value.ival;
					}
				else if(hodnota->type == O_INT && phodnota->type == O_REAL){
					Tape->active->result->value.bval=hodnota->value.ival==phodnota->value.rval;
					Tape->active->result->type=O_BOOL;
				}

				else if(hodnota->type == O_REAL && phodnota->type == O_INT){
					Tape->active->result->type=O_BOOL;
					Tape->active->result->value.rval=hodnota->value.rval==phodnota->value.ival;
				}

				else if(hodnota->type == O_REAL && phodnota->type == O_REAL){
					Tape->active->result->type=O_BOOL;
					Tape->active->result->value.rval=hodnota->value.rval==phodnota->value.rval;
				}
				else if(hodnota->type == O_BOOL && phodnota->type == O_BOOL){
					Tape->active->result->type=O_BOOL;
					Tape->active->result->value.rval=hodnota->value.rval==phodnota->value.rval;
				}
				else if(hodnota->type == O_STRING && phodnota->type == O_STRING)
					{
					Tape->active->result->type=O_BOOL;
					if(strCmpstring(&(hodnota->value.sval),&(phodnota->value.sval)) == 0)
						Tape->active->result->value.bval=1;
					else Tape->active->result->value.bval=0;
					}
				else return E_INTERN;


	   		}
	   		else if(hodnota!=NULL && phodnota==NULL)
	   		{

				if(hodnota->type == O_INT && Tape->active->op2->type == O_INT)
					{
						Tape->active->result->type=O_BOOL;
						Tape->active->result->value.ival=hodnota->value.ival==Tape->active->op2->value.ival;
					}
				else if(hodnota->type == O_INT && Tape->active->op2->type == O_REAL){
					Tape->active->result->value.rval=hodnota->value.ival==Tape->active->op2->value.rval;
					Tape->active->result->type=O_BOOL;
				}

				else if(hodnota->type == O_REAL && Tape->active->op2->type == O_INT){
					Tape->active->result->type=O_BOOL;
					Tape->active->result->value.rval=hodnota->value.rval==Tape->active->op2->value.ival;
				}

				else if(hodnota->type == O_REAL && Tape->active->op2->type == O_REAL){
					Tape->active->result->type=O_BOOL;
					Tape->active->result->value.rval=hodnota->value.rval==Tape->active->op2->value.rval;
				}
				else if(hodnota->type == O_BOOL && Tape->active->op2->type == O_BOOL){
					Tape->active->result->type=O_BOOL;
					Tape->active->result->value.rval=hodnota->value.rval==Tape->active->op2->value.rval;
				}
				else if(hodnota->type == O_STRING && Tape->active->op2->type == O_STRING)
					{
					Tape->active->result->type=O_BOOL;
					if(strCmpstring(&(hodnota->value.sval),&(Tape->active->op2->value.sval)) == 0)
						Tape->active->result->value.bval=1;
					else Tape->active->result->value.bval=0;
					}

				else return E_INTERN;


	   		}

	   		else if(hodnota==NULL && phodnota!=NULL)
	   		{

				if(Tape->active->op1->type == O_INT && phodnota->type == O_INT)
					{
						Tape->active->result->type=O_BOOL;
						Tape->active->result->value.ival=Tape->active->op1->value.ival==phodnota->value.ival;
					}

				else if(Tape->active->op1->type == O_INT && phodnota->type == O_REAL){
					Tape->active->result->value.rval=Tape->active->op1->value.ival==phodnota->value.rval;
					Tape->active->result->type=O_BOOL;
				}

				else if(Tape->active->op1->type == O_REAL && phodnota->type == O_INT){
					Tape->active->result->type=O_BOOL;
					Tape->active->result->value.rval=Tape->active->op1->value.rval==phodnota->value.ival;
				}

				else if(Tape->active->op1->type == O_REAL && phodnota->type == O_REAL){
					Tape->active->result->type=O_BOOL;
					Tape->active->result->value.rval=Tape->active->op1->value.rval==phodnota->value.rval;
				}
				else if(Tape->active->op1->type == O_BOOL && phodnota->type == O_BOOL){
					Tape->active->result->type=O_BOOL;
					Tape->active->result->value.rval=Tape->active->op1->value.rval==phodnota->value.rval;
				}
				else if(Tape->active->op1->type == O_STRING && phodnota->type == O_STRING)
					{
					Tape->active->result->type=O_BOOL;
					if(strCmpstring(&(Tape->active->op1->value.sval),&(phodnota->value.sval)) == 0)
						Tape->active->result->value.bval=1;
					else Tape->active->result->value.bval=0;
					}

				else return E_INTERN;
			}

	   		else
	   		{

				if(Tape->active->op1->type == O_INT && Tape->active->op2->type == O_INT)
					{
						Tape->active->result->type=O_BOOL;
						Tape->active->result->value.ival=Tape->active->op1->value.ival==Tape->active->op2->value.ival;
					}
				else if(Tape->active->op1->type == O_INT && phodnota->type == O_REAL){
					Tape->active->result->value.rval=Tape->active->op1->value.ival==Tape->active->op2->value.rval;
					Tape->active->result->type=O_BOOL;
				}

				else if(Tape->active->op1->type == O_REAL && Tape->active->op2->type == O_INT){
					Tape->active->result->type=O_BOOL;
					Tape->active->result->value.rval=Tape->active->op1->value.rval==Tape->active->op2->value.ival;
				}

				else if(Tape->active->op1->type == O_REAL && Tape->active->op2->type == O_REAL){
					Tape->active->result->type=O_BOOL;
					Tape->active->result->value.rval=Tape->active->op1->value.rval==Tape->active->op2->value.rval;
				}
				else if(Tape->active->op1->type == O_BOOL && Tape->active->op2->type == O_BOOL){
					Tape->active->result->type=O_BOOL;
					Tape->active->result->value.rval=Tape->active->op1->value.rval==Tape->active->op2->value.rval;
				}
				else if(Tape->active->op1->type == O_STRING && Tape->active->op2->type == O_STRING)
					{
					Tape->active->result->type=O_BOOL;
					if(strCmpstring(&(Tape->active->op1->value.sval),&(Tape->active->op2->value.sval)) == 0)
						Tape->active->result->value.bval=1;
					else Tape->active->result->value.bval=0;
					}

				else return E_INTERN;
			}
		}
/*********************************************NEQUAL******************************************************/
			else if(Tape->active->instruction==NEQ)
		{

	   		if(Tape->active->op1->type==NONTRM)																					//ZEPTAT SE JESTLY MUZE BYT 2OP noterm
	   		{
	   			Tape->active->op1->type=Tape->active->previous->result->type;
	   		}

	   		else if(hodnota!=NULL && phodnota!=NULL)
	   		{

				if(hodnota->type == O_INT && phodnota->type == O_INT)
					{
						Tape->active->result->type=O_BOOL;
						Tape->active->result->value.bval=hodnota->value.ival!=phodnota->value.ival;
					}
				else if(hodnota->type == O_INT && phodnota->type == O_REAL){
					Tape->active->result->value.bval=hodnota->value.ival!=phodnota->value.rval;
					Tape->active->result->type=O_BOOL;
				}

				else if(hodnota->type == O_REAL && phodnota->type == O_INT){
					Tape->active->result->type=O_BOOL;
					Tape->active->result->value.rval=hodnota->value.rval!=phodnota->value.ival;
				}

				else if(hodnota->type == O_REAL && phodnota->type == O_REAL){
					Tape->active->result->type=O_BOOL;
					Tape->active->result->value.rval=hodnota->value.rval!=phodnota->value.rval;
				}
				else if(hodnota->type == O_BOOL && phodnota->type == O_BOOL){
					Tape->active->result->type=O_BOOL;
					Tape->active->result->value.rval=hodnota->value.rval!=phodnota->value.rval;
				}
				else if(hodnota->type == O_STRING && phodnota->type == O_STRING)
					{
					Tape->active->result->type=O_BOOL;
					if(strCmpstring(&(hodnota->value.sval),&(phodnota->value.sval)) != 0)
						Tape->active->result->value.bval=1;
					else Tape->active->result->value.bval=0;
					}
				else return E_INTERN;


	   		}
	   		else if(hodnota!=NULL && phodnota==NULL)
	   		{

				if(hodnota->type == O_INT && Tape->active->op2->type == O_INT)
					{
						Tape->active->result->type=O_BOOL;
						Tape->active->result->value.ival=hodnota->value.ival!=Tape->active->op2->value.ival;
					}
				else if(hodnota->type == O_INT && Tape->active->op2->type == O_REAL){
					Tape->active->result->value.rval=hodnota->value.ival!=Tape->active->op2->value.rval;
					Tape->active->result->type=O_BOOL;
				}

				else if(hodnota->type == O_REAL && Tape->active->op2->type == O_INT){
					Tape->active->result->type=O_BOOL;
					Tape->active->result->value.rval=hodnota->value.rval!=Tape->active->op2->value.ival;
				}

				else if(hodnota->type == O_REAL && Tape->active->op2->type == O_REAL){
					Tape->active->result->type=O_BOOL;
					Tape->active->result->value.rval=hodnota->value.rval!=Tape->active->op2->value.rval;
				}
				else if(hodnota->type == O_BOOL && Tape->active->op2->type == O_BOOL){
					Tape->active->result->type=O_BOOL;
					Tape->active->result->value.rval=hodnota->value.rval!=Tape->active->op2->value.rval;
				}
				else if(hodnota->type == O_STRING && Tape->active->op2->type == O_STRING)
					{
					Tape->active->result->type=O_BOOL;
					if(strCmpstring(&(hodnota->value.sval),&(Tape->active->op2->value.sval)) != 0)
						Tape->active->result->value.bval=1;
					else Tape->active->result->value.bval=0;
					}

				else return E_INTERN;


	   		}

	   		else if(hodnota==NULL && phodnota!=NULL)
	   		{

				if(Tape->active->op1->type == O_INT && phodnota->type == O_INT)
					{
						Tape->active->result->type=O_BOOL;
						Tape->active->result->value.ival=Tape->active->op1->value.ival!=phodnota->value.ival;
					}

				else if(Tape->active->op1->type == O_INT && phodnota->type == O_REAL){
					Tape->active->result->value.rval=Tape->active->op1->value.ival!=phodnota->value.rval;
					Tape->active->result->type=O_BOOL;
				}

				else if(Tape->active->op1->type == O_REAL && phodnota->type == O_INT){
					Tape->active->result->type=O_BOOL;
					Tape->active->result->value.rval=Tape->active->op1->value.rval!=phodnota->value.ival;
				}

				else if(Tape->active->op1->type == O_REAL && phodnota->type == O_REAL){
					Tape->active->result->type=O_BOOL;
					Tape->active->result->value.rval=Tape->active->op1->value.rval!=phodnota->value.rval;
				}
				else if(Tape->active->op1->type == O_BOOL && phodnota->type == O_BOOL){
					Tape->active->result->type=O_BOOL;
					Tape->active->result->value.rval=Tape->active->op1->value.rval!=phodnota->value.rval;
				}
				else if(Tape->active->op1->type == O_STRING && phodnota->type == O_STRING)
					{
					Tape->active->result->type=O_BOOL;
					if(strCmpstring(&(Tape->active->op1->value.sval),&(phodnota->value.sval)) != 0)
						Tape->active->result->value.bval=1;
					else Tape->active->result->value.bval=0;
					}

				else return E_INTERN;
			}

	   		else
	   		{

				if(Tape->active->op1->type == O_INT && Tape->active->op2->type == O_INT)
					{
						Tape->active->result->type=O_BOOL;
						Tape->active->result->value.ival=Tape->active->op1->value.ival!=Tape->active->op2->value.ival;
					}
				else if(Tape->active->op1->type == O_INT && phodnota->type == O_REAL){
					Tape->active->result->value.rval=Tape->active->op1->value.ival!=Tape->active->op2->value.rval;
					Tape->active->result->type=O_BOOL;
				}

				else if(Tape->active->op1->type == O_REAL && Tape->active->op2->type == O_INT){
					Tape->active->result->type=O_BOOL;
					Tape->active->result->value.rval=Tape->active->op1->value.rval!=Tape->active->op2->value.ival;
				}

				else if(Tape->active->op1->type == O_REAL && Tape->active->op2->type == O_REAL){
					Tape->active->result->type=O_BOOL;
					Tape->active->result->value.rval=Tape->active->op1->value.rval!=Tape->active->op2->value.rval;
				}
				else if(Tape->active->op1->type == O_BOOL && Tape->active->op2->type == O_BOOL){
					Tape->active->result->type=O_BOOL;
					Tape->active->result->value.rval=Tape->active->op1->value.rval!=Tape->active->op2->value.rval;
				}
				else if(Tape->active->op1->type == O_STRING && Tape->active->op2->type == O_STRING)
					{
					Tape->active->result->type=O_BOOL;
					if(strCmpstring(&(Tape->active->op1->value.sval),&(Tape->active->op2->value.sval)) != 0)
						Tape->active->result->value.bval=1;
					else Tape->active->result->value.bval=0;
					}

				else return E_INTERN;
			}
		}
/*********************************************ASSIG*******************************************************/
		else if(Tape->active->instruction==ASSIGN)
		{
			if(hodnota!=NULL && phodnota!=NULL)
			{
				if(hodnota->type==O_INT && phodnota->type==O_INT)
				{
					hodnota->value.ival=phodnota->value.ival;
					Tape->active->result->value.ival=hodnota->value.ival;
					Tape->active->result->type=O_INT;
				}
				else if(hodnota->type==O_REAL && phodnota->type==O_INT)
				{
					hodnota->value.rval=phodnota->value.ival;
					Tape->active->result->value.rval=hodnota->value.rval;
					Tape->active->result->type=O_REAL;
				}
				else if(hodnota->type==O_INT && phodnota->type==O_REAL)
				{
					hodnota->value.rval=phodnota->value.rval;
					Tape->active->result->value.rval=hodnota->value.rval;
					Tape->active->result->type=O_REAL;
					hodnota->type=O_REAL;
				}
				else if(hodnota->type==O_REAL && phodnota->type==O_REAL)
				{
					hodnota->value.rval=phodnota->value.rval;
					Tape->active->result->value.rval=hodnota->value.rval;
					Tape->active->result->type=O_REAL;
				}
				else if(hodnota->type==O_STRING && phodnota->type==O_STRING)
				{
					hodnota->value.sval=phodnota->value.sval;
					Tape->active->result->value.sval=hodnota->value.sval;
					Tape->active->result->type=O_STRING;
				}
				else return E_INTERN;
			}
			else if(hodnota==NULL && phodnota!=NULL)
			{
				if(Tape->active->op1->type==O_INT && phodnota->type==O_INT)
				{
					Tape->active->op1->value.ival=phodnota->value.ival;
					Tape->active->result->value.ival=Tape->active->op1->value.ival;
					Tape->active->result->type=O_INT;
				}
				else if(Tape->active->op1->type==O_REAL && phodnota->type==O_INT)
				{
					Tape->active->op1->value.rval=phodnota->value.ival;
					Tape->active->result->value.rval=Tape->active->op1->value.rval;
					Tape->active->result->type=O_REAL;
				}
				else if(Tape->active->op1->type==O_INT && phodnota->type==O_REAL)
				{
					Tape->active->op1->value.rval=phodnota->value.rval;
					Tape->active->result->value.rval=Tape->active->op1->value.rval;
					Tape->active->result->type=O_REAL;
					Tape->active->op1->type=O_REAL;
				}
				else if(Tape->active->op1->type==O_REAL && phodnota->type==O_REAL)
				{
					Tape->active->op1->value.rval=phodnota->value.rval;
					Tape->active->result->value.rval=Tape->active->op1->value.rval;
					Tape->active->result->type=O_REAL;
				}
				else if(Tape->active->op1->type==O_STRING && phodnota->type==O_STRING)
				{
					Tape->active->op1->value.sval=phodnota->value.sval;
					Tape->active->result->value.sval=Tape->active->op1->value.sval;
					Tape->active->result->type=O_STRING;
				}
				else return E_INTERN;

			}
			else if(hodnota!=NULL && phodnota==NULL)
			{
				if(hodnota->type==O_INT && Tape->active->op2->type==O_INT)
				{
					hodnota->value.ival=Tape->active->op2->value.ival;
					Tape->active->result->value.ival=hodnota->value.ival;
					Tape->active->result->type=O_INT;
				}
				else if(hodnota->type==O_REAL && Tape->active->op2->type==O_INT)
				{
					hodnota->value.rval=Tape->active->op2->value.ival;
					Tape->active->result->value.rval=hodnota->value.rval;
					Tape->active->result->type=O_REAL;
				}
				else if(hodnota->type==O_INT && Tape->active->op2->type==O_REAL)
				{
					hodnota->value.rval=Tape->active->op2->value.rval;
					Tape->active->result->value.rval=hodnota->value.rval;
					Tape->active->result->type=O_REAL;
					hodnota->type=O_REAL;
				}
				else if(hodnota->type==O_REAL && Tape->active->op2->type==O_REAL)
				{
					hodnota->value.rval=Tape->active->op2->value.rval;
					Tape->active->result->value.rval=hodnota->value.rval;
					Tape->active->result->type=O_REAL;
				}
				else if(hodnota->type==O_STRING && Tape->active->op2->type==O_STRING)
				{
					hodnota->value.sval=Tape->active->op2->value.sval;
					Tape->active->result->value.sval=hodnota->value.sval;
					Tape->active->result->type=O_STRING;
				}
				else return E_INTERN;

			}
			else if(hodnota==NULL && phodnota==NULL)
			{
				if(Tape->active->op1->type==O_INT && Tape->active->op2->type==O_INT)
				{
					Tape->active->op1->value.ival=Tape->active->op2->value.ival;
					Tape->active->result->value.ival=Tape->active->op1->value.ival;
					Tape->active->result->type=O_INT;
				}
				else if(Tape->active->op1->type==O_REAL && Tape->active->op2->type==O_INT)
				{
					Tape->active->op1->value.rval=Tape->active->op2->value.ival;
					Tape->active->result->value.rval=Tape->active->op1->value.rval;
					Tape->active->result->type=O_REAL;
				}
				else if(Tape->active->op1->type==O_INT && Tape->active->op2->type==O_REAL)
				{
					Tape->active->op1->value.rval=Tape->active->op2->value.rval;
					Tape->active->result->value.rval=Tape->active->op1->value.rval;
					Tape->active->result->type=O_REAL;
					Tape->active->op1->type=O_REAL;
				}
				else if(Tape->active->op1->type==O_REAL && Tape->active->op2->type==O_REAL)
				{
					Tape->active->op1->value.rval=Tape->active->op2->value.rval;
					Tape->active->result->value.rval=Tape->active->op1->value.rval;
					Tape->active->result->type=O_REAL;
				}
				else if(Tape->active->op1->type==O_STRING && Tape->active->op2->type==O_STRING)
				{
					Tape->active->op1->value.sval=Tape->active->op2->value.sval;
					Tape->active->result->value.sval=Tape->active->op1->value.sval;
					Tape->active->result->type=O_STRING;
				}
				else return E_INTERN;

			}

		}




		Tape->active=Tape->active->next;														// posunuti na pasce na dasi instrukci

	}


	StackDeleteDataDelete(&stack);															// uvolneni celeho zasobniku
	return E_OK;

}

void stackinit(TStack *stack)       // inicializace zaodbnika
{
    stack->top=NULL;                // top vyprazdnim

    return;
}

tErrors stackPush(TStack *stack,tParamItem *op)                                    // funkce pro nahrati na zasobnik
{
    TStackItem *phelp;                                                           // pomocna promena pro svazani na zasobniku se spodnim prvkem

    if(stack->top==NULL )                                                       // kdyz je zadobnik prazdny tak
    {
        if((stack->top=malloc(sizeof(TStackItem)))==NULL) return E_INTERN;      // alokuj misto ptvku a nahraj ho na vrchol zasobniku
        stack->top->op1=op;                                                    // a do tohoto prvku vloz operand
        stack->top->pdown=NULL;                                                      // pro jistotu vloz do ukazatele na spodni prvek null
        return E_OK;

    }
    else                                                                        // kdyz neni prazdny
    {
        if((phelp=malloc(sizeof(TStackItem)))==NULL) return E_INTERN;           // tak alokuj misto pro prvek a vloz ho do pomocneho ukazatele
        phelp->pdown=stack->top;                                                // nini v pomocnem prvku nahraj vazbu s top vrcholem
        stack->top=phelp;                                                       // a nahraj pomocny prvek jako top
        stack->top->op1=op;                                                    // nini na top vloz operand
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

void StackDeleteDataDelete(TStack *stack)                                       // fuknce pro uvolneni zasobnika
{
    if (stack->top==NULL) return;                                                       // pokud je prazdny nic nedelej
    while(TStackTopPop(&stack)!=NULL)                                                   // dokud zasobnik neni prazdny tak pop
        ;
    return;
}

int quickSort( int a[], int l, int r)
{
   int j;

   if( l < r )
   {
   	// divide and conquer
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

tParamItem *SearchStackName(string*Search)
{
	while(stack.top->op1!=NULL)
	{
		if (strCmpstring(Search,&stack.top->op1->name)==0)
		{
			return &stack.top->op1;
		}
		stack.top=stack.top->op1->next;
	}
	return NULL;
}

string *conc(string*s1,string*s2)
{
    if (s1==NULL || s2==NULL) return NULL;
	int i=s1->length;
	s1->str=(char*) realloc(s1->str,s2->allocSize);
	while(i<=(i+s2->length))
    {
        s1->str[i]=s2->str[i-s1->length];
        i++;
    }
    return s1;
}
