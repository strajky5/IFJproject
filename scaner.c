/* 
* Názov projektu: Implementace interpretu imperativního jazyka IFJ14 
* Dátum: 12/2014
* Autori: Matúš Cimerman, xcimer00
*         Tomáš Hynek ,   xhynek09
*         David Novák ,   xnovak0r
*         Michal Sedlák , xsedla0m
*         Jan Jorenek  ,  xjoren01
*/
#include <stdio.h>
#include <ctype.h>
#include "scaner.h"
#include <stdlib.h>
#include <strings.h>
extern FILE*f;
extern tToken T;

int iskeyword(char*sid) 
{
    char*s;
	s="begin";
    if(strcasecmp (sid,s)==0)
        return 1;
    else{
	s="end";
    if(strcasecmp (sid,s)==0)
        return 1;
    else{
	s="var";
    if(strcasecmp (sid,s)==0)
        return 1;
    else{
	s="then";
    if(strcasecmp (sid,s)==0)
        return 1;
    else{
	s="boolean";
    if(strcasecmp (sid,s)==0)
        return 3;
    else{
	s="string";
    if(strcasecmp (sid,s)==0)
        return 3;
    else{
	s="integer";
    if(strcasecmp (sid,s)==0)
        return 3;
    else{
	s="real";
    if(strcasecmp (sid,s)==0)
        return 3;
    else{
	s="do";
    if(strcasecmp (sid,s)==0)
        return 1;
    else{
    s="else";
    if(strcasecmp (sid,s)==0)
        return 1;
    else{
    s="function";
    if(strcasecmp (sid,s)==0)
        return 1;
    else{
    s="if";
    if(strcasecmp (sid,s)==0)
        return 1;
    else{
    s="forward";
    if(strcasecmp (sid,s)==0)
        return 1;
    else{
    s="while";
    if(strcasecmp (sid,s)==0)
        return 1;
    else{
	s="readln";
    if(strcasecmp (sid,s)==0)
        return 1;
    else{
	s="write";
    if(strcasecmp (sid,s)==0)
        return 1;
 /*   else{
	s="find";
    if(strcasecmp (sid,s)==0)
        return 1;
    else{
	s="sort";
    if(strcasecmp (sid,s)==0)
        return 1;*/
    else{
    s="false";
    if(strcasecmp (sid,s)==0)
        return 2;
    else{
    s="true";
    if(strcasecmp (sid,s)==0)
        return 2;
    else{
    return 0;
    }
    }
    }
    }
    }
    }
	}
	}
	}
	}
	}
	//}
	//}
	}
	}
	}
	}
	}
    }
    }
}

int jumpcomment() //preskocenie komentara
{
    char c;
        do
            c=fgetc(f); 
        while(c != '}'); 
        return 0;  
}

