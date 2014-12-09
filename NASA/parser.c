/* 
* Názov projektu: Implementace interpretu imperativního jazyka IFJ13. 
* Dátum: 
* Autori: - Béder Michal ,xbeder00
*         - Bělehrad Tomáš ,xbeleh03
*         - Čižmarik Roman ,xcizma04
*         - Krška Matúš ,xkrska05
*         - Ambrušová Eva ,xambru01
*/

#include <ctype.h>
#include <stdbool.h>
#include "parser.h"
#include "expressions.h"

tErrors error;
tTape *mainTape;

tErrors insertEmbeddedFunctions() {


    string *main_func = allocate(sizeof(string));
    mainTape = allocate(sizeof(tTape));
    if(mainTape == NULL) return E_INTERN;
    Tape = mainTape;
    TapeInit();

    // Vlozeni main funkce do tabulky funkci
    if(main_func != NULL) {
        if(StringInit(main_func) == STR_ERR) return E_INTERN;
        if(StringFromChar(main_func, "main") == STR_ERR) return E_INTERN;
    }
    if(insertFunTableItem(main_func, NULL, mainTape) != E_OK) return E_INTERN;
    // Vlozeni vestavenych funkci...
    if(insertFunTableItemEmbedded("boolval") != E_OK) return E_INTERN;
    if(insertFunTableItemEmbedded("strval") != E_OK) return E_INTERN;
    if(insertFunTableItemEmbedded("intval") != E_OK) return E_INTERN;
    if(insertFunTableItemEmbedded("doubleval") != E_OK) return E_INTERN;
    if(insertFunTableItemEmbedded("get_string") != E_OK) return E_INTERN;
    if(insertFunTableItemEmbedded("strlen") != E_OK) return E_INTERN;
    if(insertFunTableItemEmbedded("sort_string") != E_OK) return E_INTERN;
    if(insertFunTableItemEmbedded("get_substring") != E_OK) return E_INTERN;
    if(insertFunTableItemEmbedded("find_string") != E_OK) return E_INTERN;
    if(insertFunTableItemEmbedded("put_string") != E_OK) return E_INTERN;

    StringFree(main_func);
    freePointer(main_func);

    return E_OK;
}

tErrors parser() {  

    initFunTable();
    insertEmbeddedFunctions();

    error = program();

    return error;
}

tErrors checkToken(tTypes type) {
    if(T.type != type) {
        if(T.type == T_ERRORTOKEN)  return E_LEX;
        if(T.type == T_ERRORSYSTEM) return E_INTERN;
        return E_SYN;
    }
    return E_OK;
}

// Pravidla:
// <program> -> function id(<param-list>) {<block-list>} <program>
// <program> -> <stat> <program>
// <program> -> EOF
tErrors program() {
    gettoken();
    switch(T.type) {
        case T_KEYWORD:
            // function definition
            if(!StringCharCmp(&(T.s), "function")) {

                // Initialize needed structures
                tTape *new_tape = allocate(sizeof(tTape));
                tParamList *new_params = allocate(sizeof(tParamList));
                string name;
                if(new_tape == NULL) return E_INTERN;
                if(new_params == NULL) return E_INTERN;
                initParamlist(new_params);
                Tape = new_tape;
                TapeInit();
                if(StringInit(&name) != STR_OK) return E_INTERN;
                gettoken();
                if((error = checkToken(T_ID)) != E_OK) return error;
                // check for redefinition
                if(searchFunTable(&(T.s)) != NULL) return E_SYN;
                StringCpy(&name, &(T.s));
                gettoken();
                if((error = checkToken(T_PAR_L)) != E_OK) return error;
                if((error = defParamList(new_params)) != E_OK) return error;
                if((error = checkToken(T_PAR_R)) != E_OK) return error;
                gettoken();

                // insert to funTable
                if(insertFunTableItem(&name, new_params, new_tape) != E_OK) return E_INTERN;
                if((error = checkToken(T_BRACE_L)) != E_OK) return error;
                if((error = blocklist()) != E_OK) return error;
                if((error = checkToken(T_BRACE_R)) != E_OK) return error;

                //DEBUG CODE
                //printf("New function %s tape:\n\n", name.ptr);
                //printf("\n\n");
                Tape = mainTape;
                if((error = program()) != E_OK) return error;
                return E_OK;
            }
            // any other keyword except else is allowed as statement
            if(StringCharCmp(&(T.s), "else")) {
                if((error = stat()) != E_OK) return error;
                if((error = program()) != E_OK) return error;
                return E_OK;
            } else {
                return E_SYN;
            }
            break;
        case T_ID:
        case T_IDVAR:
            // function call or variable assigment
            if((error = stat()) != E_OK) return error;
            if((error = program()) != E_OK) return error;
            return E_OK;
        case T_EOF:
            return E_OK;
        default:
            // anything else is syntax error...
            return E_SYN;
    }
    return E_INTERN;
}

