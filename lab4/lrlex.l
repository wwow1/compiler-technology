%{
	#include <stdlib.h>
	void yyerror(char *);
	#include "ast.h"
	#include "lrparser.tab.h"
	int lineno=0;
%}

DIGIT [0-9]
%%

[ ]*	{}
[\t]*	{}
[\n]	{lineno++;}
\/\*([^\*^\/]*|[\*^\/*]*|[^\**\/]*)*\*\/	{int length=strlen(yytext); for(int i=0;i<length;i++){if(yytext[i]=='\n') lineno++;}}
\/\/[^\n]*    								{lineno++;}
\"[^"]*\"  						{strcpy(yylval.strval,yytext);return STRING;} 
[1-9]{DIGIT}*|[0-9]					{strcpy(yylval.strval,yytext);return NUMBER;}
"int" 							{return INT;}
"str"							{return STR;}
"void"							{return VOID;}
"if"							{return IF;}
"else"							{return ELSE;}
"while"							{return WHILE;}
"return"						{return RETURN;}
"print"							{return PRINT;}
"scan"							{return SCAN;}
";"|","|"="|"{"|"}"|"["|"]"|"+"|"-"|"*"|"/"|"%"|"("|")"		{return *yytext;}
">="|"<="|"=="|">"|"<"			{strcpy(yylval.strval,yytext);return CMP;}
[_a-zA-Z][_a-zA-Z0-9]*			{strcpy(yylval.strval,yytext);return ID;}
. 								{yyerror("invalid character");}

%%

int yywrap(void) 
{
	return 1;
}
