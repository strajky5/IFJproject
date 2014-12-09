#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "list.h"

tAllocations allocs;        // seznam alokovanych pointeru
tFunList funList;         	// seznam funkci
tTape *Tape;


//#####################################################################################################################################
//############ INICIALIZACE SEZNAMU ALOKOVANYCH POINTRU #################
void initAlloc()
{
	allocs.first = NULL;    // inicializace ukazatele na prvni prvek 
    allocs.last = NULL;     // inicializace ukazatele na posledni prvek 
}                               

//############## VLOZENI A ALOKACE POINTRU DO SEZNAMU ##################
void *allocate(size_t bytes)            // alokuje novy pointer, vlozi jej do seznamu pointru a vraci ukazatel na nej
{          
    void *alloc = malloc(bytes);        // alokace pointru na velikost bytes
    if((alloc != NULL) && (insertAlloc(alloc) != E_OK))     // pokud alokace neproblehla uspesne a vlozeni pointru do seznamu take ne
    {
        return NULL;			// funkce vraci NULL
    } 
    else                        // pokud probehla alokace i vlozeni do seznamu uspesne 
    {
        return alloc;			// => vrat ukazatel na pointer
    }
}

//############# VLOZENI ALOKOVANEHO POINTRU DO SEZNAMU #################
tErrors insertAlloc(void *pointer)      
{               
    if((allocs.first) == NULL)           // pokud seznam nema jeste zadny prvek
    {
        if((allocs.first = malloc(sizeof(tAllocItem))) == NULL)  // alokuj mi pointer jako prvni prvek seznamu velikostu tAllocItem
        {
            return E_INTERN;							// pokud nastane chyba vrat E_INTERN
        } 
        else                                            // pokud alokace probehne uspene
        {   
            allocs.first->allocation = pointer;         // uloz mi vkladany pointer do ukazatele na alokovany prvek
            allocs.first->next = NULL;                  // dalsi prvek NULL
            allocs.first->previous = NULL;              // predchozi prvek NULL
            allocs.last = allocs.first;                 // uloz jej zaroven i jako posledni prvek seznamu 
        }
    } 
    else                                // pokud v seznamu uz je nejaky prvek
    {
        tAllocItem *item = malloc(sizeof(tAllocItem));   // alokuj mi pointer a uloz ukazatel na nej
        tAllocItem *item2 = allocs.last;                 // uloz mi ukazatel na posledni prvek

        if(item != NULL)                          // pokud alokace probehne uspene
        {
        	item->allocation = pointer;           // uloz ukazatel na alokovanou pamet
        	item->previous = item2;               // provaz vkladany prvek s prechozim prvkem
        	item->next = NULL;                  // dalsi prvek bude NULL
            allocs.last->next = item;             // posledni prvek bude ukazovat na vkladany prvek
            allocs.last = item;                   // vkladany prvek bude jako posledni prvek
        } 
        else                                      // pokud alokace neprobehne uspene
        {
            return E_INTERN;                      //vrat chybu
        }
    }
	return E_OK;                                 // vrat v poradku
}

//################### ODEBERE POINTER ZE SEZNAMU ######################
void disposeAlloc(tAllocItem *item)         
{
    if(item != NULL)                        // pokud pointer, ktery chceme odebrat neni NULL
    {
        if(item == allocs.first)            // pokud prvek je roven prvnimu prvku seznamu
        {
            allocs.first = item->next;      // uloz jako prvni prvek nasledujici prvek
            
            if(item->next != NULL)          // pokud nasledujici prvek neni NULL
            {
                allocs.first->previous = NULL;   // predchozi prvek nebude    
            }
            
            free(item);                         // uvolni prvek
            return;
        } 
        else                                    // pokud neni prvek prvni
        {
            if(item == allocs.last)             // pokud je prvek posledni
            {
                allocs.last = item->previous;   // nahraj predchozi prvek jako posledni
                if(item->previous != NULL)      // pokud neni predchozi prvek NULL
                { 
                    allocs.last->next = NULL;   // dalsi prvek nebude
                }
                free(item);                     // uvolni prvek
                return;
            } 
            else                                // pokud neni prvek prvni ani posledni 
            {
                item->previous->next = item->next;      // provaz mi predchozi prvek s dalsim
                item->next->previous = item->previous;  // provaz dalsi prvek s predchozim
                free(item);                             // uvolni mi prvek
                return;
            }
        }   
    }
    else			// pokud je item NULL
    {
        return;		// konci
    }
}

