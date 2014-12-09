
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <stdbool.h>
#include "ial.h"


/***********************************/
/*        TABULKA SYMBOLOV         */
/***********************************/

void BSTInit (tBSTNodePtr *RootPtr) 
{
    *RootPtr = NULL;
}

tVariable* BSTSearch (tBSTNodePtr RootPtr, string name)  
{
    if (!RootPtr)
       return NULL;
    if (!StringCmp(&RootPtr->tree_data->name, &name))
        return RootPtr->tree_data;
    else if (StringCmp(&RootPtr->tree_data->name, &name) > 0)
        return BSTSearch(RootPtr->LPtr, name);
    else 
        return BSTSearch(RootPtr->RPtr, name);
} 

tErrors BSTInsert (tBSTNodePtr* RootPtr, string name, tVariable* var)    
{   
    if (!*RootPtr)
    {               
        tBSTNodePtr temp;
        if ((temp = malloc (sizeof(struct tBSTNode))) == NULL)
            return E_INTERN;
        if ((temp->tree_data = malloc (sizeof(tVariable))) == NULL)
            return E_INTERN;
        if (StringInit(&(temp)->tree_data->name))
            return E_INTERN;
        if (StringCpy(&temp->tree_data->name, &name))
            return E_INTERN;

        temp->tree_data->type = var->type;
        if(temp->tree_data->type == O_BOOL)
            temp->tree_data->value.bval = var->value.bval;
        if(temp->tree_data->type == O_INT)
            temp->tree_data->value.ival = var->value.ival;
        if(temp->tree_data->type == O_DOUBLE)
            temp->tree_data->value.dval = var->value.dval;
        if(temp->tree_data->type == O_STRING)
        {
            if (StringInit(&(temp->tree_data->value.sval)) == STR_ERR)
                return E_INTERN;
            if (StringCpy(&(temp->tree_data->value.sval), &var->value.sval) == STR_ERR)
                return E_INTERN;
        }
        temp->LPtr = NULL;
        temp->RPtr = NULL;
        *RootPtr = temp;
        /*=========================KONTROLNY VYPIS====================================*/
     /*  if(temp->tree_data->type == O_DOUBLE)
            printf("uzol: %s:%f\n",temp->tree_data->name.ptr,temp->tree_data->value.dval);
        if(temp->tree_data->type == O_INT)
            printf("uzol: %s:%d\n",temp->tree_data->name.ptr,temp->tree_data->value.ival);
        if(temp->tree_data->type == O_BOOL)
             printf("uzol: %s:%d\n",temp->tree_data->name.ptr,temp->tree_data->value.bval);
        if(temp->tree_data->type == O_STRING)
             printf("uzol: %s:%s\n",temp->tree_data->name.ptr,temp->tree_data->value.sval.ptr);*/
    }
    else if (StringCmp(&(*RootPtr)->tree_data->name, &name) > 0)          
        BSTInsert (&(*RootPtr)->LPtr, name,var);
    else if (StringCmp(&(*RootPtr)->tree_data->name, &name) < 0)
        BSTInsert (&(*RootPtr)->RPtr, name,var);
    else
    { 
        (*RootPtr)->tree_data->type = var->type;                      //aktualize + pretypovanie
        if((*RootPtr)->tree_data->type == O_BOOL)
            (*RootPtr)->tree_data->value.bval = var->value.bval;
        if((*RootPtr)->tree_data->type == O_INT)
            (*RootPtr)->tree_data->value.ival = var->value.ival;
        if((*RootPtr)->tree_data->type == O_DOUBLE)
            (*RootPtr)->tree_data->value.dval = var->value.dval;
        if((*RootPtr)->tree_data->type == O_STRING)
        {
            if (StringInit(&(*RootPtr)->tree_data->value.sval) == STR_ERR)
                    return E_INTERN;
            if (StringCpy(&(*RootPtr)->tree_data->value.sval, &var->value.sval) == STR_ERR)
                return E_INTERN;
        }

            /*    =========================KONTROLNY VYPIS====================================*/
       /* if((*RootPtr)->tree_data->type == O_DOUBLE)
            printf("uzol: %s:%f\n",(*RootPtr)->tree_data->name.ptr,(*RootPtr)->tree_data->value.dval);
        if((*RootPtr)->tree_data->type == O_INT)
            printf("uzol: %s:%d\n",(*RootPtr)->tree_data->name.ptr,(*RootPtr)->tree_data->value.ival);
        if((*RootPtr)->tree_data->type == O_BOOL)
            printf("uzol: %s:%d\n",(*RootPtr)->tree_data->name.ptr,(*RootPtr)->tree_data->value.bval);
        if((*RootPtr)->tree_data->type == O_STRING)
             printf("uzol: %s:%s\n",(*RootPtr)->tree_data->name.ptr,(*RootPtr)->tree_data->value.sval.ptr);*/

    }
    return E_OK;
}

