%{

#include <stdio.h>
#include<stdlib.h>
#include<string.h>
#include "ast.h"
int yylex();
void yyerror(char *);
extern FILE* yyin;
//#define YYDEBUG 1
%}

%union{
	int intval;
	char strval[100];
	past pAst;
}
%token INT
%token STR
%token VOID
%token IF
%token ELSE
%token RETURN
%token PRINT
%token SCAN
%token CMP
%token ASSIGN
%token WHILE
%token <pAst> ID
%token <pAst> NUMBER
%token <pAst> STRING
%type  <pAst> program external_declaration function_definition declaration
%type  <pAst> init_declarator 
%type  <pAst> init_declarator_list
%type  <pAst> intstr_list initializer declarator
%type  <pAst> direct_declarator parameter_list parameter type
%type  <pAst> statement  compound_statement begin_scope end_scope
%type  <pAst> statement_list expression_statement selection_statement
%type  <pAst> jump_statement
%type  <pAst> iteration_statement   print_statement 
%type  <pAst> scan_statement  expr  assign_expr cmp_expr add_expr mul_expr primary_expr 
%type  <pAst> expr_list id_list id cmp

%%

program: external_declaration					{showAst($1,0);}
        | program external_declaration			{showAst($2,0);}
        ;
	

external_declaration: function_definition		{$$=$1;}
                    | declaration  			{$$=$1;}
                    ;
		    

function_definition: type declarator compound_statement		{addExpr($1,$2);addExpr($1,$3);$$=newExpr(NULL,"function_definition",$1);}
                   ;

declaration: type init_declarator_list ';'		{addExpr($1,$2);$$=newExpr(NULL,"declaration",$1);}
            ;

init_declarator_list: init_declarator  			{$$=$1;}
                     | init_declarator_list ',' init_declarator  {addExpr($1,$3);$$=$1;}
                     ;

init_declarator: declarator  							{$$=$1;}
                | declarator '=' add_expr 				{addExpr($1,$3);$$=newExpr("=","init_declarator",$1);}
                | declarator '=' '{' intstr_list '}'	{past tmp=newExpr("{}","intstr_list",$4);addExpr($1,tmp);$$=newExpr("=","init_declarator",$1);}
                ;

intstr_list: initializer								{$$=$1;}
            | intstr_list ',' initializer				{addExpr($1,$3);$$=$1;}
            ;

initializer: NUMBER								{$$=newExpr(yylval.strval,"number",NULL);}
            | STRING							{$$=newExpr(yylval.strval,"string",NULL);}
            ;

declarator: direct_declarator 					{$$=$1;}
          ;
direct_declarator: id 									{$$=$1;}
                  | direct_declarator '(' parameter_list ')'	{past tmp=newExpr("()","parameter_list",$3);addExpr($1,$3);$$=$1;}
                  | direct_declarator '(' ')'					{$$=$1;}
                  | id '[' expr ']'								{strcat($1->ivalue,"[expr]");addExpr($1,$3);$$=$1;}
                  | id '[' ']'									{strcat($1->ivalue,"[]");$$=$1;}
                  ;

parameter_list: parameter 										{$$=$1;}
               | parameter_list ',' parameter 					{addExpr($1,$3);$$=$1;}
               ;

parameter: type id 												{addExpr($1,$2);$$=newExpr(NULL,"parameter",$1);}
         ;

type: INT 														{$$=newExpr(NULL,"int",NULL);}
     | STR 														{$$=newExpr(NULL,"str",NULL);}
     | VOID 													{$$=newExpr(NULL,"void",NULL);}
     ;

statement: compound_statement  									{$$=$1;}
          | expression_statement 								{$$=$1;}
          | selection_statement 								{$$=$1;}
          | iteration_statement 								{$$=$1;}
          | jump_statement   									{$$=$1;}
          | print_statement										{$$=$1;}
          | scan_statement 										{$$=$1;}
          | declaration 										{$$=$1;}
          ;

compound_statement: begin_scope end_scope 						{addExpr($1,$2);$$=newExpr(NULL,"compound_statement",$1);}
                  | begin_scope statement_list end_scope 		{addExpr($1,$2);addExpr($1,$3);$$=newExpr(NULL,"compound_statement",$1);}
                  ;

