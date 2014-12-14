/* 
* Názov projektu: Implementace interpretu imperativního jazyka IFJ14 
* Dátum: 12/2014
* Autori: Matúš Cimerman, xcimer00
*         Tomáš Hynek ,   xhynek09
*         David Novák ,   xnovak0r
*         Michal Sedlák , xsedla0m
*         Jan Jorenek  ,  xjoren01
*/
#include <stdlib.h>
#include <stdio.h>
#include "expressions.h"
#include "parser.h"
#include "scaner.h"
#include "interpret.h"

tTape *Tape;
TStack stack;           //Globálna premenná  pre zásobník

tErrors interpret()											// interpret
{
  //  tTapeItem *savepositionCall=NULL;
//    tTapeItem *savepositionJump=NULL;
    tParamItem *hodnota,*phodnota;
	stackinit(&stack);
	Tape->active = Tape->first;		// inicializace pole
	while (Tape->active->instruction != MAINFUNC) {
		if (Tape->active == NULL) return E_RUNX;
		Tape->active = Tape->active->next;
	}
	Tape->active = Tape->active->next;
	while(Tape->active != Tape->last->next)
	{
	printf("jsem v Interpret------------------------------------------------------------\n");
	printf("instrukce na zacatku interpetu %d\n",Tape->active->instruction);
		hodnota=SearchStackName(&Tape->active->op1->name);
		phodnota=SearchStackName(&Tape->active->op2->name);

/*********************************************ASSIG*******************************************************/
		if(Tape->active->instruction==ASSIGN)
		{
			printf("jsem v Assign\n");

			if(phodnota!=NULL)
			{
				if(phodnota->valFull==NODATA) return E_RUNVAR;
			}
			else if(Tape->active->op2->valFull==NODATA)
				return E_RUNVAR;
				

			
			
			if(hodnota!=NULL && phodnota!=NULL)
			{
				if(hodnota->type==O_INT && phodnota->type==O_INT)
				{
					hodnota->value.ival=phodnota->value.ival;

				}
				else if(hodnota->type==O_REAL && phodnota->type==O_INT)
				{
					hodnota->value.rval=phodnota->value.ival;

				}
				else if(hodnota->type==O_BOOL && phodnota->type==O_BOOL)
				{
					hodnota->value.bval=phodnota->value.bval;

					hodnota->type=O_REAL;
				}
				else if(hodnota->type==O_REAL && phodnota->type==O_REAL)
				{
					hodnota->value.rval=phodnota->value.rval;

				}
				else if(hodnota->type==O_STRING && phodnota->type==O_STRING)
				{
					hodnota->value.sval=phodnota->value.sval;

				}
				else return E_INTERN;
			}
			else if(hodnota==NULL && phodnota!=NULL)
			{
				if(Tape->active->op1->type==O_INT && phodnota->type==O_INT)
				{
					Tape->active->op1->value.ival=phodnota->value.ival;

				}
				else if(Tape->active->op1->type==O_REAL && phodnota->type==O_INT)
				{
					Tape->active->op1->value.rval=phodnota->value.ival;

				}
				else if(Tape->active->op1->type==O_BOOL && phodnota->type==O_BOOL)
				{
					Tape->active->op1->value.bval=phodnota->value.bval;
				}
				else if(Tape->active->op1->type==O_REAL && phodnota->type==O_REAL)
				{
					Tape->active->op1->value.rval=phodnota->value.rval;

				}
				else if(Tape->active->op1->type==O_STRING && phodnota->type==O_STRING)
				{
					Tape->active->op1->value.sval=phodnota->value.sval;

				}
				else return E_INTERN;

			}
			else if(hodnota!=NULL && phodnota==NULL)
			{
				if(hodnota->type==O_INT && Tape->active->op2->type==O_INT)
				{
					hodnota->value.ival=Tape->active->op2->value.ival;

				}
				else if(hodnota->type==O_REAL && Tape->active->op2->type==O_INT)
				{
					hodnota->value.rval=Tape->active->op2->value.ival;

				}
				else if(hodnota->type==O_BOOL && Tape->active->op2->type==O_BOOL)
				{
					hodnota->value.bval=Tape->active->op2->value.bval;

					hodnota->type=O_REAL;
				}
				else if(hodnota->type==O_REAL && Tape->active->op2->type==O_REAL)
				{
					hodnota->value.rval=Tape->active->op2->value.rval;

				}
				else if(hodnota->type==O_STRING && Tape->active->op2->type==O_STRING)
				{
					hodnota->value.sval=Tape->active->op2->value.sval;

				}
				else return E_INTERN;

			}
			else if(hodnota==NULL && phodnota==NULL)
			{
				printf("%d %d typy u prirazeni\n",Tape->active->op1->type,Tape->active->op2->type );
				if(Tape->active->op1->type==O_INT && Tape->active->op2->type==O_INT)
				{
					Tape->active->op1->value.ival=Tape->active->op2->value.ival;
					Tape->active->result->value.ival=Tape->active->op1->value.ival;
					printf("prirazeno je int/int=%d\n",Tape->active->op1->value.ival );


				}
				else if(Tape->active->op1->type==O_REAL && Tape->active->op2->type==O_INT)
				{
					
					Tape->active->op1->value.rval=Tape->active->op2->value.ival;
					Tape->active->result->value.rval=Tape->active->op1->value.rval;
					printf("prirazeno je real/int=%lf \n",Tape->active->op1->value.rval );
				}
				else if(Tape->active->op1->type==O_BOOL && Tape->active->op2->type==O_BOOL)
				{
					Tape->active->op1->value.bval=Tape->active->op2->value.bval;
					printf("prirazeno je bool/bool=%d\n",Tape->active->op1->value.bval );
				}
				else if(Tape->active->op1->type==O_REAL && Tape->active->op2->type==O_REAL)
				{
					Tape->active->op1->value.rval=Tape->active->op2->value.rval;
					printf("prirazeno je real/real=%lf\n",Tape->active->op1->value.rval );
				}
				else if(Tape->active->op1->type==O_STRING && Tape->active->op2->type==O_STRING)
				{
					Tape->active->op1->value.sval=Tape->active->op2->value.sval;
					printf("prirazeno je string=%s\n",Tape->active->op1->value.sval.str );
				}
				else return E_INTERN;


				Tape->active->op1->valFull=DATA;

			}

		}

		if (Tape->active->instruction==JUMPN)											// pokud jde o if neho while tak kontroluji na jump
		{
		
		    //savepositionJump=Tape->active;
		    //Tape->active->op2->value.tape_pointer=savepositionJump;
			/*if(Tape->active->previous->result->value.bval==FALSE && Tape->active->previous->result->type==O_BOOL) // kdyz je predchozi vysledek false tak skacu
			{
				savepositionJump=Tape->active->op1->value.tape_pointer;
				Tape->active=Tape->active->result->value.tape_pointer;				// a skacu tam kam ukazuje pointer
				printf("02 \n ");
				continue;																			// a na zacatek cyklu
			}
			else return E_INTERN;*/																				// pokud neni false tak nic nedelej
			if(Tape->active->op1->value.bval==FALSE && Tape->active->op1->type==O_BOOL) // kdyz je predchozi vysledek false tak skacu
			{
				printf("jumpN\n ");
				Tape->active=Tape->active->op2->value.tape_pointer;				// a skacu tam kam ukazuje pointer
				printf("po skoku \n ");
				continue;																			// a na zacatek cyklu
			}
		
		//	else return E_INTERN;

		}
		else if (Tape->active->instruction==JUMP)
		{
			printf("jump\n ");
			Tape->active=Tape->active->op2->value.tape_pointer;
			continue;
		}
	/*	else if (Tape->active->instruction==NOP)
		{
			Tape->active=savepositionCall->next;
			TStackTopPop(&stack);

		}*/

		else if (Tape->active->instruction==NOP)
		{

		printf("jsem v nop\n");
			Tape->active=Tape->active->next;
			continue;
		}

        if(Tape->active->instruction==READ)							// pokud je readln
		{printf("jsem v read\n");
			char c;
			switch (Tape->active->op1->type)
			{													// kontroluji podle typu
				case O_INT: 	scanf("%d",&Tape->active->op1->value.ival);						// jde o inttak ho naskenuji a nahraji do op2
								Tape->active->result->type=O_INT;									// nastavim typ na int
								Tape->active->result->value.ival = Tape->active->op1->value.ival;
								Tape->active->op1->valFull=DATA;
								break;
				case O_REAL:	//c = getchar();
								//printf("%c\n",c);
								//if (c == ".")
								//	return E_RUNX;

								scanf("%lf",&Tape->active->op1->value.rval);						// pokud jde o real tak ho naskenuji do op2
								printf("JE TO REAL!!!!!!!!\n");
								Tape->active->result->value.rval=Tape->active->op1->value.rval;
								Tape->active->result->type=O_REAL;									// nasatavim typ
								Tape->active->op1->valFull=DATA;
								break;
				case O_STRING: 	Tape->active->op1->value.sval = Readstring();
                               	Tape->active->result->type = O_STRING;
                               	Tape->active->op1->valFull = DATA;
                               	strCopystring(&(Tape->active->result->value.sval), &(Tape->active->op1->value.sval));
		   					   	break;
				default : return E_RUNX;
			}
		}
		
		if(hodnota!=NULL)
        {
            if(hodnota->valFull==NODATA)
           		return E_RUNVAR;
        }
        else
        {
            if(Tape->active->op1->valFull==NODATA )
                return E_RUNVAR;
        }

        if(phodnota!=NULL)
        {
            if(phodnota->valFull==NODATA )
            return E_RUNVAR;
        }
        else
        {	
        	if (Tape->active->op2 != NULL)
            {
               	if(Tape->active->op2->valFull==NODATA )
               		return E_RUNVAR;
            }
        }
        
        if (Tape->active->instruction==WRITE)									// pokud je vestavena funce write delej
		{			
			printf("ve write\n");
				
			switch (Tape->active->op1->type)
			{											// budem rozdelovat podle typu
				case O_INT: printf("%d",Tape->active->op1->value.ival);break;			// jde o int tak tiskni int
				case O_REAL: printf("%g",Tape->active->op1->value.rval);break;		// jde o real tak tiskni real
				case O_BOOL: if(Tape->active->op1->value.bval==TRUE)					// jde o bool tiskni bool
								printf("TRUE");
							 else printf("FALSE");
										break;
				case O_STRING: printf("%s",Tape->active->op1->value.sval.str);break;		// jde o string tisku string
				default: return E_RUNX;												// pokud neni ani jedna ztechto moznosti tak chyba
						
			}
		}
		
		/*if(Tape->active->instruction==CALL)													// pokud je je typ funkce tak jdi do vetve pro funkce
		{
			printf("jsem v funkci\n");
		    tParamItem *pomocna;
		    pomocna = Tape->active->op1->value.param_pointer;
		    printf("%d\n",Tape->active->op1->value.param_pointer->value.ival);
			printf("po pocmonceeuc\n");
			stackPush(&stack,&pomocna);		
			printf("%d\n",pomocna->value.ival);										// pushnu si na zasobnik parametry funkce
            printf("fuhsdhihsduifudsifhdhufihiusdhfihdyusfhui\n");  */                                                                      // nini jdu do funci ktere jsou vestavene
            
			/*
			else if(strCmpConstStr(&Tape->active->op1->name, "length"))							// pokud jde o length tak
			{
				if(stack.top->op1->type!=O_STRING) return E_RUNX;								// pokud neni typ string vrat chybu
				Tape->active->op1->value.ival=strGetLength(&stack.top->op1->value.sval);			// do operandu dva nahrani vysledek
				Tape->active->op1->type=O_INT;													// a typ
                Tape->active->op1->value.valFull=TRUE;
			}
			else if(strCmpConstStr(&Tape->active->op1->name, "copy")) //// nevim jstly funguje		// pokud je copy
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
				ukaz[pocet]='\0';																// na jeho konec nahraji /0
				pomocny.str=ukaz;																// nini nahraji ukazatel do stringu
				pomocny.length=pocet;															// dom do jeho parametru delku
				pomocny.allocSize=sizeof(char)*pocet;											// a jakou velikost pameti potrebuje
				strCopystring(&pomocny, &Tape->active->op1->value.sval);							// nini ho nahraji do op2
				Tape->active->op1->type=O_STRING;												// atyp prepisi na string
                Tape->active->op1->value.valFull=TRUE;

			}
			//else if(strCmpConstStr(&Tape->active->op1->name, "find"))								// pokud je funkce find
			//{*/
			/*	int position=0;
				if(stack.top->op1->type!=O_STRING) return E_RUNX;								// pokud parametr neni sting error
				if(stack.top->op1->next->type!=O_STRING) return E_RUNX;							// pokud neni druchu paramter string error

				char* String_vzor = stack.top->op1->value.sval.str;
				char* find_str = stack.top->op1->next->value.sval.str;

				position = BoyerMoore(String_vzor,strGetLength(&stack.top->op1->value.sval), find_str, strGetLength(&stack.top->op1->next->value.sval.str));

				Tape->active->op1->value.ival = position;
*/
		/*	}
			else if(strCmpConstStr(&Tape->active->op1->name, "sort"))								// pokud je sort
			{*/
			/*	if(stack.top->op1->type!=O_STRING) return E_RUNX;								// pokud neni typ sting error
				quickSort(stack.top->op1->value.sval.str, 0, strGetLength(&stack.top->op1->value.sval));			// propehne serazeni str get leng da velikost - \0
			*/	/*Tape->active->op1->type=O_STRING;		*/										// a nastavim typ na string
//            Tape->active->op1->value.valFull=TRUE;
			//}
			//}
		
		/*********************************************ADD*********************************************************/
	   else if(Tape->active->instruction==ADD)
	   {
	   		printf("jsem v ADD\n");
	   		
	   		if(hodnota!=NULL && phodnota!=NULL)
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
                //Tape->active->result->value.valFull=DATA;

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

                //Tape->active->result->value.valFull=DATA;
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
				//Tape->active->result->value.valFull=DATA;
			}

	   		else
	   		{
	   			printf("add\n");
	   			if(Tape->active->op1->type == O_STRING && Tape->active->op2->type == O_STRING)
					{	
						string cop;
					if(conc(&Tape->active->op1->value.sval,&Tape->active->op2->value.sval)==NULL)	/// spojeni dvou stringu
							return E_RUNX;
						//strCmpConstStr(&cop,Tape->active->op1->value.sval.str);
						//strCmpstring(&Tape->active->op1->value.sval,&cop);
						Tape->active->result->value.sval=Tape->active->op1->value.sval;
						Tape->active->result->type=O_STRING;
						
					}
				else if(Tape->active->op1->type == O_INT && Tape->active->op2->type == O_INT)
					{
						Tape->active->result->value.ival=Tape->active->op1->value.ival+Tape->active->op2->value.ival;
						printf("soucet%d+%d\n",Tape->active->op1->value.ival,Tape->active->op1->value.ival);
						printf("vysledok je : %d \n",Tape->active->result->value.ival);
					}
				else if(Tape->active->op1->type == O_INT && Tape->active->op2->type == O_REAL){
					printf("soucet%d+%lf\n",Tape->active->op1->value.ival,Tape->active->op1->value.rval);
					Tape->active->result->value.rval=Tape->active->op1->value.ival+Tape->active->op2->value.rval;
					Tape->active->result->type=O_REAL;
					printf("vysledok je : %lf \n",Tape->active->result->value.rval);
				}

				else if(Tape->active->op1->type == O_REAL && Tape->active->op2->type == O_INT){
					Tape->active->result->type=O_REAL;
					Tape->active->result->value.rval=Tape->active->op1->value.rval+Tape->active->op2->value.ival;

					//printf("vysledok je : %d \n",Tape->active->result->value.rval);
				}

				else if(Tape->active->op1->type == O_REAL && Tape->active->op2->type == O_REAL){
					Tape->active->result->type=O_REAL;
					Tape->active->result->value.rval=Tape->active->op1->value.rval+Tape->active->op2->value.rval;
						//printf("vysledok je : %d \n",Tape->active->result->value.rval);
				}

				else return E_INTERN;
				////Tape->active->result->value.valFull=DATA;
			}
		}

/*********************************************SUB*********************************************************/
		 else if(Tape->active->instruction==SUB)
	   {
	   		printf("jsem v subb\n");
	   		if(hodnota!=NULL && phodnota!=NULL)
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
				else if(Tape->active->op1->type == O_INT && Tape->active->op2->type == O_REAL){
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
			//Tape->active->result->value.valFull=DATA;
		}

/*********************************************MUL*********************************************************/
		 else if(Tape->active->instruction==MUL)
	   {
	   		printf("jsem v mul\n");
	   		if(hodnota!=NULL && phodnota!=NULL)
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
				else if(Tape->active->op1->type == O_INT && Tape->active->op2->type == O_REAL){
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
			//Tape->active->result->value.valFull=DATA;
		}

/*********************************************DIV*********************************************************/
		else if(Tape->active->instruction==DIV)
	   {

	   		printf("jsem v div\n");
	   		printf("nula ma type:%d\n",Tape->active->op2->value.ival );
	   		if(hodnota!=NULL && phodnota!=NULL)
	   		{

				if(hodnota->type == O_INT && phodnota->type == O_INT)
					{
						if(phodnota->value.ival==0) return E_RUNDIVZ;
						Tape->active->result->type=O_REAL;
						Tape->active->result->value.rval=hodnota->value.ival/phodnota->value.ival;
					}
				else if(hodnota->type == O_INT && phodnota->type == O_REAL){
					if(phodnota->value.rval==0) return E_RUNDIVZ;
					Tape->active->result->value.rval=hodnota->value.ival/phodnota->value.rval;
					Tape->active->result->type=O_REAL;
				}

				else if(hodnota->type == O_REAL && phodnota->type == O_INT){
					if(phodnota->value.ival==0) return E_RUNDIVZ;
					Tape->active->result->type=O_REAL;
					Tape->active->result->value.rval=hodnota->value.rval/phodnota->value.ival;
				}

				else if(hodnota->type == O_REAL && phodnota->type == O_REAL){
					if(phodnota->value.rval==0) return E_RUNDIVZ;
					Tape->active->result->type=O_REAL;
					Tape->active->result->value.rval=hodnota->value.rval/phodnota->value.rval;
				}

				else return E_INTERN;


	   		}
	   		else if(hodnota!=NULL && phodnota==NULL)
	   		{


				if(hodnota->type == O_INT && Tape->active->op2->type == O_INT)
					{
						if(Tape->active->op2->value.ival==0) return E_RUNDIVZ;
						Tape->active->result->type=O_REAL;
						Tape->active->result->value.rval=hodnota->value.ival/Tape->active->op2->value.ival;
					}
				else if(hodnota->type == O_INT && Tape->active->op2->type == O_REAL){
					if(Tape->active->op2->value.rval==0) return E_RUNDIVZ;
					Tape->active->result->value.rval=hodnota->value.ival/Tape->active->op2->value.rval;
					Tape->active->result->type=O_REAL;
				}

				else if(hodnota->type == O_REAL && Tape->active->op2->type == O_INT){
					if(Tape->active->op2->value.ival==0) return E_RUNDIVZ;
					Tape->active->result->type=O_REAL;
					Tape->active->result->value.rval=hodnota->value.rval/Tape->active->op2->value.ival;
				}

				else if(hodnota->type == O_REAL && Tape->active->op2->type == O_REAL){
					if(Tape->active->op2->value.rval==0) return E_RUNDIVZ;
					Tape->active->result->type=O_REAL;
					Tape->active->result->value.rval=hodnota->value.rval/Tape->active->op2->value.rval;
				}

				else return E_INTERN;


	   		}

	   		else if(hodnota==NULL && phodnota!=NULL)
	   		{

				if(Tape->active->op1->type == O_INT && phodnota->type == O_INT)
					{
						if(phodnota->value.ival==0) return E_RUNDIVZ;
						Tape->active->result->type=O_REAL;
						Tape->active->result->value.rval=Tape->active->op1->value.ival/phodnota->value.ival;
					}

				else if(Tape->active->op1->type == O_INT && phodnota->type == O_REAL){
					if(phodnota->value.rval==0) return E_RUNDIVZ;
					Tape->active->result->value.rval=Tape->active->op1->value.ival/phodnota->value.rval;
					Tape->active->result->type=O_REAL;
				}

				else if(Tape->active->op1->type == O_REAL && phodnota->type == O_INT){
					if(phodnota->value.ival==0) return E_RUNDIVZ;
					Tape->active->result->type=O_REAL;
					Tape->active->result->value.rval=Tape->active->op1->value.rval/phodnota->value.ival;
				}

				else if(Tape->active->op1->type == O_REAL && phodnota->type == O_REAL){
					if(phodnota->value.rval==0) return E_RUNDIVZ;
					Tape->active->result->type=O_REAL;
					Tape->active->result->value.rval=Tape->active->op1->value.rval/phodnota->value.rval;
				}

				else return E_INTERN;
			}

	   		else
	   		{

				if(Tape->active->op1->type == O_INT && Tape->active->op2->type == O_INT)
					{
						if(Tape->active->op2->value.ival==0) return E_RUNDIVZ;
						Tape->active->op1->value.rval=Tape->active->op1->value.ival;
						Tape->active->op2->value.rval=Tape->active->op2->value.ival;
						Tape->active->result->type=O_REAL;
						Tape->active->result->value.rval=Tape->active->op1->value.rval/Tape->active->op2->value.rval;
						printf("div : vysledek: %lf\n", Tape->active->result->value.rval);
					}
				else if(Tape->active->op1->type == O_INT && Tape->active->op2->type == O_REAL){
					if(Tape->active->op2->value.rval==0) return E_RUNDIVZ;
					Tape->active->result->value.rval=Tape->active->op1->value.ival/Tape->active->op2->value.rval;
					Tape->active->result->type=O_REAL;
				}

				else if(Tape->active->op1->type == O_REAL && Tape->active->op2->type == O_INT){
					if(Tape->active->op2->value.ival==0) return E_RUNDIVZ;
					Tape->active->result->type=O_REAL;
					Tape->active->result->value.rval=Tape->active->op1->value.rval/Tape->active->op2->value.ival;
				}

				else if(Tape->active->op1->type == O_REAL && Tape->active->op2->type == O_REAL){
					if(Tape->active->op2->value.rval==0) return E_RUNDIVZ;
					Tape->active->result->type=O_REAL;
					Tape->active->result->value.rval=Tape->active->op1->value.rval/Tape->active->op2->value.rval;
				}

				else return E_INTERN;
			}
			//Tape->active->result->value.valFull=DATA;
		}

/*********************************************BIGGER******************************************************/
		else if(Tape->active->instruction==MORE)		//op1>op2
		{
			printf("jsem v bigger\n");
            if(hodnota!=NULL && phodnota!=NULL)
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
					Tape->active->result->value.bval=hodnota->value.rval>phodnota->value.ival;
				}

				else if(hodnota->type == O_REAL && phodnota->type == O_REAL){
					Tape->active->result->type=O_BOOL;
					Tape->active->result->value.bval=hodnota->value.rval>phodnota->value.rval;
				}
				else if(hodnota->type == O_BOOL && phodnota->type == O_BOOL){
					Tape->active->result->type=O_BOOL;
					Tape->active->result->value.bval=hodnota->value.bval>phodnota->value.bval;
				}
				else if(hodnota->type == O_STRING && phodnota->type == O_STRING)
					{
					Tape->active->result->type=O_BOOL;
					if(strCopystring(&hodnota->value.sval,&phodnota->value.sval) > 0)
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
						Tape->active->result->value.bval=hodnota->value.ival>Tape->active->op2->value.ival;
					}
				else if(hodnota->type == O_INT && Tape->active->op2->type == O_REAL){
					Tape->active->result->value.bval=hodnota->value.ival>Tape->active->op2->value.rval;
					Tape->active->result->type=O_BOOL;
				}

				else if(hodnota->type == O_REAL && Tape->active->op2->type == O_INT){
					Tape->active->result->type=O_BOOL;
					Tape->active->result->value.bval=hodnota->value.rval>Tape->active->op2->value.ival;
				}

				else if(hodnota->type == O_REAL && Tape->active->op2->type == O_REAL){
					Tape->active->result->type=O_BOOL;
					Tape->active->result->value.bval=hodnota->value.rval>Tape->active->op2->value.rval;
				}
				else if(hodnota->type == O_BOOL && Tape->active->op2->type == O_BOOL){
					Tape->active->result->type=O_BOOL;
					Tape->active->result->value.bval=hodnota->value.bval>Tape->active->op2->value.bval;
				}
				else if(hodnota->type == O_STRING && Tape->active->op2->type == O_STRING)
					{
					Tape->active->result->type=O_BOOL;
					if(strCopystring(&(hodnota->value.sval),&(Tape->active->op2->value.sval)) > 0)
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
						Tape->active->result->value.bval=Tape->active->op1->value.ival>phodnota->value.ival;

					}

				else if(Tape->active->op1->type == O_INT && phodnota->type == O_REAL){
					Tape->active->result->value.bval=Tape->active->op1->value.ival>phodnota->value.rval;
					Tape->active->result->type=O_BOOL;
				}

				else if(Tape->active->op1->type == O_REAL && phodnota->type == O_INT){
					Tape->active->result->type=O_BOOL;
					Tape->active->result->value.bval=Tape->active->op1->value.rval>phodnota->value.ival;
				}

				else if(Tape->active->op1->type == O_REAL && phodnota->type == O_REAL){
					Tape->active->result->type=O_BOOL;
					Tape->active->result->value.bval=Tape->active->op1->value.rval>phodnota->value.rval;
				}
				else if(Tape->active->op1->type == O_BOOL && phodnota->type == O_BOOL){
					Tape->active->result->type=O_BOOL;
					Tape->active->result->value.bval=Tape->active->op1->value.bval>phodnota->value.bval;
				}
				else if(Tape->active->op1->type == O_STRING && phodnota->type == O_STRING)
					{
					Tape->active->result->type=O_BOOL;
					if(strCopystring(&(Tape->active->op1->value.sval),&(phodnota->value.sval)) > 0)
						Tape->active->result->value.bval=1;
					else Tape->active->result->value.bval=0;
					}

				else return E_INTERN;
			}

	   		else
	   		{
	   			printf("jsem v bigger\n");
				if(Tape->active->op1->type == O_INT && Tape->active->op2->type == O_INT)
					{
						Tape->active->result->type=O_BOOL;
						
						Tape->active->result->value.bval=Tape->active->op1->value.ival>Tape->active->op2->value.ival;
						printf("jsem v bigger a vracim %d\n",Tape->active->result->value.bval);
					}
				else if(Tape->active->op1->type == O_INT && phodnota->type == O_REAL){
					Tape->active->result->value.bval=Tape->active->op1->value.ival>Tape->active->op2->value.rval;
					Tape->active->result->type=O_BOOL;
				}

				else if(Tape->active->op1->type == O_REAL && Tape->active->op2->type == O_INT){
					Tape->active->result->type=O_BOOL;
					Tape->active->result->value.bval=Tape->active->op1->value.rval>Tape->active->op2->value.ival;
				}

				else if(Tape->active->op1->type == O_REAL && Tape->active->op2->type == O_REAL){
					Tape->active->result->type=O_BOOL;
					Tape->active->result->value.bval=Tape->active->op1->value.rval>Tape->active->op2->value.rval;
				}
				else if(Tape->active->op1->type == O_BOOL && Tape->active->op2->type == O_BOOL){
					Tape->active->result->type=O_BOOL;
					Tape->active->result->value.bval=Tape->active->op1->value.bval>Tape->active->op2->value.bval;
				}
				else if(Tape->active->op1->type == O_STRING && Tape->active->op2->type == O_STRING)
					{
					Tape->active->result->type=O_BOOL;
					if(strCopystring(&(Tape->active->op1->value.sval),&(Tape->active->op2->value.sval)) > 0)
						Tape->active->result->value.bval=1;
					else Tape->active->result->value.bval=0;
					}

				else return E_INTERN;
			}
			//Tape->active->result->value.valFull=DATA;
		}


/*********************************************SMALLER*****************************************************/
		else if(Tape->active->instruction==LESS)													/// op1<op2
		{
				printf("jsem v less\n");
				tVariable*prohozeni;
				prohozeni=Tape->active->op1;
				Tape->active->op1=Tape->active->op2;
				Tape->active->op2=prohozeni;
				Tape->active->instruction=MORE;
				continue;
		}

/*********************************************EBIGGER*****************************************************/
		else if(Tape->active->instruction==EQM)						// op1>=op2
		{
			printf("jsem v lessEq\n");
	   		if(hodnota!=NULL && phodnota!=NULL)
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
					Tape->active->result->value.bval=hodnota->value.rval>=phodnota->value.ival;
				}

				else if(hodnota->type == O_REAL && phodnota->type == O_REAL){
					Tape->active->result->type=O_BOOL;
					Tape->active->result->value.bval=hodnota->value.rval>=phodnota->value.rval;
				}
				else if(hodnota->type == O_BOOL && phodnota->type == O_BOOL){
					Tape->active->result->type=O_BOOL;
					Tape->active->result->value.bval=hodnota->value.bval>=phodnota->value.bval;
				}
				else if(hodnota->type == O_STRING && phodnota->type == O_STRING)
					{
					Tape->active->result->type=O_BOOL;
					if(strCopystring(&(hodnota->value.sval),&(phodnota->value.sval)) >= 0)
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
						Tape->active->result->value.bval=hodnota->value.ival>=Tape->active->op2->value.ival;
					}
				else if(hodnota->type == O_INT && Tape->active->op2->type == O_REAL){
					Tape->active->result->value.bval=hodnota->value.ival>=Tape->active->op2->value.rval;
					Tape->active->result->type=O_BOOL;
				}

				else if(hodnota->type == O_REAL && Tape->active->op2->type == O_INT){
					Tape->active->result->type=O_BOOL;
					Tape->active->result->value.bval=hodnota->value.rval>=Tape->active->op2->value.ival;
				}

				else if(hodnota->type == O_REAL && Tape->active->op2->type == O_REAL){
					Tape->active->result->type=O_BOOL;
					Tape->active->result->value.bval=hodnota->value.rval>=Tape->active->op2->value.rval;
				}
				else if(hodnota->type == O_BOOL && Tape->active->op2->type == O_BOOL){
					Tape->active->result->type=O_BOOL;
					Tape->active->result->value.bval=hodnota->value.bval>=Tape->active->op2->value.bval;
				}
				else if(hodnota->type == O_STRING && Tape->active->op2->type == O_STRING)
					{
					Tape->active->result->type=O_BOOL;
					if(strCopystring(&(hodnota->value.sval),&(Tape->active->op2->value.sval)) >= 0)
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
						Tape->active->result->value.bval=Tape->active->op1->value.ival>=phodnota->value.ival;
					}

				else if(Tape->active->op1->type == O_INT && phodnota->type == O_REAL){
					Tape->active->result->value.bval=Tape->active->op1->value.ival>=phodnota->value.rval;
					Tape->active->result->type=O_BOOL;
				}

				else if(Tape->active->op1->type == O_REAL && phodnota->type == O_INT){
					Tape->active->result->type=O_BOOL;
					Tape->active->result->value.bval=Tape->active->op1->value.rval>=phodnota->value.ival;
				}

				else if(Tape->active->op1->type == O_REAL && phodnota->type == O_REAL){
					Tape->active->result->type=O_BOOL;
					Tape->active->result->value.bval=Tape->active->op1->value.rval>=phodnota->value.rval;
				}
				else if(Tape->active->op1->type == O_BOOL && phodnota->type == O_BOOL){
					Tape->active->result->type=O_BOOL;
					Tape->active->result->value.bval=Tape->active->op1->value.bval>=phodnota->value.bval;
				}
				else if(Tape->active->op1->type == O_STRING && phodnota->type == O_STRING)
					{
					Tape->active->result->type=O_BOOL;
					if(strCopystring(&(Tape->active->op1->value.sval),&(phodnota->value.sval)) >= 0)
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
						Tape->active->result->value.bval=Tape->active->op1->value.ival>=Tape->active->op2->value.ival;
					}
				else if(Tape->active->op1->type == O_INT && phodnota->type == O_REAL){
					Tape->active->result->value.bval=Tape->active->op1->value.ival>=Tape->active->op2->value.rval;
					Tape->active->result->type=O_BOOL;
				}

				else if(Tape->active->op1->type == O_REAL && Tape->active->op2->type == O_INT){
					Tape->active->result->type=O_BOOL;
					Tape->active->result->value.bval=Tape->active->op1->value.rval>=Tape->active->op2->value.ival;
				}

				else if(Tape->active->op1->type == O_REAL && Tape->active->op2->type == O_REAL){
					Tape->active->result->type=O_BOOL;
					Tape->active->result->value.bval=Tape->active->op1->value.rval>=Tape->active->op2->value.rval;
				}
				else if(Tape->active->op1->type == O_BOOL && Tape->active->op2->type == O_BOOL){
					Tape->active->result->type=O_BOOL;
					Tape->active->result->value.bval=Tape->active->op1->value.bval>=Tape->active->op2->value.bval;
				}
				else if(Tape->active->op1->type == O_STRING && Tape->active->op2->type == O_STRING)
					{
					Tape->active->result->type=O_BOOL;
					if(strCopystring(&(Tape->active->op1->value.sval),&(Tape->active->op2->value.sval)) >= 0)
						Tape->active->result->value.bval=1;
					else Tape->active->result->value.bval=0;
					}

				else return E_INTERN;
			}
			//Tape->active->result->value.valFull=DATA;
		}
