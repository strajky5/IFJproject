
#include <stdbool.h>
#include <string.h>
#include <stdio.h>
#include "interpret.h"
#include "ial.h"
#include "strings.h"
#include "types.h"
#include "errors.h"

extern tRv_stack RV;
extern tTape *Tape;
extern tFunTable funTable;


/*=======================================RV ZASOBNIK===================================*/
void SInitRe (tRv_stack *S)  																				//inicializace zasobniku
{
    S->top = NULL;  																						// vrchol je prazdny
}   

tErrors SPushRe (tRv_stack *S, tVariable* var)																// funkce na pushnuti na zasobnik
{ 	    
	tRetval* temp;																							// pomocna funkce
	if ((temp = malloc (sizeof(tRetval))) == NULL) return E_INTERN;       									// alokuji si prvek zasobniku
    if ((temp->retval_data = malloc (sizeof(tVariable))) == NULL) return E_INTERN;   						// alokuji si prvek pro ulozeni informaci o tokenu ID(INT,REAL,STRING,BOOL,...)
    if(StringGetLenght(&(var)->name) != 0){																	// pokud je nazev prvku vetsi nez 0 
	  	if (StringInit(&(temp)->retval_data->name))  return E_INTERN;    									// tak inicializuj v zasobniku prvek (name)
	    if (StringCpy(&(temp)->retval_data->name, &(var)->name)) return E_INTERN;							// a nahraj do zasobniku prvek var (name)
	}
        temp->retval_data->type = var->type;																// do prvku na zasobniku nahraj typ
        if(temp->retval_data->type == O_BOOL)																// nastaveni prvku podle typu kdyz be bool
            temp->retval_data->value.bval = var->value.bval;												// tak nahrej do valuje->bool value = var...
        if(temp->retval_data->type == O_INT)																// kdyz je int
            temp->retval_data->value.ival = var->value.ival;												// tak do structury value nahraj ival=var...
        if(temp->retval_data->type == O_DOUBLE)																// real --||--
            temp->retval_data->value.dval = var->value.dval;
        if(temp->retval_data->type == O_STRING)																// kdyz je string tak nahraje string do value
        {
            if (StringInit(&(temp->retval_data->value.sval)) == STR_ERR) return E_INTERN;					
            if (StringCpy(&(temp->retval_data->value.sval), &var->value.sval) == STR_ERR) return E_INTERN;                
        }

    temp->rptr=S->top;																						// do predchoziho nahraje vrchol
    S->top=temp;																							// a oznaci prvek jako vrchol
    return E_OK;
}   

tVariable* STopRe (tRv_stack *S)																			// funkce na cteni prvku na vrcholu zasobniku
{
    if(S->top) return((S->top->retval_data));																// kdyz je top ruzny od null vrati data
    else return NULL;  																						// jinka vrati null
    	   
}   

void SPopRe(tRv_stack *S){																					// 
	if(S->top){																								// kdyz neni zasobnik prazdny
		tRetval* temp;																						// vytvorim si pom ukazatel
    	temp=S->top;																						// do neho nahraji vrchol
    	S->top=S->top->rptr;																				// a posunu ukazatel vrcholu o jedno dolu
    	free(temp->retval_data);																			// uvolnim data
    	free(temp);																							// a uvolnim prvek zasobnika
	}
	else
		return;																								// pokud je prazdny vraci se zpet
}

void SDisposeRe(tRv_stack *S){																				// funkce na uvolneni celeho zasobnika
	tRetval* temp;																							//	vytvoreni pomocne promene
	while(S->top){																							// dokud neni zasobnik prazdny tak 
    	temp=S->top;																						// nahrej do pomocne vrchol
    	S->top=S->top->rptr;																				// vrchol posun o jeden prvek zpet
    	free(temp->retval_data);																			// uvolni data
    	free(temp);																							// uvolni ukazatel temp
	}
	return;				
}
/*================================KONIEC RV ZASOBNIK=================================================*/	