begin_scope: '{' 												{$$=newExpr(NULL,"{",NULL);}
           ;

end_scope: '}' 													{$$=newExpr(NULL,"}",NULL);}
         ;

statement_list: statement 										{$$=$1;}
               | statement_list statement 						{addExpr($1,$2);$$=$1;}
               ;

expression_statement: ';' 										{$$=newExpr(";","expression_statement",NULL);} 
                      | expr ';' 								{$$=newExpr(NULL,"expression_statement",$1);}
                      ;

selection_statement: IF '(' expr ')' statement 					{addExpr($3,$5);$$=newExpr("if","selection_statement",$3);}
                    | IF '(' expr ')' statement ELSE statement  {addExpr($3,$5);addExpr($3,$7);$$=newExpr("if-else","selection_statement",$3);}
                    ;

iteration_statement: WHILE '(' expr ')' statement 				{addExpr($3,$5);$$=newExpr("while","iteration_statement",$3);}
                   ;

jump_statement: RETURN ';' 										{$$=newExpr("return","jump_statement",NULL);}
               | RETURN expr ';' 								{$$=newExpr("return","jump_statement",$2);}
               ;

print_statement: PRINT ';' 										{$$=newExpr("print","print_statement",NULL);}
                | PRINT expr_list ';' 							{$$=newExpr("print","print_statement",$2);}
                ;

scan_statement: SCAN id_list ';' 								{$$=newExpr("scan","scan_statement",$2);}
              ;

expr: assign_expr 												{$$=$1;}
    ;

assign_expr: cmp_expr 											{$$=$1;}
            | id ASSIGN assign_expr 						{addExpr($1,$3);$$=newExpr("assign","assign_expr",$1);}
            | id '=' assign_expr							{addExpr($1,$3);$$=newExpr("=","assign_expr",$1);}
            | id '[' expr ']' '=' assign_expr 				{strcat($1->ivalue,"[]");addExpr($1,$3),addExpr($1,$6);$$=newExpr("=","assign_expr",$1);}
            ;

cmp_expr: add_expr 											{$$=$1;}
         | cmp_expr cmp add_expr							{addExpr($1,$3);$2->left=$1;$$=$2;}
         ;

add_expr: mul_expr											{$$=$1;}
         | add_expr '+' mul_expr 							{addExpr($1,$3);$$=newExpr("+","add_expr",$1);}
         | add_expr '-' mul_expr 							{addExpr($1,$3);$$=newExpr("-","add_expr",$1);}
         ;

mul_expr: primary_expr 										{$$=$1;}
         | mul_expr '*' primary_expr 						{addExpr($1,$3);$$=newExpr("*","mul_expr",$1);}
         | mul_expr '/' primary_expr 						{addExpr($1,$3),$$=newExpr("/","mul_expr",$1);}
         | mul_expr '%' primary_expr 						{addExpr($1,$3);$$=newExpr("%","mul_expr",$1);}
         | '-' primary_expr 								{$$=newExpr("-","mul_expr",$2);}
         ;

primary_expr: id '(' expr_list ')' 			{addExpr($1,$3);$$=newExpr("id(expr_list)","primary_expr",$1);}
             | id '(' ')'									{$$=newExpr("id()","primary_expr",$1);}
             | '(' expr ')'									{$$=newExpr("(expr)","primary_expr",$2);}
             | id 											{$$=$1;}
             | initializer 									{$$=$1;}
             | id '[' expr ']' 				{addExpr($1,$3);$$=newExpr("id[expr]","primary_expr",$1);}
             ;

expr_list: expr 											{$$=$1;}
          | expr_list ',' expr 								{addExpr($1,$3);$$=$1;}
          ;

id_list: id 												{$$=$1;}
        | id_list ',' id 									{addExpr($1,$3);$$=$1;}
        ;
id:ID 				{$$=newExpr(yylval.strval,"id",NULL);}
  ;

cmp:CMP				{$$=newExpr(yylval.strval,"cmp_expr",NULL);}
   ;

%%

