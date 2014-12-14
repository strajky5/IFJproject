/* 
* Názov projektu: Implementace interpretu imperativního jazyka IFJ14 
* Dátum: 12/2014
* Autori: Matúš Cimerman, xcimer00
*         Tomáš Hynek ,   xhynek09
*         David Novák ,   xnovak0r
*         Michal Sedlák , xsedla0m
*         Jan Jorenek  ,  xjoren01
*/
#include <ctype.h>
#include <stdbool.h>
#include "parser.h"

tErrors error;
void constructOpStringLine1(string *dest, tVariable *operand);
void constructOpStringLine2(string *dest, tVariable *operand);
void constructInstStringLine1(string *dest, tInstruction instruction);
void constructInstStringLine2(string *dest, tTapeItem *instruction);
tErrors printTape(tTape *tape);
extern tTape* Tape;
int program();
int function();
int blockList();
int commands();
int params();
int testToken(tTypes type);
int globalDecl();
int localDecl();
int ExpParser();
tErrors writefun();


int var = 0;
int semi = 0;
int pc;
string IDstr;
string ActFun;  //ADDED
int afun = 0;   //ADDED
tBSTNodePtr TempTree;
tBSTNodePtr TempTreeL;
tVariable *TempVar;
tParamList *paramlist;

tFunListItem *item; 
int testToken(tTypes type) { // test tokenu
	if (T.type != type) { // !
	  if(T.type == T_ERRORTOKEN) return E_LEX;
	  if(T.type == T_ERRORSYSTEM) return E_INTERN;
		return E_SYN;
	}
	else
		return E_OK;
}

int parser() {
	initAlloc(); 
	BSTInit(&TempTree);
	Tape = allocate(sizeof(tTape));
	paramlist = allocate(sizeof(tParamList));

	initFunList();    
    IniTape();
	if(insertFunListItemEmbed("length") != E_OK) return E_INTERN;
    if(insertFunListItemEmbed("find") != E_OK) return E_INTERN;
    if(insertFunListItemEmbed("copy") != E_OK) return E_INTERN;
    if(insertFunListItemEmbed("sort") != E_OK) return E_INTERN;
	error = program();
		printf("eror je %d \n",error);
	 if (error != E_OK)
        return error;
	//free(TempVar);

	//error = printTape(Tape);
//	BSTDispose(&TempTreeL);
//	BSTDispose(&TempTree);
//	printf("eror je %d \n",error);
//	printf("eror je %d \n",error);
//freeAlloc(); 
	return error;
}


int program() {
	gettoken();
	switch(T.type){
      case T_KEYWORD:{
		if (var == 0){
		  if (!(strCmpConstStr (&(T.s), "var"))){ //var
			var = 1;
			gettoken();
			TempVar = allocate(sizeof(tVariable));
	      if ((error = globalDecl()) != E_OK) return error; // deklarace promennych
		    //free(TempVar);
		  }
		}
	    if (!strCmpConstStr (&(T.s), "begin")) {// BEGIN
		//if(searchFunListFW() != NULL) return E_SEMA;
		if((error = searchFunListCH()) != E_OK) return E_SEMA;
		    gettoken();
			if (!strCmpConstStr (&(T.s), "end")){           //BEGIN END. - prazdne
			    gettoken();
									error = InsertEmptyItemTape();        //vkladam novy prazdny prvek na pasku
					if (error == E_INTERN)
						return error;
					Tape->last->instruction = NOP;
               	if ((error = testToken(T_DOT)) != E_OK) return error;
		          gettoken();
		        if ((error = testToken(T_EOF)) != E_OK) return error;
				return E_OK;
			}
	        if ((error  = blockList()) != E_OK) return error;
		}
	    else if (!strCmpConstStr (&(T.s), "function")) { // funkce
			
		      BSTInit(&TempTreeL);
		      TempVar = allocate(sizeof(tVariable));
			  afun = 1; // ADDED
	       if ((error  = function()) != E_OK){
			   return error;
		   }
		   afun = 0;    //ADDED
		   //strFree(&ActFun); //ADDED
		   if ((error = program()) != E_OK) return error;
		   return E_OK;
		}
		else   {
			return E_SYN;}
		break;
		}
	  default:
	  {
		  return E_SYN;
		  }
    }
				error = InsertEmptyItemTape();        //vkladam novy prazdny prvek na pasku
					if (error == E_INTERN)
						return error;
					Tape->last->instruction = NOP;
		gettoken();
		if ((error = testToken(T_DOT)) != E_OK) return error;
		gettoken();
		if ((error = testToken(T_EOF)) != E_OK) return error;

	 return E_OK;
}

