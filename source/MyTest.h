#include <cxxtest/TestSuite.h>
#include "ast.h"
#include "parser.tab.h"
#include "scanner.h"

extern FILE *yyin;
extern int parserror;
extern int scannerror;
class MyTest: public CxxTest::TestSuite
{
public:
	void testValidId(void)
	{
		scannerror = 0;
		parserror = 0;
		yyin = fopen("./roman/id.txt", "r");
		yyparse();
		TS_ASSERT_EQUALS(0, scannerror);
		TS_ASSERT_EQUALS(0, parserror);
	}
	void testValidCircle(void)
	{
		scannerror = 0;
		parserror = 0;
		yyin = fopen("./roman/circle.txt", "r");
		yyparse();
		TS_ASSERT_EQUALS(0, scannerror);
		TS_ASSERT_EQUALS(0, parserror);
	}
	void testValidShot(void)
	{
		scannerror = 0;
		parserror = 0;
		yyin = fopen("./roman/shot.txt", "r");
		yyparse();
		TS_ASSERT_EQUALS(0, scannerror);
		TS_ASSERT_EQUALS(0, parserror);
	}
	void testValidaa(void)
	{
		scannerror = 0;
		parserror = 0;
		yyin = fopen("./roman/aa", "r");
		yyparse();
		TS_ASSERT_EQUALS(1, scannerror);
	}
	void testValidLexeme(void)
	{
		int l;
		yyin = fopen("./roman/lex1", "r");
		l = yylex();
		TS_ASSERT_EQUALS((int)TyOver, l);
		l = yylex();
		TS_ASSERT_EQUALS((int)TyLess, l);
		l = yylex();
		TS_ASSERT_EQUALS((int)TyIf, l);
		l = yylex();
		TS_ASSERT_EQUALS((int)TyReturn, l);
		l = yylex();
		TS_ASSERT_EQUALS((int)TyElse, l);
		l = yylex();
		TS_ASSERT_EQUALS((int)TyDo, l);
		l = yylex();
		TS_ASSERT_EQUALS((int)TyWhile, l);
		l = yylex();
		TS_ASSERT_EQUALS((int)TyInt, l);
		l = yylex();
		TS_ASSERT_EQUALS((int)TyMain, l);
		l = yylex();
		TS_ASSERT_EQUALS((int)TyStringname, l);
		l = yylex();
		TS_ASSERT_EQUALS((int)TyPrint, l);
		l = yylex();
		TS_ASSERT_EQUALS((int)TyInvest, l);
		l = yylex();
		TS_ASSERT_EQUALS((int)OverEq, l);
		l = yylex();
		TS_ASSERT_EQUALS((int)LessEq, l);
		l = yylex();
		TS_ASSERT_EQUALS((int)TyNotEql, l);
		l = yylex();
		TS_ASSERT_EQUALS((int)TyEql, l);
		l = yylex();
		TS_ASSERT_EQUALS((int)TyEql, l);
		l = yylex();
		TS_ASSERT_EQUALS((int)TyNumber, l);
		l = yylex();
		TS_ASSERT_EQUALS((int)TyIdentifier, l);
	}
	void testValidpa(void)
	{
		scannerror = 0;
		parserror = 0;
		yyin = fopen("./roman/pa", "r");
		yyparse();
		TS_ASSERT_EQUALS(0, scannerror);
		TS_ASSERT_EQUALS(1, parserror);
	}
	void testValidParserIf(void)
	{
		scannerror = 0;
		parserror = 0;
		yyin = fopen("./roman/if", "r");
		yyparse();
		TS_ASSERT_EQUALS(0, scannerror);
		TS_ASSERT_EQUALS(0, parserror);
	}
	void testValidParserWhile(void)
	{
		scannerror = 0;
		parserror = 0;
		yyin = fopen("./roman/while", "r");
		yyparse();
		TS_ASSERT_EQUALS(0, scannerror);
		TS_ASSERT_EQUALS(0, parserror);
	}
	void testValidParserIfWhile(void)
	{
		scannerror = 0;
		parserror = 0;
		yyin = fopen("./roman/if_while", "r");
		yyparse();
		TS_ASSERT_EQUALS(0, scannerror);
		TS_ASSERT_EQUALS(0, parserror);
	}
        
};

void yyerror(char *errmsg)
{
    parserror += 1;
}