/*Funkcia, ktora spracuje jeden parameter a vrati ho cez parameter tmp_param. Pouzita pri volani vstavanych funckii*/
tErrors internfun_param_chceck(tBSTNodePtr* RootPtr,tParamItem *param,tVariable* tmp_param){				// funkce vraci samostatny prvek tparamitem s naplnenimi hodnoty
	if(param == NULL) return E_SEMB;																		// pokud je param prazdny vrat Error 
	tVariable* tmp_ptr;																						// vytvorim pomocnou promenou
	if(StringGetLenght(&(param)->name) != 0){        														// pokud je delka stringu vetsi jak 0
		if((tmp_ptr=BSTSearch(*RootPtr,param->name)) == NULL) return E_SEMC;								// prohledaj strom (koreny)jestly tam je hledany name a vrat ukazatel na neho
		tmp_param->type = tmp_ptr->type;																	// do struktury nahraj typ so je ve stromu
        if(tmp_ptr->type == O_BOOL)																			// pokud je bool...
            tmp_param->value.bval = tmp_ptr->value.bval;
        if(tmp_ptr->type == O_INT)
            tmp_param->value.ival = tmp_ptr->value.ival;
        if(tmp_ptr->type == O_DOUBLE)
            tmp_param->value.dval = tmp_ptr->value.dval;
        if(tmp_ptr->type == O_STRING)
        {
        	if (StringInit(&(tmp_param->value.sval)) == STR_ERR) return E_INTERN;
       		if (StringCpy(&(tmp_param->value.sval), &(tmp_ptr)->value.sval) == STR_ERR) return E_INTERN;
        }
	}
	else{        																							// nebo kdyz nemam zadne jmeno	tak nehledam ve stromu a proste zkopiruji na ukaz
        tmp_param->type = param->type;																		// nahraj param typ na  ukazatel parametru 
        if(param->type == O_BOOL)																			// pokud je bool tak nahraj do valuje to co je na parametru
            tmp_param->value.bval = param->val.bval;														// ....
        if(param->type == O_INT)
            tmp_param->value.ival = param->val.ival;
        if(param->type == O_DOUBLE)
            tmp_param->value.dval = param->val.dval;
        if(param->type == O_STRING)
        { 
        	if (StringInit(&(tmp_param->value.sval)) == STR_ERR) return E_INTERN;
       		if (StringCpy(&(tmp_param->value.sval), &param->val.sval) == STR_ERR) return E_INTERN;
        }		

	}
	return E_OK;										
}

