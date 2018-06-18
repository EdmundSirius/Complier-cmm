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
extern int getFuncArgSum(char*);

// extern int vCounter;

#endif
