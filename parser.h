/* 
* Názov projektu: Implementace interpretu imperativního jazyka IFJ14 
* Dátum: 12/2014
* Autori: Matúš Cimerman, xcimer00
*         Tomáš Hynek ,   xhynek09
*         David Novák ,   xnovak0r
*         Michal Sedlák , xsedla0m
*         Jan Jorenek  ,  xjoren01
*/
#ifndef PAR_H
#define PAR_H

#include <stdio.h>
#include "scaner.h"
#include "str.h"
#include "errors.h"
#include "ial.h"
#include "expressions.h"

extern FILE*f;
extern tToken T;
extern tOperand res;
int parser();

#endif

