#ifndef GENERATE_H
#define GENERATE_H

extern void CodeGenerate();
extern int getRegNo(Operand operand);
extern void printRegTable();

struct {
   Operand op;
} regTable[10000];

#endif