void gettoken()
{
    strClear(&(T.s));
    int x; 
    char c; 
    char h; 
    int state=0; 
    c=fgetc(f); 
	//printf(" c = %c \n",c);
    if(c==EOF) 
    {
        T.type=T_EOF; 
        return; 
    }
    while(1) 
    {
    switch(state) 
        {		
            case 0: 
            {
                if(c == '{') 
                {
                    if((x=jumpcomment()) == 1) 
                    {
                        T.type=T_ERRORTOKEN; 
                        return; 
                    }
                c=fgetc(f); 
                break;
                }
                else
                {
                if(isspace(c) != 0) 
                {
                    c=fgetc(f); 
                    break;
                }
                else
                {
                if(isalpha(c) != 0 || c=='_') 
                {
                    state=3; //T_ID
                    if(strAddChar(&(T.s),c)== STR_ERROR)
                    {
                        T.type=T_ERRORSYSTEM; 
                        return; 
                    }
                    c=fgetc(f); 
                    break;
                }
                else{
                if(isdigit(c) != 0) 
                {
                    state=4; //->T_INT/T_REAL
                    if(strAddChar(&(T.s),c)== STR_ERROR)
                    {
                        T.type=T_ERRORSYSTEM; 
                        return; 
                    }
                    c=fgetc(f); 
                    break;
                }
                else{
                if(c==EOF)
                {
                    T.type=T_EOF; //T_EOF
                    return;
                }
                else{ 
                switch(c) //bude prepinat medzi dalsimi moznostami (stale v pociatocnom stave)
                {
                case '\'': 
                {
                    state=11;//->STRING
                    c=fgetc(f);
                    break;
                }
                case '(': 
                {
                    T.type=T_LB;//T_LB
					return;
                    break;
                }
                case ')' : 
                {
                    T.type=T_RB;//T_RB
					return;
                    break;
                }
				case ':' :
                {
                    state=14;//T_COLON/T_ASSIGN
					 c=fgetc(f);
                    break;
                }
                case '*' :
                {
                    T.type=T_MUL;//T_MUL
					return;
                    break;
                }
                case '+' :
                {
                    T.type=T_ADD;//T_ADD
					return;
                    break;
                }
                case '-' :
                {
                    T.type=T_SUB;//T_SUB
					return;
                    break;
                }
                case '/' :
                {
                    T.type=T_DIV;//T_DIV
					return;
                    break;
                }
                case '<' :
                {
                    state=12;//T_LESS/T_LESS_EQ
                    if(strAddChar(&(T.s),c)== STR_ERROR)
                    {
                        T.type=T_ERRORSYSTEM; 
                        return; 
                    }
                    c=fgetc(f);
                    break;
                }
                case '>' :
                {
                    state=13;//T_MORE/T_MORE_EQ
                    if(strAddChar(&(T.s),c)== STR_ERROR)
                    {
                        T.type=T_ERRORSYSTEM; 
                        return; 
                    }
                    c=fgetc(f);
                    break;
                }
                case '=' :
                {
                    T.type=T_EQUAL;//T_EQUAL
					return;
                    break;
                }
				 case '.' :
                {
                    T.type=T_DOT;//T_DOT
					return;
                    break;
                }
                case ',' :
                {
                    T.type=T_COMMA;//T_COMMA
					return;
                    break;
                }
                case ';' :
                {
                    T.type=T_SEMICOLON;//T_SEMICOLON
					return;
                    break;
                }
                default : //neplatny token
                {
                    T.type=T_ERRORTOKEN; 
                    return; 
                }
                } //switch c
                } // vylezie z else
                }
                }
                }
                }

            break;
            }
            case 3 : 
            {
                if(isalpha(c) != 0 || isdigit(c) !=0 || c=='_') 
                {
                    if(strAddChar(&(T.s),c)== STR_ERROR)
                    {
                        T.type=T_ERRORSYSTEM; 
                        return; 
                    }					
                    c=fgetc(f);			
                }
                else
                {
                    ungetc(c,f); 
                    if(iskeyword(strGetStr(&(T.s)))==1){  
                        T.type=T_KEYWORD; 
					}
					else if (iskeyword(strGetStr(&(T.s)))==3){
						T.type=T_DATATYPE;
					}
                    else{
                    if(iskeyword(strGetStr(&(T.s)))==2) 
                        T.type=T_KONST; 
                    else{                   
                        T.type=T_ID;
                    }					
                    }
                    return; 
                }
            break;
            }
            case 4 : 
            {
                if(c == '.') 
                {
                    state=5;
                    h=c;
                    c=fgetc(f);
                }
                else{
                    if(c=='e' ||c=='E')
                    {
                        if(strAddChar(&(T.s),c)== STR_ERROR)
                        {
                            T.type=T_ERRORSYSTEM; 
                            return; 
                        }
                        state=7;
                        c=fgetc(f);
                        break;
                    }
                    else{
                    if(isdigit(c)!=0) 
                    {
                        if(strAddChar(&(T.s),c)== STR_ERROR)
                        {
                            T.type=T_ERRORSYSTEM; 
                            return; 
                        }
                        c=fgetc(f);
                    }
                    else{ 
                    ungetc(c,f);
                    T.type=T_INT;
                    return;
                    }
                    }
                }
            break;
            }
            case 5 :  
            {
                if(isdigit(c)!=0)
                {
                    if(strAddChar(&(T.s),h)== STR_ERROR)
                    {
                        T.type=T_ERRORSYSTEM; 
                        return; 
                    }
                    if(strAddChar(&(T.s),c)== STR_ERROR)
                    {
                        T.type=T_ERRORSYSTEM; 
                        return; 
                    }
                    state=6;
                    c=fgetc(f);
                }
                else{ 
                ungetc(c,f);
                ungetc(h,f);
                T.type=T_INT;
                return;
                }
            break;
            }
            case 6 :
            {
                if(isdigit(c)!=0) 
                {
                    if(strAddChar(&(T.s),c)== STR_ERROR)
                    {
                        T.type=T_ERRORSYSTEM; 
                        return; 
                    }
                    c=fgetc(f);
                }
                else{
                if(c=='e' || c=='E') 
                {
                    if(strAddChar(&(T.s),c)== STR_ERROR)
                    {
                        T.type=T_ERRORSYSTEM; 
                        return; 
                    }
                    state=7;
                    c=fgetc(f);
                    break;
                }
                else
                {
                    ungetc(c,f);
                    T.type=T_REAL;
                    return;
                }
                }
            break;
            }
            case 7 : 
            {
                if(isdigit(c)!=0) 
                {
                    if(strAddChar(&(T.s),c)== STR_ERROR)
                    {
                        T.type=T_ERRORSYSTEM; 
                        return; 
                    }
                    state=9;
                    c=fgetc(f);
                    break;
                }
                else{  
                if(c=='+' || c=='-')
                {
                    if(strAddChar(&(T.s),c)== STR_ERROR)
                    {
                        T.type=T_ERRORSYSTEM; 
                        return; 
                    }
                    state=8;
                    c=fgetc(f);
                    break;
                }
                else{ 
                T.type=T_ERRORTOKEN;
                return;
                }
                }
            break;
            }
            case 8 : 
            {
                if(isdigit(c)!=0) 
                {
                    if(strAddChar(&(T.s),c)== STR_ERROR)
                    {
                        T.type=T_ERRORSYSTEM; 
                        return; 
                    }
                    state=9;
                    c=fgetc(f);
                    break;
                }
                else
                {
                    T.type=T_ERRORTOKEN;
                    return;
                }
            break;
            }
            case 9 :  
            {
                if(isdigit(c)!=0) 
                {
                    if(strAddChar(&(T.s),c)== STR_ERROR)
                    {
                        T.type=T_ERRORSYSTEM; 
                        return; 
                    }
                    state=9;
                    c=fgetc(f);
                    break;
                }
                else
                {
                    T.type=T_REAL;
                    ungetc(c,f);
                    return;
                }
            break;
            }       
	    case 11 :
            {
			if ( c == '\'')
			{
			c=fgetc(f);
				if ( c == '\''){
				c=fgetc(f);
				break;}
				
				if ( c == '#'){
				c=fgetc(f);
				if(isdigit(c)!=0)
                {
                    if(strAddChar(&(T.s),h)== STR_ERROR)
                    {
                        T.type=T_ERRORSYSTEM; 
                        return; 
                    }
				}
				else 
				{
				T.type = T_ERRORTOKEN;
				return;
				break;
				}
				c=fgetc(f);
				if(isdigit(c)!=0)
                {
                    if(strAddChar(&(T.s),h)== STR_ERROR)
                    {
                        T.type=T_ERRORSYSTEM; 
                        return; 
                    }
				}
				else 
				{
				T.type = T_ERRORTOKEN;
				return;
				break;
				}
				c=fgetc(f);
				if(isdigit(c)!=0)
                {
                    if(strAddChar(&(T.s),h)== STR_ERROR)
                    {
                        T.type=T_ERRORSYSTEM; 
                        return; 
                    }
				}
				else 
				{
				ungetc(c,f);
				}
								
				c=fgetc(f);
				if  ( c != '\'') 
				{
				T.type = T_ERRORTOKEN;
				return;
				}
				c=fgetc(f);
				break;
			}
			else 
			{ 
			ungetc(c,f);
			T.type = T_STRING;
			return;
			}
			}
			else	
			{
				while (c != '\'')
				{
					if(strAddChar(&(T.s),c)== STR_ERROR)
                    {
                        T.type=T_ERRORSYSTEM; 
                        return; 
                    }
					c=fgetc(f);
					  if(c==EOF)
                {
                    T.type=T_ERRORTOKEN;
                    return;
                }
				}
				state = 11;
				break;
			}
			}			
            case 12:
            {
                if(c=='=')
                {
                    if(strAddChar(&(T.s),c)== STR_ERROR)
                    {
                        T.type=T_ERRORSYSTEM; 
                        return; 
                    }
                    T.type=T_LESS_EQ;
					return;
                }
				else if (c=='>')
				{
					if(strAddChar(&(T.s),c)== STR_ERROR)
                    {
                        T.type=T_ERRORSYSTEM;
                        return; 
                    }
                    T.type=T_NOT_EQ;
					return;
				}
                else
                {
                    ungetc(c,f); 
                    T.type=T_LESS; 
                    return;
                }
            break;
            }
            case 13: 
            {
                if(c=='=')
                {
                    if(strAddChar(&(T.s),c)== STR_ERROR)
                    {
                        T.type=T_ERRORSYSTEM; 
                        return; 
                    }
                    T.type=T_MORE_EQ;
					return;
                }
                else 
                {
                    ungetc(c,f); 
                    T.type=T_MORE;
                    return;
                }
            break;
            }
            case 14: //:=
            {
                if(c=='=') 
                {
                    if(strAddChar(&(T.s),c)== STR_ERROR)
                    {
                        T.type=T_ERRORSYSTEM; 
                        return;
                    }
                     T.type=T_ASSIGN;
					 return;
                }
                else 
                {
                    ungetc(c,f); 
                    T.type=T_COLON;
                    return;
                }
            break;
            }
        }	
    }
}

