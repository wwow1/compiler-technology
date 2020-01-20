#include<stdlib.h>
#include<stdio.h>
#include<string.h>
#include<stdbool.h>
#include "ast.h"
#define MAX 500
enum {T_INT = 1,T_ID=2};
#define true 1
#define false 0
typedef struct{
    int head;
    int tail;
    char keep[MAX][MAX];
}queue;
extern int lineno;
extern FILE* yyin;
extern int yyparse();
past astRoot;
queue q;
int variableNum=1;
bool returnMark=false;
int returnTarget=-1;
char* symbolTable[MAX];
char sprintfBuffer[MAX];
int genGetID(past node,bool put);
int genAddExpr(past node);
void genInit(past root);
void genAssign(past root);
void genDeclaration(past root);
void genFunction(past root);
void getOper(char* cmp,char* oper);
void genCmpExpr(past root);
void genIf(past root,bool elseMark);
bool genStatement(past root);
void genReturn(past root);
void genWhile(past root);
int getvariableNum(){
    return variableNum++;
}
bool queue_empty_or_full(){
    if(q.head==q.tail){
        return 1; //empty;
    }
    if(q.tail+1==q.head){
        return -1; //full
    }
    return 0;
}
void push_queue(char* codes){
    if(queue_empty_or_full()!=-1){
        strcpy(q.keep[q.tail],codes);
        q.tail=(q.tail+1)%MAX;
    }
    else{
        printf("instrution queue is full");
        exit(0);
    }
}
char* pop_queue(){
    if(queue_empty_or_full()!=1){
        int tmp=q.head;
        q.head=(q.head+1)%MAX;
        return q.keep[tmp];
    }
    else
        return NULL;
}
void addLLVMCodes(char* codes){
    push_queue(codes);
}
int genGetID(past root,bool put){
    for(int i=1;i<variableNum;i++){
        if(symbolTable[i]==NULL){
            continue;
        }
        if(!strcmp(root->ivalue,symbolTable[i])){
            if(put) {
                sprintf(sprintfBuffer, "%%%d = load i32, i32* %%%d, align 4", getvariableNum(), i);
                addLLVMCodes(sprintfBuffer);
            }
            return i;
        }
    }
    printf("error：%s is undefined in line %d",root->ivalue,lineno);
    exit(0);
}
int genPrimaryExpr(past root, char* operand) //如果其中包含ID？ 怎么弄？
{
    int type = -1;
    if(strcmp(root->nodeType, "number") == 0)
    {
        type = 1;
        if(operand != NULL)
            sprintf(operand, "%s", root->ivalue);
    }
    else if(!strcmp(root->nodeType,"id")){
        type=1;
        if(operand != NULL){
            genGetID(root,true);
            sprintf(operand,"%%%d",variableNum-1);
        }
    }
    else if(!strcmp(root->nodeType, "expr"))
    {
        //表达式的中间结果用临时变量保存
        //其结果为当前的 temVarNum
        type=genAddExpr(root);  //!!!!! changed
        if(operand != NULL)
            sprintf(operand, "%%%d", variableNum-1);
    }
    else
    {
        printf("ERROR: 发现不支持的运算类型, line %d\n", lineno);
    }
    return type;
}

