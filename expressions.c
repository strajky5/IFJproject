#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <strings.h>
#include "str.h"
#include "expressions.h"
#include "list.h"

tOperand res;
bool flag; 
int reduct;
//######################### Konvertovani Tokenu ##########################
//budeme identifikovat co tam je .. jestli ID nebo realcni op., atd. a prepiseme ve strukture T.type aktualni typ tokenu
tErrors ConvertToken(tExpType *type)
{
    switch(T.type)
    {
        case T_ERRORTOKEN : return E_LEX;
        case T_ERRORSYSTEM : return E_INTERN;
        case T_KONST :  
        case T_BOOLEAN: 
        case T_INT :   
        case T_REAL :  
        case T_ID :     
        case T_STRING : *type = ID; break;
        case T_ADD :    *type = ADD_SUB; break;
        case T_SUB :    *type = ADD_SUB; break;
        case T_MUL :
        case T_DIV :    *type = MUL_DIV; break;
        case T_LB :     *type = LB; break;
        case T_RB :     *type = RB; break;
        case T_LESS :
        case T_MORE :
        case T_LESS_EQ :
        case T_MORE_EQ :
        case T_NOT_EQ :
        case T_EQUAL :     *type = REL_OP; break;
        case T_ASSIGN :    *type = ASSIGNED; break;
        case T_KEYWORD :   *type = KEYWORD; break;
        case T_SEMICOLON : *type = KEYWORD; break;
        case T_COMMA :     *type = COMMA; break;
        default :      return E_SYN;
    }
    return E_OK;
}

//########################## HLEDANI ID V BVS ###########################
tVariable *SearchDataType(tBSTNodePtr TTree, tBSTNodePtr TTreeL)
{
    tVariable *TreePtr;                                     // ukazatel TreePtr na strukturu tVariable
   
    if(TTreeL != NULL)                                      // pokud neni LBVS prazdny
    {
        if ((TreePtr = BSTSearch (TTreeL, T.s))!= NULL)     // hledej dany token v LOKALNIM BVS a pokud ho najdes
        {       
            return TreePtr;                                 // vrat ukazatele na nej
        }
    }
    if(TTree != NULL)                                       // pokud neni BVS prazdny
    {
        if ((TreePtr = BSTSearch (TTree, T.s))!= NULL)      // hledej dany token v GLOBALNIM BVS a pokud ho najdes
        {       
            return TreePtr;                                 // vrat ukazatele na nej
        }
    }
    return NULL;                                            // pokud token nenajdes v BVS, vrat NULL
}   

