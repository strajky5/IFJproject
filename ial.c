#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <stdbool.h>
#include "ial.h"



//############# BINARNI VYHLEDAVACI STROM ##############
void BSTInit (tBSTNodePtr *RootPtr) 
{
    *RootPtr = NULL;
}

tVariable* BSTSearch (tBSTNodePtr RootPtr, string name)  
{
    if (!RootPtr)
       return NULL;
    if (!strCmpstring(&RootPtr->tree_data->name, &name))
        return RootPtr->tree_data;
    else if (strCmpstring(&RootPtr->tree_data->name, &name) > 0)
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
        if (strInit(&(temp)->tree_data->name))
            return E_INTERN;
        if (strCopystring(&temp->tree_data->name, &name))
            return E_INTERN;

        temp->tree_data->type = var->type;
        if(temp->tree_data->type == O_BOOL)
            temp->tree_data->value.bval = var->value.bval;
        if(temp->tree_data->type == O_INT)
            temp->tree_data->value.ival = var->value.ival;
        if(temp->tree_data->type == O_REAL)
            temp->tree_data->value.rval = var->value.rval;
        if(temp->tree_data->type == O_STRING)
        {
            if (strInit(&(temp->tree_data->value.sval)) == STR_ERROR)
                return E_INTERN;
            if (strCopystring(&(temp->tree_data->value.sval), &var->value.sval) == STR_ERROR)
                return E_INTERN;
        }
		
        temp->LPtr = NULL;
        temp->RPtr = NULL;
        *RootPtr = temp;
    }
    else if (strCmpstring(&(*RootPtr)->tree_data->name, &name) > 0)          
        BSTInsert (&(*RootPtr)->LPtr, name,var);
    else if (strCmpstring(&(*RootPtr)->tree_data->name, &name) < 0)
        BSTInsert (&(*RootPtr)->RPtr, name,var);

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


//################### QUICK SORT #####################
void swap(char *a, char *b)
{
    int tmp = *a;
    *a = *b;
    *b = tmp;
}

int partition (char *s, int left, int right)
{
    int pivot,i,j;
    pivot = s[left];
    i = left;
    j = right+1;

   while(TRUE)
   {
        do ++i; while(s[i] <= pivot && i <= right);
        do --j; while(s[j] > pivot);
        if(i >= j)
            break;
        swap(&s[i], &s[j]);
   }
   swap(&s[left], &s[j]);
   return j;
}
void quickSort ( char *string, int left, int right)
{
    int i;
    
    if ( left < right)
    {
        j = partition( s, left, right);
        quickSort( s, left, i-1);
        quickSort( s, j+1, right);
    }
}