int function() {
	int enumerator;
	int fw =0;
	pc =0;
	gettoken();
	printf (" bla ");
	if ((error = testToken(T_ID)) != E_OK) return error; // ID_funkce - mozna upresnit
	string name;                                          // tu je vkladanie funkcii do seznamu 98-103
	if(strInit(&name) != STR_SUCCESS) return E_INTERN;
	if(strInit(&ActFun) != STR_SUCCESS) return E_INTERN;     //added
    //if(searchFunList(&(T.s)) != NULL) return E_SEMA;
    strCopystring(&name, &(T.s));
	strCopystring(&ActFun, &(T.s));                          //added
	gettoken();
	
	if ((error = testToken(T_LB)) != E_OK) return error; // leva zavorka
	gettoken();
	if ((testToken(T_RB) != E_OK) && (testToken(T_ID) != E_OK)) return E_SYN; // prava zavorka(muze byt prazdne) nebo parametr
	if (testToken(T_ID) == E_OK){
		initParamlist(paramlist);
		if((error = params()) != E_OK) return error; //parametry
	}
	if ((error = testToken(T_RB)) != E_OK) return error;
	gettoken();
	if ((error = testToken(T_COLON)) != E_OK) return error; // ":"
	gettoken();
	if ((error = testToken(T_DATATYPE)) != E_OK) return error; // "typ" - mozna funkci pro typy - semantika? !!!!! TYPE
	if(!(strCmpConstStr(&(T.s), "boolean")))	    enumerator = O_BOOL;		
	else if(!(strCmpConstStr(&(T.s), "integer")))	enumerator = O_INT;	
	else if(!(strCmpConstStr(&(T.s), "real")))      enumerator = O_REAL;
	else if(!(strCmpConstStr(&(T.s), "string")))    enumerator = O_STRING;
	else return E_SEMA;	
	//if((searchFunList(&name)!= NULL)) return E_SYN;  
	gettoken();
	if ((error = testToken(T_SEMICOLON)) != E_OK) return error; //  ";"
	gettoken();
	switch(T.type){

	case T_KEYWORD:
		if (!strCmpConstStr (&(T.s), "forward")) {
			gettoken();
			fw = 1;
	
			if((searchFunListFW(&name)!= NULL)) return E_SEMA;
			if(insertFunListItem(&name,enumerator,paramlist,fw,pc) != E_OK) return E_INTERN;
			if ((error = testToken(T_SEMICOLON)) != E_OK) return error;  // FORWARD a ";"
		    return E_OK;
		}

		item = searchFunListN(&name);
		if((item != NULL)) {
		printf("tu sa nikdy nedostane \n");
		if (item->forward != 2)
		return E_SEMA;
		}
		else {
		fw = 2;
			printf("tu sa dostane \n");
		if(insertFunListItem(&name,enumerator,paramlist,fw,pc) != E_OK) return E_INTERN;}
		//if(InsertEmptyItemTape() != E_OK) return E_INTERN;
		//item = searchFunListN(&name);
		//Tape->last->instruction = FUNC;
		//item->tape_ptr = Tape->last;

		if (!strCmpConstStr (&(T.s), "var")) {
			gettoken();
			if ((error = localDecl()) != E_OK) return error; // deklarace lokalnich promennych
		}
	//	printf("za local decl \n");
		if (!strCmpConstStr (&(T.s), "begin")) {
		printf (" bla ");
			gettoken();
		//	printf("za bein \n");
			if ((error = blockList()) != E_OK) return error;
			gettoken();
			printf (" bla ");
		//	printf("semicollon \n");
			if ((error = testToken(T_SEMICOLON)) != E_OK) return error;
			return E_OK;
		}
		else
			return E_SYN;
		break;
	default:
		 return E_SYN;
	}
}


