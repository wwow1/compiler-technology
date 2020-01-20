%{
	#include<stdio.h>
	#include<string.h>
	int yylval=0;	
	int line=1;	enum{INI=258,LONG,CHAR,DFLOAT,DOUBLE,ID,STRING,INC,DEC,AND,OR,RS,LS,RSE,LSE,URS,ERROR,FZF,START};
	char* keywords[]={"abstract","boolean","break","byte","case","catch","assert",\
	"char","class","const","continue","default","do","double","else","else","enum",\
	"extends","final","finally","float","for","goto","if","implements","import",\
	"instanceof","int","interface","long","native","new","package","private",\
	"protected","public","return","short","static","String","strictfp","super","switch",\
	"synchronized","this","throw","throws","transient","try","void","volatile","while","END"};
%}
DIGIT       [0-9]
LETTER 	    [a-zA-Z]
FIRST_ID    [a-zA-Z_$]
NEXT_ID		[a-zA-Z0-9_\$]
SP_SYMBOL 		[-/\+\*&\^\|<>=%]
SPACE 		[ \n\t]
SYMBOL 		[\?\+\*/><\{}():\'\"\^&\|\.;,@=\[\]%-]
%%
[ \t]*					{} 
[\n]					{line++;}
{DIGIT}+[lL]			{printf("line %4d   < %6d , %s , long>\n",line,LONG,yytext);}
[0][x]{DIGIT}+			{printf("line %4d   < %6d , %s , int>\n",line,INI,yytext);}
{DIGIT}+     			{printf("line %4d   < %6d , %s , int>\n",line,INI,yytext);}
{DIGIT}*"."{DIGIT}+[E]?[+-]?{DIGIT}+[D]		{printf("line %4d   < %6d , %s , double>\n",line,DOUBLE,yytext);}
{DIGIT}*"."{DIGIT}+[E]?[+-]?{DIGIT}+[fF]?    {printf("line %4d   < %6d , %s , float>\n",line,DFLOAT,yytext);}
\"[^"]*\"  			{yytext[strlen(yytext)-1]='\0';printf("line %4d   < %6d , %s ,string>\n",line,STRING,yytext+1);}
\/\/[^\n]*    			{}
\/\*([^\*^\/]*|[\*^\/*]*|[^\**\/]*)*\*\/	{for(int i=0;i<strlen(yytext);i++)if(yytext[i]=='\n') line++;}
[\'].[\']				{printf("line %4d   < %6d , %s , char>\n",line,CHAR,yytext);}
">>>"					{printf("line %4d   < %6d , %s , symbol>\n",line,URS,yytext);}
">>="					{printf("line %4d   < %6d , %s , symbol>\n",line,RSE,yytext);}
"<<="					{printf("line %4d   < %6d , %s , symbol>\n",line,LSE,yytext);}
"++"					{printf("line %4d   < %6d , %s , symbol>\n",line,INC,yytext);}
"--"					{printf("line %4d   < %6d , %s , symbol>\n",line,DEC,yytext);}
"&&"					{printf("line %4d   < %6d , %s , symbol>\n",line,AND,yytext);}
"||"					{printf("line %4d   < %6d , %s , symbol>\n",line,OR,yytext);}
">>"					{printf("line %4d   < %6d , %s , symbol>\n",line,RS,yytext);}
"<<"					{printf("line %4d   < %6d , %s , symbol>\n",line,LS,yytext);}
{SP_SYMBOL}"="				{printf("line %4d   < %6d , %s , symbol>\n",line,128+*yytext,yytext);}
{SYMBOL}				{printf("line %4d   < %6d , %s , symbol>\n",line,*yytext,yytext);}

"abstract"|"boolean"|"break"|"byte"|"case"|"catch"|"assert"|"char"|"class"|"const"|"continue"|"default"|"do"|"double"|"else"|"else"|"enum"|"extends"|"final"|"finally"|"float"|"for"|"goto"|"if"|"implements"|"import"|"instanceof"|"int"|"interface"|"long"|"native"|"new"|"package"|"private"|"protected"|"public"|"return"|"short"|"static"|"String"|"strictfp"|"super"|"switch"|"synchronized"|"this"|"throw"|"throws"|"transient"|"try"|"void"|"volatile"|"while" {int i=0;
					 while(strcmp(keywords[i],"END")){
					 	if(!strcmp(keywords[i],yytext)){
					 		printf("line %4d   < %6d , %s , keywords>\n",line,START+i,yytext);
							return 1;
					 	}
					 	i++;
					 }
					}

[a-zA-Z_$][a-zA-Z0-9_$]*   {printf("line %4d   < %6d , %s , id>\n",line,ID,yytext);}
.			  			{printf("line %4d   ERROR:invalid symbol %c.\n",line,*yytext);}

%%
int yywrap(){return 1;}
int main(int argc, char* argv[]) {
	int tok;
   	if (argc > 1) {
       if (!(yyin = fopen(argv[1], "r"))) {   
           perror(argv[1]);
           return 1;
       }
   }
   while (yylex());
   return 0;
}