// Pravidla:
// <param-list> -> <item> <item-list>
// <param-list> -> Îµ
// <item> -> idvar
// <item> -> const
// <item> -> int
// <item> -> double
// <item> -> string
tErrors paramlist(tParamList *params) {
    gettoken();
    tValue value;
    switch (T.type) {
        case T_IDVAR:
            if(insertParameter(params, &(T.s)) != E_OK) return E_INTERN;
            if((error = itemlist(params)) != E_OK) return error;
            return E_OK;
        case T_KONST:
            value.bval = StringCharCmp(&(T.s), "false");
            if(insertParameterVal(params, O_BOOL, value) != E_OK) return E_INTERN;
            if((error = itemlist(params)) != E_OK) return error;
            return E_OK;
        case T_NULL:
            if(insertParameterVal(params, O_NULL, value) != E_OK) return E_INTERN;
            if((error = itemlist(params)) != E_OK) return error;
            return E_OK;
        case T_INT:
            value.ival = strtol(T.s.ptr, NULL, 10);
            if(insertParameterVal(params, O_INT, value) != E_OK) return E_INTERN;
            if((error = itemlist(params)) != E_OK) return error;
            return E_OK;
        case T_DOUBLE:
            value.dval = strtod(T.s.ptr, NULL);
            if(insertParameterVal(params, O_DOUBLE, value) != E_OK) return E_INTERN;
            if((error = itemlist(params)) != E_OK) return error;
            return E_OK;
        case T_STRING:
            if(StringInit(&(value.sval)) != STR_OK) return E_INTERN;
            if(StringCpy(&(value.sval), &(T.s)) != STR_OK) return E_INTERN;
            if(insertParameterVal(params, O_STRING, value) != E_OK) return E_INTERN;
            if((error = itemlist(params)) != E_OK) return error;
            return E_OK;
        default:
            return E_OK;
    }
}

// Pravidla:
// <item-list> -> ,<item> <item-list>
// <item-list> -> Îµ
// <item> -> idvar
// <item> -> const
// <item> -> int
// <item> -> double
// <item> -> string
tErrors itemlist(tParamList *params) {
    gettoken();
    tValue value;

    if(checkToken(T_COMMA) == E_OK) {
        gettoken();
        switch (T.type) {
        case T_ERRORTOKEN:
            return E_LEX;
        case T_ERRORSYSTEM:
            return E_INTERN;
        case T_IDVAR:
            if(insertParameter(params, &(T.s)) != E_OK) return E_INTERN;
            if((error = itemlist(params)) != E_OK) return error;
            return E_OK;
        case T_KONST:
            value.bval = StringCharCmp(&(T.s), "false");
            if(insertParameterVal(params, O_BOOL, value) != E_OK) return E_INTERN;
            if((error = itemlist(params)) != E_OK) return error;
            return E_OK;
        case T_NULL:
            if(insertParameterVal(params, O_NULL, value) != E_OK) return E_INTERN;
            if((error = itemlist(params)) != E_OK) return error;
            return E_OK;
        case T_INT:
            value.ival = strtol(T.s.ptr, NULL, 10);
            if(insertParameterVal(params, O_INT, value) != E_OK) return E_INTERN;
            if((error = itemlist(params)) != E_OK) return error;
            return E_OK;
        case T_DOUBLE:
            value.dval = strtod(T.s.ptr, NULL);
            if(insertParameterVal(params, O_DOUBLE, value) != E_OK) return E_INTERN;
            if((error = itemlist(params)) != E_OK) return error;
            return E_OK;
        case T_STRING:
            if(StringInit(&(value.sval)) != STR_OK) return E_INTERN;
            if(StringCpy(&(value.sval), &(T.s)) != STR_OK) return E_INTERN;
            if(insertParameterVal(params, O_STRING, value) != E_OK) return E_INTERN;
            if((error = itemlist(params)) != E_OK) return error;
            return E_OK;
        default:
            return E_SYN;
        }
    }
    return E_OK;
}

tErrors defParamList(tParamList *params) {
    gettoken();
    switch (T.type) {
        case T_IDVAR:
            if(insertParameter(params, &(T.s)) != E_OK) return E_INTERN;
            if((error = defItemList(params)) != E_OK) return error;
            return E_OK;
        default:
            return E_OK;
    }
}

tErrors defItemList(tParamList *params) {
    gettoken();
    if(checkToken(T_COMMA) == E_OK) {
        gettoken();
        switch (T.type) {
            case T_ERRORTOKEN:
                return E_LEX;
            case T_ERRORSYSTEM:
                return E_INTERN;
            case T_IDVAR:
                if(insertParameter(params, &(T.s)) != E_OK) return E_INTERN;
                if((error = defItemList(params)) != E_OK) return error;
                break;
            default:
                return E_SYN;
        }
    }
    return E_OK;
}