int blockList() {
//printf("block list \n");
	switch(T.type){
		case T_KEYWORD:
			if (!(strCmpConstStr (&(T.s), "end"))) {
			  return E_OK;
			}
			if ((error = commands()) != E_OK) return error;
			if(semi == 1){
				gettoken();
			if (!(strCmpConstStr (&(T.s), "end"))) { //uprava
			  return E_SYN;
			  }
				semi = 0;
			}
			if ((error = blockList()) != E_OK) return error;
		    return E_OK;
        break;
		case T_ID:
			if ((error = commands()) != E_OK) return error;
			if(semi == 1){
				gettoken();
		       if (!(strCmpConstStr (&(T.s), "end"))) { //uprava
			     return E_SYN;
			   }
				semi = 0;
			}
			if ((error = blockList()) != E_OK) return error;
			return E_OK;
		break;
		default:
			return E_SYN; //E_OK puvodne
	}
	return E_OK;
}

int commands() 
{
	tParamList *paramlistfun;
	tVariable *glob = NULL;
	tVariable *loc = NULL;
	tParamList *par = NULL;
	string *name = allocate(sizeof(string));
    tTapeItem *previous = Tape->last;
    tVariable *op1 = allocate(sizeof(tVariable));
    tVariable *op2 = allocate(sizeof(tVariable));
    tVariable *result = allocate(sizeof(tVariable));
    if((name == NULL) || (op1 == NULL) || (op2 == NULL) || (result == NULL)) return E_INTERN;
	switch(T.type){
		case T_ID:
			if(afun == 1){                                 //ADDED pokud neni ve funkci kontroluje jen global
			  if ((par = searchParam(paramlist, &(T.s))) == NULL)
			  {            //added
			    if(strCmpstring(&(T.s), &ActFun) != 0)
				{ //zde budu muset nejspis dat jen jmenu aktualni fkce
			      if((loc = BSTSearch (TempTreeL, T.s)) == NULL)
				  {
			        if((glob = BSTSearch (TempTree, T.s)) == NULL)
					{
						return E_SEMA;
					}
				  }    
			    }
		      }
			}
			else{
				if((glob = BSTSearch (TempTree, T.s)) == NULL)
					return E_SEMA;
				}



			gettoken();
			if ((error = testToken(T_ASSIGN)) != E_OK) return error;
			if ((error = ExpParser()) != E_OK) return error;
			//if(InsertEmptyItemTape() != E_OK) return E_INTERN;
			Tape->last->instruction = ASSIGN;
	       
			if (loc != NULL){
				Tape->last->op1 = loc;
			}
			else if (glob != NULL){
				Tape->last->op1 = glob;
			}
			 if (res != Tape->last->op1->type ){
			  if (res != 1)
			   return E_SEMB;
			  else
			   Tape->last->op1->type = O_REAL;
			  }
			Tape->last->op2 = Tape->last->previous->result;
			Tape->last->result = Tape->last->op1;
			if ((strCmpConstStr (&(T.s), "end"))) {
				if ((error = testToken(T_SEMICOLON)) != E_OK) return error;
				semi = 1;
			}
			break;

		case T_KEYWORD:
			if(!strCmpConstStr (&(T.s), "begin")){
				gettoken();
				if((error = blockList()) != E_OK) return error;
                gettoken();
			    if ((strCmpConstStr (&(T.s), "end"))) {
				    if ((error = testToken(T_SEMICOLON)) != E_OK) return error;
				    semi = 1;
			    }
				break;
			}
			if(!strCmpConstStr (&(T.s), "if")){ //if
				tTapeItem *label;
                tTapeItem *endelse;
                tVariable *op11 = allocate(sizeof(tVariable));
                tVariable *op22 = allocate(sizeof(tVariable));
                if((op11 == NULL) || (op22 == NULL)) return E_INTERN;
				
				if((error = ExpParser()) != E_OK) return error; //vyraz
				if (res != 3 ) return E_SEMB;
				//ZAKONECNTOVANO if(InsertEmptyItemTape() != E_OK) return E_INTERN; 
				Tape->last->instruction = JUMPN;
                Tape->last->op1 = Tape->last->previous->result;
                Tape->last->op2 = op2;
                Tape->last->op2->type = TAPE_POINTER;
                label = Tape->last;
				
				if((error = testToken(T_KEYWORD)) == E_OK){
					if(strCmpConstStr (&(T.s),"then")) return E_SYN; //then
				}
				else
					return error;
				gettoken();
				if((error = testToken(T_KEYWORD)) == E_OK){
					if(strCmpConstStr (&(T.s),"begin")) return E_SYN; //begin slozeneho prikazu
				}
				else
					return error;
				gettoken();
				if((error = blockList()) != E_OK) return error; //blocklist
				gettoken();
				
				Tape->last->instruction = JUMPN;
                Tape->last->op1 = op11;
                Tape->last->op1->type = O_BOOL;
                Tape->last->op1->value.bval = false;
                if(strInit(&(Tape->last->op1->name)) != STR_SUCCESS) return E_INTERN;
                Tape->last->op2 = op22;
                Tape->last->op2->type = TAPE_POINTER;
                endelse = Tape->last;
				
				if((error = testToken(T_KEYWORD)) != E_OK) return error;
				if(strCmpConstStr (&(T.s),"else")) return E_SYN;
				
				if(InsertEmptyItemTape() != E_OK) return E_INTERN;
                Tape->last->instruction = NOP;
                label->op2->value.tape_pointer = Tape->last;
				
				gettoken();
				if((error = testToken(T_KEYWORD)) != E_OK) return error;
				if(strCmpConstStr (&(T.s),"begin")) return E_SYN; //begin slozeneho prikazu
				gettoken();
				if((error = blockList()) != E_OK) return error; //blocklist
				gettoken();
			    if ((strCmpConstStr (&(T.s), "end"))) {
				    if (testToken(T_SEMICOLON) != E_OK) return E_SYN;
				    semi = 1;
			    }
				
				if(InsertEmptyItemTape() != E_OK) return E_INTERN;
                Tape->last->instruction = NOP;
                endelse->op2->value.tape_pointer = Tape->last;


				break;


			}
		    if(!strCmpConstStr (&(T.s), "while")){
				
				 if(InsertEmptyItemTape() != E_OK) return E_INTERN;
                Tape->last->instruction = NOP;
                tTapeItem *label1;
                tTapeItem *label2 = Tape->last;             // ukazatel na pasku kam se bude cyklus vracet
                tVariable *op11 = allocate(sizeof(tVariable));
                tVariable *op22 = allocate(sizeof(tVariable));
                if((op11 == NULL) || (op22 == NULL)) return E_INTERN;
				
                if((error = ExpParser()) != E_OK) return error; //vyraz
                if (res != 3 ) return E_SEMB;
				
				//ZAKONECNTOVANO  if(InsertEmptyItemTape() != E_OK) return E_INTERN;
                Tape->last->instruction = JUMPN;
                Tape->last->op1 = Tape->last->previous->result;
                Tape->last->op2 = op22;
                Tape->last->op2->type = TAPE_POINTER;
                label1 = Tape->last;  
				
				if((error = testToken(T_KEYWORD)) == E_OK){
					if(strCmpConstStr (&(T.s),"do")) return E_SYN; //begin slozeneho prikazu
				}
				else
					return error;
				gettoken();
				if((error = testToken(T_KEYWORD)) == E_OK){
					if(strCmpConstStr (&(T.s),"begin")) return E_SYN; //begin slozeneho prikazu
				}
				else{
					return error;
				}
				gettoken();
				if((error = blockList()) != E_OK) return error; //blocklist
				gettoken();
			    if ((strCmpConstStr (&(T.s), "end"))) {
				    if ((error = testToken(T_SEMICOLON)) != E_OK) return error;
				    semi = 1;
			    }
				
				if(InsertEmptyItemTape() != E_OK) return E_INTERN;
                Tape->last->instruction = JUMP;
                Tape->last->op1 = op1;
                Tape->last->op1->type = O_BOOL;
                Tape->last->op1->value.bval = false;
                if(strInit(&(Tape->last->op1->name)) != STR_SUCCESS) return E_INTERN;
                Tape->last->op2 = op2;
                Tape->last->op2->type = TAPE_POINTER;
                Tape->last->op2->value.tape_pointer = label2;     // nepodmineny skok zpet na zacatek vyhodnocovani podminky cyklu
                if(InsertEmptyItemTape() != E_OK) return E_INTERN;
                Tape->last->instruction = NOP;                    // instrukce, na kterou se skoci v pripade nesplneni podminky whilu
                label1->op2->value.tape_pointer = Tape->last;
				
				break;
			}
			if (!strCmpConstStr (&(T.s), "readln")) {
				
				gettoken();
				if((error = testToken(T_LB)) != E_OK) return error;
				gettoken();
				if((error = testToken(T_ID)) != E_OK) return error;
				if(afun == 1){
				  if(strCmpstring(&(T.s), &ActFun) != 0){                             //added: Jmeno fkce, params, global, local
				    if (searchParam(paramlist, &(T.s)) == NULL){
				      if((TempVar = BSTSearch (TempTreeL, T.s)) == NULL){
					    if((TempVar = BSTSearch(TempTree, T.s)) == NULL) return E_SEMA;
				      }
				    }
				  }
				}
				else{
					if((TempVar = BSTSearch(TempTree, T.s)) == NULL) return E_SEMA;
				}
				if (TempVar->type == O_BOOL){
			        TempVar = NULL;
					return E_SEMB;
				}
                if(InsertEmptyItemTape() != E_OK) return E_INTERN;
				Tape->last->instruction = READ;
				Tape->last->op1 = TempVar;
				gettoken();
                if((error = testToken(T_RB)) != E_OK){
					TempVar = NULL;
					return error;
				}
				gettoken();
			    if ((strCmpConstStr (&(T.s), "end"))) {
				    if ((error = testToken(T_SEMICOLON)) != E_OK){
						TempVar = NULL;
						return error;
					}
				    semi = 1;
			    }
				TempVar = NULL;
				break;
			}
	        if (!strCmpConstStr (&(T.s), "write")) { //write(ID) !!! integer or real !!!
				gettoken();
				if((error = testToken(T_LB)) != E_OK) return error;
				gettoken();
				if((error = writefun()) != E_OK) return error;          
				gettoken();
			    if ((strCmpConstStr (&(T.s), "end"))) {
				    if ((error = testToken(T_SEMICOLON)) != E_OK) return error;
				    semi = 1;
			    }
				break;
			}
		default:
			return E_SYN;
	}
	return E_OK;

}

