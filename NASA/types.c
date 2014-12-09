#include <stdio.h>
#include <stdlib.h>
#include "types.h"



tTape *Tape;                //globalna premenna -> ukazuje na aktualni zoznam instrukcii funkce
tAllocations allocs;        // seznam alokovanych pointeru
tFunTable funTable;


void initAllocs() {                     // inicializace seznamu allocs
    allocs.first = NULL;
    allocs.last = NULL;
}

void *allocate(size_t bytes) {          // alokuje novou pamet a vlozi alokovany pointer do seznamu
    void *alloc = malloc(bytes);
    if((alloc != NULL) && (insertAlloc(alloc) != E_OK)) {
        return NULL;
    } else {
        return alloc;
    }
}

tErrors insertAlloc(void *pointer) {    // vlozi alokovany pointer do globalniho seznamu allocs
    if((allocs.first) == NULL) {
        if((allocs.first = malloc(sizeof(tAllocItem))) == NULL) {
            return E_INTERN;
        } else {
            allocs.first->allocation = pointer;
            allocs.first->next = NULL;
            allocs.first->previous = NULL;
            allocs.last = allocs.first;
        }
    } else {
        tAllocItem *tmp = malloc(sizeof(tAllocItem));
        tAllocItem *tmp2 = allocs.last;
        if(tmp != NULL) {
            allocs.last->next = tmp;
            allocs.last = tmp;
            allocs.last->allocation = pointer;
            allocs.last->previous = tmp2;
            allocs.last->next = NULL;
        } else {
            return E_INTERN;
        }
    }
     return E_OK;
}

void disposeAlloc(tAllocItem *item) {
    if(item != NULL) {
        if(item == allocs.first) {
            allocs.first = item->next;
            if(item->next != NULL) {
                allocs.first->next = item->next->next;
                allocs.first->previous = NULL;
            }
            free(item);
            return;
        } else {
            if(item == allocs.last) {
                allocs.last = item->previous;
                if(item->previous != NULL) {
                    allocs.last->previous = item->previous->previous;
                    allocs.last->next = NULL;
                }
                free(item);
                return;
            } else {
                item->previous->next = item->next;
                item->next->previous = item->previous;
                free(item);
                return;
            }
        }
    }
}

tAllocItem *searchAlloc(void *pointer) {
    tAllocItem *iter = allocs.first;
    while(iter != NULL) {
        if((iter->allocation) == pointer) {
            return iter;
        } else {
            iter = iter->next;
        }
    }
    return NULL;
}

void freeAllocs() {                     // uvolni veskerou pamet programu
    tAllocItem *next_item = allocs.first;
    tAllocItem *tmp;
    while(next_item != NULL) {
        free(next_item->allocation);
        tmp = next_item;
        next_item = next_item->next;
        free(tmp);
    }
}

void freePointer(void *pointer) {
    tAllocItem *tmp = searchAlloc(pointer);
    if(tmp != NULL) {
        free(pointer);
        disposeAlloc(tmp);
    }
}

void *reallocate(void *pointer, size_t bytes) {
    tAllocItem *old = searchAlloc(pointer);
    if(old == NULL) {
        return NULL;
    }
    void *alloc = realloc(pointer, bytes);
    if(alloc != NULL) {
        old->allocation = alloc;
        return alloc;
    } else {
        return NULL;
    }
}

//Vlozi na koniec zoznamu prazdnu strukturu TapeItem
tErrors TapeNewVoidLastItem()    
{
    tTapeItem *temp;
    if ((temp = allocate(sizeof(struct TapeItem))) == NULL)
        return E_INTERN;

    temp->op1 = NULL;
    temp->op2 = NULL;
    temp->result = NULL;
    temp->previous = Tape->last;
    temp->next = NULL;

    if ((Tape->first) != NULL)
        Tape->last->next = temp;
    else
        Tape->first = temp;
    Tape->last = temp;
    return E_OK;
}