void BSTDispose (tBSTNodePtr *RootPtr) 
{   
    if (*RootPtr)
    {
        BSTDispose(&(*RootPtr)->LPtr);
        BSTDispose(&(*RootPtr)->RPtr);
        free((*RootPtr)->tree_data);
        free(*RootPtr);
        *RootPtr = NULL;
    }
    return;
}

/***********************************/
/*     KONIEC TABULKY SYMBOLOV     */
/***********************************/


/***********************************/
/*             PREVODY             */
/***********************************/

void boolval(tVariable *term)
{
    switch(term->type)
    {
        case O_DOUBLE :
            if (term->value.dval == 0.0)
                term->value.bval = false;
            else term->value.bval = true;
            break;
        case O_INT : 
            if (term->value.ival == 0)
                term->value.bval = false;
            else term->value.bval = true;
            break;
        case O_STRING : 
            if (strcmp(term->value.sval.ptr, "") == 0)
                term->value.bval = false;
            else term->value.bval = true;
            break;
        case O_NULL : 
            term->value.bval = false;
            break;
        case O_BOOL :   
        default : break;
    }
    term->type = O_BOOL;
}

tErrors strval(tVariable *term)
{
    char buff[30];
    switch(term->type)
    {
        case O_DOUBLE :
            sprintf(buff, "%g", term->value.dval);
            if (StringInit(&(term->value.sval)) == STR_ERR)
                    return E_INTERN;
            if (StringFromChar(&(term->value.sval), buff) == STR_ERR)
                return E_INTERN;
            break;
        case O_BOOL : 
            if (term->value.bval == true)
            {
                if (StringInit(&(term->value.sval)) == STR_ERR)
                    return E_INTERN;
                if (StringFromChar(&(term->value.sval), "1") == STR_ERR)
                    return E_INTERN;
            }
            else 
            {
                if (StringInit(&(term->value.sval)) == STR_ERR)
                    return E_INTERN;
            }
            break;
        case O_NULL : 
            if (StringInit(&(term->value.sval)) == STR_ERR)
                return E_INTERN;
            break;
        case O_INT :
            sprintf(buff, "%d", term->value.ival);
            if (StringInit(&(term->value.sval)) == STR_ERR)
                return E_INTERN;
            if (StringFromChar(&(term->value.sval), buff) == STR_ERR)
                return E_INTERN;
            break;
        case O_STRING :   
        default : break;
        }
        term->type = O_STRING;
        return E_OK;
}

void intval(tVariable *term)
{
    int i;
    switch(term->type)
    {
        case O_DOUBLE :
            i = (int) term->value.dval;
            term->value.ival = i;
            break;
        case O_STRING : 
            i = strtol(term->value.sval.ptr, (char **)NULL, 10);
            term->value.ival = i;
            break;
        case O_NULL : 
            term->value.ival = 0;
            break;
        case O_BOOL :  
            i = (int) term->value.bval;
            term->value.ival = i;
            break;
        case O_INT : 
        default : break;
    }
    term->type = O_INT;
}

void doubleval(tVariable *term)
{
    double d;
    switch(term->type)
    {
        case O_INT :
            d = (double) term->value.ival;
            term->value.dval = d;
            break;
        case O_STRING : 
            d = strtod(term->value.sval.ptr, (char **)NULL);
            term->value.dval = d;
            break;
        case O_NULL : 
            term->value.dval = 0;
            break;
        case O_BOOL :  
            d = (double) term->value.bval;
            term->value.dval = d;
            break;
        case O_DOUBLE : 
        default : break;
    }
    term->type = O_DOUBLE;
}


/***********************************/
/*         KONIEC PREVODOV         */
/***********************************/

string get_string(){
    string s;
    char c;
    c = getchar();
    StringInit(&s);

    if(c == EOF)
        return s;
    while(c != '\n' && c != EOF){
        StringAppChar(&s, c);
        c = getchar();
    }
    return s;
}

tErrors put_string(tBSTNodePtr* RootPtr,tVariable* result,tParamItem *param,tVariable* tmp_param){
	int i=0;
	tErrors err=E_OK;
	while(param)
	{      
		if((err=internfun_param_chceck(RootPtr,param,tmp_param)) != E_OK) return err;
	 	strval(tmp_param);
	 	printf("%s",tmp_param->value.sval.ptr);
	 	i++;
	 	param=param->next;
	}
	result->value.ival=i;
	return E_OK;
}

int strLen(char *s){
    return strlen(s);
}

