#ifndef GENERATE_H
#define GENERATE_H

#include "ir.h"
#include "semantic.h"

extern void CodeGenerate();
extern void printRegTable();
extern void printVar();
extern int getRegSum();
extern int funcNo;
extern void setupTable();
extern void setTempVar(Operand);
// extern int vCounter;

#endif
