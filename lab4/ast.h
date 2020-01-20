typedef struct _ast ast;
typedef struct _ast *past;
struct _ast{
    char ivalue[100];
    char nodeType[100];
    past left;
    past right;
};
past newAst();
void addExpr(past main,past right);
past newExpr(char* oper, char* type,past left);
void showAst(past node, int nest);