//################### HLEDANI A ZPRACOVANI FUNKCE #######################
tErrors SearchFun()
{
    tFunListItem *FunPtr;   // ukazatel na prvek seznamu funkce
    tVariable *pTree;       // ukazatel na strukturu tVariable
    tParamItem *param;      // ukazatel na parametr
    tErrors er;             // promenna typu tErrors pro ulozeni vysledneho error kodu
    tExpType type;          // typ prvku na zasobniku
    tVariable *temp;  
    tVariable *fun_pointer;
    tParamList *parameterList;       
    int param_counter = 0;
    int param_number = 0;
    
    parameterList = allocate(sizeof(tParamList));
    initParamlist(parameterList);

    FunPtr = searchFunList(&(T.s));            // pokud najdes funkci v seznamu funkci 
       
        param_number = FunPtr->param_count;    // ulozeni - pocet parametru funkce do param
        param = FunPtr->param_list->first;     // ulozeni ukazatele na prvni prvek v seznamu parametru funkci

        gettoken();
        er = ConvertToken(&type);   // konvertovani tokenu
        if (er)                     // pokud konvert neprobehl v poradku
            return E_SYN;           // koncim s chybou
        if (type != LB)             // pokud nenasleduje leva zavorka, E_SYN
            return E_SYN;

        while(type != RB)    // ber novy token dokud neprojdes vsechny parametry funkce         
        {
            gettoken();
            er = ConvertToken(&type);   // konvertovani tokenu
            if (er)                     // pokud konvert neprobehl v poradku
                return E_SYN;           // koncim s chybou

            if (T.type == T_ID)         // pokud je typ ID, vyhledej jej v BVS
            {
                if ((SearchDataType(TempTree, TempTreeL)) != NULL)      // pokud je token typu ID a je nalezen v BVS
                {
                    pTree = SearchDataType(TempTree, TempTreeL);        // uloz mi ukazatel na prvek do stromu
                    if (pTree->type != param->type)                     // pokud neni typ tokenu roven typu parametru funkce 
                        return E_SEMB;                                  // vraci E_SEMB
                }
                else
                {                                                    // pokud neni prvek nalezen
                    return E_SEMA;                                   // vraci E_SEMA
                }
            }
            else if (T.type == T_INT)
            {
                if (param->type != O_INT)     // pokud neni typ tokenu roven typu parametru funkce 
                        return E_SEMB;
            }
            else if (T.type == T_REAL)
            {
                if (param->type != O_REAL)     // pokud neni typ tokenu roven typu parametru funkce 
                        return E_SEMB;
            }
            else if (T.type == T_STRING)
            {
                if (param->type != O_STRING)     // pokud neni typ tokenu roven typu parametru funkce 
                        return E_SEMB;
            }
            else if (T.type == T_KONST)
            {
                if (param->type != O_BOOL)     // pokud neni typ tokenu roven typu parametru funkce 
                        return E_SEMB;
            }
            else                    // pokud neni parametr ID
            {
                return E_SYN;       // vraci chybu E_SYN
            }

            if (insertParam(parameterList, &(T.s), T.type) != E_OK) return E_INTERN; 

            gettoken();
            er = ConvertToken(&type);   // konvertovani tokenu
            if (er)                     // pokud konvert neprobehl v poradku
                return E_SYN;           // koncim s chybou
            
            param_counter++;    // pocitadlo paramateru

            if (((param_counter == param_number) && (type != RB)) && (type != COMMA))  // pokud neni za ID carka nebo za posledni parametrem neni prava zavorka
                return E_SYN;           // vrat chybu E_SYN
            
            if (param_counter > param_number)   // pokud pocitadlo parametru je vetsi jako hodnota, ktera je udana u funkce v seznamu
                return E_SEMB;                  // vrat chybu E_SEMB
            if (param->next != NULL)
                param = param->next;                // posunuti na dalsi parametr funkce ???????????????????????
        }

        if (param_number != param_counter)
            return E_SEMB;

        if ((temp = allocate(sizeof(tVariable))) == NULL)    //pokud neni dostatek pameti => E_INTERN
            return E_INTERN;
        if (strInit(&(temp->name)) == STR_ERROR)            //pokud funkce init. stringu vrati chybu => E_INTERN
            return E_INTERN; 
        
        if ((fun_pointer = allocate(sizeof(tVariable))) == NULL)    //pokud neni dostatek pameti => E_INTERN
            return E_INTERN;
        if (strInit(&(fun_pointer->name)) == STR_ERROR)            //pokud funkce init. stringu vrati chybu => E_INTERN
            return E_INTERN; 

        res = FunPtr->ret_type;
        
        temp->type = FUNCTION;          // ulozeni do instrukce resultu ze je to funkce
        temp->name = FunPtr->name;
        //temp->value.tape_pointer = FunPtr->tape_ptr;   // vlozeni do value ukazatele, kde je funkce na pasce 
        temp->value.param_pointer = parameterList->first;

        Tape->last->op1 = temp;        // ulozeni ukazatele na tVariable polozku, ktera obsahuje ukazatel na funkci na pasce 
        Tape->last->instruction = FUNCTION;   // instrukce je funkce
        Tape->last->op2 = NULL;
        Tape->last->result = NULL; 

        return E_OK;                      // pokud to najde funkci E_OK
}

