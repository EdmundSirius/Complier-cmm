#ifndef IR_H
#define IR_H

#include <string.h>
#include <stdio.h>
#include "list.h"
#include "semantic.h"

typedef struct Operand_* Operand;

typedef struct Operand_ {
    enum { OP_VARIABLE, OP_TEMPORARY, OP_CONSTANT, OP_ADDRESS, OP_LABEL, OP_FUNCTION, OP_VALUE, OP_SIZE } kind;
    union {
        int no;
        int value;
    } u;
} Operand_;

typedef enum { RELGT, RELLT, EQUAL, NOTEQ, RELGE, RELLE } Relop;

typedef struct InterCode {
    enum {
      IR_ADD, IR_SUB, IR_MUL, IR_DIV, IR_IFGOTO,
      IR_LABEL, IR_FUNC, IR_GOTO, IR_RET, IR_DEC, IR_ARG, IR_PARAM, IR_READ, IR_WRITE,
      IR_ASSIGN, IR_GETADD, IR_GETVAL, IR_SETVAL, IR_CALL
    } kind;

    union {
        struct { Operand x; } op;
        struct { Operand x, y; } biop;
        struct { Operand x, y, z; } triop;
        struct { Operand x, y, z; Relop relop; } ifop;
    };
} InterCode;

typedef struct InterCodes
{
    InterCode intercode;
    struct list_head list;
    int isLeader;
} InterCodes;

extern struct list_head head;


#define CREATE_TEMP_OP() \
createTempOperand()

#define CREATE_IR_8(arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8) \
insertInterCode(intercodeConstruct(arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8))

#define CREATE_IR_7(arg1, arg2, arg3, arg4, arg5, arg6, arg7) \
insertInterCode(intercodeConstruct(arg1, arg2, arg3, arg4, arg5, arg6, arg7))

#define CREATE_IR_6(arg1, arg2, arg3, arg4, arg5, arg6) \
insertInterCode(intercodeConstruct(arg1, arg2, arg3, arg4, arg5, arg6))

#define CREATE_IR_5(arg1, arg2, arg3, arg4, arg5) \
insertInterCode(intercodeConstruct(arg1, arg2, arg3, arg4, arg5))

#define CREATE_IR_4(arg1, arg2, arg3, arg4) \
insertInterCode(intercodeConstruct(arg1, arg2, arg3, arg4))

#define CREATE_IR_3(arg1, arg2, arg3) \
insertInterCode(intercodeConstruct(arg1, arg2, arg3))

#define CREATE_IR_2(arg1, arg2) \
insertInterCode(intercodeConstruct(arg1, arg2))

#define CREATE_IR_1(arg1) \
insertInterCode(intercodeConstruct(arg1))

#define CREATE_IR_N(_1, _2, _3, _4, _5, _6, _7, _8, NAME, ...) NAME
#define CREATE_IR(...) CREATE_IR_N(__VA_ARGS__, CREATE_IR_8, CREATE_IR_7, CREATE_IR_6, CREATE_IR_5, CREATE_IR_4, CREATE_IR_3, CREATE_IR_2, CREATE_IR_1, ...)(__VA_ARGS__)

#define PrintInterCodeErrorMsg() \
printf("\033[;31mCannot translate\033[0m: Code contains variables or parameters of structure type.\n"); \
exit(0)

#endif
