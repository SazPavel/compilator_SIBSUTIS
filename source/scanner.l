%{
    #include "ast.h"
    #define YYSTYPE char*
    #include <stdio.h>
    #include "parser.tab.h"
    extern void yyerror(char *);
    int scannerror = 0 ;
    int ch = 0;
%}

%option yylineno
%option noyywrap

digit[0-9]
letter[a-zA-Z]
string (\"(\\.|[^\\"])*\")
comment(#.*)
ws[ \t]
wsn[\n]

%%
si {
    ch+= yyleng;
    return TyIf;
    }
redi {
    ch+= yyleng;
    return TyReturn;
    }
aliud {
    ch+= yyleng;
    return TyElse;
    }
facite {
    ch+= yyleng;
    return TyDo;
    }
dum {
    ch+= yyleng;
    return TyWhile;
    }
totus {
    ch+= yyleng;
    return TyInt;
    }
verum {
    ch+= yyleng;
    return TyFloat;
    }
SPQR {
    ch+= yyleng;
    return TyMain;
    }
filum {
    ch+= yyleng;
    return TyStringname;
    }
eventus {
    ch+= yyleng;
    return TyPrint;
    }
definire {
    ch+= yyleng;
    return TyDef;
    }
investigation {
    ch+= yyleng;
    return TyInvest;
    }
(>=) {
    ch += yyleng;
    return OverEq;
    }
(<=) {
    ch += yyleng;
    return LessEq;
    }
(!=) {
    ch += yyleng;
    return TyNotEql;
    }
(==|paritas) {
    ch += yyleng;
    return TyEql;
    }
[-{};()=+*/!] { 
    ch += yyleng;
    return *yytext; 
}
("_"|{letter})("_"|{letter}|{digit})*((("_"|{letter})("_"|{letter}|{digit})*|[-+]?({digit}*\.{digit}+|{digit}+\.|{digit}+)([eE][-+]?{digit}+)?)\]) {
    ch+= yyleng;
    yylval = strdup(yytext);
    return TyMas;
    }
("_"|{letter})("_"|{letter}|{digit})* { 
    ch+= yyleng;
    yylval = strdup(yytext);
    return TyIdentifier;
    }
[-+]?({digit}+) { 
    ch+= yyleng;
    yylval = strdup(yytext);
    return TyNumber;
    }
(<) {
    ch += yyleng;
    return TyLess;
    }
(>) {
    ch += yyleng;
    return TyOver;
    }
{string} {
    ch+= yyleng;
    yylval = strdup(yytext);
    return TyString;
    }
{comment} {
    ch+= yyleng; 
    }
{wsn} {
    ch = 1;
    }
{ws}+ {
    ch += yyleng;
    }
. {
    ch += yyleng; 
    scannerror += 1;
    yyerror((char*)"Invalid character");
  }
%%
