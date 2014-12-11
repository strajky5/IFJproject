#ifndef LIST
#define LIST

#include <stdlib.h>
#include <stdbool.h>
#include "errors.h"
#include "str.h"


typedef struct ParamItem tParamItem;
typedef struct TapeItem tTapeItem;

typedef enum {                  // typ operandu na pasku
    O_NOTDATA,                  // 0 zadny operand
    O_INT,                      // 1 integer
    O_REAL,                     // 2 real
    O_BOOL,                     // 3 boolean
    O_STRING,                   // 4 string
    NONTRM,                     // 5 nonterm
    FUNCTION,                   // 6 funkce
    TAPE_POINTER,               // 7 ukazatel na jiny prvek pasku
    PARAM_POINTER               // 8 ukazatel na jiny prvek parametru funkce
} tOperand;

typedef enum {                  // instrukce na pasku
    ADD,                        // 0 +
    MUL,                        // 1 *
    SUB,                        // 2 -
    DIV,                        // 3 /
    EQ,                         // 4 =
    NEQ,                        // 5 <>
    LESS,                       // 6 <
    MORE,                       // 7 >
    EQL,                        // 8 <=
    EQM,                        // 9 >=
    ASSIGN,                     // 10 :=
    FUNC,                       // 11 if,while
    CALL,                       // 12 volani funkce
    NOP,                         // 13 prazdna instrukce
    JUMPN,                      // 14 instrukce pro skok podmienka
    JUMP,
    NOPJ
} tInstruction;

typedef union {                 // hodnota daneho tokenu po konvertovani
    int ival;                   // promenna pro integer
    double rval;                // promenna pro real
    bool bval;                  // promenna pro boolean
    bool valFull;               // ma hodnotu ?
    string sval;                // promenna pro string
    tTapeItem *tape_pointer;    // typ ukazatel pro vysledky na pasce
    tParamItem *param_pointer;  // typ ukazatel pro vysledky parametru funkce
} tValue;

typedef struct {                // prvek pro ulozeni informaci o tokenu ID(INT,REAL,STRING,BOOL,...)
    tOperand type;              // jaky je to typ
    string name;                // nazev tokenu
    tValue value;               // hodnota tokenu
} tVariable;

//############################# TAPE ##############################################
struct TapeItem{                // prvek instrukcni pasky
    tInstruction instruction;   // instrukce, ktera se ma provest
    tVariable *op1;             // oprand 1
    tVariable *op2;             // operand 2
    tVariable *result;          // vysledek
    tTapeItem *previous;        // predchozi prve pasky
    tTapeItem *next;            // nasledujici prvek pasky
};

typedef struct {                // insrukcni paska
    tTapeItem *first;           // ukazatel na prvni prvek instrukcni pasky
    tTapeItem *last;            // ukazatel na posledni prvek instrukcni pasky
    tTapeItem *active;          // aktualni prvek instrukcni pasky
} tTape;
//###########################################################################

//############################### FUNKCE ######################################
//---------------------------- SEZNAM FUNKCI ----------------------------------
//---------------------- SEZNAM PARAMETRU FUNKCE --------------------------------
struct ParamItem {              // prvek pro ulozeni informaci o parametrech funkce
    tOperand type;              // typ parametru funkce
    tValue value;                 // hodnota parametru funkce
    string name;                // nazev parametru
    struct ParamItem *next;     // ukazatel na nasledujici parametr funkce
};

typedef struct {                // jednosmerny seznam parametru funkce
    tParamItem *first;          // ukazatel na prvni paramaetr funkce
    tParamItem *last;           // ukazatel na posledni parametr funkce
}tParamList;

typedef struct FunListItem {    // prvek tabulky pro funkce
    string name;                // nazev prvku
    int param_count;            // pocitadlo parametru
	int forward;
    tOperand ret_type;          // navratovy typ funkce
    tParamList *param_list;     // ukazatel na seznam parametru
    tTape *tape_ptr;            // ukazatel na pasku, kde je funkce
    struct FunListItem *next;  // ukazatel na dalsi prvek seznamu
} tFunListItem;

typedef struct {                 // seznam funkci
    tFunListItem *first;         // ukazatel na prvni prvek tabulky
    tFunListItem *last;          // ukazatel na posledni prvek tabulky
} tFunList;





//############################### ALLOC ######################################
typedef struct AllocItem{       // prvek seznamu alkovanych prvku
    void *allocation;           // ukazatel na alokovanou pamet
    size_t alloc_size;          // alokovana velikost size_t - 16bit
    struct AllocItem *next;     // ukazatel na dalsi prvek
    struct AllocItem *previous; // ukazatel na predchozi prvek
} tAllocItem;

typedef struct {                // seznam s alokovanymi prvky
    tAllocItem *first;          // ukazatel na prvni prvek seznamu
    tAllocItem *last;           // ukazatel na posledni prvek seznamu
} tAllocations;



void initFunList();                                                          // inicializace tabulky funkci
tErrors insertFunListItem(string *name, tOperand r_type, tParamList *params, int forward, int paramcount); // vlozeni funkce do tabulky funkci
tErrors insertFunListItemEmbed(char *name);                                  // pouziva se pro vlozeni vestavenych funkci do seznamu funkci
tFunListItem *searchFunList(string *name);                                  // vyhleda danou funkci v tabulce a vraci ukazatel na ni
tFunListItem *searchFunListN(string *name);
tFunListItem *searchFunListFW(string *name);
void initParamlist(tParamList *paramlist);                                    // incializace seznamu parametru
tErrors insertParam(tParamList *paramlist, string *name, tOperand type);   // vlozeni parametru do seznamu
tParamItem *searchParam(tParamList *paramlist, string *name);                 // hledani parametru v seznamu
tErrors searchFunListCH();

void initAlloc();                                 // inicializuje seznamu alokovanych pointru
tErrors insertAlloc(void *pointer);               // alokace a vlozeni alokovaneho pointeru do seznamu
void disposeAlloc(tAllocItem *item);              // odebere polozku ze seznamu alokaci
tAllocItem *searchAlloc(void *pointer);           // vyhleda dany pointer v seznamu alokaci a vraci ukazatel na vyhledanou polozku

void *allocate(size_t bytes);                     // alokuje novy pointer, vlozi jej do seznamu pointru a vraci ukazatel na nej
void *reallocate(void *pointer, size_t bytes);    // realokuje pointer a uloz alokovany pointer zpet do seznamu alokaci
void freePointer(void *pointer);                  // uvolni pamet na kterou ukazuje pointer
void freeAlloc();                                 // uvolni veskerou pamet, ktera byla alokovana pomoci funkce allocate/reallocate

void IniTape();                                   // inicializuje pasku
tErrors InsertEmptyItemTape();                    // vlozi prazny prvek na konec pasky
void WriteTape();
#endif
