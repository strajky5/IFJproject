#include "list.h"
#include "str.h"

void interpret(TapeItem **ta_table, tBTSUzolPtr root);
void znic_navestie(int *jump_array);
void najdi_navestie(TapeItem **Table, int **jump_array)

TStack stack;  //Globálna premenná  pre zásobník

struct TStackItem{   			// struktura pro prvek zasobnika
								// obsahuje:
tParamItem *op1;					// ukazatel na operand
TStack * pdown;					// ukazatel na prvek pod nim
               
};

struct TStack{   				// struktura zasobnika

TStackItem * top;				// ukazatel na vrchol zasobniku
               
};

