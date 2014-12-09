#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include "expressions.h"
#include "strings.h"

//inicializuje zoznam
void ExpStackInit (tExpStack *L)
{
    L->Top = NULL;
}

//zrusi cely zoznam
void ExpStackDispose (tExpStack *L)
{
    tElemPtr temp = L->Top;
    while (temp)
    {
        L->Top = L->Top->rptr;
        free(temp);
        temp = L->Top;
    }
}

//vlozi prvok na zaciatok zoznamu
tErrors ExpStackPush (tExpStack *L, tExpType val) 
{
    tElemPtr temp;
    if ((temp = malloc(sizeof(struct tElem))) == NULL)
        return E_INTERN;
    //ked davam na zasobnik '('  ')' alebo ';'
    if (val == LP || val == RP || val == SC)
    {
        temp->data = val; //do data vlozim  interny typ precedencnej tabulky
        temp->tempVarPtr = NULL; //pointer na operand je NULL
        temp->rptr = L->Top;
        L->Top = temp;
        return E_OK;
    }
    //ked davam na zasobnik neterminal 
    else if (val == NONTERM)
    {
        if ((temp->tempVarPtr = allocate(sizeof(tVariable))) == NULL)
            return E_INTERN;
        temp->tempVarPtr->type = NONTERM; //typ je NONTERM
        
        if (StringInit(&(temp->tempVarPtr->name)) == STR_ERR)
                    return E_INTERN;
        
        temp->data = val; //do data vlozim interny typ precedencnej tabulky
        temp->rptr = L->Top;
        L->Top = temp;
        return E_OK;
    }
    else
    {
        //podla typu tokenu vytvaram instrukcie alebo premenne
        switch(T.type)
        { 
            //blok s instrukciami
            case T_MUL : temp->instruction = MUL; break;
            case T_DIV : temp->instruction = DIV; break;
            case T_ADD : temp->instruction = ADD; break;
            case T_SUB : temp->instruction = SUB; break;
            case T_CAT : temp->instruction = CON; break;
            case T_LESS : temp->instruction = LESS; break;
            case T_MORE : temp->instruction = MORE; break;
            case T_LESS_EQ : temp->instruction = EQL; break;
            case T_MORE_EQ : temp->instruction = EQM; break;
            case T_COMP : temp->instruction = EQ; break;
            case T_NEG : temp->instruction = NEQ; break;

            //null ukladam ako operand, typ O_NULL
            case T_NULL : 
                if ((temp->tempVarPtr = allocate(sizeof(tVariable))) == NULL)
                    return E_INTERN;
                if (StringInit(&(temp->tempVarPtr->name)) == STR_ERR)
                    return E_INTERN;
                temp->tempVarPtr->type = O_NULL;
                break;

            //hodnotu string z tokenu musim prekonvertovat zo stringu a ulozit ju ako int, typ O_INT
            case T_INT :
                if ((temp->tempVarPtr = allocate(sizeof(tVariable))) == NULL)
                    return E_INTERN;
                if (StringInit(&(temp->tempVarPtr->name)) == STR_ERR)
                    return E_INTERN;
                temp->tempVarPtr->type = O_INT;
                temp->tempVarPtr->value.ival = (int) strtol(T.s.ptr, (char **)NULL, 10);
                break;
            
            //hodnotu string z tokenu musim prekonvertovat zo stringu a ulozit ho ako double, typ O_DOUBLE
            case T_DOUBLE :
                if ((temp->tempVarPtr = allocate(sizeof(tVariable))) == NULL)
                    return E_INTERN;
                if (StringInit(&(temp->tempVarPtr->name)) == STR_ERR)
                    return E_INTERN;
                temp->tempVarPtr->type = O_DOUBLE;
                temp->tempVarPtr->value.dval = strtod(T.s.ptr, (char **)NULL);
                break;
        
            //hodnotu string z tokenu skopirujem do stringu v operande
            case T_STRING :
                if ((temp->tempVarPtr = allocate(sizeof(tVariable))) == NULL)
                    return E_INTERN;
                if (StringInit(&(temp->tempVarPtr->name)) == STR_ERR)
                    return E_INTERN;
                temp->tempVarPtr->type = O_STRING;
                if (StringInit(&(temp->tempVarPtr->value.sval)) == STR_ERR)
                    return E_INTERN;
                if (StringCpy(&(temp->tempVarPtr->value.sval), &(T.s)) == STR_ERR)
                    return E_INTERN;
                break;
        
            //konstanty ukladam ako true resp. false
            case T_KONST :
                if ((temp->tempVarPtr = allocate(sizeof(tVariable))) == NULL)
                    return E_INTERN;
                if (StringInit(&(temp->tempVarPtr->name)) == STR_ERR)
                    return E_INTERN;
                temp->tempVarPtr->type = O_BOOL;
                if (!strcmp(T.s.ptr, "true"))
                    temp->tempVarPtr->value.ival = true;
                else temp->tempVarPtr->value.ival = false;
                break;

            //premennu ulozim podla mena do name struktury tVariable
            case T_IDVAR :
                if ((temp->tempVarPtr = allocate(sizeof(tVariable))) == NULL)
                    return E_INTERN;
                if (StringInit(&(temp->tempVarPtr->name)) == STR_ERR)
                    return E_INTERN;
                if (StringCpy(&(temp->tempVarPtr->name), &(T.s)) == STR_ERR)
                    return E_INTERN;
                temp->tempVarPtr->type = VARIABLE;
           default : break;
        }
    }
    temp->data = val; //do data vlozim interny typ precedencnej tabulky
    temp->rptr = L->Top;
    L->Top = temp;
    return E_OK;
}

