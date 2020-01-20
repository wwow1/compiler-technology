%{
	#include<stdio.h>
	#include<string.h>
	int yylval=0;	
	int line=1;
enum{ID = 258,STR,NUMBER,STRING,ASSIGN,CMP,PRINT,SCAN,RETURN,WHILE,IF,ELSE,INT,VOID,SYMBOL};
	int yylval;
%}
DIGIT [0-9]
%%
[ ]*	{}
[ \t]*	{}
[\n]	{line++;}
\/\*([^\*^\/]*|[\*^\/*]*|[^\**\/]*)*\*\/	{int i;for(i=0;i<strlen(yytext);i++){if(yytext[i]=='\n') line++;}}
\/\/[^\n]*    			{line++;}
"{"|"}"|","|";"|"("|")"|"["|"]"|"+"|"-"|"*"|"/"|"%"|"="|"-" {return yytext[0];}
\"[^"]*\"  			{return STRING;} 
[1-9]{DIGIT}*|[0-9]		{return NUMBER;}  
"str"				{return STR;}  
"assign"			{return ASSIGN;}
">="|"<="|"=="|">"|"<"		{return CMP;}
"print"				{return PRINT;}
"scan"				{return SCAN;}
"return"			{return RETURN;}
"while"				{return WHILE;}
"if"				{return IF;}
"else"				{return ELSE;}
"int"				{return INT;}
"void"				{return VOID;}
[_a-zA-Z][_a-zA-Z0-9]*		{return ID;}
.				{printf("ERROR:invalid symbol");}
%%
int yywrap(){return 1;}
/*
int main(int argc, char* argv[]) {
	int tok;
	char path[100];
	printf("Please input the path:");
	scanf("%s",path);
	strcat(path,"\0");
   	yyin=fopen(path,"r");
	if(!yyin) printf("error: open fail");
   	while (tok=yylex()){
		printf("%s\n",yytext);
	}
	fclose(yyin);
   	return 0;
}

int main(int argc, char* argv[]) {
	int tok;
   	if (argc > 1) {
       if (!(yyin = fopen(argv[1], "r"))) {   
           perror(argv[1]);
           return 1;
       }
   }
   while (tok=yylex()){
	printf("%s  %d\n",yytext,tok);
	}
   return 0;
}
*/