tErrors writefun() {
     if(InsertEmptyItemTape() != E_OK) return E_INTERN;
	 Tape->last->instruction = WRITE;
	if((error = testToken(T_ID)) == E_OK){
	  if(afun == 1){
	    if(strCmpstring(&(T.s), &ActFun) != 0){                             //added: Jmeno fkce, params, global, local
		  if (searchParam(paramlist, &(T.s)) == NULL){
		    if((TempVar = BSTSearch (TempTreeL, T.s)) == NULL){
			  if((TempVar = BSTSearch(TempTree, T.s)) == NULL) return E_SEMA;
		    }
		  }
	    }
	  }
	  else{
        if((TempVar = BSTSearch(TempTree, T.s)) == NULL) return E_SEMA;
		if(!(strCmpConstStr(&(T.s), "boolean"))) TempVar->type = O_BOOL;
		if(!(strCmpConstStr(&(T.s), "integer"))) TempVar->type = O_INT;		
		if(!(strCmpConstStr(&(T.s), "real")))    TempVar->type = O_REAL;
		if(!(strCmpConstStr(&(T.s), "string"))){
			TempVar->type = O_STRING;
			strInit(&TempVar->value.sval);
		}
		TempVar->valFull = NODATA;		
		Tape->last->op1 = TempVar;
	  }
		gettoken();
      if((error = testToken(T_RB)) == E_OK){
		  return E_OK;
	  }
	  else if((error = testToken(T_COMMA)) == E_OK){
		  gettoken();
		  if((error = writefun()) != E_OK) return error;
	  }
	  else
		  return error;
	}
	else if((error = testToken(T_STRING)) == E_OK){
				if ((TempVar = allocate(sizeof(tVariable))) == NULL)	//pokud neni dostatek pameti => E_INTERN
                    return E_INTERN;
                if (strInit(&(TempVar->name)) == STR_ERROR)			//pokud funkce init. stringu vrati chybu => E_INTERN
                    return E_INTERN;
                TempVar->type = O_STRING;
                if (strInit(&(TempVar->value.sval)) == STR_ERROR)    //pokud funkce init. stringu vrati chybu => E_INTERN
                    return E_INTERN;
                if (strCopystring(&(TempVar->value.sval), &(T.s)) == STR_ERROR)
                    return E_INTERN;
        Tape->last->op1 = TempVar;       

		gettoken();
      if((error = testToken(T_RB)) == E_OK){
		  return E_OK;
	  }
	  else if((error = testToken(T_COMMA)) == E_OK){
		  gettoken();
		  if((error = testToken(T_STRING)) == E_OK) return error;
		  if((error = writefun()) != E_OK) return error;
	  }
	  else
		  return error;
	}
	else
		return error;
	return E_OK;
}