//vrati hodnotu prveho prvku
void ExpStackTop (tExpStack *L, tExpType *val) 
{
    if (L->Top)
        *val = L->Top->data;
}

//vrati hodnotu druheho prvku
void ExpStackSecTop (tExpStack *L, tExpType *val) 
{
    if (L->Top)
        *val = L->Top->rptr->data;
}

//najde na zasobniku prvy terminal a jeho hodnotu ulozi do val
void ExpStackTopTerminal (tExpStack *L, tExpType *val)
{
    tElemPtr temp = L->Top;
    if (!temp)
        return;
    while (temp->data == NONTERM)
        temp = temp->rptr;
    *val = temp->data;
}

//vymaze prvy prvok v zozname
void ExpStackPop (tExpStack *L) 
{
    if (!L->Top)
        return;
    tElemPtr temp;
    temp = L->Top;
    L->Top = L->Top->rptr;
    free(temp);
}

//vymaze druhy prvok v zozname
void ExpStackSecPop (tExpStack *L) 
{
    if (!L->Top)
        return;
    tElemPtr temp;
    temp = L->Top->rptr;
    L->Top->rptr = L->Top->rptr->rptr;
    free(temp);
}

//tabulka priorit operatorov, pouzivam ju aby som zistil
//aku akciu spravim na zasobniku
const tTabSigns PTable [TABLESIZE][TABLESIZE] =
{
               //+    *    ===  < =<  
               //- .  /    !==  > => (    )    id   ;
    /* + - . */ {M,   L,   M,   M,   L,   M,   L,   M},
      /* * / */ {M,   M,   M,   M,   L,   M,   L,   M},
   /* === !==*/ {L,   L,   M,   L,   L,   M,   L,   M},
/* < > <= >= */ {L,   L,   M,   M,   L,   M,   L,   M},
        /* ( */ {L,   L,   L,   L,   L,   Q,   L,   E},
        /* ) */ {M,   M,   M,   M,   E,   M,   E,   M},
       /* id */ {M,   M,   M,   M,   E,   M,   E,   M},
        /* ; */ {L,   L,   L,   L,   L,   F,   L,   E},
};

//vrati mi index z tabulky na zaklade typu z prichadzajuceho
//tokenu a typu terminalu, ktory je na vrchole zasobniku
tTabSigns SearchSign(tExpType row, tExpType col)
{
    return PTable [row][col];
}

//prevedie token na zaklade jeho typu na interny typ expparseru
tErrors ConvertToken(tExpType *type)
{
    switch(T.type)
    {
        case T_ERRORTOKEN : return E_LEX;
        case T_ERRORSYSTEM : return E_INTERN;
        case T_KONST :
        case T_NULL :
        case T_INT :
        case T_DOUBLE :
        case T_STRING :
        case T_IDVAR : *type = ID; break;
        case T_PAR_L : *type = LP; break;
        case T_PAR_R : *type = RP; break;
        case T_MUL :
        case T_DIV : *type = MU_DI; break;
        case T_ADD :
        case T_SUB :
        case T_CAT : *type = AD_SU_CO; break;
        case T_LESS :
        case T_MORE :
        case T_LESS_EQ :
        case T_MORE_EQ : *type = LS_MO_LE_LM; break;
        case T_COMP :
        case T_NEG : *type = EQ_NE; break;
        case T_SEMICOLON : *type = SC; break;
        default : return E_SYN;
    }
    return E_OK;
}

