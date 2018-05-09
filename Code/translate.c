#include "translate.h"
#include "ir.h"
#include "semantic.h"
#include <stdarg.h>

// Function writefunction;

void preInterCodeGenerate() {
    Function *readfunction = (Function*)malloc(sizeof(Function));
    readfunction->argsum = 0;
    readfunction->returnvalue = INT;
    insertFuncSymbolTable("read", readfunction);

    Function *writefunction = (Function*)malloc(sizeof(Function));
    writefunction->argsum = 1;
    writefunction->argbasic[0] = INT;
    writefunction->returnvalue = INT;
    insertFuncSymbolTable("write", writefunction);

    /* read(), return(int)
       write(int)，return(0) */
}

void interCodeGenerate() {

    InterCode ir1, ir2, ir3, ir4, ir5;

    ir1 = intercodeConstruct(IR_FUNC, 2);
    ir2 = intercodeConstruct(IR_READ, OP_TEMPORARY, 1);
    ir3 = intercodeConstruct(IR_ASSIGN, OP_VARIABLE, 1, OP_TEMPORARY, 1);
    ir4 = intercodeConstruct(IR_ASSIGN, OP_TEMPORARY, 2, OP_ADDRESS, 0);
    ir5 = intercodeConstruct(IR_IFGOTO, OP_VARIABLE, 1, OP_TEMPORARY, 2, OP_LABEL, 1);

    printInterCode(ir1);
    printInterCode(ir2);
    printInterCode(ir3);
    printInterCode(ir4);
    printInterCode(ir5);

}

InterCode intercodeConstruct(int kind,...) {
    InterCode ir;
    va_list va_ptr;
    ir.kind = kind;
    int argSum, arg1, arg2, arg3, arg4, arg5, arg6;
    switch (kind) {
      case IR_FUNC:
          argSum = 1;
          va_start(va_ptr, kind);
          arg1 = va_arg(va_ptr, int);
          va_end(va_ptr);
          break;
      case IR_READ:
          argSum = 2;
          va_start(va_ptr, kind);
          arg1 = va_arg(va_ptr, int);
          arg2 = va_arg(va_ptr, int);
          va_end(va_ptr);
          break;
      case IR_ASSIGN:
          argSum = 4;
          va_start(va_ptr, kind);
          arg1 = va_arg(va_ptr, int);
          arg2 = va_arg(va_ptr, int);
          arg3 = va_arg(va_ptr, int);
          arg4 = va_arg(va_ptr, int);
          va_end(va_ptr);
          break;
      case IR_IFGOTO:
          argSum = 6;
          va_start(va_ptr, kind);
          arg1 = va_arg(va_ptr, int);
          arg2 = va_arg(va_ptr, int);
          arg3 = va_arg(va_ptr, int);
          arg4 = va_arg(va_ptr, int);
          arg5 = va_arg(va_ptr, int);
          arg6 = va_arg(va_ptr, int);
          va_end(va_ptr);
          break;
    }
    Operand x = (Operand)malloc(sizeof(Operand_));
    Operand y = (Operand)malloc(sizeof(Operand_));
    Operand z = (Operand)malloc(sizeof(Operand_));
    switch (kind) {
        case IR_FUNC:
            x->kind = OP_FUNCTION;
            x->u.no = arg1;
            ir.op.x = x;
            break;
        case IR_READ:
        case IR_WRITE:
            x->kind = arg1;
            x->u.no = arg2;
            ir.op.x = x;
            break;
        case IR_ASSIGN:
            x->kind = arg1;
            x->u.no = arg2;
            ir.biop.x = x;
            y->kind = arg3;
            y->u.no = arg4;
            ir.biop.y = y;
            break;
        case IR_IFGOTO:
            x->kind = arg1;
            x->u.no = arg2;
            ir.triop.x = x;
            y->kind = arg3;
            y->u.no = arg4;
            ir.triop.y = y;
            z->kind = arg5;
            z->u.no = arg6;
            ir.triop.z = z;
    }
    return ir;
}