// Pravidla:
// <stat> -> idvar = <assigment>;
// <stat> -> if(expresion) {<block-list>} else {<block-list>}
// <stat> -> while(expresion) {<block-list>}
// <stat> -> id(<param-list>);
// <stat> -> return expresion;

tErrors stat() {

    string *name = allocate(sizeof(string));
    tTapeItem *previous = Tape->last;
    tVariable *op1 = allocate(sizeof(tVariable));
    tVariable *op2 = allocate(sizeof(tVariable));
    tVariable *result = allocate(sizeof(tVariable));
    if((name == NULL) || (op1 == NULL) || (op2 == NULL) || (result == NULL)) return E_INTERN;

    switch (T.type) {
        case T_IDVAR:
            if(StringInit(name) != STR_OK) return E_INTERN;
            if(StringCpy(name, &(T.s)) != STR_OK) return E_INTERN;
            gettoken();
            if((error = checkToken(T_ASSIGN)) != E_OK) return error;
            if((error = assigment()) != E_OK) return error;
            if((error = checkToken(T_SEMICOLON)) != E_OK) return error;
            Tape->last->result = result;
            Tape->last->result->type = VARIABLE;
            if(StringInit(&(Tape->last->result->name)) != STR_OK) return E_INTERN;
            if(StringCpy(&(Tape->last->result->name), name) != STR_OK) return E_INTERN;
            Tape->last->result->type = VARIABLE;
            StringFree(name);
            break;

        case T_KEYWORD:
            if(!StringCharCmp(&(T.s), "if")) {
                tTapeItem *label1;
                tTapeItem *endelsepointer;
                tVariable *op11 = allocate(sizeof(tVariable));
                tVariable *op22 = allocate(sizeof(tVariable));
                if((op11 == NULL) || (op22 == NULL)) return E_INTERN;
                gettoken();
                if((error = checkToken(T_PAR_L)) != E_OK) return error;
                gettoken();
                if((error = ExpParser()) != E_OK) return error;
                if(TapeNewVoidLastItem() != E_OK) return E_INTERN;
                Tape->last->instruction = NGOTO;
                Tape->last->op1 = Tape->last->previous->result;
                Tape->last->op2 = op2;
                Tape->last->op2->type = TAPE_POINTER;
                label1 = Tape->last;
                if((error = checkToken(T_PAR_R)) != E_OK) return error;
                gettoken();
                if((error = checkToken(T_BRACE_L)) != E_OK) return error;
                if((error = blocklist()) != E_OK) return error;
                if((error = checkToken(T_BRACE_R)) != E_OK) return error;
                if(TapeNewVoidLastItem() != E_OK) return E_INTERN;
                Tape->last->instruction = NGOTO;
                Tape->last->op1 = op11;
                Tape->last->op1->type = O_BOOL;
                Tape->last->op1->value.bval = false;
                if(StringInit(&(Tape->last->op1->name)) != STR_OK) return E_INTERN;
                Tape->last->op2 = op22;
                Tape->last->op2->type = TAPE_POINTER;
                endelsepointer = Tape->last;
                gettoken();
                if((error = checkToken(T_KEYWORD)) != E_OK) return error;
                if(StringCharCmp(&(T.s), "else")) return E_SYN;
                if(TapeNewVoidLastItem() != E_OK) return E_INTERN;
                Tape->last->instruction = NOP;
                label1->op2->value.tape_pointer = Tape->last;
                gettoken();
                if((error = checkToken(T_BRACE_L)) != E_OK) return error;
                if((error = blocklist()) != E_OK) return error;
                if((error = checkToken(T_BRACE_R)) != E_OK) return error;
                if(TapeNewVoidLastItem() != E_OK) return E_INTERN;
                Tape->last->instruction = NOP;
                endelsepointer->op2->value.tape_pointer = Tape->last;
                break;
            }

            if(!StringCharCmp(&(T.s), "while")) {           // BROKEN
                // nutno vlozit prazdnou instrukci jako navesti
                if(TapeNewVoidLastItem() != E_OK) return E_INTERN;
                Tape->last->instruction = NOP;
                tTapeItem *label1;
                tTapeItem *label2 = Tape->last;             // ukazatel na pasku kam se bude cyklus vracet
                tVariable *op11 = allocate(sizeof(tVariable));
                tVariable *op22 = allocate(sizeof(tVariable));
                if((op11 == NULL) || (op22 == NULL)) return E_INTERN;
                gettoken();
                if((error = checkToken(T_PAR_L)) != E_OK) return error;
                gettoken();
                if((error = ExpParser()) != E_OK) return error;
                if(TapeNewVoidLastItem() != E_OK) return E_INTERN;
                Tape->last->instruction = NGOTO;
                Tape->last->op1 = Tape->last->previous->result;
                Tape->last->op2 = op22;
                Tape->last->op2->type = TAPE_POINTER;
                label1 = Tape->last;                        // ukazatel na GOTO instrukci, jiz bude pozdeji treba nastavit adresu skoku
                if((error = checkToken(T_PAR_R)) != E_OK) return error;
                gettoken();
                if((error = checkToken(T_BRACE_L)) != E_OK) return error;
                if((error = blocklist()) != E_OK) return error;
                if((error = checkToken(T_BRACE_R)) != E_OK) return error;
                if(TapeNewVoidLastItem() != E_OK) return E_INTERN;
                Tape->last->instruction = NGOTO;
                Tape->last->op1 = op1;
                Tape->last->op1->type = O_BOOL;
                Tape->last->op1->value.bval = false;
                if(StringInit(&(Tape->last->op1->name)) != STR_OK) return E_INTERN;
                Tape->last->op2 = op2;
                Tape->last->op2->type = TAPE_POINTER;
                Tape->last->op2->value.tape_pointer = label2;     // nepodmineny skok zpet na zacatek vyhodnocovani podminky cyklu
                if(TapeNewVoidLastItem() != E_OK) return E_INTERN;
                Tape->last->instruction = NOP;                    // instrukce, na kterou se skoci v pripade nesplneni podminky whilu
                label1->op2->value.tape_pointer = Tape->last;
                break;
            }

            if(!StringCharCmp(&(T.s), "return")) {
                gettoken();
                if((error = ExpParser()) != E_OK) return error;
                previous = Tape->last;
                if(TapeNewVoidLastItem() != E_OK) return E_INTERN;
                Tape->last->instruction = RET;
                Tape->last->op1 = op1;
                Tape->last->op1 = previous->result;
                if((error = checkToken(T_SEMICOLON)) != E_OK) return error;
                break;
            } else { // everything else is syntax error
                return E_SYN;
            }
            break;

        default:
            return E_SYN;
    }
    freePointer(name);
    return E_OK;
}