//###################### SEMANTICKA KONTROLA #############################
tErrors ExpParserSem(tOperand *tOp1, tOperand *tOp2, tInstruction *tInstr)
{
    if (flag == TRUE) // ############## KDYZ JE ASSIGN ###################
    {
        //printf(":= Op1: %d Instrukce:%d Op2: %d \n",*tOp1,*tInstr,*tOp2);
        if (((*tOp1) == O_INT) && ((*tOp2) ==  O_INT))          // INT | INT
        { 
            if ((*tInstr == ADD) || (*tInstr == SUB) || (*tInstr == MUL) || (*tInstr == DIV))
            {
                res = O_INT;
            }
            else
            {
                res = O_BOOL;
            }

            return E_OK;
        }
        else if (((*tOp1) == O_REAL) && ((*tOp2) ==  O_REAL))   // REAL | REAL
        {
            if ((*tInstr == ADD) || (*tInstr == SUB) || (*tInstr == MUL) || (*tInstr == DIV))
            {
                res = O_REAL;
            }
            else
            {
                res = O_BOOL;
            }
            
           return E_OK;
        }
        else if (((*tOp1) == O_INT) && ((*tOp2) ==  O_REAL))    // INT | REAL
        {
            if ((*tInstr == ADD) || (*tInstr == SUB) || (*tInstr == MUL) || (*tInstr == DIV))
            {
                res = O_REAL;
            }
            else
            {
                return E_SEMB;
            }
            return E_OK;
        }
        else if (((*tOp1) == O_REAL) && ((*tOp2) ==  O_INT))    // REAL | INT
        {
            if ((*tInstr == ADD) || (*tInstr == SUB) || (*tInstr == MUL) || (*tInstr == DIV))
            {
                res = O_REAL;
            }
            else
            {
                return E_SEMB;
            }
            return E_OK;
        }
        else if (((*tOp1) == O_STRING) && ((*tOp2) ==  O_STRING))  // STRING | STRING
        {
             if ((*tInstr == SUB) || (*tInstr == MUL) || (*tInstr == DIV))
            {
                return E_SEMB;
            }
            else if (*tInstr == ADD) 
            {
                res = O_STRING;
            }
            else 
            {
                res = O_BOOL;
            }
            return E_OK;
        }
        else if (((*tOp1) == O_BOOL) && ((*tOp2) ==  O_BOOL))   // BOOL | BOOL
        {
            if ((*tInstr == ADD) || (*tInstr == SUB) || (*tInstr == MUL) || (*tInstr == DIV))
            {
                return E_SEMB;
            }
             res = O_BOOL;
            return E_OK;
        }
        else
        {   
            return E_SEMB;
        }
    }
    else // ################# KDYZ JE KEYWORD => IF, WHILE ###############
    {
        //printf("IF|WHILE OP1:%d INST:%d OP2:%d \n",*tOp1,*tInstr,*tOp2);
        if (((*tOp1) == O_INT) && ((*tOp2) ==  O_INT))          // INT | INT
        { 
            if ((*tInstr == ADD) || (*tInstr == SUB) || (*tInstr == MUL) || (*tInstr == DIV))
            {
                res = O_INT;
            }
            else
            {
                res = O_BOOL;
            }
            return E_OK;
        }
        else if (((*tOp1) == O_REAL) && ((*tOp2) ==  O_REAL))   // REAL | REAL
        {
            if ((*tInstr == ADD) || (*tInstr == SUB) || (*tInstr == MUL) || (*tInstr == DIV))
            {
                res = O_REAL;
            }
            else
            {
                res = O_BOOL;
            }
            return E_OK;
        }
        else if (((*tOp1) == O_INT) && ((*tOp2) ==  O_REAL))    // INT | REAL
        {
            if ((*tInstr == ADD) || (*tInstr == SUB) || (*tInstr == MUL) || (*tInstr == DIV))
            {
                res = O_REAL;
            }
            else
            {
                return E_SEMB;
            }
            return E_OK;
        }
        else if (((*tOp1) == O_REAL) && ((*tOp2) ==  O_INT))    // REAL | INT
        {
            if ((*tInstr == ADD) || (*tInstr == SUB) || (*tInstr == MUL) || (*tInstr == DIV))
            {
                res = O_REAL;
            }
            else
            {
                return E_SEMB;
            }
            return E_OK;
        }
        else if (((*tOp1) == O_STRING) && ((*tOp2) ==  O_STRING))   // STRING | STRING
        {
            if ((*tInstr == SUB) || (*tInstr == MUL) || (*tInstr == DIV))
            {
                return E_SEMB;
            }
            else if (*tInstr == ADD) 
            {
                res = O_STRING;
            }
            else 
            {
                res = O_BOOL;
            }
            return E_OK;
        }
        else if (((*tOp1) == O_BOOL) && ((*tOp2) ==  O_BOOL))   // BOOL | BOOL
        {
            if ((*tInstr == ADD) || (*tInstr == SUB) || (*tInstr == MUL) || (*tInstr == DIV))
            {
                return E_SEMB;
            }
            res = O_BOOL;
            return E_OK;
        }
        else if (((*tOp1) == O_BOOL) && ((*tOp2) ==  O_NOTDATA))   // BOOL | BOOL
        {
            res = O_BOOL;
            return E_OK;
        }
        else
        {   
            return E_SEMB;
        }
    }
    return E_OK;
}


