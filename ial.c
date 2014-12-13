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

   while(1)
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
    int j;
    
    if ( left < right)
    {
        j = partition( string, left, right);
        quickSort( string, left, j-1);
        quickSort( string, j+1, right);
    }
}
/*
//########## BOYER-MOOREUV ALGORITMUS ################
//Boyer-Mooreov algoritmus vyhladavania zadaneho pattern_stru "pattern_str" s poctom znakov "length_p"
//v retazci "find_str" s poctom znakov "length_f"
//funkcia vracia index prveho znaku najdeneho pattern_stru v retazci 
//(v pripade neuspechu vyhladavania vracia -1)
// pattern_str - vzorovy string, length_p - delka vzoroveho stringu     -- v tomto stringu hledam
// find_str - hledany podstring, length_f - delka hledaneho podstringu  -- tento  string hledam
int findBM(char *pattern_str, int length_p, char *find_str, int length_f) 
{
    int Array_Pattern[MAX_LENGTH_pattern_str];              // ciselne pole velikosti delky vzoroveho stringu
    int Array_Occurence[MAX_LENGTH_ABC];                    // ciselne pole velikosti abecedy 
 
    prefixes_occurence(pattern_str, length_p, Array_Pattern);   //
    prefixes_pattern(pattern_str, length_p, Array_Occurence);   //
 
    int j = 0;
    while (j <= length_f - length_p) {
        for (int i = length_p - 1; i  >= 0 && pattern_str[i] == find_str[i + j]; i--);
        if (i < 0) 
        {
            return j;
            //j += Array_Pattern[0];
        }
        else if (Array_Pattern[i] >= Array_Occurence[find_str[i + j]] - length_p + 1 + i) 
            j += Array_Pattern[i];
        else 
            j += Array_Occurence[find_str[i + j]] - length_p + 1 + i;
    }
    return 0;   // pokud BMA nenajdepodreteec, vraci o
}
//funkcia na vytvorenie pomocneho pola pre BM algoritmus 
void prefixes_occurence(char *pattern_str, int length, int Array_Pattern[]) 
{
    int suff[MAX_LENGTH_pattern_str];
    suffixes(pattern_str, length, suff);

    for (int i = 0; i  <  length; i++) 
        Array_Pattern[i] = length;

    int j = 0;
    
    for (i = length - 1; i >= -1; i--)
        if (i == -1 || suff[i] == i + 1)
            for (; j < length - 1 - i; j++)
                if (Array_Pattern[j] == length) Array_Pattern[j] = length - 1 - i;
    for (i = 0; i  <= length - 2; i++) 
        Array_Pattern[length - 1 - suff[i]] = length - 1 - i;
}
//funkcia na vytvorenie pomocneho pola pre BM algoritmus 
//(pre funkciu, ktora vytvara pomocne pole pre BM algoritmus)
void suffixes(char *pattern_str, int length, int *suff) 
{
    int f;
    suff[length - 1] = length;
    int g = length - 1;

    for (int i=length - 2; i >= 0; --i) 
    {
        if ( i > g && suff[i + length - 1 - f]  < i - g)
            suff[i] = suff[i + length - 1 - f];
        else 
        {
            if (i < g) g = i;
                f = i;
            while (g  >= 0 && pattern_str[g] == pattern_str[g + length - 1 - f]) 
                g--;
            suff[i] = f - g;
        }
    }
}
//funkcia na vytvorenie pomocneho pola pre BM algoritmus
void prefixes_pattern(char *pattern_str, int length, int Array_Occurence[]) 
{
    for (int i = 0; i < MAX_LENGTH_ABC; i++)
        Array_Occurence[i] = length;
    for (i = 0; i < length - 1; i++)
        Array_Occurence[pattern_str[i]] = length - i - 1;
}
*/
//////////////////////BMA verzia mišo/////////////////////

//// Definovanie poľa posunov a dĺžky vstupnej abecedy

/*private static int shift[]; // pole posunu
private static final int K = 512; // konecne pole s ktorym sa bude porovnavat spodny retazec


 ////// Počiatočné vypočítanie prvkov poľa posunov

 private static void UrobPosun(String_word) { // funkcia na posun
shift = new int[K]; /////////////  posun si inicializujeme ako pole
int M = word.length(); //////// do N vlozime dlzku slova 
for (int i = 0; i < K; i++) { // posuvaj dokym nenarazis nenarazis na rovnaky index
 shift[i] = M; //posun dlzku slova po tamade kde sa nachadza index
    }
for (int i = 0; i < M; i++) {
 shift[(int)(word.charAt(i))] = M - i -1;
    }
 }


/////// Hlavná metóda


public static int BoyerMoore(String_vzor, String_retaz) {
 UrobPosun();
    int M = vzor.length();
    int N = retaz.length();
    int i, j;
    for (i = M - 1, j = M - 1; j > 0; i--, j--) {
        while (retaz.charAt(i) != vzor.charAt(j)) {
    int x = shift[(int)(retaz.charAt(i))];
        if (M - j > x)
            i += M - j;
        else
             i += x;
        if (i >= N)
    return -1;
        j = M - 1;
        }
    }
   return i;
}*/