// Pravidla:
// <assigment> -> expresion
// <assigment> -> id(<param-list>)

tErrors assigment() {
    gettoken();
    if(checkToken(T_ID) == E_OK) {
        tVariable *op1 = allocate(sizeof(tVariable));
        tVariable *op2 = allocate(sizeof(tVariable));
        string *funName = allocate(sizeof(string));
        tParamList *params = allocate(sizeof(tParamList));
        if((op1 == NULL) || (op2 == NULL) || (funName == NULL) || (params == NULL)) return E_INTERN;
        if(StringInit(funName) != STR_OK) return E_INTERN;
        if(StringCpy(funName, &(T.s)) != STR_OK) return E_INTERN;
        initParamlist(params);
        gettoken();
        if((error = checkToken(T_PAR_L)) != E_OK) return error;
        if((error = paramlist(params)) != E_OK) return error;
        if((error = checkToken(T_PAR_R)) != E_OK) return error;
        if(TapeNewVoidLastItem() != E_OK) return E_INTERN;
        Tape->last->instruction = FUNC;
        Tape->last->op1 = op1;
        Tape->last->op2 = op2;
        Tape->last->op1->type = FUNCTION;
        if(StringInit(&(Tape->last->op1->name)) != STR_OK) return E_INTERN;
        if(StringCpy(&(Tape->last->op1->name), funName) != STR_OK) return E_INTERN;
        Tape->last->op2->type = PARAM_POINTER;
        Tape->last->op2->value.param_pointer = params->first;
        gettoken();
        StringFree(funName);
        freePointer(funName);
        return E_OK;
    } else {
        if((error = ExpParser()) != E_OK) return error;
        if(TapeNewVoidLastItem() != E_OK) return E_INTERN;
        Tape->last->instruction = ASSIGN;
        Tape->last->op1 = Tape->last->previous->result;
        return E_OK;
    }
}

tErrors blocklist() {
    gettoken();
    switch(T.type) {
        case T_KEYWORD:
            // any other keyword except else is allowed as statement
            if(StringCharCmp(&(T.s), "else")) {
                if((error = stat()) != E_OK) return error;
                if((error = blocklist()) != E_OK) return error;
                return E_OK;
            } else {
                return E_SYN;
            }
            break;
        case T_ID:
        case T_IDVAR:
            // function call or variable assigment
            if((error = stat()) != E_OK) return error;
            if((error = blocklist()) != E_OK) return error;
            return E_OK;
        default:
            return E_OK;
    }
}


























