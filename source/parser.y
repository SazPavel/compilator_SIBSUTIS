%{
    #include "ast.h"
    #include <iostream>
    #include <stdio.h>
    #define YYSTYPE Node*
    extern int yylineno;
    extern FILE *yyin;
    extern char *yytext;
    extern char *name;
    int parserror = 0;
    void yyerror(char *s);
    #define YYPRINT(file, type, value) fprintf(file, "%d", value);
    int yylex();
    int yywrap()
    {
       return 1;
    }
    Node *root;
%}
%token-table
%token TyIf TyElse TyDo TyWhile
%token TyInt TyFloat TyStringname TyMas
%token TyReturn TyMain TyPrint TyDef TyInvest
%token TyIdentifier TyNumber TyString TySet
%token OverEq LessEq TyEql TyNotEql TyLess TyOver
%token TyPlus TyMinus TyMul TyDivision

%%

PROGRAM: TyMain OPS { $$ = new Node((char*)"main", TyMain, $2, NULL, NULL); root = $$; }
;

OPS:    OP
|       OPS OP { $$ = new Node((char*)"seq", 1, $1, $2, NULL); }
;

OP1:    '{' OPS '}' { $$ = new Node((char*)"ops", 4, $2, NULL, NULL); }
|       EXPR ';' { $$ = new Node((char*)"expr", 3, $1, NULL, NULL); }
|       TyIf '(' EXPR ')' OP1 TyElse OP1 { $$ = new Node((char*)"IfElse", TyElse, $3, $5, $7); }
|       TyWhile '(' EXPR ')' OP1 { $$ = new Node((char*)"while", TyWhile, $3, $5, NULL); }
|       TyDo  OP1  TyWhile '(' EXPR ')' ';' { $$ = new Node((char*)"do", TyDo, $2, $5, NULL); }
|       TyReturn ';' { $$ = new Node((char*)"Return", TyReturn, $1, NULL, NULL); }
;

OP2:    TyIf '(' EXPR ')' OP { $$ = new Node((char*)"If", TyIf, $3, $5, NULL); }
|       TyIf '(' EXPR ')' OP1 TyElse OP2 { $$ = new Node((char*)"IfElse", TyElse, $3, $5, $7); }
|       TyDo  OP2  TyWhile '(' EXPR ')' ';' { $$ = new Node((char*)"do", TyDo, $2, $5, NULL); }
|       TyWhile '(' EXPR ')' OP2 { $$ = new Node((char*)"while", TyWhile, $3, $5, NULL); }
|       TyInt ID '=' EXPR1 ';' { $$ = new Node((char*)"set", TySet, $2, $4, NULL); }
//|       TyFloat ID '=' EXPR1 ';'
|       TyStringname ID '=' STRING ';' { $$ = new Node((char*)"set", TySet, $2, $4, NULL); }
|       TyPrint PRINT ';' { $$ = new Node((char*)"Print", TyPrint, $2, NULL, NULL); }
;

OP:     OP1
|       OP2
;

STRING: TyString { $$ = new Node((char *)yylval, TyString, NULL, NULL, NULL); }
;

EXPR:   EXPR1
|       ID '=' EXPR { $$ = new Node((char*)"set", TySet, $1, $3, NULL); }
;

EXPR1:  EXPR2
|       TyInvest '(' PRINT PRINT ')' { $$ = new Node((char*)"investigation", TyInvest, $3, $4, NULL); }
|       EXPR1 OverEq EXPR2 { $$ = new Node((char*)">=", OverEq, $1, $3, NULL); }
|       EXPR1 LessEq EXPR2 { $$ = new Node((char*)"<=", LessEq, $1, $3, NULL); }
|       EXPR1 TyEql EXPR2 { $$ = new Node((char*)"==", TyEql, $1, $3, NULL); }
|       EXPR1 TyNotEql EXPR2 { $$ = new Node((char*)"!=", TyNotEql, $1, $3, NULL); }
|       EXPR1 TyOver EXPR2 { $$ = new Node((char*)">", TyOver, $1, $3, NULL); }
|       EXPR1 TyLess EXPR2 { $$ = new Node((char*)"<", TyLess, $1, $3, NULL); }
;

PRINT:  TyString { $$ = new Node((char *)yylval, TyString, NULL, NULL, NULL); }
|       TyNumber { $$ = new Node((char *)yylval, TyNumber, NULL, NULL, NULL); }
|       TyIdentifier { $$ = new Node((char *)yylval, TyIdentifier, NULL, NULL, NULL); }
//|       TyFloat
;

EXPR2:  TERM
|       EXPR2 '+' TERM  { $$ = new Node((char*)"+", TyPlus, $1, $3, NULL); }
|       EXPR2 '-' TERM  { $$ = new Node((char*)"-", TyMinus, $1, $3, NULL); }
;

TERM:   VAL
|       TERM '*' VAL { $$ = new Node((char*)"*", TyMul, $1, $3, NULL); }
|       TERM '/' VAL { $$ = new Node((char*)"/", TyDivision, $1, $3, NULL); }
;

VAL:    TyNumber { $$ = new Node((char *)yylval, TyNumber, NULL, NULL, NULL); }
|       '(' EXPR ')'
|       TyIdentifier { $$ = new Node((char *)yylval, TyIdentifier, NULL, NULL, NULL); }
;

ID:     TyIdentifier { $$ = new Node((char *)yylval, TyIdentifier, NULL, NULL, NULL); }
;

%%

const char* token_name(int t)
{
    return yytname[YYTRANSLATE(t)];
}