int params() {
	int enumerator;
	if ((error = testToken(T_ID)) != E_OK) return error; // "ID_parametru" - mozna specifikovat
	strInit(&IDstr);
    strCopystring(&IDstr, &T.s);
	gettoken();
	if ((error = testToken(T_COLON)) != E_OK) return error; // ":"
	gettoken();
	if ((error = testToken(T_DATATYPE)) != E_OK) return error; 
	if(!(strCmpConstStr(&(T.s), "boolean")))	    enumerator = O_BOOL;		
	else if(!(strCmpConstStr(&(T.s), "integer")))	enumerator = O_INT;	
	else if(!(strCmpConstStr(&(T.s), "real")))      enumerator = O_REAL;
	else if(!(strCmpConstStr(&(T.s), "string")))    enumerator = O_STRING;
	else return E_SEMA;		
	gettoken();
	pc++;
	if(strCmpstring(&IDstr, &ActFun) == 0) return E_SEMA;          //ADDED
	if (searchParam(paramlist, &IDstr) != NULL) return E_SEMA;    
	if ((error = insertParam(paramlist,&IDstr, enumerator)) != E_OK) return error;
	//strFree(&IDstr);
	if (testToken(T_SEMICOLON) == E_OK) {  
	  gettoken();
	  if((error = params()) != E_OK) return error; //vice parametru - rekurze
	}
	return E_OK;
}

