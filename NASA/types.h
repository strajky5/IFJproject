#include <stdlib.h>
#include "errors.h"
#include "strings.h"
#include <stdbool.h>

typedef struct tDLElem {
        int data;
        struct tDLElem *lptr;
        struct tDLElem *rptr;
} *tDLElemPtr;

typedef struct {
    tDLElemPtr First;
    tDLElemPtr Act;
    tDLElemPtr Last;
} tDLList;

typedef enum {
    ADD,
    MUL,
    SUB,
    DIV,
    CON,
    EQ,
    NEQ,
    EQL,
    EQM,
    MORE,
    LESS,
    NOP,
    NGOTO,
    RET,
    ASSIGN,
    FUNC
} tInstruction;

typedef enum {
    O_BOOL,
    O_INT,
    O_DOUBLE,
    O_STRING,
    O_NULL,
    NONTERM,
    VARIABLE,
    FUNCTION,
    TAPE_POINTER,
    PARAM_POINTER
} tOperand;

typedef struct TapeItem tTapeItem;
typedef struct ParamItem tParamItem;

typedef union {
    bool bval;
    int ival;
    double dval;
    string sval;
    tTapeItem *tape_pointer;
    tParamItem *param_pointer;
} tValue;

typedef struct {
    tOperand type;
    string name;
    tValue value;
} tVariable;

struct TapeItem{
    tInstruction instruction;
    tVariable *op1;
    tVariable *op2;
    tVariable *result;
    tTapeItem *previous;
    tTapeItem *next;
};

typedef struct {
    tTapeItem *first;
    tTapeItem *last;
    tTapeItem *active;
} tTape;

struct ParamItem {
    tOperand type;
    tValue val;
    string name;
    struct ParamItem *next;
};

typedef struct {
    tParamItem *first;
    tParamItem *last;
} tParamList;

typedef struct FunTableItem{
    string name;
    int param_count;
    tParamList *param_list;
    tTape *tape_ptr;
    struct FunTableItem *next;
} tFunTableItem;

typedef struct {
    tFunTableItem *first;
    tFunTableItem *last;
} tFunTable;

typedef struct AllocItem{
    void *allocation;
    size_t alloc_size;
    struct AllocItem *next;
    struct AllocItem *previous;
} tAllocItem;

typedef struct {
    tAllocItem *first;
    tAllocItem *last;
} tAllocations;


//Pridane funkcie na globalnu premennu Tape
tErrors TapeNewVoidLastItem();
void TapeInit();

tErrors initTape(tTape *tape);

tErrors insertTapeItem(tTape *tape, tTapeItem *item);


void initAllocs();                                          // inicializuje seznam alokaci
tErrors insertAlloc(void *pointer);                         // vlozi alokovany pointer do seznamu
void disposeAlloc(tAllocItem *item);                        // odpere polozku ze seznamu alokaci
tAllocItem *searchAlloc(void *pointer);                     // vyhleda pointer v seznamu alokaci a vrati ukazatel na vyhledanou polozku
void *allocate(size_t bytes);                               // odbdoba alloc, ulozi alokovay pointer do seznamu alokaci
void *reallocate(void *pointer, size_t bytes);              // realloc + to vyse
void freePointer(void *pointer);                            // free, odebere pointer ze seznamu alokaci
void freeAllocs();                                          // uvolni veskerou pamet alokovanou pomoci funkce allocate/reallocate

void initFunTable();
tErrors insertFunTableItem(string *name, tParamList *params, tTape *tape);
tErrors insertFunTableItemEmbedded(char *name);
tFunTableItem *searchFunTable(string *name);

void initParamlist(tParamList *paramlist);
tErrors insertParameter(tParamList *paramlist, string *name);
tErrors insertParameterVal(tParamList *paramlist, tOperand type, tValue val);
tParamItem *searchParameter(tParamList *paramlist, string *name);


#endif
