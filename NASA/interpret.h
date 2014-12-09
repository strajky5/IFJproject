
#ifndef _INTERPRET_TYPES_
#define _INTERPRET_TYPES_

#include "types.h"
#include "strings.h"
#include "errors.h"
#include "ial.h"

typedef struct	retval{                          /* prvek zasobniku*/
    tVariable* retval_data;						// ukazatel na hodnota na zasobniku
    struct  retval* rptr;						// ukazatel na prvek pod vrcholem
} tRetval;

typedef struct rv_stack{						// zasobnik
    tRetval* top;    							// ukazatel na vrchol zasobniku
}tRv_stack;

//prototypy
void SInitRe (tRv_stack *S);					// funkce pro inicializaci zasobniku
tVariable* STopRe (tRv_stack *S);				// funkce na cteni dat na vrcholu zasobnika vraci ukazatel na prvek vrcholu
void SPopRe(tRv_stack *S);						// zruseni prvku na vrcholu zasobniku 
tErrors interpret(tFunTableItem* function);		// f
void SDisposeRe(tRv_stack *S);					// funkce na zruseni celeho zasobnika

#endif