//zredukuje zasobnik v pripade hodnoty sign = alebo >
tErrors ExpStackReduct(tExpStack *S, tTabSigns sign)
{
    tExpType type_on_top = -1;
    ExpStackTop(S, &type_on_top);
    tErrors err = E_OK;

    //pouzivam pravidlo ID->E
    if (type_on_top == ID) 
    {
        S->Top->data = NONTERM; //zmenim len interne oznacenie z ID na NONTERM
        return E_OK;
    }

    if (sign == Q)     //ked je na vstupe )
    {   
        //pouzivam pravidlo E->(E) pricom ) nemam na zasobniku
        if (type_on_top != NONTERM)      //ked nie je na vrchole zasobnika terminal 
            return E_SYN;                 //-> error

        ExpStackSecTop(S, &type_on_top);
        if (type_on_top != LP)      //ked nie je na druhom mieste v zasobniku ( 
            return E_SYN;           //-> error

        ExpStackSecPop(S);         //odstranim zatvorku
    }
    else
    {   
        //pouzivam ostatne pravidla E->EopE napr E->E+E, 
        if (type_on_top != NONTERM)      //ked nie je na vrchole zasobnika neterminal
            return E_SYN;           //-> error
        else 
        {
            Tape->last->op2 = S->Top->tempVarPtr;
            ExpStackPop(S);
        }
        
        ExpStackTop(S, &type_on_top);
        if (type_on_top == NONTERM)      //ked je na vrchole zasobnika neterminal
            return E_SYN;           //-> error
        else 
        {
            Tape->last->instruction = S->Top->instruction;
            ExpStackPop (S);
        }

        ExpStackTop(S, &type_on_top);
        if (type_on_top != NONTERM)      //ked nie je na vrchole zasobnika neterminal 
            return E_SYN;           //-> error
        else 
        {
            Tape->last->op1 = S->Top->tempVarPtr;
            ExpStackPop (S);
        }
        
        ExpStackPush(S, NONTERM);       //vkladam naspat na zasobnik novy neterminal
        Tape->last->result = S->Top->tempVarPtr;  
        err = TapeNewVoidLastItem();
        if (err)
            return err;
    }
    return E_OK;
}


//hlavna funkcia pre vyhodnocovanie vyrazov, vracia zodpovedajuci error code
tErrors ExpParser()
{
    bool finish = false;
    tExpType type = -1;          //uchovava typ tokenu pushuje sa na stack
    tExpType stack_type = -1;    //uchovava typ prveho terminalu na stacku
    tErrors err = E_OK;     //vysledny error code
    tTabSigns sign;          //moze byt > < = error alebo koniec v pripade 
                            //prvej nesparovanej )
    tExpStack S;
    ExpStackInit(&S);       //inicializacia zasobniku
    ExpStackPush(&S, SC);          //na zasobnik davam konvocy znak
    
    err = ConvertToken(&type);
    if (err) //zistim si typ prveho tokenu
    {
        ExpStackDispose(&S);
        return err;
    }
    
    //TapeNewVoidLastItem(); //vytvaram novu prazdnu instrukciu
    ExpStackTopTerminal(&S, &stack_type);
    sign = SearchSign(stack_type, type); //zistujem sign pomocou typu aktualneho
                                         //tokenu a typu prveho terminalu na stacku
    do
    {
        switch(sign)
        {
            case L :    //LESS nizsia priorita
                err = ExpStackPush(&S, type);
                if (err) 
                 {
                     ExpStackDispose(&S);
                     return err;
                 }
                 gettoken();
                 break;
            case M :    //MORE vyssia priorita
                 err = ExpStackReduct(&S, sign);
                 if (err) 
                 {
                     ExpStackDispose(&S);
                     return err;
                 }
                 break;
            case Q :    //EQUAL rovna sa pri zatvorkach
                 err = ExpStackReduct(&S, sign);
                 if (err)
                 {
                     ExpStackDispose(&S);
                     return err;
                 }
                 gettoken();
                 break;
            case E :    //ERROR chyba
                return E_SYN;
            case F :    //FINISH koniec pri prvej nesparovanej pravej zatvorke
                finish = true;
        }

        if (finish) break;
        err = ConvertToken(&type);
        if (err)  //zistujem typ prichadzajuceho noveho tokenu
        {
            ExpStackDispose(&S);
            return err;
        }
        ExpStackTopTerminal(&S, &stack_type);   //zistim typ prveho terminalu na zasobniku
        sign = SearchSign(stack_type, type); 

    } while (type != SC || stack_type != SC);
    
    Tape->last->instruction = NOP;
    Tape->last->result = S.Top->tempVarPtr;
    ExpStackDispose(&S);
    return E_OK;
}