//inicializuje globalnu premennu Tape
void TapeInit()
{
        Tape->first = NULL;
        Tape->last = NULL;
        Tape->active = NULL;
}

void initFunTable() {
    funTable.first = NULL;
    funTable.last = NULL;
}

tErrors insertFunTableItem(string *name, tParamList *params, tTape *tape) {
    tFunTableItem *item = allocate(sizeof(tFunTableItem));
    if(item != NULL) {
        if(StringInit(&(item->name)) != STR_OK) return E_INTERN;
        if(StringCpy(&(item->name), name) != STR_OK) return E_INTERN;
        item->param_count = 0;
        item->param_list = params;
        item->tape_ptr = tape;
        item->next = NULL;
        if(funTable.first == NULL) {
            funTable.first = item;
        } else {
            funTable.last->next = item;
        }
        funTable.last = item;
    }
    return E_OK;
}

tErrors insertFunTableItemEmbedded(char *name) {
    tFunTableItem *item = allocate(sizeof(tFunTableItem));
    if(item != NULL) {
        if(StringInit(&(item->name)) != STR_OK) return E_INTERN;
        if(StringFromChar(&(item->name), name) != STR_OK) return E_INTERN;
        item->param_count = 0;
        item->param_list = NULL;
        item->tape_ptr = NULL;
        item->next = NULL;
        if(funTable.first == NULL) {
            funTable.first = item;
        } else {
            funTable.last->next = item;
        }
        funTable.last = item;
    }
    return E_OK;
}

tFunTableItem *searchFunTable(string *name) {
    tFunTableItem *iter = funTable.first;
    while(iter != NULL) {
        if(StringCmp(&(iter->name), name) == 0) {
            return iter;
        }
        iter = iter->next;
    }
    return NULL;
}

void initParamlist(tParamList *paramlist) {
    if(paramlist != NULL) {
        paramlist->first = NULL;
        paramlist->last = NULL;
    }
}

tErrors insertParameter(tParamList *paramlist, string *name) {
    tParamItem *item = allocate(sizeof(tParamItem));
    if((paramlist != NULL) && (name != NULL) && (item != NULL)) {
        if(StringInit(&(item->name)) != STR_OK) return E_INTERN;
        if(StringCpy(&(item->name), name) != STR_OK) return E_INTERN;
        item->next = NULL;
        if((paramlist->first) == NULL) {
            paramlist->first = item;
        } else {
            paramlist->last->next = item;
        }
        paramlist->last = item;
        return E_OK;
    } else {
        return E_INTERN;
    }
}

tErrors insertParameterVal(tParamList *paramlist, tOperand type, tValue val) {
    tParamItem *item = allocate(sizeof(tParamItem));
    if((paramlist != NULL) && (item != NULL)) {
        if(StringInit(&(item->name)) != STR_OK) return E_INTERN;
        item->type = type;
        switch(type) {
        case O_NULL:
            item->val.bval = NULL;
        case O_BOOL:
            item->val.bval = val.bval;
            break;
        case O_INT:
            item->val.ival = val.ival;
            break;
        case O_DOUBLE:
            item->val.dval = val.dval;
            break;
        case O_STRING:
            if(StringInit(&(item->val.sval)) != STR_OK) return E_INTERN;
            if(StringCpy(&(item->val.sval), &(val.sval)) != STR_OK) return E_INTERN;
            break;
        default:
            return E_INTERN;
        }
        item->next = NULL;
        if((paramlist->first) == NULL) {
            paramlist->first = item;
       } else {           
            paramlist->last->next = item;
        }
	paramlist->last = item;
        return E_OK;
    } else {
        return E_INTERN;
    }
}

tParamItem *searchParameter(tParamList *paramlist, string *name) {
    if((paramlist != NULL) && (name != NULL)) {
        tParamItem *iter = paramlist->first;
        while(iter != NULL) {
            if(StringCmp(&(iter->name), name) == 0) return iter;
            iter = iter->next;
        }
    }
    return NULL;
}
