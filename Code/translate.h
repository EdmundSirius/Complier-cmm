#ifndef TRANSLATE_H
#define TRANSLATE_H

#include "ir.h"

extern void preInterCodeGenerate();
extern void interCodeGenerate();
extern void printInterCode(InterCode);

extern struct InterCode intercodeConstruct(int kind,...);
extern void symbolHandle(char*, Operand);

#endif