/*********************************************ESMALLER****************************************************/
		else if(Tape->active->instruction==EQL)
		{		printf("jsem v <=\n");
				tVariable*prohozeni=Tape->active->op1;
				Tape->active->op1=Tape->active->op2;
				Tape->active->op2=prohozeni;
				Tape->active->instruction=EQM;
				continue;
		}
/*********************************************EQUAL*******************************************************/
		else if(Tape->active->instruction==EQL)
		{	printf("jsem v ==\n");
            if(hodnota!=NULL && phodnota!=NULL)
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
					Tape->active->result->value.bval=hodnota->value.rval==phodnota->value.ival;
				}

				else if(hodnota->type == O_REAL && phodnota->type == O_REAL){
					Tape->active->result->type=O_BOOL;
					Tape->active->result->value.bval=hodnota->value.rval==phodnota->value.rval;
				}
				else if(hodnota->type == O_BOOL && phodnota->type == O_BOOL){
					Tape->active->result->type=O_BOOL;
					Tape->active->result->value.bval=hodnota->value.bval==phodnota->value.bval;
				}
				else if(hodnota->type == O_STRING && phodnota->type == O_STRING)
					{
					Tape->active->result->type=O_BOOL;
					if(strCopystring(&(hodnota->value.sval),&(phodnota->value.sval)) == 0)
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
						Tape->active->result->value.bval=hodnota->value.ival==Tape->active->op2->value.ival;
					}
				else if(hodnota->type == O_INT && Tape->active->op2->type == O_REAL){
					Tape->active->result->value.bval=hodnota->value.ival==Tape->active->op2->value.rval;
					Tape->active->result->type=O_BOOL;
				}

				else if(hodnota->type == O_REAL && Tape->active->op2->type == O_INT){
					Tape->active->result->type=O_BOOL;
					Tape->active->result->value.bval=hodnota->value.rval==Tape->active->op2->value.ival;
				}

				else if(hodnota->type == O_REAL && Tape->active->op2->type == O_REAL){
					Tape->active->result->type=O_BOOL;
					Tape->active->result->value.bval=hodnota->value.rval==Tape->active->op2->value.rval;
				}
				else if(hodnota->type == O_BOOL && Tape->active->op2->type == O_BOOL){
					Tape->active->result->type=O_BOOL;
					Tape->active->result->value.bval=hodnota->value.bval==Tape->active->op2->value.bval;
				}
				else if(hodnota->type == O_STRING && Tape->active->op2->type == O_STRING)
					{
					Tape->active->result->type=O_BOOL;
					if(strCopystring(&(hodnota->value.sval),&(Tape->active->op2->value.sval)) == 0)
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
						Tape->active->result->value.bval=Tape->active->op1->value.ival==phodnota->value.ival;
					}

				else if(Tape->active->op1->type == O_INT && phodnota->type == O_REAL){
					Tape->active->result->value.bval=Tape->active->op1->value.ival==phodnota->value.rval;
					Tape->active->result->type=O_BOOL;
				}

				else if(Tape->active->op1->type == O_REAL && phodnota->type == O_INT){
					Tape->active->result->type=O_BOOL;
					Tape->active->result->value.bval=Tape->active->op1->value.rval==phodnota->value.ival;
				}

				else if(Tape->active->op1->type == O_REAL && phodnota->type == O_REAL){
					Tape->active->result->type=O_BOOL;
					Tape->active->result->value.bval=Tape->active->op1->value.rval==phodnota->value.rval;
				}
				else if(Tape->active->op1->type == O_BOOL && phodnota->type == O_BOOL){
					Tape->active->result->type=O_BOOL;
					Tape->active->result->value.bval=Tape->active->op1->value.bval==phodnota->value.bval;
				}
				else if(Tape->active->op1->type == O_STRING && phodnota->type == O_STRING)
					{
					Tape->active->result->type=O_BOOL;
					if(strCopystring(&(Tape->active->op1->value.sval),&(phodnota->value.sval)) == 0)
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
						Tape->active->result->value.bval=Tape->active->op1->value.ival==Tape->active->op2->value.ival;
					}
				else if(Tape->active->op1->type == O_INT && phodnota->type == O_REAL){
					Tape->active->result->value.bval=Tape->active->op1->value.ival==Tape->active->op2->value.rval;
					Tape->active->result->type=O_BOOL;
				}

				else if(Tape->active->op1->type == O_REAL && Tape->active->op2->type == O_INT){
					Tape->active->result->type=O_BOOL;
					Tape->active->result->value.bval=Tape->active->op1->value.rval==Tape->active->op2->value.ival;
				}

				else if(Tape->active->op1->type == O_REAL && Tape->active->op2->type == O_REAL){
					Tape->active->result->type=O_BOOL;
					Tape->active->result->value.bval=Tape->active->op1->value.rval==Tape->active->op2->value.rval;
				}
				else if(Tape->active->op1->type == O_BOOL && Tape->active->op2->type == O_BOOL){
					Tape->active->result->type=O_BOOL;
					Tape->active->result->value.bval=Tape->active->op1->value.bval==Tape->active->op2->value.bval;
				}
				else if(Tape->active->op1->type == O_STRING && Tape->active->op2->type == O_STRING)
					{
					Tape->active->result->type=O_BOOL;
					if(strCopystring(&(Tape->active->op1->value.sval),&(Tape->active->op2->value.sval)) == 0)
						Tape->active->result->value.bval=1;
					else Tape->active->result->value.bval=0;
					}

				else return E_INTERN;
			}
			//Tape->active->result->value.valFull=DATA;
		}