//#######################################################################
//############################### ZASOBNIK ##############################
//###################### Inicializace zasobniku #########################
void ExpStackInit (tExpStack *L)
{
   
    L->Top = NULL;      // inicializace vrcholu zasobniku
}
//##################### Zruseni celeho zasobniku ########################
void ExpStackDispose (tExpStack *L)
{
    tElemPtr temp = L->Top;					//uloz prvni prvek zasobniku do temp
    while (temp)							//dokud je temp TRUE delej
    {
        L->Top = L->Top->rptr;				//uloz ukazatel prvku na dalsi prvek
        free(temp);							//uvolni prvek
        temp = L->Top;						//uloz prvni prvek zasobniku do temp
    }
}
//############### Vlozeni prvku na zacatek zasobniku ####################
tErrors ExpStackPush (tExpStack *L, tExpType val, tVariable *item)
{
	tElemPtr temp;					//nova promenna typu tElemPtr

    if ((temp = malloc(sizeof(struct tElem))) == NULL)	//pokud neni dostatek pameti => E_INTERN
        return E_INTERN;

    if ((val == LB) || (val == RB) || (val == DOLAR))	//kdyz davam na zasobnik '(' nebo ')' nebo '$'
    {
        temp->data = val; 			//do data vlozim  interni typ precedencni tabulky
        temp->rptr = L->Top;		//ukazatel bud ukazovat na vrchol zasobniku
        L->Top = temp;				//ulozeni prvku do ukazatele na vrchol zasobniku
        return E_OK;
    }
    else if (val == NONTERM)		//kdyz davam na zasobnik neterminal
    {
        if ((temp->tempVarPtr = allocate(sizeof(tVariable))) == NULL)	//pokud neni dostatek pameti => E_INTERN
        	return E_INTERN;
        if (strInit(&(temp->tempVarPtr->name)) == STR_ERROR)			//pokud funkce init. stringu vrati chybu => E_INTERN
        	return E_INTERN; 

        temp->tempVarPtr->type = res; 		
        temp->data = val;						//do data vlozim  interni typ precedencni tabulky
        temp->rptr = L->Top;					//ukazatel bud ukazovat na vrchol zasobniku
        L->Top = temp;							//ulozeni prvku do ukazatele na vrchol zasobniku
        return E_OK;
    }
    else
    {
        switch(T.type)			
        { //###################### INSTRUKCE ###################
        	case T_ADD :     temp->instruction = ADD; break;					
            case T_SUB :     temp->instruction = SUB; break;
            case T_MUL :     temp->instruction = MUL; break;
            case T_DIV :     temp->instruction = DIV; break;
            case T_LESS :    temp->instruction = LESS; break;
            case T_MORE :    temp->instruction = MORE; break;
            case T_LESS_EQ : temp->instruction = EQL; break;
            case T_MORE_EQ : temp->instruction = EQM; break;
            case T_NOT_EQ :  temp->instruction = NEQ; break;
            case T_EQUAL :   temp->instruction = EQ; break;

         //####################### OPERANDY ####################
            case T_INT :			// push na stack token typu INT, alokace  mistro pro ulozeni jmena, typu a hodnoty tokenu
                if ((temp->tempVarPtr = allocate(sizeof(tVariable))) == NULL)	//pokud neni dostatek pameti => E_INTERN
                    return E_INTERN;
                if (strInit(&(temp->tempVarPtr->name)) == STR_ERROR)			//pokud funkce init. stringu vrati chybu => E_INTERN
                    return E_INTERN;
                temp->tempVarPtr->type = O_INT;									//operand typu O_INT
                temp->tempVarPtr->value.ival = strtol(T.s.str,NULL, 10); 
                break;

            case T_REAL :			// push na stack token typu REAL, alokace  mistro pro ulozeni jmena, typu a hodnoty tokenu
                if ((temp->tempVarPtr = allocate(sizeof(tVariable))) == NULL)	//pokud neni dostatek pameti => E_INTERN
                    return E_INTERN;
                if (strInit(&(temp->tempVarPtr->name)) == STR_ERROR)			//pokud funkce init. stringu vrati chybu => E_INTERN
                    return E_INTERN;
                temp->tempVarPtr->type = O_REAL;								//operand typu O_REAL
                temp->tempVarPtr->value.rval = strtod(T.s.str, NULL);	
                break;
        																			
            case T_STRING :			// push na stack token typu STRING, alokace  mistro pro ulozeni jmena, typu a hodnoty tokenu
                if ((temp->tempVarPtr = allocate(sizeof(tVariable))) == NULL)	//pokud neni dostatek pameti => E_INTERN
                    return E_INTERN;
                if (strInit(&(temp->tempVarPtr->name)) == STR_ERROR)			//pokud funkce init. stringu vrati chybu => E_INTERN
                    return E_INTERN;
                temp->tempVarPtr->type = O_STRING;
                if (strInit(&(temp->tempVarPtr->value.sval)) == STR_ERROR)    //pokud funkce init. stringu vrati chybu => E_INTERN
                    return E_INTERN;
                if (strCopystring(&(temp->tempVarPtr->value.sval), &(T.s)) == STR_ERROR)
                    return E_INTERN;
                break;

            case T_KONST :			// push na stack token typu KONST - BOOL, alokace  mistro pro ulozeni jmena, typu a hodnoty tokenu											//konstanty ukladam jako true nebo false
                if ((temp->tempVarPtr = allocate(sizeof(tVariable))) == NULL)	//pokud neni dostatek pameti => E_INTERN
                    return E_INTERN;
                if (strInit(&(temp->tempVarPtr->name)) == STR_ERROR)			//pokud funkce init. stringu vrati chybu => E_INTERN
                    return E_INTERN;
                temp->tempVarPtr->type = O_BOOL;								//operand typu O_BOOL - boolean
                if (!strCmpConstStr(&(T.s), "true"))							//pokud hodnota tokenu true
                    temp->tempVarPtr->value.bval = true;						// uloz hodnotu operandu jako true
                else if (!strCmpConstStr(&(T.s), "false"))                     //pokud hodnota tokenu je false
                    temp->tempVarPtr->value.bval = false;                     //uloz hodnotu operandu jako false
                else
                    return E_SYN;                                             //vraci error E_SYN
                break;
           default : break;
        }
    }
    temp->data = val; 				// do data vlozim  interni typ precedencni tabulky
    temp->rptr = L->Top;			// ukazatel bud ukazovat na vrchol zasobniku
    if(T.type == T_ID)              // pokud typ tokenu je ID (nejaka promenna) 
        temp->tempVarPtr = item;    // uloz mi ukazatel na promennou do stromu LBVS/GBVS
    L->Top = temp;					// ulozeni prvku do ukazatele na vrchol zasobniku

    return E_OK;
}
//############# Vraci hodnotu prvniho prvku zasobniku ###################
void ExpStackTop (tExpStack *L, tExpType *val)
{
    if (L->Top)						//pokud neni zasobnik prazdny
        *val = L->Top->data;		//uloz do *val hodnotu prvniho prvku na zasobniku
}
//############# Vraci hodnotu druheho prvku zasobniku ###################
void ExpStackSecTop (tExpStack *L, tExpType *val)
{
    if (L->Top)						//pokud neni zasobnik prazdny
        *val = L->Top->rptr->data;	//uloz do *val hodnotu druheho prvku na zasobniku
}
//############# Vraci ukazatel prvniho prvku zasobniku ########### OP1 ##
void ExpStackTopPoint (tExpStack *L, tElemPtr *val)
{
    if (L->Top)               //pokud neni zasobnik prazdny
        *val = L->Top;        //uloz do *val hodnotu prvniho prvku na zasobniku
}
//############# Vraci ukazatel tretiho prvku zasobniku ########### OP2 ##
void ExpStackSecTopPoint (tExpStack *L, tElemPtr *val)
{
    if (L->Top)                     //pokud neni zasobnik prazdny
        *val = L->Top->rptr->rptr;  //uloz do *val hodnotu druheho prvku na zasobniku
}
//############# Vraci hodnotu druheho prvku zasobniku ###### INSTRUKCE ##
void ExpStackInstructionTopPoint (tExpStack *L, tElemPtr *val)
{
    if (L->Top)               //pokud neni zasobnik prazdny
        *val = L->Top->rptr;  //uloz do *val hodnotu druheho prvku na zasobniku
}
//############# Hleda prvni terminal, hodnotu ulozi do val ##############
void ExpStackTopTerminal (tExpStack *L, tExpType *val)
{   
    tElemPtr temp = L->Top;			//nova promenna typu tElemPtr, ktera bude reprezentovat prvni prvek na zasobniku
    if (!temp)						//pokud je zasobnik prazdny - konec
        return;
    while ((temp->data == NONTERM)) //cykli dokud nenarazis na neterminal
        temp = temp->rptr;			//uloz do temp druhy prvek zasobniku
    *val = temp->data;				//uloz do *val data prvku
}
//################ Vymaze prvek na vrcholu zasobniku ####################
void ExpStackPop (tExpStack *L)
{
	tElemPtr temp = L->Top;			//nova promenna typu tElemPtr, ktera bude reprezentovat prvni prvek na zasobniku
    if (!temp)						//pokud je zasobnik prazdny - konec
        return;
    L->Top = L->Top->rptr;			//uloz do temp nasledujici prvek zasobniku
    free(temp);						//vymaz prvek co byl na vrcholu zasobniku
}
//############# Vymaze druhy prvek na vrcholu zasobniku #################
void ExpStackSecPop (tExpStack *L)
{
	tElemPtr temp = L->Top;				//nova promenna typu tElemPtr, ktera bude reprezentovat prvni prvek na zasobniku
	temp = temp->rptr;					//do tempu ulozi druhy prvek na zasobniku
    if (!temp)							//pokud je druhy prvek neexistuje - konec
        return;
    L->Top->rptr = L->Top->rptr->rptr;	//provaz prvni prvek se tretim prvkem
    free(temp);							//vymaz prvek co byl na vrcholu zasobniku
}
//######################## Redukce zasobniku ############################
tErrors ExpStackReduct(tExpStack *S, tTabSigns sign)
{
	tExpType toptype = -1;			//promenna typu tExptype a  hodnotou -1 pro uchovani typu tokenu
    tElemPtr TopOp;                 //zpracovavany OP2
    tElemPtr SecTopOp;              //zpracovavany OP1 
    tElemPtr TopIstruction;         //provadena instrukce mezi OP1|OP2
    tErrors er;                     //promenna typu tErrors


    ExpStackTop(S, &toptype);		//vraci hodnotu prvniho prvku zasobniku
 
    if (toptype == ID) 	//vstup=ID, pravidlo E->ID
    {
        S->Top->data = NONTERM; 	//zmena interniho typu na NONTERM podle pravidla
        return E_OK;
    }

    if (sign == Q)     //vstup = ')', pravidlo E->(E) (ale ')' nemam na zasobniku)
    {  
	reduct =1;
        if (toptype != NONTERM)      //kdyz neni na vrcholu zasobnika NONTERM
            return E_SYN;            //vraci error E_SYN
        ExpStackSecTop(S, &toptype); //vraci hodnotu druheho prvku

        if (toptype != LB)      	 //pokud je na druhem miste v zasobniku '('
            return E_SYN;            //vraci error E_SYN
        ExpStackSecPop(S);           //odstranim zavorku
        return E_OK; 
    }
    else			   //ostatni pravidla E->EopE (pr. E->E+E, E->E-E,...)
    {
        if (toptype != NONTERM)     //kdyz neni na vrcholu zasobnika neterminal
            return E_SYN;           //vraci error E_SYN
        else 						//kdyz je na zasobniku nonterm
        {  
            ExpStackTopPoint (S, &TopOp);                       // druhy operand OP2 (prvni prvek na STACKU)
			ExpStackInstructionTopPoint (S, &TopIstruction);    // instrukce (druhy prvek na STACKU)
            if (TopIstruction->data != DOLAR)
            {    //printf("tady nemam co delat \n");
			printf(" delat \n");
                ExpStackSecTopPoint (S, &SecTopOp);                 // prvni operand OP1 (treti prvek na STACKU)
           reduct =1;
                //printf("OP2: %d OP1:%d \n",TopOp->tempVarPtr->type,SecTopOp->tempVarPtr->type);
                er = ExpParserSem(&(TopOp->tempVarPtr->type),&(SecTopOp->tempVarPtr->type),&(TopIstruction->instruction)); // aplikuju semanticka pravidla
                if (er == E_SEMB)       //pokud je semantika vyrazu nespravana
                {
                    return er;          //koncim s chybou
                }
				Tape->last->op2 = S->Top->tempVarPtr;
                ExpStackPop(S);         
                	
            }
            else
            {
			printf("co %d \n",reduct);
               // printf("OP2: %d INST:%d \n",TopOp->tempVarPtr->type,TopIstruction->data);
                if (TopOp->tempVarPtr->type != O_BOOL)
                    return E_SEMB;
					printf("co %d \n",reduct);
                if (reduct != 1)
                { 
                    Tape->last->op1 = S->Top->tempVarPtr;
                    
                    if ((temp = allocate(sizeof(tVariable))) == NULL)    //pokud neni dostatek pameti => E_INTERN
                        return E_INTERN;
                    if (strInit(&(temp->name)) == STR_ERROR)            //pokud funkce init. stringu vrati chybu => E_INTERN
                        return E_INTERN;
                    temp->value->bval = TRUE;
                    temp->type->O_BOOL;
                    Tape->instruction = MORE;
                    Tape->last->op2 = temp;
				
				    er = InsertEmptyItemTape();        //vkladam novy prazdny prvek na pasku
                    if (er == E_INTERN)
			            return er;
				}
				ExpStackPop(S);
                return E_OK;
            }
        }


        ExpStackTop(S, &toptype);	 //vraci hodnotu prvniho prvku
        if (toptype == NONTERM)      //kdyz je na vrcholu zasobnika neterminal
             return E_SYN;           //vraci error E_SYN
        else 						 //pokud tam neni neterminal
        {
		printf("tady  \n");
            Tape->last->instruction = S->Top->instruction; 
            ExpStackPop (S);		 //odstranim dany operand
        }

        ExpStackTop(S, &toptype);	 //vraci hodnotu prvniho prvku
        if (toptype != NONTERM)      //kdyz neni na vrcholu zasobnika neterminal
             return E_SYN;           //vraci error E_SYN
        else
        {
		printf(" nemam  \n");
            Tape->last->op1 = S->Top->tempVarPtr; 
            ExpStackPop (S);		 //odstranim dany operand
        }
		printf("kolko krat? \n");
        ExpStackPush(S, NONTERM, NULL);    //vlozim zpatky na zasobnik novy neterminal
        Tape->last->result = S->Top->tempVarPtr; // ukladam vysledek OP1 a OP2 do result na pasku	
        er = InsertEmptyItemTape();        //vkladam novy prazdny prvek na pasku
        if (er == E_INTERN)
            return er;
    }
    return E_OK;                    //koncim s redukci -> vse OK
}