int globalDecl() { //za decl nebrat token
		if ((error = testToken(T_ID)) != E_OK) return error; //ID
		if(BSTSearch(TempTree, T.s) != NULL){
			return E_SEMA;
		}
		strInit(&IDstr);
		strCopystring(&IDstr, &T.s);
		gettoken();
		if ((error = testToken(T_COLON)) != E_OK) return error;
		    gettoken();
		if ((error = testToken(T_DATATYPE)) != E_OK) return error;

/******************************************INSERT***************************************************************/

		if(!(strCmpConstStr(&(T.s), "boolean"))) TempVar->type = O_BOOL;
		if(!(strCmpConstStr(&(T.s), "integer"))) TempVar->type = O_INT;		
		if(!(strCmpConstStr(&(T.s), "real")))    TempVar->type = O_REAL;
		if(!(strCmpConstStr(&(T.s), "string"))){
			TempVar->type = O_STRING;
			strInit(&TempVar->value.sval);
		}
		BSTInsert (&TempTree, IDstr, TempVar); 
		
/******************************************INSERT***************************************************************/

		gettoken();
		if ((error = testToken(T_SEMICOLON)) != E_OK) return error; // ";"
		gettoken();
		//strFree(&IDstr);
		if (testToken(T_ID) == E_OK)
		    if ((error = globalDecl()) != E_OK) return error;	//dalsia promena?
		return E_OK;
}

