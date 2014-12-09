#include <ctype.h>
#include <stdbool.h>
#include "parser.h"

tErrors error;
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
int writefun();

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
	//free(TempVar);
	BSTDispose(&TempTree);
//	printf("eror je %d \n",error);
freeAlloc(); 
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
			//printf("main \n");
			if (!strCmpConstStr (&(T.s), "end")){           //BEGIN END. - prazdne
			    gettoken();
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
		   //printf("za funkcwma \n");
		   afun = 0;    //ADDED
		   //strFree(&ActFun); //ADDED
		   BSTDispose(&TempTreeL);
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

		if (item->forward != 2)
		return E_SEMA;
		}
		else {
		fw = 2;
		if(insertFunListItem(&name,enumerator,paramlist,fw,pc) != E_OK) return E_INTERN;}
		if(InsertEmptyItemTape() != E_OK) return E_INTERN;
		item = searchFunListN(&name);
		Tape->last->instruction = FUNC;
		item->tape_ptr = Tape->last;

		if (!strCmpConstStr (&(T.s), "var")) {
			gettoken();
			if ((error = localDecl()) != E_OK) return error; // deklarace lokalnich promennych
		}
	//	printf("za local decl \n");
		if (!strCmpConstStr (&(T.s), "begin")) {
			gettoken();
		//	printf("za bein \n");
			if ((error = blockList()) != E_OK) return error;
			gettoken();
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

int commands() {
	switch(T.type){
		case T_ID:
			tBSTNodePtr *glob;
			tBSTNodePtr *loc;
			tParamList *par;
			if(InsertEmptyItemTape() != E_OK) return E_INTERN;
			Tape->last->instruction = ASSIGN;
			if(afun == 1){                                 //ADDED pokud neni ve funkci kontroluje jen global
			  if ((par = searchParam(paramlist, &(T.s))) == NULL){            //added
			    if(strCmpstring(&(T.s), &ActFun) != 0){ //zde budu muset nejspis dat jen jmenu aktualni fkce
			      if((loc = BSTSearch (TempTreeL, T.s)) == NULL){
			        if((glob = BSTSearch (TempTree, T.s)) == NULL){
						return E_SEMA;
					}
					else{
                      Tape->last->op1 = glob;
					}    
			      }
				  else{
                    Tape->last->op1 = loc;
				  }
			    }
				else{
                  Tape->last->op1 = item;
				}
			  }
			  else{
                Tape->last->op1 = par;
			  }
			}
			else {
				if((glob = BSTSearch (TempTree, T.s)) == NULL){
					return E_SEMA;
				}
				else{
				  Tape->last->op1 = glob;
				}
			}

			Tape->last->result = Tape->last->previous->result;

			gettoken();
			if ((error = testToken(T_ASSIGN)) != E_OK) return error;
			if ((error = ExpParser()) != E_OK) return error;
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
				if((error = ExpParser()) != E_OK) return error; //vyraz
				if (res != 3 ) return E_SEMB;
				Tape->last->instruction = JUMP;
				Tape->last->op2 = Tape->last->previous->result;
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
				if((error = testToken(T_KEYWORD)) != E_OK) return error;
				if(strCmpConstStr (&(T.s),"else")) return E_SYN;
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


				break;


			}
		    if(!strCmpConstStr (&(T.s), "while")){
                if((error = ExpParser()) != E_OK) return error; //vyraz
				Tape->last->instruction = JUMP;
				Tape->last->op2 = Tape->last->previous->result;
				if (res != 3 ) return E_SEMB;
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
//					printf("tento\n");
					return error;
				}
				gettoken();
				if((error = blockList()) != E_OK) return error; //blocklist
				gettoken();
			    if ((strCmpConstStr (&(T.s), "end"))) {
				    if ((error = testToken(T_SEMICOLON)) != E_OK) return error;
				    semi = 1;
			    }
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

int writefun() {
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
	if ((error = insertParam(paramlist,&IDstr, enumerator)) != E_OK) return error;;
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