/*Funkcia, ktora spracuje parametre uzivatelom definovanej funckie a vlozi ich do param listu danej funkcie vo funtable*/
tErrors fun_param_check(tBSTNodePtr* RootPtr,tParamItem *declared_param,tParamItem *called_param){							// funkce spracuje parametry funkce a ulozije do tabulky
	tVariable* tmp_ptr;																										// vytvorim si pomocnou promenou
	while(declared_param){																									// cykly dokud nedojdes na konec
		if(called_param==NULL)																								// pokud volany param je prazdny
			return E_SEMB;																									// vratim chybu
		if(StringGetLenght(&(called_param)->name) != 0){        															// prvek vol param name je nenulovy
			tmp_ptr=BSTSearch(*RootPtr,called_param->name);																	// tak hledej ve stromu a vrat ukazatel do tmp_ptr
			if(tmp_ptr == NULL) return E_SEMC;																				// kdyz je ukazatel prazdny vrat chybu
			declared_param->type = tmp_ptr->type;																			// do deklar param val nahraj value ze stromu ...
	        if(tmp_ptr->type == O_BOOL)
	            declared_param->val.bval = tmp_ptr->value.bval;
	        if(tmp_ptr->type == O_INT)
	            declared_param->val.ival = tmp_ptr->value.ival;
	        if(tmp_ptr->type == O_DOUBLE)
	            declared_param->val.dval = tmp_ptr->value.dval;
	        if(tmp_ptr->type == O_STRING)
	        {
	        	if (StringInit(&(declared_param->val.sval)) == STR_ERR) return E_INTERN;
	       		if (StringCpy(&(declared_param->val.sval), &(tmp_ptr)->value.sval) == STR_ERR) return E_INTERN;
	        }
    	}							
    	else																												// jinak nahraje to co je v ve volanem parametru
    	{        
	        declared_param->type = called_param->type;
	        if(called_param->type == O_BOOL)
	            declared_param->val.bval = called_param->val.bval;
	        if(called_param->type == O_INT)
	            declared_param->val.ival = called_param->val.ival;
	        if(called_param->type == O_DOUBLE)
	            declared_param->val.dval = called_param->val.dval;
	        if(called_param->type == O_STRING)
	        { 
	        	if (StringInit(&(declared_param->val.sval)) == STR_ERR) return E_INTERN;
		        if (StringCpy(&(declared_param->val.sval), &(called_param)->val.sval) == STR_ERR) return E_INTERN;    
	        }		

		}
		declared_param=declared_param->next;																			// posun ukazatele na dalsi 
		called_param=called_param->next;																				// posun volany parametr na dalsi
	}
return E_OK;
}
/*Funkcia priradenia do premennej*/
tErrors assign(tBSTNodePtr* RootPtr,tVariable* op,tVariable* result){   												// funkce pro prirazeni do promene ve strome

	if(StringGetLenght(&op->name) == 0){																				// pokud operand ma jmeno 
		if((BSTInsert(RootPtr,result->name,op)) == E_INTERN) return E_INTERN;											// vloz do  stromu 
	}
	else																												// jinak
	{
		tVariable* tmp_data;																							// vytvor ukazatel pomocny
    	tmp_data=BSTSearch(*RootPtr,op->name);																			// hledej ve strome operand
    	if(tmp_data != NULL){																							// pokud tam je
			if((BSTInsert(RootPtr,result->name,tmp_data)) == E_INTERN) return E_INTERN;									// do vysledku ve strome nahrej operand
    	}
		else
			return E_SEMC;
	}
	return E_OK;
}

/*Funkcia spracuje operand aritmetickych operacii a vrati jeho hodnoty cez parameter*/
tErrors aritmetic_op_check(tBSTNodePtr* RootPtr,tVariable* op,double* val){
	if(StringGetLenght(&op->name) == 0){	//je to hodnota
		if (op->type == O_INT)
		{
			*val=op->value.ival; 
			return E_OK;	
		}
		else if (op->type == O_DOUBLE)
		{
			*val=op->value.dval; 
			return E_OK;	
		}
		else				//nekompatibilny typ ar. operacie
			return E_SEMF;
	}
	else				//je to premenna
	{
		tVariable* tmp_data;
    	tmp_data=BSTSearch(*RootPtr,op->name);
    		if(tmp_data != NULL)
    		{	
    			if (tmp_data->type == O_INT)
				{
					op->type=O_INT;
					*val=tmp_data->value.ival; 
					return E_OK;	
				}
				else if (tmp_data->type == O_DOUBLE)
				{
					op->type=O_DOUBLE;
					*val=tmp_data->value.dval; 
					return E_OK;	
				}
				else			//nekompatibilny typ ar. operacie
				return	E_SEMF;
    		}
    		else{
    			return E_SEMC;		//nedekl. premnenna
    		}
	}
}

bool equal(tVariable* op1,tVariable* op2){
	
	switch(op1->type){
		case O_BOOL:
			if(op1->value.bval == op2->value.bval)
				return true;
			else
				return false;
			break;
		case O_INT:
			if(op1->value.ival == op2->value.ival)
				return true;
			else
				return false;
			break;
    	case O_DOUBLE:
    		if(op1->value.dval == op2->value.dval)
				return true;
			else
				return false;
			break;
    	case O_STRING:
    		if( StringCmp(&op1->value.sval,&op2->value.sval)==0)
				return true;
			else
				return false;
			break;
		case O_NULL:
				return true;
		default:		
			return false;

	}
}

