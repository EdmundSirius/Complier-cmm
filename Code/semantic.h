#ifndef SEMANTIC_H
#define SEMANTIC_H

#define SYMBOL_TABLE_SIZE 0x4000

#include "parse.h"
#include "generate.h"
#include "ir.h"
#include <assert.h>

typedef enum { false, true } bool;

typedef struct Type_* Type;
typedef struct FieldList_* FieldList;
typedef struct StructNode_* StructNode;

typedef struct Function
{
    enum { INT, FLOAT } returnvalue;
    int argsum;
    int argbasic[8];
    int framesize;
} Function;


typedef struct Type_
{
    enum { BASIC, ARRAY, STRUCTURE, FUNCTION } kind;
    union
    {
        int basic;
        struct { Type elem; int size; } array;
        FieldList structure;
        Function function;
    } u;
} Type_;

typedef struct Array
{
    int dim;
    int basic;
    int size[32];
} Array;

typedef struct FieldList_
{
    char name[32];
    Type type;
    FieldList tail;
} FieldList_;

typedef struct Symbol {
    int no[512];
    int type[512];
    int scale[512];
    int size;
} Symbol;

typedef struct SymbolTable {
  	char name[32];
    Type type;
    int lineno;
    bool occupied;
    bool isParam;
    Symbol para;
    Symbol temp;
    int argno;
} SymbolTable;

typedef struct FunctionTable {
    SymbolTable symboltable;
    Symbol para;
    Symbol temp;
    int argno;
} FunctionTable;

struct SymbolTable symboltable[0x4000];

struct FunctionTable functionTable[128];

struct SymbolTable varTable[128];

typedef struct StructNode_ {
    int no;
    char name[32];
    FieldList children;
    StructNode next;
} StructNode_;

StructNode structlist;

#define Childsum root->childsum

#define OneArgument(no1) root->child[no1]
#define TwoArguments(no1, no2) root->child[no1]->child[no2]
#define ThreeArguments(no1, no2, no3) root->child[no1]->child[no2]->child[no3]
#define FourArguments(no1, no2, no3, no4) root->child[no1]->child[no2]->child[no3]->child[no4]
#define FiveArguments(no1, no2, no3, no4, no5) root->child[no1]->child[no2]->child[no3]->child[no4]->child[no5]
#define SixArguments(no1, no2, no3, no4, no5, no6) root->child[no1]->child[no2]->child[no3]->child[no4]->child[no5]->child[no6]
#define GetChild(_1, _2, _3, _4, _5, _6, NAME, ...) NAME
#define Child(...) GetChild(__VA_ARGS__, SixArguments, FiveArguments, FourArguments, ThreeArguments, TwoArguments, OneArgument, ...)(__VA_ARGS__)

#define PrintSemErrorMsg(type, lineno, msg) \
char message[128];\
switch (type) { \
  case 1: sprintf(message, "Undefined variable \"%s\"", msg); break; \
  case 2: sprintf(message, "Undefined function \"%s\"", msg); break; \
  case 3: sprintf(message, "Redefined variable \"%s\"", msg); break; \
  case 4: sprintf(message, "Redefined function \"%s\"", msg); break; \
  case 5: strcpy(message, "Type mismatched for assignment"); break; \
  case 6: strcpy(message, "The left-hand side of an assignment must be a variable"); break; \
  case 7: strcpy(message, "Type mismatched for operands"); break; \
  case 8: strcpy(message, "Type mismatched for return"); break; \
  case 9: strcpy(message, msg); break; \
  case 10: sprintf(message, "\"%s\" is not an array", msg); break; \
  case 11: sprintf(message, "\"%s\" is not a function", msg); break; \
  case 12: sprintf(message, "\"%s\" is not an integer", msg); break; \
  case 13: strcpy(message, "Illegal use of \".\""); break; \
  case 14: sprintf(message, "Non-existent field \"%s\"", msg); break; \
  case 15: sprintf(message, "Redefined field or initialized struct field \"%s\"", msg); break; \
  case 16: sprintf(message, "Duplicated name \"%s\"", msg); break; \
  case 17: sprintf(message, "Undefined structure \"%s\"", msg); break; \
} \
printf("Error type \033[;31m%d\033[0m at Line %d: \033[;31m%s\033[0m.\n", type, lineno, message) \

void semanticAnalysis();

void handleCompSt(Node, int);
void handleExtDef(Node);
void handleFunDec(Node, Function*, char*);
void handleVarDec(Node, int);
int handleSpecifier(Node);
void handleExtDecList(Node, int);
void handleExtDefList(Node);
void handleDefList(Node);
void handleStmtList(Node, int);
void handleDef(Node);
void handleStmt(Node, int);
void handleDecList(Node, int);
int handleExp(Node);
void handleDec(Node, int);
void handleArgs(Node);
void handleVarList(Node, Function*);
void handleParamDec(Node, Function*);
void handleDecList(Node, int);
int handleStructSpecifier(Node);

int getStructNo(char*);
void getStructName(int, char*);
void getSpecifierName(int, char*, FieldList*);
int getHandleExpType(Node);

void compareType(Node, Node);
void compareOperandType(Node, Node);
void initSymbolTable();
void printSymbolTable();
void printStructList();
int getFuncReturnType(char*);
int getArgSum(Node);
int getVarSum(Node);
void getVarType(int[], int, Node);
void getFuncPrototype(char*, char*);
void getArgType(int*, Node);
void getArguments(char*, int*, int);
bool isLegalField(Node, Node);
// void setFuncFrameSize(char *, int);

int getNodeType(Node);
int getRetValue(char*);
void getFuncName(char*, int);
int getFuncNo(char*);
int getOpVarNo(char*);
int getArrayBasicType(char*);

unsigned int hashPJW(char* name);
void insertVarSymbolTable(char* text, Type type);
void insertFuncSymbolTable(char* name, Function *function);
bool findSymbolTable(char* text);
void createStructNode(char* text);
void insertStructList(FieldList);

bool isStructVar(char* name);
bool isInStructure(int, char*);
bool isArray(char*);
bool isEqualStruct(int, int);
bool isFunction(char*);


extern char functionName[128];
extern char name[128];
extern char specifierName[128];
extern char specifierStructName[128];
extern char specifierSubStructName[128];

#endif
