#ifndef IR_H
#define IR_H

#define MAX_LINE 1024

typedef struct Operand_* Operand;

typedef struct Operand_ {
    enum { OP_VARIABLE, OP_TEMPORARY, OP_CONSTANT, OP_ADDRESS, OP_LABEL, OP_FUNCTION } kind;
    union {
        // int var_no;
        int no;
        // int tmp_no;
        int value;
        // int label_no;
        // int func_no;
    } u;
} Operand_;

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
    };
} InterCode;

InterCode u;

struct InterCode codes[MAX_LINE];

#endif