void printInterCode(InterCode intercode) {
    char func[128];
    char op1[128];
    char op2[128];
    char op3[128];
    char op4[128];
    char op5[128];
    char op6[128];

    switch (intercode.kind) {
        case IR_LABEL:
            symbolHandle(op1, intercode.op.x);
            printf("LABEL %s :\n", op1); break;

        case IR_FUNC:
            getFuncName(func, intercode.op.x->u.no);
            printf("FUNCTION %s:\n", func); break;

        case IR_ASSIGN:
            symbolHandle(op1, intercode.biop.x);
            symbolHandle(op2, intercode.biop.y);
            printf("%s := %s\n", op1, op2); break;

        case IR_ADD:
            symbolHandle(op1, intercode.triop.x);
            symbolHandle(op2, intercode.triop.y);
            symbolHandle(op3, intercode.triop.z);
            printf("%s := %s + %s\n", op1, op2, op3); break;

        case IR_SUB:
            symbolHandle(op1, intercode.triop.x);
            symbolHandle(op2, intercode.triop.y);
            symbolHandle(op3, intercode.triop.z);
            printf("%s := %s - %s\n", op1, op2, op3); break;

        case IR_MUL:
            symbolHandle(op1, intercode.triop.x);
            symbolHandle(op2, intercode.triop.y);
            symbolHandle(op3, intercode.triop.z);
            printf("%s := %s * %s\n", op1, op2, op3); break;

        case IR_DIV:
            symbolHandle(op1, intercode.triop.x);
            symbolHandle(op2, intercode.triop.y);
            symbolHandle(op3, intercode.triop.z);
            printf("%s := %s / %s\n", op1, op2, op3); break;

        case IR_GETADD:
            symbolHandle(op1, intercode.biop.x);
            symbolHandle(op2, intercode.biop.y);
            printf("%s := &%s\n", op1, op2); break;

        case IR_GETVAL:
            symbolHandle(op1, intercode.biop.x);
            symbolHandle(op2, intercode.biop.y);
            printf("%s := *%s\n", op1, op2); break;

        case IR_SETVAL:
            symbolHandle(op1, intercode.biop.x);
            symbolHandle(op2, intercode.biop.y);
            printf("*%s := %s\n", op1, op2); break;

        case IR_GOTO:
            symbolHandle(op1, intercode.op.x);
            printf("GOTO %s\n", op1); break;

        case IR_IFGOTO:
            symbolHandle(op1, intercode.triop.x);
            symbolHandle(op2, intercode.triop.y);
            symbolHandle(op3, intercode.triop.z);
            printf("IF %s [relop] %s GOTO %s\n", op1, op2, op3); break;

        case IR_RET:
            symbolHandle(op1, intercode.triop.x);
            printf("RETURN %s\n", op1); break;

        case IR_DEC:
            symbolHandle(op1, intercode.op.x);
            printf("DEC %s [size]", op1); break;

        case IR_ARG:
            symbolHandle(op1, intercode.op.x);
            printf("ARG %s\n", op1); break;

        case IR_CALL:
            symbolHandle(op1, intercode.biop.x);
            getFuncName(func, intercode.biop.y->u.no);
            printf("%s := CALL %s\n", op1, func); break;

        case IR_PARAM:
            symbolHandle(op1, intercode.op.x);
            printf("PARAM %s\n", op1); break;

        case IR_READ:
            symbolHandle(op1, intercode.op.x);
            printf("READ %s\n", op1); break;

        case IR_WRITE:
            symbolHandle(op1, intercode.op.x);
            printf("WRITE %s\n", op1); break;

        default: assert(0);
    }
}


void symbolHandle(char *name, Operand operand) {
    switch (operand->kind) {
        case OP_VARIABLE: sprintf(name, "v%d", operand->u.no); break;
        case OP_TEMPORARY: sprintf(name, "t%d", operand->u.no); break;
        case OP_ADDRESS: sprintf(name, "#%d", operand->u.no); break;
        case OP_LABEL: sprintf(name, "label%d", operand->u.no); break;
        default: assert(0);
    }
}
