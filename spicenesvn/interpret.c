
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

extern tTable *Table;

void najdi_navestie(TapeItem **Table, int **jump_array)


void interpret(TapeItem **Table, Pointers)
{
    if((*ta_table) == NULL)  //prazdny program, koniec
    {
        return;
    }
    int end = (*Table)[0].end; //vynuluj tabulku
    size_t operators; //velkost operatorov
    Pointers *op1, *op2, *result, *pom *previous, *next;
    *op1 = NULL;
    *op2 = NULL;
    *result = NULL;
    *pom = NULL;
    *previous= NULL;
    *next = NULL;
    int *jump_array = NULL ;
    find_labels(Table, &jump_array);


    void znic_navestie(int *jump_array) 
{
    jump_array = NULL; //
}
/******************************PRÁCA ZO ZÁSOBNÍKOM*******************************/

void stackinit(TStack *stack)       // inicializace zasodbnika
{
    stack->top=NULL;                // vypraázdním vrchol zásobníku
    
    return;
}

tErrors stackPush(TStack *stack,tParamItem *op1)                                // funkce pro nahrati na zasobnik 
{
    TStackItem *phelp                                                           // pomocna promena pro svazani na zasobniku se spodnim prvkem

    if(stack->top==NULL )                                                       // kdyz je zadobnik prazdny tak
    {
        if((stack->top = malloc(sizeof(TStackItem)))==NULL) return E_INTERN;      // alokuj misto prvku a nahraj ho na vrchol zasobniku
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

void StackDeleteDataDelete(TStack *stack)                                       // fuknce pro uvolneni zasobnika
{
    if (stack->top==NULL) return;                                                       // pokud je prazdny nic nedelej
    while(TStackTopPop(&stack)!=NULL);                                            // dokud zasobnik neni prazdny tak pop
        
    return;

}
        /**************************KONIEC ZÁSOBNÍka*********************************/

void EditPar(tVariable *edit)                                                   // kdyz promena nema hodnotu je poslana do funkce a ja ze zasobniku pokud tam je tak je zamenim 
{
    tParamItem *pom=stack->top;                                                 // pomocna pro prohledani na zasobniku

    while(pom!=NULL)                                                            // cykli do konce zasobniku (parametry jsou spojene tak projizdim do konce)
    {
        if(strCmpstring(edit->name,pom->name))                                  // porovnej jestli je schoda vyskoc
            break;
        pom=pom->next;                                                          // pomocnou posun dal
    }

    if(pom==NULL) return;                                                       // pokud je pom null tak neni na zasobniku

    switch pom->TYPE
        case O_INT: edit->data.hodnotaI = pom->data.hodnotaI; break;                    // kontroluji hledam typ a podle to ho pak kopiruji 
        case O_REAL: edit->data.hodnotaR=pom->data.hodnotaR; break;   
        case O_STRING: strCopystring(edit->data.hodnotaS,pom->data.hodnotaS); break;
        case O:BOOL: edit->data.hodnotaB = pom->data.hodnotaB break;
        case NONTRM: break;
    return;
}



    for(int position  = 0; position < end; position++)
    {
        ta_Get(Table, &operators, &op1, &op2, &result, &previos, &next position);
        switch(operators)
        {
            /******************************aritmetické operácie***********************************/

        case ADD: //operator scitania *+*

            if(result == NULL)
            {
                break;
            }
            if((op1->data.typ == rval) && (op2->data.typ == rval)) // ak je to typ rval  vysledok uloz do rval
            {
                result->data.hodnotaD = (op1->data.hodnotaD) + (op2->data.hodnotaD);
                result->data.typ = rval;
            }
            else if((op1->data.typ == sval) && (op2->data.typ == sval)) // ak je to retezec 
            {
                string text1, text2; // uloz data do hodnoty s  
                text1.data = op1->data.hodnotaS;
                text2.data = op2->data.hodnotaS;
                result->data.hodnotaS = strAppendString(&text1, &text2); // spoj tieto dva retezce do jedneho
            }
            else // inac nic 
            {
                error = E_SEMA;
                Destroy(Table); // znic tu tabulku nainicializujeme si novu s cistimy promennymi
                return;
            }
            break;


        case SUB: // operator odcitania *-*

            if(result == NULL)
            {
                break;
            }
            if((op1->data.typ == rval) && (op2->data.typ == rval)) // ak su oba rval, vysledek do rval 
            {
                result->data.hodnotaD = op1->data.hodnotaD - op2->data.hodnotaD;
                result->data.typ = rval; //vysledok rval
            }
            else // inak nic tabulka dead
            {
                error = E_SEMA;
                Destroy(Table);
                return;
            }
            break;


        case MUL:  // oeprator nasobenia  "*"

            if(result == NULL)
            {
                break;
            }
            if((op1->data.typ == rval) && (op2->data.typ == rval)) //ak oba rval tak vysledek uloz do doublu
            {
                result->data.hodnotaD = op1->data.hodnotaD * op2->data.hodnotaD;
                result->data.typ=rval; //vysledok rval
            }
            else // inak erorr
            {
                error = E_SEMA;
                Destroy(Table); // znič tabulku
                znic_navestie(jump_array);
                return;
            }
            break;


        case DIV: // operator delenie */*

            if(result == NULL)
            {
                break;
            }
            if((op1->data.typ == rval) && (op2->data.typ == rval)) // ak oba oper. su rval tak 
            {
                if(op2->data.hodnotaD == 0.0) // druhy operator nesmie byt 0 inak erorr
                {
                    error = EINT;
                    Destroy(ta_table);
                    return;
                }
                result->data.hodnotaD = op1->data.hodnotaD / op2->data.hodnotaD; // vysledek do doublu
                result->data.typ = rval; //vysledok rval
            }

            else // inak erorr
            {
                error = E_SEMA;
                Destroy(Table);
                return;
            }
            break;

        case SIGN: //boolean or string or rval *druh*

            if(result == NULL)
            {
                break;
            }
            if(op1->data.typ == bval)  // ak je typu bool tak vysledek bool
            {
                result->data.hodnota.b = op1->data.hodnota.b;
                result->data.typ = bval; //vysledok bool
            }
            else if(op1->data.typ == rval) // ak rval vysledek bude rval
            {
                result->data.hodnotaD = op1->data.hodnotaD;
                result->data.typ = rval; //vysledok rval
            }
            else if(op1->data.typ == sval) // ak string vysledek bude string
            {
                result->data.hodnotaS = op1->data.hodnotaS;
                result->data.typ = sval; //vysledok string
            }
            else if(op1->data.typ == NOP) //ak je nula tak FALSE
            {
                result->data.typ = NOP;
                result->data.hodnota.b = FALSE;
            }
            else // inak erorr
            {
                error = E_SEMA;
                Destroy(Table);
                return;
            }
            break;

        case POW: //umocniť retazce

            if(result == NULL)
            {
                break;
            }
            if((op1->data.typ == rval) && (op2->data.typ == rval)) // 
            {
                result->data.hodnotaD= pow(op1->data.hodnotaD, op2->data.hodnotaD);
                result->data.typ=rval;
            }
            else
            {
                error = E_SEMA;
                Destroy(Table);
                znic_navestie(jump_array);
                return;
            }
            break;

            /****************porovnanie**********************/


        case EQUAL: //ak je rovne   *=*

            if(result == NULL)
            {
                break;
            }
            if(op1->data.typ == rval) //ak je op 1 rval 
            {
                if(op2->data.typ != rval) // a op2 nie je 
                {
                    result->data.hodnota.b=FALSE; // tak FALSE
                    result->data.typ=bval; //vysledok bool
                }
                else
                {
                    if(op2->data.hodnotaD == op1->data.hodnotaD) // ak sa rovnaju tak je to TRUE
                    {
                        result->data.hodnota.b = TRUE; // TRUE
                        result->data.typ = bval; //vysledok bool
                    }
                    else
                    {
                        result->data.hodnota.b = FALSE; // inak FALSE
                        result->data.typ = bval; //vysledok bool
                    }
                }
            }
            else if(op1->data.typ == bval) // ak je dat. typ bool
            {
                if(op2->data.typ != bval) // a ak nieje
                {
                    result->data.hodnota.b = FALSE; // tak FALSE
                    result->data.typ = bval; //vysledok bool
                }
                else
                {
                    if(op2->data.hodnota.b == op1->data.hodnota.b) // ak op1 je rovny op 2 tak vrat TRUE
                    {
                        result->data.hodnota.b = TRUE;
                        result->data.typ = bval; //vysledok bool
                    }
                    else // inak chyba
                    {
                        result->data.hodnota.b = FALSE;
                        result->data.typ = bval; //vysledok bool
                    }
                }
            }
            else if (op1->data.typ == sval && op2->data.typ  == sval) 
            {
                if(strcmp(op1->data.hodnotaS,op2->data.hodnotaS) == 0) //porovnaj 2 stringi a ak sa = 0 vrat TRUE
                {
                    result->data.hodnota.b = TRUE;
                    result->data.typ = bval; //vysledok bool
                }
                else
                {
                    result->data.hodnota.b = FALSE; ///inak FALSE
                    result->data.typ = bval; //vysledok bool
                }
            }
            else //inak erorr
            {
                error = E_SEMA;
                Destroy(Table);
                znic_navestie(jump_array);
                return;
            }
            break;

        case NEQUAL: // ak nieje rovne *!=*

                if(result == NULL)
            {
                break;
            }
            if(op1->data.typ == rval) 
            {
                if(op2->data.typ != rval) // ak nie je rval vrat TRUE
                {
                    result->data.hodnota.b = TRUE;
                    result->data.typ = bval; //vysledok bool
                }
                else
                {
                    if(op1->data.hodnotaD != op2->data.hodnotaD) // ak op1 z op2 sa nerovnaju vrat TRUE
                    {
                        result->data.hodnota.b = TRUE;
                        result->data.typ = bval; //vysledok bool
                    }
                    else
                    {
                        result->data.hodnota.b = FALSE; // inak FALSE
                        result->data.typ = bval; //vysledok bool
                    }
                }
            }
            if(op1->data.typ == bval)
            {
                if(op2->data.typ != bval) // ak nieje bool  tak vrat TRUE
                {
                    result->data.hodnota.b = TRUE;
                    result->data.typ = bval; //vysledok bool
                }
                else
                {
                    if(op1->data.hodnota.b != op2->data.hodnota.b) //ak op1 sa nerovna op2 vrat TRUE
                    {
                        result->data.hodnota.b = TRUE;
                        result->data.typ = bval; //vysledok bool
                    }
                    else
                    {
                        result->data.hodnota.b = FALSE; //inak FALSE
                        result->data.typ = bval; //vysledok bool
                    }
                }
            }
            else if (op1->data.typ == sval && op2->data.typ == sval) // mame dva retazce
            {
                if(strcmp(op1->data.hodnotaS,op2->data.hodnotaS) !=0 ) // porovnaj ich a ak sa nerovnaju 0
                {
                    result->data.hodnota.b = TRUE; //vrat TRUE
                    result->data.typ = bval; //vysledok bool
                }
                else
                {
                    result->data.hodnota.b = FALSE; //inak FALSE
                    result->data.typ = bval; //vysledok bool
                }
            }
            else //inak ERORR
            {
                error = E_SEMA;
                Destroy(Table);
                znic_navestie(jump_array);
                return;
            }
            break;

        case ELESS: // rovne alebo mensie *<=*

            if(result == NULL)
            {
                break;
            }
            if(op1->data.typ == rval) // ak  je datoveho typu rval
            {
                if(op2->data.typ != rval) // ak nieje 
                {
                    result->data.hodnota.b = FALSE; //tak vrat FALSE
                    result->data.typ=bval; //vysledok bool
                }
                else
                {
                    if(op1->data.hodnotaD <= op2->data.hodnotaD) // ak op1 je mensi alebo rovny  op2 tak vrat TRUE
                    {
                        result->data.hodnota.b = TRUE;
                        result->data.typ = bval; //vysledok bool
                    }
                    else
                    {
                        result->data.hodnota.b = FALSE; // inak FALSE
                        result->data.typ = bval; //vysledok bool
                    }
                }
            }
            else if(op1->data.typ == bval) // ak je datoveho typu bool
            {
                if(op2->data.typ != bval) // a ak nieje 
                {
                    result->data.hodnota.b = FALSE;  //vrat FALSE
                    result->data.typ = bval; //vysledok bool
                }
                else
                {
                    if(op2->data.hodnota.b == op1->data.hodnota.b) //ak sa rovna  op1 op2
                    {
                        result->data.hodnota.b = TRUE; // vrat TRUE
                        result->data.typ = bval; //vysledok bool
                    }
                    else
                    {
                        result->data.hodnota.b = FALSE; //inak FALSE
                        result->data.typ=bval; //vysledok bool
                    }
                }
            }
            else if (op1->data.typ == sval && op2->data.typ ==sval) // ak mame dva retazce
            {
                if(strcmp(op1->data.hodnotaS,op2->data.hodnotaS) <= 0)  // porovnaj dva retazce 
                {                                                        // ak retazce su mensie alebo rovne
                    result->data.hodnota.b=TRUE;                         // vrat TRUE
                    result->data.typ = bval; //vysledok bool
                }
                else
                {
                    result->data.hodnota.b = FALSE; // inak FALSE
                    result->data.typ = bval; //vysledok bool
                }
            }
            else //ERoRR
            {
                error = E_SEMA;
                Destroy(Table);
                znic_navestie(jump_array);
                return;
            }
            break;

        case LESS: // ak je mensie *<*

            if(result == NULL)
            {
                break;
            }
            if(op1->data.typ == rval) // ak je typ rval
            {
                if(op2->data.typ != rval) //ak nie je vrat FALSE
                {
                    result->data.hodnota.b = FALSE; // inak FALSE
                    result->data.typ = bval; //vysledok bool
                }
                else
                {
                    if(op1->data.hodnotaD < op2->data.hodnotaD)  // ak je op1 mensi ako op2
                    {
                        result->data.hodnota.b = TRUE;  // vrat TRUE
                        result->data.typ=bval; //vysledok bool
                    }
                    else
                    {
                        result->data.hodnota.b = FALSE; //inak FALSE
                        result->data.typ=bval; //vysledok bool
                    }
                }
            }
            else if (op1->data.typ == sval && op2->data.typ == sval) // ak mame dva stringi
            {
                if(strcmp(op1->data.hodnotaS,op2->data.hodnotaS) < 0)  //stringi porovnaj 
                {                                                        // ak je mensie ako nula
                    result->data.typ = bval;                            //vrat TRUE
                    result->data.hodnota.b = TRUE;
                }
                else
                {
                    result->data.typ = bval;  //vysledok bool
                    result->data.hodnota.b = FALSE; // ak nie je vrat FALSE
                } 
            }
            else //inak ERORR
            {
                error = E_SEMA;
                Destroy(Table);
                znic_navestie(jump_array);
                return;
            }
            break;

        case EMORE: // ak je rovne alebo vacsie *>=*

            if(result == NULL)
            {
                break;
            }
            if(op1->data.typ == rval) // ak je typu rval
            {
                if(op2->data.typ != rval) // ak NIEJE typu rval
                {
                    result->data.hodnota.b = FALSE; //vrat FALSE
                    result->data.typ = bval;
                }
                else
                {
                    if(op1->data.hodnotaD >= op2->data.hodnotaD) // a ak op1 je vecsi abo rovny op2 
                    {
                        result->data.hodnota.b = TRUE; //vrat TRUE
                        result->data.typ = bval; //vysledok bool
                    }
                    else
                    {
                        result->data.hodnota.b = FALSE;     //inak FALSE
                        result->data.typ = bval; //vysledok bool
                    }
                }
            }
            else if(op1->data.typ == bval) // ak je typu BOOLEAN
            {
                if(op2->data.typ != bval) //ak nieje typu BOOLEAN
                {
                    result->data.hodnota.b = FALSE; //vrat FALSE
                    result->data.typ = bval; //vysledok bool
                }
                else
                {
                    if(op2->data.hodnota.b == op1->data.hodnota.b) // ak op1 sa rovna op2
                    {
                        result->data.hodnota.b = TRUE; //vrat TRUE
                        result->data.typ = bval; //vysledok bool
                    }
                    else
                    {
                        result->data.hodnota.b = FALSE; //inak FALSE
                        result->data.typ = bval; //vysledok bool
                    }
                }
            }
            else if (op1->data.typ == sval && op2->data.typ == sval) // ak mame dva STRINGI
            {
                if(strcmp(op1->data.hodnotaS,op2->data.hodnotaS) >= 0) // porovnaj ich
                {                                                        //ak je retazec vacsi alebo rovny 0
                    result->data.hodnota.b = TRUE;          //vrat TRUE
                    result->data.typ = bval; //vysledok bool
                }
                else
                {
                    result->data.hodnota.b = FALSE;     //inak FALSE
                    result->data.typ = bval; //vysledok bool
                }
            }
            ELESS//inak ERORR
            {
                error = E_SEMA;
                Destroy(Table);
                znic_navestie(jump_array);
                return;
            }
            break;

        case MORE: // ak je vacsie *>*

            if(result == NULL)
            {
                break;
            }
            if(op1->data.typ == rval)    //ak je typu rval
            {
                if(op2->data.typ != rval)    //ak nieje rval
                {
                    result->data.hodnota.b = FALSE; //vrat FALSE
                    result->data.typ = bval; //vysledok bool
                }
                else
                {
                    if(op1->data.hodnotaD > op2->data.hodnotaD)
                    {
                        result->data.hodnota.b = TRUE;
                        result->data.typ=bval; //vysledok bool
                    } 
                    else
                    {
                        result->data.hodnota.b = FALSE;
                        result->data.typ=bval; //vysledok bool
                    }
                }
            }
            else if (op1->data.typ == sval && op2->data.typ == sval)
            {
                if(strcmp(op1->data.hodnotaS,op2->data.hodnotaS) > 0)
                {
                    result->data.hodnota.b = TRUE;
                    result->data.typ = bval; //vysledok bool
                }
                else
                {
                    result->data.hodnota.b = FALSE;
                    result->data.typ = bval; //vysledok bool
                }

            }
            else
            {
                error = E_SEMA;
                Destroy(Table);
                znic_navestie(jump_array);
                return;
            }
            break;

            /********************************vstavané funkcie*************************************/
        case READ: // citanie

            if(op1 == NULL)
            {
                break;
            }
            if(op1->data.typ == rval)
            {
                if(result == NULL)
                {
                    return;
                }
                string text;
				        strCreate(&text);

                int orez = op1->data.hodnotaD;
                int znak = 0;
                while(orez > 0)
                {

                    znak = getchar();
                    if(result != NULL)
                    {

						strAppendChar(&text, (char) znak);

                    }
                  (orez)--;
                }
                result->data.hodnotaS=text.data;
                result->data.typ = sval;

            }
            else if (op1->data.typ == sval)
            {
                int prepinac=-1;

                if(strcmp(op1->data.hodnotaS,"*n")==0)
                {
                    prepinac = 1;
                }
                else if(strcmp(op1->data.hodnotaS,"*l")==0)
                {
                    prepinac = 2;
                }
                else if(strcmp(op1->data.hodnotaS,"*a")==0)
                {
                    prepinac = 3;
                }
                else
                {
                    error = E_SEMA;
                }

                string text;
                strCreate(&text);
                int znak = 0;
                int ret = 0; // len overenie scanf

                switch(prepinac)
                {
                case -1:
                    return;
                    break;

                case 1:
                    if(result != NULL)
                    {
                        ret = scanf("%lf",&(result->data.hodnotaD));
                        if(ret != 1) {
                            error = EINT;
                            return;
                        }
                        result->data.typ = rval;
                    }
                    break;

                case 2:
                    while(1)
                    {
                        {

                            char znak = 0;
                            znak=getchar();
                            if((znak == 10) || (znak == 13))
                            {
                                break;
                            }
                            znak=znak;
                            strAppendChar(&text, znak);


                        }
                    }
                    if(result != NULL)
                    {
                        result->data.hodnotaS = text.data;
                        result->data.typ=sval;
                    }
                    break;

                case 3:
                    while(1)
                    {
                        {
                            if((znak=getchar()) == EOF)
                            {
                                break;
                            }
                            char znak = 0;
                            znak=znak;
                            strAppendChar(&text, znak);
                        }
                    }
                    if(result != NULL)
                    {
                        result->data.hodnotaS = text.data;
                        result->data.typ = sval;
                    }

                }
            }
            else
            {
                error = E_SEMA;
                Destroy(Table);
                znic_navestie(jump_array);
                return;
            }
            break;

            case WRITE: // zapisovanie

            if(op1 == NULL)
            {
                break;
            }
            if(op1->data.typ == rval)
            {
                printf("%g", op1->data.hodnotaD);
            }
            else if(op1->data.typ == sval)
            {
                char *retezec = op1->data.hodnotaS;
                if(retezec == NULL)
                {
                    break;
                }
                for(int i=0; retezec[i] != '\0' ; i++)
                {
                    if( retezec[i] == '\\' )
                    {
                        if( retezec[i+1] == 'n' )
                        {
                            printf("\n");
                            i++;
                        }
                        else if( retezec[i+1] == 't' )
                        {
                            printf("\t");
                            i++;
                        }
                        else if( retezec[i+1] == '\\' )
                        {
                            printf("\\");
                            i++;
                        }
                        else if( retezec[i+1] == '\"' )
                        {
                            printf("\"");
                            i++;
                        }
                        else if( isdigit(retezec[i+1]) && isdigit(retezec[i+2]) && isdigit(retezec[i+3]) )
                        {
                            int ascii = atoi(&retezec[i+1]);
                            putchar(ascii);
                            i = i+3;
                        }
                    }
                    else
                    {
                        putchar(retezec[i]);
                    }
                }

            }
            else
            {
                error = EINT;
                Destroy(Table);
                znic_navestie(jump_array);
                return;
            }
            break;

        case SORT: //  druh parametrov

            if(op1 == NULL)
            {
                break;
            }
            pom=prep_quickSort(op1);
            if(result != NULL)
            {
                if(pom->data.typ == sval)
                {
                    result->data.hodnotaS=pom->data.hodnotaS;
                    result->data.typ=sval;
                }
                else if(pom->data.typ == NOP)
                {
                    result->data.typ = NOP;
                }
            }
            break;

        case SUBS:  //odčítanie 

            pom = built_substr(op1, op2, result);
            position++;
            ta_Get(ta_table, &operators, &op1, &op2, &result, position);
            if(result != NULL)
            {
                if(pom->data.typ==sval)
                {
                    result->data.hodnotaS=pom->data.hodnotaS;
                    result->data.typ=sval;
                }
                else if(pom->data.typ==NOP)
                {
                    result->data.typ=NOP;
                }
            }
            break;

        case FIND: //nájdi

            pom = find(op1,op2); // najdi operand 1 a operand 2

            if(pom->data.typ == sval) // ak je pomocna String
            {
                result->data.hodnotaS = pom->data.hodnotaS;
                result->data.typ=sval;
            }
            else if(pom->data.typ == NOP)
            {
                result->data.typ = NOP;
            }
            else if(pom->data.typ == bval)
            {
                result->data.hodnota.b = pom->data.hodnota.b;
                result->data.typ = bval;
            }
            else if(pom->data.typ == rval)
            {
                result->data.hodnotaD = pom->data.hodnotaD;
                result->data.typ = rval;
            }
            break;

        case TYPE:

            pom = built_type(op1);
            if(result!=NULL)
            {
                result->data.hodnotaS = pom->data.hodnotaS;
                result->data.typ = sval;
            }
            break;š


                void znic_navestie(int *jump_array)
              {
                  jump_array = NULL; //uvolni riadok
              }
                /*************************SKOKY A LABELY*********************************/


        case JUMP: // jump
                       
    if (tTape->active->instuction==JUMP && tTape->active->previous->result==FALSE)    // pokud aktivni instrukce obsahuje jump a predchozi vysledek je false
        
         {
          tTape->active=TapeItem->active->result->hodnota->tape_pointer;                     // tak preskoc na dalsi instrukce(preskoc if instrukce while instrukce)
         }
            else if (tTape->active->instuction == JUMP && tTape->active->previous->result == TRUE)

     if (tTape->active->instuction==JUMP && tTape->active->previous->result==FALSE)

        {
             tTape->active=tTape->active->next;                                            // pokud je vysledek true tak pokracuj v dalsi instrukci
         }
     if (tTape->active->instuction == CALL)                                             // pokud instrukce obsahuje call tak je tam funkce a skaci na jeji pozici
         {

             stackPush(&stack,tTape->active->op2->hodnota->param_pointer);                  // pushnu na globalni zasobnik parametry funkce
             tTape->active=tTape->active->result->hodnota->Table_pointer;                     // posunuse na pozadovanou pozici
         }

