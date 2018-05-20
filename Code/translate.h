#ifndef TRANSLATE_H
#define TRANSLATE_H

#include "ir.h"
#include "parse.h"
#include "semantic.h"

extern void preInterCodeGenerate();
extern void interCodeGenerate();
extern void printInterCode(InterCode);
extern InterCode intercodeConstruct(int kind,...);

extern void symbolHandle(char*, Operand);

extern int getTranslateExpType(Node);
extern void translate_ExtDefList(Node);
extern void translate_ExtDef(Node);
extern InterCode translate_Specifier(Node);
extern void translate_FunDec(Node);
extern void translate_CompSt(Node);
extern void translate_Args(Node, Operand*);
extern void translate_Exp(Node, Operand, int);
extern void translate_Stmt(Node);
extern void translate_StmtList(Node);
extern void translate_Cond(Node root, int label_true, int label_false);
int atoi(const char *str);

extern InterCode concatCode(InterCode code1, InterCode code2);
void insertInterCode(InterCode code);
extern Operand createTempOperand();
extern Array *getArray(Node);
extern int temp_no;
extern FILE *fp;

#endif
