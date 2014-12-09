//jednoducha knihovna pro praci s nekonecne dlouhymi retezci
#include <strings.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "str.h"
#include "list.h"



int strInit(string *s)
// funkce vytvori novy retezec
{
  
   if ((s->str = (char*) allocate(STR_LEN_INC)) == NULL)
      return STR_ERROR;
   s->str[0] = '\0';
   s->length = 0;
   s->allocSize = STR_LEN_INC;
   return STR_SUCCESS;
}


void strFree(string *s)
// funkce uvolni retezec z pameti
{
   free(s->str);
}

void strClear(string *s)
// funkce vymaze obsah retezce
{
   s->str[0] = '\0';
   s->length = 0;
}

int strAddChar(string *s1, char c)
// prida na konec retezce jeden znak
{
   if (s1->length + 1 >= s1->allocSize)
   {
      // pamet nestaci, je potreba provest realokaci
      if ((s1->str = (char*) realloc(s1->str, s1->length + STR_LEN_INC)) == NULL)
         return STR_ERROR;
      s1->allocSize = s1->length + STR_LEN_INC;
   }
   s1->str[s1->length] = c;
   s1->length++;
   s1->str[s1->length] = '\0';
   return STR_SUCCESS;
}

int strCopystring(string *s1, string *s2) //kopirovanie stringov
// prekopiruje retezec s2 do s1
{
   int newLength = s2->length;
   if (newLength >= s1->allocSize)
   {
      // pamet nestaci, je potreba provest realokaci
      if ((s1->str = (char*) realloc(s1->str, newLength + 1)) == NULL)
         return STR_ERROR;
      s1->allocSize = newLength + 1;
   }
   strcpy(s1->str, s2->str);
   s1->length = newLength;
   return STR_SUCCESS;
}

int strFromChar(string *S1, char *S2)
{
    if(S2 != NULL) {
        int i = 0;
        strClear(S1);
        while (S2[i] != '\0') {
            if(strAddChar(S1, S2[i]) != STR_SUCCESS) {
                return STR_ERROR;
            }
            i++;
        }
    }
    return STR_SUCCESS;
}

int strCmpstring(string *s1, string *s2)
// porovna oba retezce a vrati vysledek
{
   return strcasecmp(s1->str, s2->str);
}

int strCmpConstStr(string *s1, char* s2)
// porovna nas retezec s konstantnim retezcem
{
   return strcasecmp (s1->str, s2);
}

char *strGetStr(string *s)
// vrati textovou cast retezce
{
   return s->str;
}

int strGetLength(string *s)
// vrati delku daneho retezce
{
   return s->length;
}