//########################## PRECEDENCNI TABULKA ########################
//M - MORE - VICE; L - LESS - MENE; Q - EQUAL - ROVNO; E - ERROR;
const tTabSigns PTable [TABSIZE][TABSIZE] =
{   
//							    <,<>
//						* 	+   <=,=
//						/	-   >=,>  (   )   id 	$  KEYWORD
/*      * /       */{	M,  M,   M,   L,  M,  L,	M, 	  M    },
/*      + -       */{	L,  M,   M,   L,  M,  L,	M, 	  M    },
/* < > <= >= <> = */{	L,  L,   M,   L,  M,  L,	M, 	  M    },
/*      (      	  */{	L,  L,   L,   L,  Q,  L,	M, 	  M    },
/*      )      	  */{	M,  M,   M,   E,  M,  E,	M, 	  M    },
/*      id     	  */{	M,  M,   M,   E,  M,  E,	M, 	  M    },
/*		$ 		  */{	L,  L,   L,   L,  E,  L,    E, 	  M    },
/*    KEYWORD     */{   L,  L,   L,   L,  L,  L,    L,    E    }
};
//################### Vraci index z precedencni tabulky #################
//Vraci index z precedencni tabulky na zaklade typu z predchazejiciho tokenu a typu terminalu, ktery je na vrcholu zasobniku
tTabSigns SearchSign(tExpType row, tExpType col)
{
    return PTable [row][col];
}