int genAddExpr(past root)
{
    if(root == NULL)
        return -1;
    char tmp=root->ivalue[0];
    int ltype = -1;
    int rtype = -1;
    if( strcmp(root->nodeType, "expr") == 0)
    {
        char loperand[50];
        char roperand[50];
        char oper[10];
        //一元表达式左操作符为空
        // should be changed
        root=root->left;
        ltype = genPrimaryExpr(root, loperand);
        if(root->right)
            rtype = genPrimaryExpr(root->right, roperand);
        if(root->ivalue[0] == 'M')
        {
            //处理一元表达式
            rtype = ltype;
            sprintf(loperand, "0");
        }
        if( ltype == rtype && ltype == 1)
        {
            switch(tmp)
            {
                case '+': sprintf(oper, "add nsw"); break;
                case '-': sprintf(oper, "sub nsw"); break;
                case '*': sprintf(oper, "mul nsw"); break;
                case '/': sprintf(oper, "sdiv"); break;
                case '%': sprintf(oper, "srem"); break;
                case 'M': sprintf(oper, "sub nsw"); break;
            }

            sprintf(sprintfBuffer, "%%%d = %s i32 %s, %s",getvariableNum(), oper, loperand, roperand);
            addLLVMCodes(sprintfBuffer);

            return T_INT;
        }

    }
    return -1;
}
void genInit(past root){
    if(!root) return;
    char value[50];
    char buffer[MAX];
    int varNum=getvariableNum();
    sprintf(buffer, "%%%d = alloca i32, align 4", varNum);
    addLLVMCodes(buffer);
    past tmp=root->left;
    symbolTable[varNum]=tmp->ivalue;//存入符号表
    if(!strcmp(root->nodeType,"evaluation")) {
        tmp = tmp->right;
        genPrimaryExpr(tmp,value);
        sprintf(sprintfBuffer,"store i32 %s, i32* %%%d, align4",value,varNum);
        addLLVMCodes(sprintfBuffer);
        /*
        if(genAddExpr(tmp,varNum)) {
            sprintf(buffer, "store i32 %%%d, i32* %%%d, align 4", variableNum - 1, varNum);  //后面这个varNum-1不确定
            addLLVMCodes(buffer);
        }
         */
        genInit(root->right);
    }
    else{
        genInit(root->right);
    }
}
void genAssign(past root){ //这里root是 "=" "expr"
    char value[50];
    root=root->left;
    int varID=genGetID(root,false);
    root=root->right;
    genPrimaryExpr(root,value);
    sprintf(sprintfBuffer,"store i32 %s, i32* %%%d, align 4",value,varID);
    addLLVMCodes(sprintfBuffer);
    /*
    if(genAddExpr(root,varID)) {  //如果右侧表达式的值只是数字，就在genExpr内部把它的llvm指令加入队列
        sprintf(sprintfBuffer, "store i32 %%%d, i32* %%%d, align 4", variableNum - 1, varID);
        addLLVMCodes(sprintfBuffer);
    }
     */
}
void genDeclaration(past root){
    root=root->left;
    root=root->right;
    if(root)
        genInit(root);
}
void getOper(char* cmp,char* oper){
    if(!strcmp(cmp,">="))
        strcpy(oper,"sge");
    else if(!strcmp(cmp,"<="))
        strcpy(oper,"sle");
    else if(!strcmp(cmp,"=="))
        strcpy(oper,"eq");
    else if(!strcmp(cmp,">"))
        strcpy(oper,"sgt");
    else if(!strcmp(cmp,"<"))
        strcpy(oper,"slt");
}
void genCmpExpr(past root){
    char oper[10];
    char loperand[50];
    char roperand[50];
    getOper(root->ivalue,oper);
    root=root->left;
    genPrimaryExpr(root,loperand);
    root=root->right;
    genPrimaryExpr(root,roperand);
    sprintf(sprintfBuffer,"%%%d = icmp %s i32 %s, %s",getvariableNum(),oper,loperand,roperand);
    addLLVMCodes(sprintfBuffer);
}
void genIf(past root,bool elseMark){
    bool returnInConStrm=false; //控制流  如果出现return 最后的br就直接跳转到结束的label处
    int branch1;
    int branch2;
    int end;
    root=root->left;
    genCmpExpr(root);
    root=root->right;//转移到语句节点
    int judgeVar=variableNum-1;
    branch1=getvariableNum();branch2=getvariableNum();
    sprintf(sprintfBuffer,"br i1 %%%d, label %%%d, label %%%d",judgeVar,branch1,branch2);
    addLLVMCodes(sprintfBuffer);
    if(elseMark)
        end=getvariableNum();
    else
        end=branch2;
    addLLVMCodes("  ");
    sprintf(sprintfBuffer,"<label>:%d:",branch1);  //条件满足时
    addLLVMCodes(sprintfBuffer);
    returnInConStrm=genStatement(root);
    if(returnInConStrm){
        sprintf(sprintfBuffer,"br label %%%d",returnTarget);
        addLLVMCodes(sprintfBuffer);
        returnInConStrm=false;
    }
    else{
        sprintf(sprintfBuffer,"br label %%%d",end);
        addLLVMCodes(sprintfBuffer);
    }
    if(elseMark){
        root=root->right; //转移到else后的语句
        addLLVMCodes("  ");
        sprintf(sprintfBuffer,"<label>:%d:",branch2);
        addLLVMCodes(sprintfBuffer);
        returnInConStrm=genStatement(root);
        if(returnInConStrm){
            sprintf(sprintfBuffer,"br label %%%d",returnTarget);
            addLLVMCodes(sprintfBuffer);
            returnInConStrm=false;
        }
        else{
            sprintf(sprintfBuffer,"br label %%%d",end);
            addLLVMCodes(sprintfBuffer);
        }
    }
    addLLVMCodes("  ");
    sprintf(sprintfBuffer,"<label>:%d:",end);
    addLLVMCodes(sprintfBuffer);
}
void genReturn(past root){ //need to process
    char value[50];
    if(returnTarget==-1)
        returnTarget=getvariableNum();
    if(root->left){
        root=root->left;
        if(!strcmp(root->nodeType,"cmp_expr")){
        	genCmpExpr(root);
        	sprintf(sprintfBuffer,"%%%d = zext i1 %%%d to i32",getvariableNum(),variableNum-1);
        	addLLVMCodes(sprintfBuffer);
        	sprintf(sprintfBuffer,"store i32 %%%d, i32* %%1, align 4",variableNum-1);
        	addLLVMCodes(sprintfBuffer);
        }
        else{
        	genPrimaryExpr(root,value);
        	sprintf(sprintfBuffer,"store i32 %s, i32* %%1, align 4",value);
        	addLLVMCodes(sprintfBuffer);
        }
    }
    returnMark=true;
}
void genWhile(past root){
    bool returnInConStrm=false;
    int judge=getvariableNum();
    sprintf(sprintfBuffer,"br label %%%d\n",judge);
    addLLVMCodes(sprintfBuffer);
    sprintf(sprintfBuffer,"<label>:%d:",judge);
    addLLVMCodes(sprintfBuffer);
    root=root->left;
    genCmpExpr(root);
    int judgeVar=variableNum-1;
    int into=getvariableNum();int outo=getvariableNum();
    sprintf(sprintfBuffer,"br i1 %%%d, label %%%d, label %%%d",judgeVar,into,outo);
    addLLVMCodes(sprintfBuffer);
    root=root->right; //转移到语句节点
    addLLVMCodes("  ");
    sprintf(sprintfBuffer,"<label>:%d:",into);  //条件满足时
    addLLVMCodes(sprintfBuffer);
    returnInConStrm=genStatement(root);
    if(returnInConStrm){
        sprintf(sprintfBuffer,"br label %%%d",returnTarget);
        addLLVMCodes(sprintfBuffer);
        returnInConStrm=false;
    }
    else{
        sprintf(sprintfBuffer,"br label %%%d",judge);
        addLLVMCodes(sprintfBuffer);
    }
    addLLVMCodes("  ");
    sprintf(sprintfBuffer,"<label>:%d:",outo);
    addLLVMCodes(sprintfBuffer);
}
bool genStatement(past root){  //这里的返回值只是为了确定是否出现了return语句  不用全局变量的原因是会导致控制流出错
    bool returnT=false;
    if(!strcmp(root->nodeType,"compound_statement")){
        root=root->left->right;
        while(root){
            if(genStatement(root))
                returnT=true;
            root=root->right;
        }
    }
    else if(!strcmp(root->nodeType,"declaration")){
        genDeclaration(root);
    }
    else if(!strcmp(root->nodeType,"expression_statement")){
        if(root->left){
            genAssign(root->left);
        }
    }
    else if(!strcmp(root->nodeType,"if_statement")){
        genIf(root,false);
    }
    else if(!strcmp(root->nodeType,"if-else_statement")){
        genIf(root,true);
    }
    else if(!strcmp(root->nodeType,"while_statement")){
        genWhile(root);
    }
    else if(!strcmp(root->nodeType,"jump_statement")){
        genReturn(root);
        returnT=true;
    }
    return returnT;
}
void genFunction(past root){  //选择是否要做->决定不做
    char type[10];
    root=root->left;
    if(!strcmp(root->ivalue,"int") || !strcmp(root->ivalue,"str")){
        strcpy(type,"i32");
    }
    else if(!strcmp(root->ivalue,"void"))
        strcpy(type,"void");
    root=root->right;
    //int varNum=getvariableNum();
    //symbolTable[varNum]=root->ivalue;  //这里由于只做了少数语句的翻译，所以用不上函数调用，就不弄函数的符号表了
    sprintf(sprintfBuffer,"define %s @%s(){",type,root->ivalue);
    addLLVMCodes(sprintfBuffer);
    root=root->right->left;
    if(!strcmp(type,"i32")){
        sprintf(sprintfBuffer,"%%%d = alloca i32, align 4",getvariableNum());
        addLLVMCodes(sprintfBuffer);
    }
    while(root){
        genStatement(root);
        root=root->right;
    }
    if(returnMark){ //如果之前出现了return的控制流   就建立一个标签来结束，并且把主函数的控制流也引过来
        sprintf(sprintfBuffer,"br label %%%d\n",returnTarget);
        addLLVMCodes(sprintfBuffer);
        sprintf(sprintfBuffer,"<label>:%%%d:",returnTarget);
        addLLVMCodes(sprintfBuffer);
        returnMark=false;
    }
    if(!strcmp(type,"void")){
        addLLVMCodes("ret void");
    }
    else{
        sprintf(sprintfBuffer,"%%%d = load i32, i32* %%1, align4",getvariableNum());
        addLLVMCodes(sprintfBuffer);
        sprintf(sprintfBuffer,"ret i32 %%%d",variableNum-1);
        addLLVMCodes(sprintfBuffer);
    }
    addLLVMCodes("}");
}
void showllvm(){
    while(queue_empty_or_full()!=1){
        printf("%s\n",pop_queue());
    }
}
void travelTree(past root){
    while(root){
        if(!strcmp(root->nodeType,"function_definition")){
            genFunction(root);
        }
        else if(!strcmp(root->nodeType,"declaration")){
            genDeclaration(root);
        }
        root=root->right;
    }
}
void yyerror(char* s){
    printf("%s\n",s);
}
int main(int argc,char* argv[]){
    q.head=0;
    q.tail=0;
    if(argc>1){
        if(!(yyin=fopen(argv[1],"r"))){
            perror(argv[1]);
            return 1;
        }
    }
    astRoot=newExpr(NULL,"root",NULL);
    if(yyparse()) {
        printf("syntax error !!\n");
        return -1;
    }
    fclose(yyin);
    past root=astRoot->right;
    showAst(root,0);
    printf("llvm code:\n");
    travelTree(root);
    showllvm();
    return 0;
}
