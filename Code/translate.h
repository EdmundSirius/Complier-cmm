#ifndef TRANSLATE_H
#define TRANSLATE_H

#include "parse.h"
#include "semantic.h"
#include "ir.h"

extern void preInterCodeGenerate();
extern void interCodeGenerate();
extern void printInterCode(InterCode);
extern InterCode intercodeConstruct(int kind,...);
extern void interCodeOptimize();

extern void symbolHandle(char*, Operand);

extern int getTranslateExpType(Node);
extern void translate_ExtDefList(Node);
extern void translate_ExtDef(Node);
extern void translate_FunDec(Node);
extern void translate_CompSt(Node);
extern void translate_Args(Node, Operand*);
extern void translate_Exp(Node, Operand, int);
extern void translate_Stmt(Node);
extern void translate_StmtList(Node);
extern void translate_Cond(Node root, int label_true, int label_false);
extern int atoi(const char *str);

extern InterCode concatCode(InterCode code1, InterCode code2);
extern void insertInterCode(InterCode code);
extern Operand createTempOperand();
extern Array *getArray(Node);
extern int temp_no;
extern FILE *fp;
extern char outputFile[128];

extern void writeFile(char *name);
extern void divideBasicBlock();
extern void eliminateConstant();
extern void calculateConstExp();

#endif