int localDecl() { //za decl nebrat token
		if ((error = testToken(T_ID)) != E_OK) return error; //ID
		if (searchParam(paramlist, &(T.s)) != NULL){ //zde budu muset nejspis dat jen jmenu aktualni fkce
			return E_SEMA;
		}
		if(strCmpstring(&(T.s), &ActFun) == 0){     //ADDED
			return E_SEMA;
		}
		if(BSTSearch (TempTreeL, T.s) != NULL){
			return E_SEMA;
		}
		strInit(&IDstr);
		strCopystring(&IDstr, &T.s);
		gettoken();
		if ((error = testToken(T_COLON)) != E_OK) return error;
		    gettoken();
		if ((error = testToken(T_DATATYPE)) != E_OK) return error; // typ
		
/******************************************INSERT***************************************************************/

		if(!(strCmpConstStr(&(T.s), "boolean"))) TempVar->type = O_BOOL;		
		if(!(strCmpConstStr(&(T.s), "integer"))) TempVar->type = O_INT;		
		if(!(strCmpConstStr(&(T.s), "real")))    TempVar->type = O_REAL;
		if(!(strCmpConstStr(&(T.s), "string"))){
			TempVar->type = O_STRING;
			strInit(&TempVar->value.sval);
		}
		BSTInsert (&TempTreeL, IDstr, TempVar); 

/******************************************INSERT***************************************************************/
		gettoken();
		if ((error = testToken(T_SEMICOLON)) != E_OK) return error; // ";"
		gettoken();
		//strFree(&IDstr);
		if (testToken(T_ID) == E_OK)
		    if ((error = localDecl()) != E_OK) return error;	//dalsia promena?
		return E_OK;
}
void constructOpStringLine1(string *dest, tVariable *operand) {
    switch(operand->type) {
    case O_BOOL:
        strFromChar(dest, "BOOLEAN");
        break;
    case O_INT:
        strFromChar(dest, "INT");
        break;
    case O_REAL:
        strFromChar(dest, "DOUBLE");
        break;
    case O_STRING:
        strFromChar(dest, "STRING");
        break;
    case NONTRM:
        strFromChar(dest, "NONTRM");
        break;
    case O_NOTDATA:
        strFromChar(dest, "O_NOTDATA");
        break;
    case FUNCTION:
        strFromChar(dest, "FUNCTION");
        break;
    case TAPE_POINTER:
        strFromChar(dest, "TAPE_POINTER");
        break;
    case PARAM_POINTER:
        strFromChar(dest, "PARAM_POINTER");
        break;
    }
}

void constructOpStringLine2(string *dest, tVariable *operand) {
    char str[100];      // prasarna, ale debug

    switch(operand->type) {
    case O_BOOL:
        if(operand->value.bval == true) {
            strFromChar(dest, "true");
        } else {
            strFromChar(dest, "false");
        }
        break;
    case O_INT:
        sprintf(str, "%d", &operand->value.ival);
        strFromChar(dest, str);
        break;
    case O_REAL:
        sprintf(str, "%d", &operand->value.rval);
        strFromChar(dest, str);
        break;
    case O_STRING:
        strCopystring(dest, &(operand->value.sval));
        break;
    case NONTRM:
        strFromChar(dest, "nonterm");
        break;
    case O_NOTDATA:
        strFromChar(dest, "O_NOTDATA");
        break;
    case FUNCTION:
        strCopystring(dest, &(operand->name));
        break;
    case TAPE_POINTER:
        sprintf(str, "%llx", (long long)operand->value.tape_pointer);
        strFromChar(dest, str);
        break;
    case PARAM_POINTER:
        sprintf(str, "%llx", (long long)operand->value.param_pointer);
        strFromChar(dest, str);
        break;

    }
}

