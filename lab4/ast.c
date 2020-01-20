#include<stdlib.h>
#include<stdio.h>
#include<string.h>
#include"ast.h"
past newAst(){
    past now=(past)malloc(sizeof(ast));
    now->left=NULL;now->right=NULL;
    return now;
}
past newExpr(char* oper, char* type,past left){
    past now=newAst();
    if(oper!=NULL){
        strcpy(now->ivalue,oper);
    }
    strcpy(now->nodeType,type);
    if(left==NULL){
        return now;
    }
    else{
        now->left=left;
    }
    return now;
}
void addExpr(past main,past right){
    if(main->right==NULL){
        main->right=right;
    }
    else{
        addExpr(main->right,right);
    }
}
void showAst(past node, int nest){
    if(node==NULL){
        return;
    }
    for(int i=1;i<=nest;i++){
        printf("----");
    }
    printf("%s",node->nodeType);
    if(node->ivalue!=NULL){
        printf("    %s\n",node->ivalue);
    }
    else
        printf("\n");
    showAst(node->left,nest+1);
    showAst(node->right,nest);
}