bool less(tVariable* op1,tVariable* op2){
	switch(op1->type){
		case O_BOOL:
			if(op1->value.bval < op2->value.bval)
				return true;
			else
				return false;
			break;
		case O_INT:
			if(op1->value.ival < op2->value.ival)
				return true;
			else
				return false;
			break;
    	case O_DOUBLE:
    		if(op1->value.dval < op2->value.dval)
				return true;
			else
				return false;
			break;
    	case O_STRING:
    		if( StringCmp(&op1->value.sval,&op2->value.sval) < 0)
				return true;
			else
				return false;
			break;
		default:
			return false;
		
	}

}
/*Funckia, ktora nacita parametre z param listu*/
tErrors param_load(tBSTNodePtr* RootPtr,tParamItem* param){
	tVariable* tmp_param;
	if ((tmp_param = malloc (sizeof(tVariable))) == NULL) return E_INTERN; 
	while(param)
	{	
		tmp_param->value=param->val;
		tmp_param->type=param->type;
		if((BSTInsert(RootPtr,param->name,tmp_param)) == E_INTERN){
			free(tmp_param);
			return E_INTERN;
		} 
		param=param->next;
	}
	free(tmp_param);
	return E_OK;
}

/*Funkcia interpretu*/
tErrors interpret(tFunTableItem* function){
if(function == NULL) return E_INTERN;	
/*Deklaracie premnennych*/
tTapeItem* tape_instr=function->tape_ptr->first;
if(tape_instr==NULL) return E_OK;
tBSTNodePtr RootPtr;
BSTInit(&RootPtr);
tFunTableItem * declared_fun;
tVariable* tmp_op1;
tVariable* tmp_op2;
bool jumped=false;
bool rv_pushed=false;
tErrors re_err=E_OK;
tVariable* tmp_param;
if ((tmp_param = malloc (sizeof(tVariable))) == NULL) return E_INTERN;   
if (StringInit(&(tmp_param->value.sval)) == STR_ERR) re_err=E_INTERN;    
if (StringInit(&(tmp_param->name)) == STR_ERR)  re_err=E_INTERN;    
double* op1;
if ((op1 = malloc (sizeof(double))) == NULL) re_err=E_INTERN;
double* op2;
if ((op2 = malloc (sizeof(double))) == NULL) re_err=E_INTERN;
if(function->param_list)
	re_err=param_load(&RootPtr,function->param_list->first);

if(re_err==E_OK)
while(1){		
	switch (tape_instr->instruction){ 
		case ASSIGN:
					if(tape_instr->result)
					re_err=assign(&RootPtr,tape_instr->op1,tape_instr->result);
					break;
		case ADD:
				re_err=aritmetic_op_check(&RootPtr,tape_instr->op1, op1);
				if(re_err == E_OK)
					re_err=aritmetic_op_check(&RootPtr,tape_instr->op2, op2);
				if(re_err!=E_OK)
					break;
				else
				{							
					if (tape_instr->op1->type== O_DOUBLE || tape_instr->op2->type== O_DOUBLE)
					{
						tape_instr->result->type=O_DOUBLE;
						tape_instr->result->value.dval= (*op1) + (*op2);	
					}
					else
					{ 
						tape_instr->result->type=O_INT;
						tape_instr->result->value.ival= (int)(*op1) + (int)(*op2);
					}
				break;
				}
		case MUL: 
				re_err=aritmetic_op_check(&RootPtr,tape_instr->op1, op1);
				if(re_err == E_OK)
					re_err=aritmetic_op_check(&RootPtr,tape_instr->op2, op2);
				if(re_err!=E_OK)
					break;
				else
				{	
					if (tape_instr->op1->type== O_DOUBLE || tape_instr->op2->type== O_DOUBLE)
					{
						tape_instr->result->type=O_DOUBLE;
						tape_instr->result->value.dval= (*op1) * (*op2);
					}
					else
					{
						tape_instr->result->type=O_INT;
						tape_instr->result->value.ival=(int)(*op1) * (int)(*op2);
					}
				break;
				}
		case SUB:
				re_err=aritmetic_op_check(&RootPtr,tape_instr->op1, op1);
				if(re_err == E_OK)
					re_err=aritmetic_op_check(&RootPtr,tape_instr->op2, op2);
				if(re_err!=E_OK)
					break;
				else
				{	
					if (tape_instr->op1->type== O_DOUBLE || tape_instr->op2->type== O_DOUBLE)
					{
						tape_instr->result->type=O_DOUBLE;
						tape_instr->result->value.dval= (*op1) - (*op2);
					}
					else
					{
						tape_instr->result->type=O_INT;
						tape_instr->result->value.ival=(int)(*op1) - (int)(*op2);
					}
				break;
				}	
		case DIV:
				re_err=aritmetic_op_check(&RootPtr,tape_instr->op1, op1);
				if(re_err == E_OK)
					re_err=aritmetic_op_check(&RootPtr,tape_instr->op2, op2);
				if(re_err!=E_OK)
					break;
				else if(*op2 == 0){
					re_err=E_SEMD;
					break;
				}
				else
				{	
					tape_instr->result->type=O_DOUBLE;
					tape_instr->result->value.dval= (*op1) / (*op2);
				break;
				}	

		case CON:
				tmp_op1=tape_instr->op1;
				tmp_op2=tape_instr->op2;
				if (StringGetLenght(&tape_instr->op1->name) != 0)
					tmp_op1=BSTSearch(RootPtr,tape_instr->op1->name);
				if(tmp_op1 == NULL){
					re_err=E_SEMC;
					break;
				}
				if(tmp_op1->type != O_STRING){
					re_err=E_SEMF;
					break;
				}
				if (StringGetLenght(&tape_instr->op2->name) != 0)
					tmp_op2=BSTSearch(RootPtr,tape_instr->op2->name);
				if(tmp_op2 == NULL){
					re_err=E_SEMC;
					break;
				}
				strval(tmp_op1);
				strval(tmp_op2);
				if((StringInit(&(tape_instr)->result->value.sval)) == STR_ERR){
					re_err=E_INTERN;
					break;
				}
				if((StringMerge(&(tmp_op1)->value.sval,&(tmp_op2)->value.sval,&(tape_instr)->result->value.sval)) == STR_ERR){
					re_err=E_INTERN;
					break;
				}
				tape_instr->result->type=O_STRING;
				break;
		case MORE:
				tmp_op1=tape_instr->op1;
				tmp_op2=tape_instr->op2;
				if (StringGetLenght(&tape_instr->op1->name) != 0)
					tmp_op1=BSTSearch(RootPtr,tape_instr->op1->name);
				if(tmp_op1 == NULL){
					re_err=E_SEMC;
					break;
				}
				if (StringGetLenght(&tape_instr->op2->name) != 0)
					tmp_op2=BSTSearch(RootPtr,tape_instr->op2->name);
				if(tmp_op2 == NULL){
					re_err=E_SEMC;
					break;
				}
				if(tmp_op1->type != tmp_op2->type){
					tape_instr->result->type=O_BOOL;
					tape_instr->result->value.bval= false;
				}
				else
				{						
					tape_instr->result->type=O_BOOL;
					if(equal(tmp_op1,tmp_op2)){
						tape_instr->result->value.bval=false;
						break;
					}
					tape_instr->result->value.bval=!(less(tmp_op1,tmp_op2));
				}
				break;
		case LESS:
				tmp_op1=tape_instr->op1;
				tmp_op2=tape_instr->op2;
				if (StringGetLenght(&tape_instr->op1->name) != 0)
					tmp_op1=BSTSearch(RootPtr,tape_instr->op1->name);
				if(tmp_op1 == NULL){
					re_err=E_SEMC;
					break;
				}
				if (StringGetLenght(&tape_instr->op2->name) != 0)
					tmp_op2=BSTSearch(RootPtr,tape_instr->op2->name);
				if(tmp_op2 == NULL){
					re_err=E_SEMC;
					break;
				}
				if(tmp_op1->type != tmp_op2->type){
					tape_instr->result->type=O_BOOL;
					tape_instr->result->value.bval= false;
				}
				else
				{						
					tape_instr->result->type=O_BOOL;
					tape_instr->result->value.bval=less(tmp_op1,tmp_op2);
				}
				break;
		case EQL:
				tmp_op1=tape_instr->op1;
				tmp_op2=tape_instr->op2;
				if (StringGetLenght(&tape_instr->op1->name) != 0)
					tmp_op1=BSTSearch(RootPtr,tape_instr->op1->name);
				if(tmp_op1 == NULL){
					re_err=E_SEMC;
					break;
				}
				if (StringGetLenght(&tape_instr->op2->name) != 0)
					tmp_op2=BSTSearch(RootPtr,tape_instr->op2->name);
				if(tmp_op2 == NULL){
					re_err=E_SEMC;
					break;
				}
				if(tmp_op1->type != tmp_op2->type){
					tape_instr->result->type=O_BOOL;
					tape_instr->result->value.bval= false;
				}
				else
				{						
					tape_instr->result->type=O_BOOL;
					if( less(tmp_op1,tmp_op2) || equal(tmp_op1,tmp_op2))
						tape_instr->result->value.bval=true;
					else
						tape_instr->result->value.bval=false;
				}
				break;
		case EQM:
				tmp_op1=tape_instr->op1;
				tmp_op2=tape_instr->op2;
				if (StringGetLenght(&tape_instr->op1->name) != 0)
					tmp_op1=BSTSearch(RootPtr,tape_instr->op1->name);
				if(tmp_op1 == NULL){
					re_err=E_SEMC;
					break;
				}
				if (StringGetLenght(&tape_instr->op2->name) != 0)
					tmp_op2=BSTSearch(RootPtr,tape_instr->op2->name);
				if(tmp_op2 == NULL){
					re_err=E_SEMC;
					break;
				}
				if(tmp_op1->type != tmp_op2->type){
					tape_instr->result->type=O_BOOL;
					tape_instr->result->value.bval= false;
				}
				else
				{						
					tape_instr->result->type=O_BOOL;
					if(!(less(tmp_op1,tmp_op2)) || equal(tmp_op1,tmp_op2))
						tape_instr->result->value.bval=true;
					else
						tape_instr->result->value.bval=false;
				}
				break;
		case EQ:
				tmp_op1=tape_instr->op1;
				tmp_op2=tape_instr->op2;
				if (StringGetLenght(&tape_instr->op1->name) != 0)
					tmp_op1=BSTSearch(RootPtr,tape_instr->op1->name);
				if(tmp_op1 == NULL){
					re_err=E_SEMC;
					break;
				}
				if (StringGetLenght(&tape_instr->op2->name) != 0)
					tmp_op2=BSTSearch(RootPtr,tape_instr->op2->name);
				if(tmp_op2 == NULL){
					re_err=E_SEMC;
					break;
				}

				if(tmp_op1->type != tmp_op2->type){
					tape_instr->result->type=O_BOOL;
					tape_instr->result->value.bval= false;
				}
				else
				{
					tape_instr->result->type=O_BOOL;
					tape_instr->result->value.bval=equal(tmp_op1,tmp_op2);
				}
			break;
		case NEQ:
				tmp_op1=tape_instr->op1;
				tmp_op2=tape_instr->op2;
				if (StringGetLenght(&(tape_instr)->op1->name) != 0)
					tmp_op1=BSTSearch(RootPtr,tape_instr->op1->name);
				if(tmp_op1 == NULL){
					re_err=E_SEMC;
					break;
				}
				if (StringGetLenght(&(tape_instr)->op2->name) != 0)
					tmp_op2=BSTSearch(RootPtr,tape_instr->op2->name);
				if(tmp_op2 == NULL){
					re_err=E_SEMC;
					break;
				}

				if(tmp_op1->type != tmp_op2->type){ 
					tape_instr->result->type=O_BOOL;
					tape_instr->result->value.bval= true;
				}
				else
				{
					tape_instr->result->type=O_BOOL;
					tape_instr->result->value.bval=(!equal(tmp_op1,tmp_op2));
				}
			break;
		case NGOTO:
				if(StringGetLenght(&(tape_instr)->op1->name) != 0){
					if((tmp_op1=BSTSearch(RootPtr,tape_instr->op1->name)) == NULL){
						re_err=E_SEMC;
						break;
					}		
					boolval(tmp_op1);
					if(tmp_op1->value.bval == false){
						tape_instr=tape_instr->op2->value.tape_pointer;
						jumped=true;
					}
					break;
				}
				else
				   	{    		
				        boolval(tape_instr->op1);
				        if(tape_instr->op1->value.bval == false){
						tape_instr=tape_instr->op2->value.tape_pointer;
						jumped=true;
					}
					break;		
					}
		case FUNC:
			if(tape_instr->op1==NULL){
				re_err=E_INTERN;
				break;
			}
			//========================vstavane funckie====================================================
			if(strcmp(tape_instr->op1->name.ptr,"boolval")==0){
				if((re_err=internfun_param_chceck(&RootPtr,tape_instr->op2->value.param_pointer,tmp_param)) != E_OK) break;
				else
				{
					tape_instr->result->type=O_BOOL;
					boolval(tmp_param);
					re_err=BSTInsert(&RootPtr,tape_instr->result->name,tmp_param);
				}
				break;
			}
			if(strcmp(tape_instr->op1->name.ptr,"strval")==0){
				if((re_err=internfun_param_chceck(&RootPtr,tape_instr->op2->value.param_pointer,tmp_param)) != E_OK) break;
				else
				{
					if((re_err=strval(tmp_param)) != E_OK) 
						break;
					else
					{
						tape_instr->result->type=O_STRING;		
						if((re_err=BSTInsert(&RootPtr,tape_instr->result->name,tmp_param)) == E_INTERN)
						break;							
					}
				}
				break;
			}
			if(strcmp(tape_instr->op1->name.ptr,"intval")==0){
				if((re_err=internfun_param_chceck(&RootPtr,tape_instr->op2->value.param_pointer,tmp_param)) != E_OK) break;
				else{
					intval(tmp_param);
					tape_instr->result->type=O_INT;
					re_err=BSTInsert(&RootPtr,tape_instr->result->name,tmp_param);
				}
				break;
			}
			if(strcmp(tape_instr->op1->name.ptr,"doubleval")==0){
				if((re_err=internfun_param_chceck(&RootPtr,tape_instr->op2->value.param_pointer,tmp_param)) != E_OK) break;
				else{
					doubleval(tmp_param);
					tape_instr->result->type=O_DOUBLE;
					re_err=BSTInsert(&RootPtr,tape_instr->result->name,tmp_param);
				}
				break;
			}
			if(strcmp(tape_instr->op1->name.ptr,"get_string")==0){
				tape_instr->result->value.sval=get_string();
				tape_instr->result->type=O_STRING;
				re_err=BSTInsert(&RootPtr,tape_instr->result->name,tape_instr->result);
				break;
			}
			if(strcmp(tape_instr->op1->name.ptr,"strlen")==0){
				if((re_err=internfun_param_chceck(&RootPtr,tape_instr->op2->value.param_pointer,tmp_param)) != E_OK) break;
				strval(tmp_param);
				tape_instr->result->value.ival=strlen(tmp_param->value.sval.ptr);
				tape_instr->result->type=O_INT;
				re_err=BSTInsert(&RootPtr,tape_instr->result->name,tape_instr->result);
				break;
			}
			if(strcmp(tape_instr->op1->name.ptr,"sort_string")==0){
				if((re_err=internfun_param_chceck(&RootPtr,tape_instr->op2->value.param_pointer,tmp_param)) != E_OK) break;
				strval(tmp_param);
				tape_instr->result->type=O_STRING;
				sort_string(tmp_param->value.sval.ptr);
				re_err=BSTInsert(&RootPtr,tape_instr->result->name,tmp_param);
				break;
			}
			if(strcmp(tape_instr->op1->name.ptr,"get_substring")==0){
				if (tape_instr->op2->value.param_pointer)
				{
					if((re_err=get_substring(&RootPtr,tape_instr->result,tape_instr->op2->value.param_pointer,tmp_param)) != E_OK) break;
					tape_instr->result->type=O_STRING;
					re_err=BSTInsert(&RootPtr,tape_instr->result->name,tape_instr->result);
					break;
				}
				else{
					re_err=E_INTERN;
					break;
				}
			}
			if(strcmp(tape_instr->op1->name.ptr,"find_string")==0){
				if (tape_instr->op2->value.param_pointer)
				{
					if((re_err=find_string(&RootPtr,tape_instr->result,tape_instr->op2->value.param_pointer,tmp_param)) != E_OK) break;
					tape_instr->result->type=O_INT;
					re_err=BSTInsert(&RootPtr,tape_instr->result->name,tape_instr->result);
					break;
				}
				else{
					re_err=E_INTERN;
					break;
				}
			}
			if(strcmp(tape_instr->op1->name.ptr,"put_string")==0){
				if (tape_instr->op2->value.param_pointer)
				{
					if((re_err=put_string(&RootPtr,tape_instr->result,tape_instr->op2->value.param_pointer,tmp_param)) != E_OK) break;
					tape_instr->result->type=O_INT;
					re_err=BSTInsert(&RootPtr,tape_instr->result->name,tape_instr->result);
					break;
				}
				else{
					re_err=E_INTERN;
					break;
				}
			}
			/*===============================KONIEC VSTAVANYCH FUNKCII====================================*/
			declared_fun=searchFunTable(&(tape_instr->op1->name));
			if(declared_fun==NULL){
				re_err=E_SEMA;
				break;
			}		
			if((re_err=fun_param_check(&(RootPtr),declared_fun->param_list->first,tape_instr->op2->value.param_pointer)) != E_OK) break;
			re_err=interpret(declared_fun);			//REKURZIVNE VOLANIE INTERPRETU!!!!!!!			
			if(re_err==E_OK){
				tmp_op2=STopRe(&RV);
				if(tmp_op2!=NULL && tape_instr->result!=NULL){
					re_err=BSTInsert(&RootPtr,tape_instr->result->name,tmp_op2);
					SPopRe(&RV);
					break;
				}		
			}
			break;
		case RET:
			tmp_op1=tape_instr->op1;
			if (StringGetLenght(&tape_instr->op1->name) != 0)
				tmp_op1=BSTSearch(RootPtr,tape_instr->op1->name);
			if(tmp_op1 == NULL){
				re_err=E_SEMC;
				break;
			}
			re_err=SPushRe(&RV,tmp_op1);
			rv_pushed=true;
			break;
		case NOP:
			break;


	}
	/*Moznosti ukoncenia cyklu nacitania*/
	if(re_err != E_OK)
		break;
	if(!jumped){
		tape_instr=tape_instr->next;
	}
	else
		jumped=false;
	if(tape_instr == NULL){
		if(!rv_pushed){
			tmp_param->type=O_NULL;
			SPushRe(&RV,tmp_param);	
		}
		break;	
	}
	if(rv_pushed) break;

}
BSTDispose(&RootPtr);
free(op1);
free(op2);
free(tmp_param);
return re_err;
}