//################### VYHLEDANI POINTERU V SEZNAMU #####################
tAllocItem *searchAlloc(void *pointer)  // vyhleda dany pointer v seznamu alokaci a vraci ukazatel na vyhledanou polozku 
{
    tAllocItem *item = allocs.first;    // uloz mi ukazatele na prvni prvek seznamu pointru do item
    
    while(item != NULL)                 // delej dokud nenarazis na konec seznamu
    {
        if((item->allocation) == pointer) // pokud je hledany pointer roven pointru (allocaton) na ktery ukazuje item
        {
            return item;                  // vrat mi tento ukazatel na dany pointer
        }
        else                              // pokud se nerovnaji  
        {
            item = item->next;			  // jdi na dalsi prvek
        }
    }
    return NULL;        // pokud nebyl nalezen prvek, vrat NULL
}

//##################### UVOLNENI PAMETI POINTERU #######################
void freePointer(void *pointer) 
{
    tAllocItem *item = searchAlloc(pointer);	// vyhleda dany pointer v seznamu a vraci ukazatel na vyhledanou polozku do item
    
    if(item != NULL) 			// pokud pointer na pamet byl nalezen
    {
        free(pointer);			// uvolni pamet na kterou ukazuje pointer
        disposeAlloc(item);		// odstran ukazatel ze seznamu a uvolni jeho pamet
    }
}

//################ UVOLNENI VESKEROU PAMET VSECH POINTERU ##############
void freeAlloc()               // uvolni veskerou pamet alokovanou pomoci funkce allocate/reallocate              
{                     
    tAllocItem *item;                       // vytvoreni pomocneho ukazatele pro uvolneni pameti
    tAllocItem *next_item = allocs.first;   // vytvoreni ukazatele ukazujici na prvni prvek

    while(next_item != NULL)                // delej dokud nenarazis na konec seznamu 
    {
        free(next_item->allocation);        // uvolni mi pamet na kterou ukazuje allocation (prvek seznamu)
        item = next_item;                   // uloz mi prvek do pomocneho ukazatele item
        next_item = next_item->next;        // jdi na dalsi prvek seznamu
        free(item);                         // uvolni mi cely prvek seznamu, ktery ukazoval na tu pamet
    }
}


//#####################################################################################################################################
//##################### INICIALIZACE SEZNAMU FUNKCI #####################
void initFunList()              
{
    funList.first = NULL;       // prvni prvek null
    funList.last  = NULL;       // druhy prvek null
    return;
}

//#################### VLOZENI FUNKCE DO SEZNAMU ########################
tErrors insertFunListItem(string *name, tOperand r_type, tParamList *params,int forward, int paramcount)          
{
    tFunListItem *item = allocate(sizeof(tFunListItem));                           // alokuje ukazatel na prvek seznamu
    if(item != NULL)                                                               // pokud alokace probehla v poradku
    {
        if(strInit(&(item->name)) != STR_SUCCESS)                                  // alokuj name na string  
            return E_INTERN;                                                       // pokud alokace neprobehne v poradku vrati interni chybu
	    if(strCopystring(&(item->name), name) != STR_SUCCESS ) 					   // nakopiruj ze vstupu name do item->name
	    	return E_INTERN;      											   // pokud kopirovani neprobehne v poradku vrati interni chybu 
		item->param_count = paramcount; 
        item->forward = forward;               // parametr pocitadlo vynuluj
        item->ret_type = r_type;             // navratovy typ funkce
        item->param_list = params;           // vloz ukazatel na seznam parametru
        //item->tape_ptr = tape;               // provaz polozku s paskou, ukazatel bude ukazovat kde je funkce na pasce
        item->next = NULL;                   // do polozky dalsi nahraj null   

        if(funList.first == NULL)            // pokud je seznam funkci prazdny
        {
            funList.first = item;           // tak vkladanou funkci nahrej na prvni pozici
        } 
        else                                 // pokud seznam neni prazdny
        {
            funList.last->next = item;       // provaz posledni prvek senznamu s novym vkladanym prvkem (novou vkladanou funkci)
        }
        funList.last = item;                 // vzdy oznac novy vkladany prvek jako posledni prvek seznamu funkci
        return E_OK;                         // vrat v poradku
    }
    else 									 // pokud alokace neprobehne v poradku
    {
    	return E_INTERN;                     // jinak vrat interni chybu
    }                                                                        
}