void constructInstStringLine1(string *dest, tInstruction instruction) {
    switch(instruction) {
    case ADD:
        strFromChar(dest, "ADD");
        break;
    case MUL:
        strFromChar(dest, "MUL");
        break;
    case SUB:
        strFromChar(dest, "SUB");
        break;
    case DIV:
        strFromChar(dest, "DIV");
        break;
    case EQ:
        strFromChar(dest, "EQ");
        break;
    case NEQ:
        strFromChar(dest, "NEQ");
        break;
    case EQL:
        strFromChar(dest, "EQL");
        break;
    case EQM:
        strFromChar(dest, "EQM");
        break;
    case MORE:
        strFromChar(dest, "MORE");
        break;
    case LESS:
        strFromChar(dest, "LESS");
        break;
    case NOP:
        strFromChar(dest, "NOP");
        break;
    case NOPJ:
        strFromChar(dest, "NOPJ");
		    case JUMP:
        strFromChar(dest, "JUMP");
        break;
    case JUMPN:
        strFromChar(dest, "JUMPN");
        break;
        break;
    case ASSIGN:
        strFromChar(dest, "ASSIGN");
        break;
	  case CALL:
        strFromChar(dest, "CALL");
        break;
    case FUNC:
        strFromChar(dest, "FUNC");
        break;
	case WRITE:
        strFromChar(dest, "WRITE");
        break;
	case READ:
        strFromChar(dest, "READ");
        break;
    }
}

void constructInstStringLine2(string *dest, tTapeItem *instruction) {
    char str[100];      // prasarna, ale debug
    sprintf(str, "%llx", (long long)instruction);
    strFromChar(dest, str);
}

tErrors printTape(tTape *tape) {

    if(tape == NULL) return E_INTERN;
    if(tape->first == NULL) return E_INTERN;

    tTapeItem *iter = tape->first;
    string inst;
    string op1;
    string op2;
    string result;

    if(strInit(&inst) != STR_SUCCESS) return E_INTERN;
    if(strInit(&op1) != STR_SUCCESS) return E_INTERN;
    if(strInit(&op2) != STR_SUCCESS) return E_INTERN;
    if(strInit(&result) != STR_SUCCESS) return E_INTERN;

    printf("||======================================================================||\n");
    printf("||\tINST\t|\tOP1\t|\tOP2\t|\tRESULT\t||\n");
    printf("||======================================================================||\n");

    while(iter != NULL) {

        // Line 1
        strClear(&inst);
        strClear(&op1);
        strClear(&op2);
        strClear(&result);

        constructInstStringLine1(&inst, iter->instruction);
        if(iter->op1 != NULL) constructOpStringLine1(&op1, iter->op1);
        if(iter->op2 != NULL) constructOpStringLine1(&op2, iter->op2);
        if(iter->result != NULL) constructOpStringLine1(&result, iter->result);
        printf("||\t%s\t|\t%s\t|\t%s\t|\t%s\t\t||\n", inst.str, op1.str, op2.str, result.str);

        // Line 2
        strClear(&inst);
        strClear(&op1);
        strClear(&op2);
        strClear(&result);

        constructInstStringLine2(&inst, iter);
        if(iter->op1 != NULL) constructOpStringLine2(&op1, iter->op1);
        if(iter->op2 != NULL) constructOpStringLine2(&op2, iter->op2);
        if(iter->result != NULL) constructOpStringLine2(&result, iter->result);
        printf("||\t%s\t|\t%s\t|\t%s\t|\t%s\t\t||\n", inst.str, op1.str, op2.str, result.str);
		



        printf("||======================================================================||\n");
        iter = iter->next;
    }

	return E_OK;
}


