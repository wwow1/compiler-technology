#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<stdbool.h>
extern int yylex();
extern char* yytext;
extern int line;
extern FILE* yyin;
#define MAX 10000
/*
typedef struct{
	int arrTok[MAX];
	int top;
}stack;
*/
enum{ID = 258,STR,NUMBER,STRING,ASSIGN,CMP,PRINT,SCAN,RETURN,WHILE,IF,ELSE,INT,VOID,};
//we should define ASSIGN STR CMP INT  VOID
extern int yylex();
extern int yylval;
extern char* yytext;
int tok;
bool external_declaration();
void advance();
bool program();
bool external_declaration();
bool decl_or_stmt();
bool declarator_list();
bool intstr_list();
bool intstr_list();
bool initializer();
bool declarator();
bool parameter_list();
bool parameter();
bool type();
bool statement();
bool statement_list();
bool expression_statement();
bool expr();
bool cmp_expr();
bool add_expr();
bool mul_expr();
bool primary_expr();
bool expr_list();
bool id_list();
/*
void push_stack(stack* p, int num) {
	if (p->top == MAX) {
		printf("the stack is full.\n");
		exit(0);
	}
	p->arrTok[p->top++] = num;
	return;
}
int top_stack(stack* p) {
	return p->arrTok[--p->top];
}
void pop_stack(stack* p) {
	p->top--;
	return;
}
int depth_stack(stack* p) {
	return p->top;
}
*/
void advance() {
    tok = yylex();
    //printf("tok: %s\n", yytext);
}
bool program() {  // the return type?
    while (external_declaration()){
        if(!tok){
            return true;
        }
    }
    return false;
    //advance();
}
bool external_declaration() {
    if (!type()) {  // !!!!we should write a type function
        return false;
    }
    //advance();
    if (!declarator()) {
        return false;
    }
    //advance();
    if (!decl_or_stmt()) {
        return false;
    }
    //advance();
    return true;
}
bool decl_or_stmt() {  // little boss
    if (tok == '{') {
        advance();
        if (tok == '}') {  // 1
            advance();
            return true;
        }
        if (statement_list()) {   // 2
            if (tok == '}') {   // the order of 1 and 2 is very important,please be careful
                advance();
                return true;
            }
            else {
                return false;
            }
        }
        return false;
    }
    else if (tok == ',') {
        advance();
        if (!declarator_list()) {
            return false;
        }
        if (tok == ';') {
            advance();
            return true;
        }
        else
            return false;
    }
    else if (tok == ';') {
        advance();
        return true;
    }
    else {  // maybe it is unnecessary
        return false;
    }
}
bool declarator_list() {
    if (!declarator()) {
        return false;
    }
    //advance();
    while (tok == ',') {
        advance();
        if (!declarator()) {
            return false;
        }
        //advance();
    }
    return true;
}
bool intstr_list() {
    if (!initializer()) {
        return false;
    }
    //advance();
    while (tok == ',') {
        advance();
        if (!initializer()) {
            return false;
        }
        //advance();
    }
    return true;
}
bool initializer() {
    if (tok == NUMBER || tok == STRING) {
        advance();
        return true;
    }
    return false;
}
bool declarator() { // little boss
    if (tok != ID) {
        return false;
    }
    advance();
    if (tok == '='){
        advance();
        if (expr()) {
            return true;
        }
        return false;
    }
    else if (tok == '(') {
        advance();
        if (tok==')') {   // be careful check again
            advance();
            return true;
        }
        if (parameter_list()) {
            if (tok == ')') {
                advance();
                return true;
            }
            return false;
        }
        else
            return false;//changed
    }
    else if (tok == '[') {
        advance();
        if (tok == ']') {
            advance();
            if (tok == '=') {
                advance();
                if (tok == '{') {
                    advance();
                    if (intstr_list()) {
                        if (tok == '}') {
                            advance();
                            return true;
                        }
                        else
                            return false;
                    }
                    else {
                        return false;
                    }
                }
                else
                    return false;
            }
            else {
                return true;
            }
        }
        if (expr()) {
            if (tok == ']') {
                advance();
                if (tok == '=') {
                    advance();
                    if (tok == '{') {
                        advance();
                        if (intstr_list()) {
                            if (tok == '}') {
                                advance();
                                return true;
                            }
                            else
                                return false;
                        }
                        else
                            return false;
                    }
                    else
                        return false;
                }
                else {
                    return true;
                }
            }
            return false;
        }
        return false;
    }
    return true;
}
bool parameter_list() {
    if (!parameter()) {
        return false;
    }
    //advance();
    while (tok == ',') {
        advance();
        if (!parameter()) {
            return false;
        }
        //advance();
    }
    return true;
}
bool parameter() {
    if(!type()){
        return false;
    }
    if (tok == ID) {
        advance();
        return true;
    }
    return false;
}
bool type() {
    if (tok == INT || tok == STR || tok == VOID) {
        advance();
        return true;
    }
    return false;
}
bool statement() {  //!!!! big boss
    if (type()) {
        if (declarator_list()) {
            if (tok == ';') {
                advance();
                return true;
            }
            else
                return false;
        }
        else
            return false;
    }
    else if (tok == '{') {
        advance();
        if (statement_list()) {//have changed
            if (tok == '}') {
                advance();
                return true;
            }
            else
                return false;
        }
        else
            return false;
    }
    else if (expression_statement()) {
        return true;
    }
    else if (tok == IF) {
        advance();
        if (tok == '(') {
            advance();
            if (expr()) {
                if (tok == ')') {
                    advance();
                    if (statement()) {
                        if (tok == ELSE) {
                            advance();
                            if (statement()) {
                                return true;
                            }
                            else
                                return false;
                        }
                        else
                            return true;
                    }
                    else
                        return false;
                }
                else
                    return false;
            }
            else
                return false;
        }
        else
            return false;
    }
    else if (tok==WHILE) {
        advance();
        if (tok == '(') {
            advance();
            if (expr()) {
                if (tok == ')') {
                    advance();
                    if (statement()) {
                        return true;
                    }
                    else
                        return false;
                }
                else
                    return false;
            }
            else
                return false;
        }
        else
            return false;
    }
    else if (tok == RETURN) {
        advance();
        if (tok == ';') {
            advance();
            return true;
        }
        else if (expr()) {
            if (tok == ';') {
                advance();
                return true;
            }
            else
                return false;
        }
        else
            return false;
    }
    else if (tok == PRINT) {
        advance();
        if (tok == ';') {
            advance();
            return true;
        }
        else if (expr_list()) {
            if (tok == ';') {
                advance();
                return true;
            }
            else
                return false;
        }
        else
            return false;
    }
    else if (tok == SCAN) {
        advance();
        if (id_list()) {
            if (tok == ';') {
                advance();
                return true;
            }
            else
                return false;
        }
        else
            return false;
    }
    else //??? suojin?
        return false;
}
bool statement_list() {  //!!! change
    while (statement()){
        if(tok!=INT && tok!=STR && tok!=VOID && tok!='{' && tok!=IF \
        && tok!=WHILE && tok!=RETURN && tok!=PRINT && tok!=SCAN && tok!=';'\
        && tok!='-' && tok!='(' && tok!=ID && tok!=NUMBER   && tok!=STRING)
            return true;
    }
    //advance();
    return false;
}
bool expression_statement() {
    if (tok == ';') {
        advance();
        return true;
    }
    else if (expr()) {
        //advance();
        if(tok == ';') {
            advance();
            return true;
        }
        return false;
    }
    return false;
}
bool expr() {
    if (cmp_expr()) {
        //advance();
        return true;
    }
    else
        return false;
}
bool cmp_expr() {
    if (!add_expr()) {
        return false;
    }
    //advance();
    while (tok == CMP) {
        advance();
        if (!add_expr()) {
            return false;
        }
        //advance();
    }
    return true;
}
bool add_expr() {
    if (!mul_expr()) {
        return false;
    }
    //advance();
    while (tok == '+' || tok == '-') {
        advance();
        if (!mul_expr()) {
            return false;
        }
        //advance();
    }
    return true;
}
bool mul_expr() {
    if (tok == '-') {
        advance();
        if (primary_expr()) {
            //advance();
            return true;
        }
        return false;
    }
    if (primary_expr()) {
        //advance();
        while (tok == '*' || tok == '/' || tok == '%') {
            advance();
            if (!primary_expr()) {
                return false;
            }
            //advance();
        }
        return true;
    }
    return false;
}
bool primary_expr() {  // thinking about the ID how to match
    if (tok == ID) {
        advance();
        if (tok == '(') {
            advance();
            if (tok == ')') { //the order?
                advance();
                return true;
            }
            else if (expr_list()) {
                if (tok == ')') {
                    advance();
                    return true;
                }
                else
                    return false;
            }
            else
                return false;
        }
        else if (tok == ASSIGN) {
            advance();
            if (expr()) {
                return true;
            }
            else
                return false;
        }
        else if (tok == '=') {
            advance();
            if (expr()) {
                return true;
            }
            else
                return false;
        }
        else if (tok == '[') {
            advance();
            if (expr()) {
                if (tok == ']') {
                    advance();
                    if (tok == '=') {
                        advance();
                        if (expr()) {
                            return true;
                        }
                        else
                            return false;
                    }
                    else
                        return true;
                }
                else
                    return false;
            }
            else
                return false;
        }
        else {
            return true;
        }
    }
    else if (tok == NUMBER) {
        advance();
        return true;
    }
    else if (tok == STRING) {
        advance();
        return true;
    }
    else if (tok == '(') {
        advance();
        if (expr()) {
            if (tok == ')') {
                advance();
                return true;
            }
        }
        else
            return false;
    }
    else
        return false;
}
bool expr_list() {
    if (!expr()) {
        return false;
    }
    //advance();
    while (tok == ',') {
        advance();
        if (!expr()) {
            return false;
        }
        //advance();
    }
    return true;
}
bool id_list() {   //!!!!warning no back
    if (tok != ID) {
        return false;
    }
    advance();
    while (tok == ',') {  // we should know tok is what
        advance();
        if (tok != ID) {
            return false;// I don't sure false or true.//may be false
        }
        advance();
    }
    return true;
}
int main(int argc,char* argv[]) {
    
    if (argc > 1) {
        if (!(yyin = fopen(argv[1], "r"))) {
            perror(argv[1]);
            return 1;
        }
    }
     /*

    char path[100];
    printf("Please input the path:");
    scanf("%s",path);
    strcat(path,"\0");
    yyin=fopen(path,"r");
*/
    advance();
    if (program()) {
        printf("this program is right.");
    }
    else {
        printf("line:%d %s does not conform to grammar rules\n",line,yytext);
        printf("Error:this program is wrong!!!");
    }
    fclose(yyin);
    return 0;
}