//#######################################################################
//############################# HLAVNI FUNKCE ###########################
int ExpParser()
{
	tExpType type = -1;        //promenna typu tExptype pro uchovani typu tokenu
    tExpType stack_type = 6;   //promenna typu tExptype pro uchovani typu prveho terminalu na zasobniku
    tErrors er;     		   //promenna typu tErrors pro ulozeni vysledneho error kodu
    tTabSigns sign = 0;        //muze byt '<','>','==', error nebo konec v pripade prvni nesparovane zavorce ')'
    tExpStack S;               //promenna S typu zasobnik
    tVariable *pointerTemp;    //ukazatel na strukturu tVariable
    tFunListItem *FunPtr;      // ukazatel na prvek seznamu funkce          

    ExpStackInit(&S);       	     //inicializace zasobniku
    ExpStackPush(&S, DOLAR,NULL);    //push na zasobnik koncovy znak '$'

    er = ConvertToken(&type); 	//konvertovani tokenu
    if (er) 					//pokud konvert neprobehl v poradku
    {
        ExpStackDispose(&S);	//rusim cely zasobnik
        return er;				//koncim s chybou
    }

    //#################### := | IF | WHILE #########################
    if ((type != KEYWORD) && (type != ASSIGNED))  //pokud neni prvni token := nebo IF/WHILE
   		return E_SYN;                           //vrat chybu
    if (type == ASSIGNED)         //pokud zpracovavame prirazeni :=
    {    //printf("je to ASSSIGNED \n");
        flag = TRUE;  }          //inicialuzj mi flagu na TRUE
    if (type == KEYWORD)        //pokud zpracovavame IF nebo WHILE
        {    //printf("je to IF,WHILE \n");
        flag = FALSE;      }     //inicializuj mi flagu na FALSE
    //##############################################################

    gettoken();                 //volam funkci pro dalsi token

    er = ConvertToken(&type);   //konvertovani tokenu
    if (er)                     //pokud konvert neprobehl v poradku
    {
        ExpStackDispose(&S);    //rusim cely zasobnik
        return er;              //koncim s chybou
    }

    er = InsertEmptyItemTape();     // vlozeni prazdneho prvku na instrukcni pasku, aby jsme do nej mohli nasledovne plnit urcite hodnoty
    if (er == E_INTERN)             // pokud se nepodari vlozeni
        return er;                  // konci s chybou

    //################### ZPRACOVANI FUNKCI ########################
    if((type == KEYWORD) && ((FunPtr = searchFunList(&(T.s))) != NULL))     // podminka pro vestavene funkce, pokud prijde token typu KEYWORD a nalezneme jej v seznamu funkci
        type = ID;                                                          // pretypuj mi tento token na ID abych s nim mohl pracovat jako s funkci

    if ((type != ID) && (type != LB)) // pokud neni dalsi token (po := nebo IF/WHILE) neni ID nebo LB 
        return E_SYN;                // vrat chybu E_SYN
    
    if (T.type == T_ID)               // pokud je typ tokenu ID              
    {  
        if ((FunPtr = searchFunList(&(T.s))) != NULL)
        { 
            er = SearchFun();         // hledej ID v seznamu funkci a pokud ji tam najdes, zpracuj funkci
            if (er)                   // pokud funkce byla nalezena, vrat chybu kterou vratila funkce SearchFun
            {   
                ExpStackDispose(&S);  //rusim cely zasobnik                     
                return er;
            }
        }
    }
    //##############################################################

    ExpStackTopTerminal(&S, &stack_type);		//hledam na zasobniku prvni terminal a uloz jeho typ do stack_type
	sign = L;                   //ze zacatku uloz prednost zpracovani na L, protoze mame na stacu $

    //################ HLAVNI CYLUS PRO ZPRACOVANI VYRAZU ################### 
    do
    {  
		if (T.type == T_ID)           // pokud je typ tokenu ID              
        {   
            if((SearchDataType(TempTree, TempTreeL)) != NULL)       // hledej ID v LBVS a GBVS a pokud jej naleznes
            {
                pointerTemp = SearchDataType(TempTree, TempTreeL);  // uloz mi ukazatel do BVS na dany prvek 
            }
            else                    // pokud je token ID a nebyl nalezen ani v seznamu a ani v jednom BVS, neni deklarovany a je to E_SEMA
            {
               return E_SEMA;       // vrat chybu E_SEMA
            }
        }
        else                        // pokud token neni ID, pointer inicializuj na NULL
        {
            pointerTemp = NULL;      
        }

        switch(sign)				// switch zpracovavajici zasobnik pdle precedenci tokenu 
        {
            case L :    		    //LESS - nizsi priorita
                		er = ExpStackPush(&S, type, pointerTemp);	//push na zasobnik
                        if (er) 						            //pokud push neprobehl v poradku
                 		{
                     		ExpStackDispose(&S);		            //rusim cely zasobnik
                            return er;					            //koncim s chybou
                 		}
                 		gettoken();						            //volam funkci pro dalsi token
                 		break;
            case M :   				//MORE - vyssi priorita
                 		er = ExpStackReduct(&S, sign);	//redukuju zasobnik
                 		if (er) 						//pokud redukce neprobehla v poradku
                 		{
                 		    ExpStackDispose(&S);		//rusim cely zasobnik
                            return er;					//koncim s chybou
                 		}

                 		break;
            case Q :         		//EQUAL - rovna se pri zavorkach
                 		er = ExpStackReduct(&S, sign);	//redukuju zasobnik
                		 if (er)						//pokud redukce neprobehla v poradku
                 		{
                 		    ExpStackDispose(&S);		//rusim cely zasobnik
                            return er;					//koncim s chybou
                		}
                		gettoken();						//volam funkci pro dalsi token
                		break;
            case E :    								//ERROR - chyba
                        return E_SYN;					//konci s chybou E_SYN
        }

        er = ConvertToken(&type);   //konvertovani nasledujiciho tokenu
        if (er)                     //pokud konvert neprobehl v poradku
        {
            ExpStackDispose(&S);    //rusim cely zasobnik
            return er;              //koncim s chybou
        }

        ExpStackTopTerminal(&S, &stack_type);           //hledam na zasobniku prvni terminal a uloz jeho typ do stack_type
        sign = SearchSign(stack_type, type);            //zjistim sign aktualniho prvniho prvku na zasobniku

        if ((type == KEYWORD) && (stack_type == DOLAR)) // pokud je typ KEYWORD nebo DOLAR
        {   
            if(flag == FALSE)
            {   printf("%d",sign);
                er = ExpStackReduct(&S, sign);
                if (er)                         //pokud redukce neprobehla v poradku
                {
                    ExpStackDispose(&S);        //rusim cely zasobnik
                    return er;                  //koncim s chybou
                }
                res = O_BOOL;
            }
        } 
        
        
        

    } while (type != KEYWORD || stack_type != DOLAR);   //zkonci jakmile narazime na KEYWORD nebo na DOLAR, ktery je na spodu zasobniku
    //################################################################## 

    ExpStackDispose(&S);			//zrus cely zasobnik

    return E_OK;					//vrat vse OK (syntaxe i semnatika v poradku)
	}