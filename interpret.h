#include "list.h"
#include "str.h"
#include "interpret.c"
#include "ial.h"
#include "scaner.h"
#include <math.h>

typedef struct Stack TStack;
typedef struct StackItem TStackItem;
struct StackItem{   			// struktura pro prvek zasobnika
								// obsahuje:
tParamItem *op1;					// ukazatel na operand
TStack * pdown;					// ukazatel na prvek pod nim

};

void stackinit(TStack *stack);								//funkce na inicializaci zasobniku
tErrors stackPush(TStack *stack,tParamItem *op);			// pro pushnuti na zasobnik
tParamItem* TStackTopPop(TStack *stack);					// pro vziti ze zasobniku
void StackDeleteDataDelete(TStack *stack);					// pro uvolneni zasobniku z pameti
quickSort( int a[], int l, int r);							// pro serazeni prvku pole
int partition( int a[], int l, int r);						// pomovna funkce pro quicksort
tParamItem *SearchStackName(string*Search);
string *conc(string*s1,string*s2);
tErrors interpret()


TStack stack;  //Globálna premenná  pre zásobník

struct TStackItem{   			// struktura pro prvek zasobnika
								// obsahuje:
tParamItem *op1;					// ukazatel na operand
TStack * pdown;					// ukazatel na prvek pod nim

};

struct TStack{   				// struktura zasobnika

TStackItem * top;				// ukazatel na vrchol zasobniku

};

