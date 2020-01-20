#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<stdbool.h>
extern int yylex();
extern char* yytext;
extern int line;
extern FILE* yyin;
typedef struct _ast ast;
typedef struct _ast* past;
struct _ast{
    bool judge;
    bool ivalueMark;
    bool print;
    char ivalue[100];
    char nodeType[100];
    past left;
    past right;
};
enum{ID = 258,STR,NUMBER,STRING,ASSIGN,CMP,PRINT,SCAN,RETURN,WHILE,IF,ELSE,INT,VOID,};
//we should define ASSIGN STR CMP INT  VOID
extern int yylex();
extern int yylval;
extern char* yytext;
int tok;
past external_declaration();
void advance();
past program();
past external_declaration();
past decl_or_stmt();
past declarator_list();
past intstr_list();
past intstr_list();
past initializer();
past declarator();
past parameter_list();
past parameter();
past type();
past statement();
past statement_list();
past expression_statement();
past expr();
past cmp_expr();
past add_expr();
past mul_expr();
past primary_expr();
past expr_list();
past id_list();
past newAST(char* Type){
    past tmp=(past)malloc(sizeof(ast));
    strcpy(tmp->nodeType,Type);
    tmp->left=NULL;
    tmp->right=NULL;
    tmp->judge=true;
    tmp->ivalueMark=false;
    tmp->print=true;
    return tmp;
}
void ADD(past mid,past l,past r){
    if(r!=NULL){
        if(mid->right==NULL){
            mid->right=r;
        }
        else
            ADD(mid->right,NULL,r);
    }
    if(l!=NULL){
        mid->left=l;
    }
}
void advance() {
    tok = yylex();
    //printf("tok: %s\n", yytext);
}
past program() {  // the return type?
    past now=newAST("program");
    past first=NULL;
    past tmp=NULL;
    first=external_declaration();
    if(first->judge==false){
        now->left=first;
        now->judge=false;
        return now;
    }
    if(!tok){
        now->left=first;
        return now;
    }
    tmp=external_declaration();
    while (tmp->judge){
        ADD(first,NULL,tmp);
        if(!tok){
            now->left=first;
            return now;
        }
        tmp=external_declaration();
    }
    now->left=first;
    now->judge=false;
    return now;
    //advance();
}
past external_declaration() {
    past now=newAST("external_declaration");
    past first=NULL;past tmp=NULL;
    first=type();
    if (first->judge==false) {
        now->judge=false;
        return now;
    }
    //advance();
    tmp=declarator();
    if (tmp->judge==false) {
        ADD(first,NULL,tmp);
        now->left=first;
        now->judge=false;
        return now;
    }
    else
        ADD(first,NULL,tmp);
    //advance();
    tmp=decl_or_stmt();
    if (tmp->judge==false) {
        ADD(first,NULL,tmp);
        now->judge=false;
    }
    else
        ADD(first,NULL,tmp);
    //advance();
    now->left=first;
    return now;
}
past decl_or_stmt() {  // little boss
    past now=newAST("decl_or_stmt");
    past first=NULL;
    past tmp=NULL;
    if (tok == '{') {
        advance();
        if (tok == '}') {  // 1
            advance();
            return now;
        }
        first=statement_list();
        if (first->judge) {   // 2
            if (tok == '}') {   // the order of 1 and 2 is very important,please be careful
                advance();
                now->left=first;
                return now;
            }
            else {
                now->left=first;
                now->judge=false;
                return now;
            }
        }
        now->judge=false;
        return now;
    }
    else if (tok == ',') {
        advance();
        first=declarator_list();
        if (first->judge==false) {
            now->judge=false;
            return now;
        }
        else{
            now->left=first;
        }
        if (tok == ';') {
            advance();
        }
        else{
            now->judge=false;
        }
        return now;
    }
    else if (tok == ';') {
        advance();
        return now;
    }
    else {  // maybe it is unnecessary
        now->judge=false;
        return now;
    }
}
past declarator_list() {
    past now=newAST("declarator_list");
    now->print=false;
    past first=NULL;past tmp=NULL;
    first=declarator();
    if (first->judge==false) {
        now->judge=false;
        return now;
    }
    //advance();
    while (tok == ',') {
        advance();
        tmp=declarator();
        if (tmp->judge==false) {
            now->judge=false;
            now->left=first;
            return now;
        }
        else
            ADD(first,NULL,tmp);
        //advance();
    }
    now->left=first;
    return now;
}
past intstr_list() {
    past now=newAST("intstr_list");
    past first=NULL;past tmp=NULL;
    now->print=false;
    first=initializer();
    if (first->judge==false) {
        now->judge=false;
        return now;
    }
    //advance();
    while (tok == ',') {
        advance();
        tmp=initializer();
        if (tmp->judge==false) {
            now->judge=false;
            now->left=first;
            return now;
        }
        else
            ADD(first,NULL,tmp);
        //advance();
    }
    now->left=first;
    return now;
}
past initializer() {
    past now=newAST("initializer");
    past first=NULL;now->print=false;
    if (tok == NUMBER) {
        first=newAST("number");
        strcpy(first->ivalue,yytext);first->ivalueMark=true;
        advance();
        now->left=first;
    }
    else if(tok==STRING){
        first=newAST("string");
        strcpy(first->ivalue,yytext);first->ivalueMark=true;
        advance();
        now->left=first;
    }
    else
        now->judge=false;
    return now;
}
past declarator() { // little boss
    past now=newAST("declarator");
    past first=NULL;past tmp=NULL;
    if (tok == ID) {
        first=newAST("ID");
        strcpy(first->ivalue,yytext);first->ivalueMark=true;
    }
    else{
        now->judge=false;
        return now;
    }
    advance();
    if (tok == '='){
        advance();
        strcpy(now->ivalue,"=");now->ivalueMark=true;
        tmp=expr();
        if (tmp->judge) {
            ADD(first,NULL,tmp);
            now->left=first;
            return now;
        }
        else{
            now->judge=false;
            now->left=first;
            return now;
        }
    }
    else if (tok == '(') {
        advance();
        if (tok==')') {   // be careful check again
            advance();
            now->left=first;
            return now;
        }
        tmp=parameter_list();
        if (tmp->judge) {
            ADD(first,NULL,tmp);
            if (tok == ')') {
                advance();
                now->left=first;
                return now;
            }
            now->left=first;
            now->judge=false;
            return now;
        }
        else{
            now->judge=false;
            now->left=first;
            return now;
        }
    }
    else if (tok == '[') {
        advance();
        if (tok == ']') {
            advance();
            if (tok == '=') {
                strcpy(now->ivalue,"=");now->ivalueMark=true;
                advance();
                if (tok == '{') {
                    advance();
                    tmp=intstr_list();
                    if (tmp->judge) {
                        ADD(first,NULL,tmp);
                        if (tok == '}') {
                            advance();
                            now->left=first;
                            return now;
                        }
                        else{
                            now->judge=false;
                            now->left=first;
                            return now;
                        }
                    }
                    else {
                        now->judge=false;
                        now->left=first;
                        return now;
                    }
                }
                else{
                    now->judge=false;
                    now->left=first;
                    return now;
                }
            }
            else {
                now->left=first;
                return now;
            }
        }
        tmp=expr();
        if (tmp->judge) {
            if (tok == ']') {
                advance();
                if (tok == '=') {
                    strcpy(now->ivalue,"=");now->ivalueMark=true;
                    advance();
                    if (tok == '{') {
                        advance();
                        tmp=intstr_list();
                        if (tmp->judge) {
                            ADD(first,NULL,tmp);
                            if (tok == '}') {
                                advance();
                            }
                            else{
                                now->judge=false;
                                now->left=first;
                                return now;
                            }
                        }
                        else{
                            now->judge=false;
                            now->left=first;
                            return now;
                        }
                    }
                    else{
                        now->judge=false;
                        now->left=first;
                        return now;
                    }
                }
                else {
                    now->left=first;
                    return now;
                }
            }
            else{
                now->judge=false;
                now->left=first;
                return now;
            }
        }
        else{
            now->left=first;
            now->judge=false;
            return now;
        }
    }
    now->left=first;
    return now;
}
past parameter_list() {
    past now=newAST("parameter_list");
    now->print=false;
    past first=NULL;past tmp=NULL;
    first=parameter();
    if (first->judge==false) {
        now->judge=false;
        return now;
    }
    //advance();
    while (tok == ',') {
        advance();
        tmp=parameter();
        if (tmp->judge==false) {
            now->judge=false;
            return now;
        }
        else
            ADD(first,NULL,tmp);
        //advance();
    }
    now->left=first;
    return now;
}
past parameter() {
    past now=newAST("parameter");
    past first=NULL;past tmp=NULL;
    first=type();
    if(first->judge==false){
        now->judge=false;
        return now;
    }
    if (tok == ID) {
        tmp=newAST("ID");
        strcpy(tmp->ivalue,yytext);tmp->ivalueMark=true;
        ADD(first,NULL,tmp);
        now->left=first;
        advance();
        return now;
    }
    now->left=first;
    now->judge=false;
    return now;
}
past type() {
    past now=newAST("type");
    past first=NULL;
    if (tok == INT) {
        strcpy(now->ivalue,"int");now->ivalueMark=true;
        advance();
        now->left=first;
        return now;
    }
    if(tok==STR){
        strcpy(now->ivalue,"str");now->ivalueMark=true;
        advance();
        now->left=first;
        return now;
    }
    if(tok==VOID){
        strcpy(now->ivalue,"void");now->ivalueMark=true;
        advance();
        now->left=first;
        return now;
    }
    now->judge=false;
    return now;
}
past statement() {  //!!!!   the tree is delay
    past now=newAST("statement");
    past first=NULL;past tmp=NULL;
    first=type();
    if (first->judge) {
        tmp=declarator_list();
        if (tmp->judge) {
            ADD(first,NULL,tmp);
            if (tok == ';') {
                advance();
                now->left=first;
                return now;
            }
            else{
                now->judge=false;
                now->left=first;
                return now;
            }
        }
        else{
            now->judge=false;
            now->left=first;
            return now;
        }
    }
    else if (tok == '{') {
        advance();
        first=statement_list();
        if (first->judge) {//have changed
            if (tok == '}') {
                advance();
                now->left=first;
                return now;
            }
            else{
                now->judge=false;
                now->left=first;
            }
        }
        else{
            now->judge=false;
            return now;
        }
    }
    first=expression_statement();
    if (first->judge) {
        now->left=first;
        return now;
    }
    else if (tok == IF) {
        strcpy(now->ivalue,"if");now->ivalueMark=true;
        advance();
        if (tok == '(') {
            advance();
            first=expr();
            if (first->judge) {
                if (tok == ')') {
                    advance();
                    tmp=statement();
                    if (tmp->judge) {
                        ADD(first,NULL,tmp);
                        if (tok == ELSE) {
                            advance();
                            tmp=statement();
                            if (tmp->judge){
                                strcpy(tmp->ivalue,"else");tmp->ivalueMark=true;
                                ADD(first,NULL,tmp);
                                now->left=first;
                                return now;
                            }
                            else{
                                now->judge=false;
                                now->left=first;
                                return now;
                            }
                        }
                        else{
                            now->judge=false;
                            now->left=first;
                            return now;
                        }
                    }
                    else{
                        now->judge=false;
                        now->left=first;
                        return now;
                    }
                }
                else{
                    now->judge=false;
                    now->left=first;
                    return now;
                }
            }
            else{
                now->judge=false;
                return now;
            }
        }
        else{
            now->judge=false;
            return now;
        }
    }
    else if (tok==WHILE) {
        advance();
        strcpy(now->ivalue,"while");now->ivalueMark=true;
        if (tok == '(') {
            advance();
            first=expr();
            if (first->judge) {
                if (tok == ')') {
                    advance();
                    tmp=statement();
                    if (tmp->judge) {
                        ADD(first,NULL,tmp);
                        now->left=first;
                        return now;
                    }
                    else{
                        now->left=first;
                        now->judge=false;
                        return now;
                    }
                }
                else{
                    now->left=first;
                    now->judge=false;
                    return now;
                }
            }
            else{
                now->judge=false;
                return now;
            }
        }
        else{
            now->judge=false;
            return now;
        }
    }
    else if (tok == RETURN) {
        advance();
        strcpy(now->ivalue,"return");now->ivalueMark=true;
        if (tok == ';') {
            advance();
            return now;
        }
        first=expr();
        if (first->judge) {
            if (tok == ';') {
                advance();
                now->left=first;
                return now;
            }
            else{
                now->judge=false;
                now->left=first;
                return now;
            }
        }
        else{
            now->judge=false;
            return now;
        }
    }
    else if (tok == PRINT) {
        advance();
        strcpy(now->ivalue,"print");now->ivalueMark=true;
        if (tok == ';') {
            advance();
            return now;
        }
        first=expr_list(); // if location
        if (first->judge) {
            if (tok == ';') {
                advance();
                now->left=first;
                return now;
            }
            else{
                now->left=first;
                now->judge=false;
                return now;
            }
        }
        else {
            now->judge = false;
            return now;
        }
    }
    else if (tok == SCAN) {
        advance();
        strcpy(now->ivalue,"scan");now->ivalueMark=true;
        first=id_list();
        if (first->judge) {
            if (tok == ';') {
                advance();
                now->left=first;
                return now;
            }
            else{
                now->judge=false;
                now->left=first;
                return now;
            }
        }
        else{
            now->judge=false;
            return now;
        }
    }
    else{
        now->judge=false;
        return now;
    }
}
past statement_list() {  //!!! change
    past now=newAST("statement_list");
    past first=NULL;past tmp=NULL;
    now->print=false;
    first=statement();
    if(first->judge==false){
        now->judge=false;return now;
    }
    if(tok!=INT && tok!=STR && tok!=VOID && tok!='{' && tok!=IF \
        && tok!=WHILE && tok!=RETURN && tok!=PRINT && tok!=SCAN && tok!=';'\
        && tok!='-' && tok!='(' && tok!=ID && tok!=NUMBER   && tok!=STRING){
        now->left=first;
        return now;
    }
    tmp=statement();
    while (tmp->judge){
        ADD(first,NULL,tmp);
        if(tok!=INT && tok!=STR && tok!=VOID && tok!='{' && tok!=IF \
        && tok!=WHILE && tok!=RETURN && tok!=PRINT && tok!=SCAN && tok!=';'\
        && tok!='-' && tok!='(' && tok!=ID && tok!=NUMBER   && tok!=STRING){
            now->left=first;
            return now;
        }
        tmp=statement();
    }
    now->left=first;
    now->judge=false;
    return now;
}
past expression_statement() {
    past now=newAST("expression_statement");
    past first=NULL;
    now->print=false;
    if (tok == ';') {
        advance();
        return now;
    }
    first=expr();
    if (first->judge) {
        //advance();
        if(tok == ';') {
            advance();
            now->left=first;
            return now;
        }
        now->left=first;now->judge=false;
        return now;
    }
    now->judge=false;
    return now;
}
past expr() {
    past now=newAST("expr");
    past first=NULL;
    now->print=false;
    first=cmp_expr();
    if (first->judge) {
        //advance();
        now->left=first;
        return now;
    }
    else{
        now->judge=false;
        return now;
    }
}
past cmp_expr() {
    past now=newAST("cmp_expr");
    past first=NULL;past tmp=NULL;
    first=add_expr();
    if (first->judge==false) {
        now->judge=false;
        return now;
    }
    //advance();
    while (tok == CMP) {
        strcpy(now->ivalue,yytext);now->ivalueMark=true;
        advance();
        tmp=add_expr();
        if (tmp->judge==false) {
            now->left=first;
            now->judge=false;
        }
        else
            ADD(first,NULL,tmp);
        //advance();
    }
    now->left=first;
    if(!now->ivalueMark)
        now->print=false;
    return now;
}
past add_expr() {
    past now=newAST("add_expr");
    past first=NULL;past tmp=NULL;
    first=mul_expr();
    if (first->judge==false) {
        now->judge=false;
        return now;
    }
    //advance();
    while (tok == '+' || tok == '-') {
        strcpy(now->ivalue,yytext);now->ivalueMark=true;
        advance();
        tmp=mul_expr();
        if (tmp->judge==false) {
            now->left=first;
            now->judge=false;
            return now;
        }
        else
            ADD(first,NULL,tmp);
        //advance();
    }
    now->left=first;
    if(!now->ivalueMark)
        now->print=false;
    return now;
}
past mul_expr() {
    past now=newAST("mul_expr");
    past first=NULL;past tmp=NULL;
    if (tok == '-') {
        advance();
        first=primary_expr();
        strcpy(now->ivalue,"-");now->ivalueMark=true;
        if (first->judge) {
            //advance();
            now->left=first;
            return now;
        }
        now->judge=false;
        return now;
    }
    first=primary_expr();
    if (first->judge) {
        //advance();
        while (tok == '*' || tok == '/' || tok == '%') {
            strcpy(now->ivalue,yytext);now->ivalueMark=true;
            advance();
            tmp=primary_expr();
            if (tmp->judge==false) {
                now->judge=false;
                now->left=first;
                return now;
            }
            else{
                ADD(first,NULL,tmp);
            }
        }
        if(!now->ivalueMark)
            now->print=false;
        now->left=first;
        return now;
    }
    now->judge=false;
    return now;
}
past primary_expr() {
    past now=newAST("primary_expr");
    past first=NULL;past tmp=NULL;
    if (tok == ID) {
        first=newAST("ID");
        strcpy(first->ivalue,yytext);first->ivalueMark=true;
        advance();
        if (tok == '(') {
            advance();
            if (tok == ')') { //the order?
                advance();
                now->left=first;
                return now;
            }
            tmp=expr_list();
            if (tmp->judge) {
                tmp->print=true;
                ADD(first,NULL,tmp);
                if (tok == ')') {
                    advance();
                    now->left=first;
                    return now;
                }
                else{
                    now->judge=false;
                    now->left=first;
                }
            }
            else{
                now->judge=false;
                return now;
            }
        }
        else if (tok == ASSIGN) {
            advance();
            tmp=expr();
            strcpy(now->ivalue,"assign");now->ivalueMark=true;
            if (tmp->judge) {
                ADD(first,NULL,tmp);
                now->left=first;
                return now;
            }
            else{
                now->judge=false;
                return now;
            }
        }
        else if (tok == '=') {
            advance();
            tmp=expr();
            ADD(first,NULL,tmp);
            strcpy(now->ivalue,"=");now->ivalueMark=true;
            if (tmp->judge) {
                now->left=first;
                return now;
            }
            else{
                now->judge=false;
                return now;
            }
        }
        else if (tok == '[') {
            advance();
            tmp=expr();
            if (tmp->judge) {
                ADD(first,NULL,tmp);
                if (tok == ']') {
                    advance();
                    if (tok == '=') {
                        advance();
                        tmp=expr();
                        strcpy(now->ivalue,"=");now->ivalueMark=true;
                        if (tmp->judge) {
                            ADD(first,NULL,tmp);
                            now->left=first;
                            return now;
                        }
                        else{
                            now->left=first;
                            now->judge=false;
                            return now;
                        }
                    }
                    else{
                        now->left=first;
                        return now;
                    }
                }
                else{
                    now->judge=false;
                    now->left=first;
                    return now;
                }
            }
            else{
                now->judge=false;
                return now;
            }
        }
        else {
            now->left=first;
            return now;
        }
    }
    else if (tok == NUMBER) {
        first=newAST("NUMBER");
        strcpy(first->ivalue,yytext);first->ivalueMark=true;
        now->left=first;
        advance();
        return now;
    }
    else if (tok == STRING) {
        first=newAST("STRING");
        strcpy(first->ivalue,yytext);first->ivalueMark=true;
        now->left=first;
        advance();
        return now;
    }
    else if (tok == '(') {
        advance();
        first=expr();
        if (first->judge) {
            if (tok == ')') {
                advance();
                now->left=first;
                return now;
            }
        }
        else{
            now->judge=false;
            return now;
        }
    }
    else{
        now->judge=false;
        return now;
    }
}
past expr_list() {
    past now=newAST("expr_list");
    past first=NULL;past tmp=NULL;
    now->print=false;
    first=expr();
    if (first->judge==false) {
        now->judge=false;
        return now;
    }
    //advance();
    while (tok == ',') {
        advance();
        tmp=expr();
        if (tmp->judge==false) {
            now->judge=false;
            now->left=first;
            return now;
        }
        else
            ADD(first,NULL,tmp);
        //advance();
    }
    now->left=first;
    return now;
}
past id_list() {
    past now=newAST("id_list");
    now->print=false;
    past tmp=NULL;past first=NULL;
    if (tok != ID) {
        now->judge=false;
        return now;
    }
    first=newAST("ID");
    strcpy(first->ivalue,yytext);first->ivalueMark=true;
    advance();
    while (tok == ',') {  // we should know tok is what
        advance();
        if (tok != ID) {
            now->judge=false;
            now->left=first;
            return now;// I don't sure false or true.//may be false
        }
        else{
            tmp=newAST("ID");
            strcpy(first->ivalue,yytext);first->ivalueMark=true;
            ADD(first,NULL,tmp);
        }
        advance();
    }
    now->left=first;
    return now;
}
void showAst(past root,int depth){
    int i;
    if(root==NULL)
        return;
    if((!strcmp(root->nodeType,"primary_expr") && !root->ivalueMark) || !root->print){
        showAst(root->left,depth);
        showAst(root->right,depth);
        return;
    }
    for(i=1;i<=depth;i++){
        printf("\t");
    }
    printf("(%d)%s",depth,root->nodeType);
    if(root->ivalueMark){
        printf("      %s\n",root->ivalue);
    }
    else
        printf("\n");
    showAst(root->left,depth+1);
    showAst(root->right,depth);
}
void free_tree(past root){
    if(root==NULL)
        return;
    free_tree(root->left);
    free_tree(root->right);
    free(root);
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
    past root=program();
    showAst(root,0);
    if (root->judge) {
        printf("this program is right.");
    }
    else {
        printf("line:%d %s does not conform to grammar rules\n",line,yytext);
        printf("Error:this program is wrong!!!");
    }
    printf("\n");
    fclose(yyin);
    free_tree(root);
    return 0;
}