tErrors get_substring(tBSTNodePtr* RootPtr,tVariable* result,tParamItem *param,tVariable* tmp_param){
    string s;
    int i1;
    int i2;
    tErrors err=E_OK;
     if(StringInit(&(result)->value.sval) == STR_ERR) return E_INTERN;
    if(param==NULL) return E_SEMB;	
    if((StringInit(&s))== STR_ERR) return E_INTERN;  
   	/*Prvy parameter--string*/
    if((err=internfun_param_chceck(RootPtr,param,tmp_param)) != E_OK) return err;
    if((err=strval(tmp_param))!=E_OK) return err;
    if((StringCpy(&s,&(tmp_param)->value.sval))== STR_ERR) return E_INTERN;
    param=param->next;
    if(param==NULL) return E_SEMB;
    /*Druhy parameter--int*/
    if((err=internfun_param_chceck(RootPtr,param,tmp_param)) != E_OK) return err;
    intval(tmp_param);
    i1=tmp_param->value.ival;
    param=param->next;
    if(param==NULL) return E_SEMB;
     /*Treti parameter--int*/
    if((err=internfun_param_chceck(RootPtr,param,tmp_param)) != E_OK) return err;
    intval(tmp_param);
    i2=tmp_param->value.ival;
    if(i1<0 || i2<0 || i1>i2 || i1>=(strLen(s.ptr)) || i2>=(strLen(s.ptr))) return E_INTERN;
    int length = i2-i1;
    char *subStr;
    if ((subStr = (char *) malloc(length+1)) == NULL){
    	free(subStr);
        return E_INTERN;
    }
    int j = 0;

    while(i1 < i2){
        subStr[j] = s.ptr[i1];
        j++;
        i1++;
    }
    subStr[length] = '\0';

    for(int i = 0; i<strLen(s.ptr); i++){
        s.ptr[i] = subStr[i];
    }
    if(StringCpy(&(result)->value.sval,&s) == STR_ERR){
    	free(subStr);
    	return	E_INTERN;
    }
    free(subStr);
    return E_OK;
}

void sort_string(char *s){
//! strval(s);
    int left = 0;
    int right = strlen(s)-1;
    quickSort(s, left, right);
}

void swap(char *a, char *b){
   	int tmp = *a;
   	*a = *b;
   	*b = tmp;
}

void quickSort(char *s, int left, int right){
   int i;

   if(left < right){
   	// rozdel a spracuj
       i = partition(s, left, right);
       quickSort(s, left, i-1);
       quickSort(s, i+1, right);
   }
}

int partition(char *s, int left, int right){
   char PM;
   PM = s[left];

   int i = left;
   int j = right+1;

   while(TRUE)
   {
        do ++i; while(s[i] <= PM && i <= right);
        do --j; while(s[j] > PM);
        if(i >= j)
            break;
        swap(&s[i], &s[j]);
   }
   swap(&s[left], &s[j]);
   return j;
}

// vyhladavacia funkcia, ktora vyuziva Boyer Moore algoritmus s
// typom heuristiky bad character
tErrors find_string(tBSTNodePtr* RootPtr,tVariable* result,tParamItem *param,tVariable* tmp_param){
	string str; 
	string subStr;	
	tErrors err=E_OK;
    if(param==NULL) return E_SEMB;	
    StringInit(&str);
    StringInit(&subStr);       
	/*Prvy parameter*/
    if((err=internfun_param_chceck(RootPtr,param,tmp_param)) != E_OK) return err; 
    strval(tmp_param);
    StringCpy(&str,&(tmp_param)->value.sval);
    param=param->next;
    /*Druhy parameter*/
    if(param==NULL) return E_SEMB;
    if((err=internfun_param_chceck(RootPtr,param,tmp_param)) != E_OK) return err; 
    strval(tmp_param);
    StringCpy(&subStr,&(tmp_param)->value.sval);

    int SSLen = strlen(subStr.ptr);
    int SLen = strlen(str.ptr);
    // inicializacia vsetkych vyskytov na inicializacnu hodnotu -1
    int badC[ASCII_SIZE] = {INIT_V};

    // vyplni skutocnu hodnotu posledneho vyskytu znaku
    for (int i = 0; i < SSLen; i++)
         badC[(int) subStr.ptr[i]] = i;

    int shift = 0;  // posuv vzoru
    while(shift <= (SLen - SSLen)){
        int j = SSLen-1; // index vzoru
        // znizuj j, kym sa znaky vzoru a textu zhoduju
        while(j >= 0 && subStr.ptr[j] == str.ptr[shift+j])
            j--;

        // ak je vzor pritomny v sucasnom shifte
        if (j < 0){
        	result->value.ival=shift;
            return E_OK;
        } //sa podretazec vysytuje na konci retazca
            // posunie vzor tak, ze dalsi znak v retazci je zarovnany
            //s jeho poslednym vyskytom vo vzore
        else{
            //posunie vzor o jeho dlzku a overi, ze shift je pozitivny
            int s = badC[(int) str.ptr[shift+j]]; //pomocna premenna
            if ((j - s) > 1)
                shift += j - s;
            else
                shift++;
        }
    }
    result->value.ival=-1;
    return E_OK;
}