//################# VLOZENI VESTAVENE FUNKCE DO SEZNAMU #################  
tErrors insertFunListItemEmbed(char *name)                                          
{
    tFunListItem *item = allocate(sizeof(tFunListItem));                                 // alokuje ukazatel na prvek seznamu
//	printf("tu pride ? \n");
    if(item != NULL)                                                                     // pokud alokace probehla v poradku
    {
//		printf("tu pride ? \n");
        if(strInit(&(item->name)) != STR_SUCCESS )                                       // alokuj name na string
            return E_INTERN;         					// pokud alokace neprobehne v poradku vrati interni chybu
//			printf("tu pride ? \n");
        if(strFromChar(&(item->name), name) != STR_SUCCESS ) 	                     // porovnani konst. se stringem 
        	return E_INTERN;      
//		printf("tu pride ? \n");
        item->forward = 2;              // parametr pocitadlo vynuluj
        item->param_list = NULL;            // NULL
		item->param_count = 0; 
     //   item->tape_ptr = NULL;              // NULL
        item->next = NULL;                  // NULL   
                                            
        if(funList.first == NULL)           // pokud je seznam funkci prazdny
        {
            funList.first = item;           // tak vkladanou funkci nahrej na prvni pozici
        } 
        else                                // pokud seznam neni prazdny
        {
            funList.last->next = item;      // provaz posledni prvek senznamu s novym vkladanym prvkem (novou vkladanou funkci)
        }
        funList.last = item;                // vzdy oznac novy vkladany prvek jako posledni prvek seznamu funkci
    	return E_OK;                        // vrat v poradku
    }
    else 									// pokud alokace neprobehne v poradku
    {
    	return E_INTERN;                    // jinak vrat interni chybu
    }                                                                                   
}

//##################### VYHLEDA FUNKCI V TABULCE #########################
tFunListItem *searchFunListFW(string *name)             // vyhleda danou funkci v tabulce a vraci ukazatel na ni, pokud nenajde vraci NULL 
{
    tFunListItem *item = funList.first;                        // vytvori ukazatel na prvni prvek seznamu
    
    while(item != NULL)                                        // cykli dokud neni na konci seznamu
    {
 		if(strCmpstring(&(item->name), name) == 0) 		   // porovnej jestly se schoduji nazvy funkci     
       		return item;                                       // kdyz ano tak vrat ukazatel na tento prvek 

        item = item->next;                                     // posun ukazatel na dalsi prvek seznamu
    }
    return NULL;                                               // pokud funkce nebyla nalezena -> vraci NULL
}


tFunListItem *searchFunListN(string *name)             // vyhleda danou funkci v tabulce a vraci ukazatel na ni, pokud nenajde vraci NULL 
{
    tFunListItem *item = funList.first;                        // vytvori ukazatel na prvni prvek seznamu
    
    while(item != NULL)                                        // cykli dokud neni na konci seznamu
    {
		if(strCmpstring(&(item->name), name) == 0){
 		if(item->forward == 1) {		// porovnej jestly se schoduji nazvy funkci     
			item->forward = 2;
       		return item;    }                                   // kdyz ano tak vrat ukazatel na tento prvek 
		else {		 		// porovnej jestly se schoduji nazvy funkci  
			item->forward = 3;
			return item;
			}
		}	
			
        item = item->next;                                     // posun ukazatel na dalsi prvek seznamu
    }
    return NULL;                                               // pokud funkce nebyla nalezena -> vraci NULL
}
tFunListItem *searchFunList(string *name)             // vyhleda danou funkci v tabulce a vraci ukazatel na ni, pokud nenajde vraci NULL 
{
    tFunListItem *item = funList.first;                        // vytvori ukazatel na prvni prvek seznamu
    
    while(item != NULL)                                        // cykli dokud neni na konci seznamu
    {
 		if(strCmpstring(&(item->name), name) == 0) 			   // porovnej jestly se schoduji nazvy funkci        
       		return item;                                       // kdyz ano tak vrat ukazatel na tento prvek 

        item = item->next;                                     // posun ukazatel na dalsi prvek seznamu
    }
    return NULL;                                               // pokud funkce nebyla nalezena -> vraci NULL
}

tErrors searchFunListCH()             // vyhleda danou funkci v tabulce a vraci ukazatel na ni, pokud nenajde vraci NULL 
{
    tFunListItem *item = funList.first;                        // vytvori ukazatel na prvni prvek seznamu
    
    while(item != NULL)                                        // cykli dokud neni na konci seznamu
    {
 		if(item->forward != 2) 			   // porovnej jestly se schoduji nazvy funkci        
       		return E_SEMA;                                     // kdyz ano tak vrat ukazatel na tento prvek 

        item = item->next;                                     // posun ukazatel na dalsi prvek seznamu
    }
    return E_OK;                                               // pokud funkce nebyla nalezena -> vraci NULL
}
//########################################################################
//#################### INICIALIZACE SEZNAMU PARAMETRU ####################
void initParamlist(tParamList *paramlist)                                   
{
    if(paramlist != NULL)                                                   // kdyz neni seznam prazdny
    {
        paramlist->first = NULL;                                            // prvni prvek NULL
        paramlist->last = NULL;                                             // posledni prvek NULL
    }

}