/*********************************************NEQUAL******************************************************/
			else if(Tape->active->instruction==NEQ)
		{	printf("jsem v !=\n");

	   		if(hodnota!=NULL && phodnota!=NULL)
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
					Tape->active->result->value.bval=hodnota->value.rval!=phodnota->value.ival;
				}

				else if(hodnota->type == O_REAL && phodnota->type == O_REAL){
					Tape->active->result->type=O_BOOL;
					Tape->active->result->value.bval=hodnota->value.rval!=phodnota->value.rval;
				}
				else if(hodnota->type == O_BOOL && phodnota->type == O_BOOL){
					Tape->active->result->type=O_BOOL;
					Tape->active->result->value.bval=hodnota->value.bval!=phodnota->value.bval;
				}
				else if(hodnota->type == O_STRING && phodnota->type == O_STRING)
					{
					Tape->active->result->type=O_BOOL;
					if(strCopystring(&(hodnota->value.sval),&(phodnota->value.sval)) != 0)
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
						Tape->active->result->value.bval=hodnota->value.ival!=Tape->active->op2->value.ival;
					}
				else if(hodnota->type == O_INT && Tape->active->op2->type == O_REAL){
					Tape->active->result->value.bval=hodnota->value.ival!=Tape->active->op2->value.rval;
					Tape->active->result->type=O_BOOL;
				}

				else if(hodnota->type == O_REAL && Tape->active->op2->type == O_INT){
					Tape->active->result->type=O_BOOL;
					Tape->active->result->value.bval=hodnota->value.rval!=Tape->active->op2->value.ival;
				}

				else if(hodnota->type == O_REAL && Tape->active->op2->type == O_REAL){
					Tape->active->result->type=O_BOOL;
					Tape->active->result->value.bval=hodnota->value.rval!=Tape->active->op2->value.rval;
				}
				else if(hodnota->type == O_BOOL && Tape->active->op2->type == O_BOOL){
					Tape->active->result->type=O_BOOL;
					Tape->active->result->value.bval=hodnota->value.bval!=Tape->active->op2->value.bval;
				}
				else if(hodnota->type == O_STRING && Tape->active->op2->type == O_STRING)
					{
					Tape->active->result->type=O_BOOL;
					if(strCopystring(&(hodnota->value.sval),&(Tape->active->op2->value.sval)) != 0)
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
						Tape->active->result->value.bval=Tape->active->op1->value.ival!=phodnota->value.ival;
					}

				else if(Tape->active->op1->type == O_INT && phodnota->type == O_REAL){
					Tape->active->result->value.bval=Tape->active->op1->value.ival!=phodnota->value.rval;
					Tape->active->result->type=O_BOOL;
				}

				else if(Tape->active->op1->type == O_REAL && phodnota->type == O_INT){
					Tape->active->result->type=O_BOOL;
					Tape->active->result->value.bval=Tape->active->op1->value.rval!=phodnota->value.ival;
				}

				else if(Tape->active->op1->type == O_REAL && phodnota->type == O_REAL){
					Tape->active->result->type=O_BOOL;
					Tape->active->result->value.bval=Tape->active->op1->value.rval!=phodnota->value.rval;
				}
				else if(Tape->active->op1->type == O_BOOL && phodnota->type == O_BOOL){
					Tape->active->result->type=O_BOOL;
					Tape->active->result->value.bval=Tape->active->op1->value.bval!=phodnota->value.bval;
				}
				else if(Tape->active->op1->type == O_STRING && phodnota->type == O_STRING)
					{
					Tape->active->result->type=O_BOOL;
					if(strCopystring(&(Tape->active->op1->value.sval),&(phodnota->value.sval)) != 0)
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
						Tape->active->result->value.bval=Tape->active->op1->value.ival!=Tape->active->op2->value.ival;
					}
				else if(Tape->active->op1->type == O_INT && phodnota->type == O_REAL){
					Tape->active->result->value.bval=Tape->active->op1->value.ival!=Tape->active->op2->value.rval;
					Tape->active->result->type=O_BOOL;
				}

				else if(Tape->active->op1->type == O_REAL && Tape->active->op2->type == O_INT){
					Tape->active->result->type=O_BOOL;
					Tape->active->result->value.bval=Tape->active->op1->value.rval!=Tape->active->op2->value.ival;
				}

				else if(Tape->active->op1->type == O_REAL && Tape->active->op2->type == O_REAL){
					Tape->active->result->type=O_BOOL;
					Tape->active->result->value.bval=Tape->active->op1->value.rval!=Tape->active->op2->value.rval;
				}
				else if(Tape->active->op1->type == O_BOOL && Tape->active->op2->type == O_BOOL){
					Tape->active->result->type=O_BOOL;
					Tape->active->result->value.bval=Tape->active->op1->value.bval!=Tape->active->op2->value.bval;
				}
				else if(Tape->active->op1->type == O_STRING && Tape->active->op2->type == O_STRING)
					{
					Tape->active->result->type=O_BOOL;
					if(strCopystring(&(Tape->active->op1->value.sval),&(Tape->active->op2->value.sval)) != 0)
						Tape->active->result->value.bval=1;
					else Tape->active->result->value.bval=0;
					}

				else return E_INTERN;
			}
			//Tape->active->result->value.valFull=DATA;
		}
		//else return E_RUNX;
		
		printf("posunuti pasky\n");
		Tape->active=Tape->active->next;														// posunuti na pasce na dasi instrukci
	}

	//StackDeleteDataDelete(&stack);															// uvolneni celeho zasobniku
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
    while(TStackTopPop(stack)!=NULL)                                                   // dokud zasobnik neni prazdny tak pop
        ;
    return;
}


tParamItem *SearchStackName(string*Search)
{
 if (stack.top == NULL) return NULL;
	TStackItem *topp=stack.top;
	while(topp->op1!=NULL)
	{
		if (strCmpstring(Search,&topp->op1->name)==0)
		{
			return topp->op1;
		}
		topp->op1=topp->op1->next;
	}

	return NULL;
}

string *conc(string*s1,string*s2)
{
    if (s1==NULL || s2==NULL) return NULL;
    
	int i = s1->length;
	int j = s2->length;
	int k;
	k = j+i;
	
	s1->str = ((char*) reallocate(s1->str,(k +1)));
	j=0;
	
	while(i <= k)
    {	
        s1->str[i] = s2->str[j];
        j++;
        i++;
    }
    s1->str[i+1] = '\0';
    return s1;
}
string Readstring()
{
    char a;
    string vracim;
    strInit(&(vracim));            //pokud funkce init. stringu vrati chybu => E_INTERN
    scanf("%c",&a);

    while(a != '\n'){	
     	strAddChar(&vracim, a);
     	scanf("%c",&a);
    }
    
    return vracim;
}