//##################### VLOZENI PARAMETRU DO SEZNAMU #####################  
tErrors insertParam(tParamList *paramlist, string *name, tOperand type)   // vlozeni parametru do seznamu 
{
//printf("sooooooooooooooom");
    tParamItem *item = allocate(sizeof(tParamItem));                   // alokuje ukazatel na prvek seznamu
    
    if((paramlist != NULL) && (item != NULL) && (name != NULL))        // pokud ukazatel na seznam neni NULL a jmeno parametru prazdny a pokud probehla spravne alokace item
    {
//		printf("sooooooooooooooom");
        if(strInit(&(item->name)) != STR_SUCCESS )                     // alokuj name na string
            return E_INTERN;                                           // pokud alokace neprobehne v poradku vrati interni chybu
	    if(strCopystring(&(item->name), name) != STR_SUCCESS ) 		   // nakopiruj ze vstupu name do item->name
	    	return E_INTERN;     									   // pokud kopirovani neprobehne v poradku vrati interni chybu

        item->type = type;            // nahraj typ parametru 
        item->next = NULL;            // ukazatel na dalsi prvek bude NULL
//		printf("typ : %d name : %s\n",item->type,item->name.str);
//		printf("*********************************************************\n");
        if((paramlist->first) == NULL)       // pokud je seznam parametru prazdny
        {
            paramlist->first = item;         // tak vkladany parametr vloz jako prvni prvek seznamu
        } 
        else                                 // kdyz neni seznam prazdny
        {
            paramlist->last->next = item;    // provaz posledni prvek s vkladanym prvkem
        }
	    paramlist->last = item;              // vzdy vkladany prvek bude jako posledni prvek
        return E_OK;                         // vrat v poradku
    } 
    else                                 	// pokud alokace neprobehla spravne nebo ukazatel na seznam ze vstupu je prazdny     
    {
        return E_INTERN;					// vrat interni chybu
    }
}

//##################### VYHLEDA PARAMETR V TABULCE ######################## 
tParamItem *searchParam(tParamList *paramlist, string *name)                       
{
    if((paramlist != NULL) && (name != NULL))            // pokud ukazatel na seznam nebo ukazatel na jmeno ze vstupu nejsou prazdne
    {
        tParamItem *item = paramlist->first;             // vytvori ukazatel na prvni prvek seznamu
//	printf("searchujem param \n");
        while(item != NULL)                              // cykli dokud nedojdes na konec seznamu
        {
            if(strCmpstring(&(item->name), name) == 0) 	 // porovnej jestly se schoduji nazvy parametru
            	return item;   							 // kdyz ano tak vrat ukazatel na tento prvek 

            item = item->next;                           // posun ukazatel na dalsi prvek seznamu
        }
    }
    return NULL;                                          // pokud parametr nebyl nalezen -> vraci NULL
}

void IniTape()                               // inicializace pasky
{

    Tape->first=NULL;                                   // prvni prvek vyprazdnime
    Tape->last=NULL;                                    // posledni prvek vyprazdnime
    Tape->active=NULL;                                  // aktivni prvek vyprazdnime
}

tErrors InsertEmptyItemTape()                         // vlozeni prazdneho prvku na konec seznamu
{
    tTapeItem* pointer;    

    if((pointer=allocate(sizeof(tTapeItem)))==NULL)     // alokuji si pamet o velikosti prvku a nahraji ho do ukazatele
        return E_INTERN;                                                // kdyz se neco pokazi vratime interni chybu
    pointer->op1=NULL;                                              // jinak inicializuji prvek tak ze do op1 nahraji null
    pointer->op2=NULL;                                              // do op2 taky nahraji null
    pointer->result=NULL;                                           // do result --||--
    pointer->previous=NULL;                                         // do predchoziho nahraji null
    pointer->next=NULL;                                             // do dalsiho nahraji null

    if(Tape->first==NULL)                                           // pokud je paska prazdna 
    {
        Tape->first=pointer;                                        // tak na prvni pozici nahraji pointer
        Tape->last=Tape->first;                                     // do posledni nahraji prvni 
        Tape->active=Tape->first;                                   // a do aktivniho nahraji prvni 
    }
    else{
        Tape->last->next=pointer;                                   // pokud neni paska prazdna tak nahraji prvek do pozice za posledni prvek
        pointer->previous=Tape->last;                               // provazu prvek s poslednim 
        Tape->last=Tape->last->next;                                // a oznacim ho jako posledni
        Tape->active=Tape->last;                                    // aktivni prvek nastavim jako posledni 
    }
    return E_OK;                                                    // vracim e_ok
}

/*void WriteTape()
{
    while(Tape->active->next != NULL)
    {
        printf("\n");
        printf("|OP1:%d|INST:%d|OP2:%d|RESULT:%d \n",Tape->active->op1->type,Tape->active->instruction,Tape->active->op2->type,Tape->active->result->type);
        printf("|OP1:%d|INST:%d|OP2:%d|RESULT:%d \n",Tape->active->op1->name,Tape->active->instruction,Tape->active->op2->name,Tape->active->result->name);
        Tape->active = Tape->active->next;
    }

}*/