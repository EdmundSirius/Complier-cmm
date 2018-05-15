#include "ir.h"
#include "semantic.h"
#include "parse.h"
#include "translate.h"

void insertInterCode(InterCode head) {
    // InterCode newhead = (InterCode)malloc(sizeof(InterCode_));
    // newhead->code = head->code;
    // list_add_tail(newhead, &interCodes_head);
    #ifdef DEBUG_IR
    printInterCode(head);
    #endif

}

// InterCode concatCode(InterCode code1, InterCode code2) {
//
//     if (code1 == NULL) { assert(0); return code2; }
//     if (code2 == NULL) { assert(0); return code1; }
//
//     InterCode newcode1 = (InterCode)malloc(sizeof(InterCode_));
//     InterCode newcode2 = (InterCode)malloc(sizeof(InterCode_));
//
//     newcode1->code = code1->code;
//     newcode2->code = code2->code;
//     newcode1->next = newcode2;
//
//     // InterCode tail = code1;
//     // while (tail->next != NULL)
//     //     tail = tail->next;
//     // tail->next = code2;
//     // code2->prev = tail;
//     // code1->next = code2;
//     return newcode1;
// }

InterCode intercodeConstruct(int kind,...) {
    InterCode ir = (InterCode)malloc(sizeof(InterCode_));
    va_list va_ptr;
    ir->code.kind = kind;
    int arg1, arg2, arg3, arg4, arg5, arg6, arg7;
    va_start(va_ptr, kind);
    switch (kind) {
      case IR_FUNC:
          arg1 = va_arg(va_ptr, int);
          break;
      case IR_READ:
      case IR_WRITE:
      case IR_GOTO:
      case IR_LABEL:
      case IR_RET:
      case IR_PARAM:
      case IR_ARG:
          arg1 = va_arg(va_ptr, int);
          arg2 = va_arg(va_ptr, int);
          break;
      case IR_ASSIGN:
      case IR_CALL:
          arg1 = va_arg(va_ptr, int);
          arg2 = va_arg(va_ptr, int);
          arg3 = va_arg(va_ptr, int);
          arg4 = va_arg(va_ptr, int);
          break;
      case IR_IFGOTO:
          arg1 = va_arg(va_ptr, int);
          arg2 = va_arg(va_ptr, int);
          arg3 = va_arg(va_ptr, int);
          arg4 = va_arg(va_ptr, int);
          arg5 = va_arg(va_ptr, int);
          arg6 = va_arg(va_ptr, int);
          arg7 = va_arg(va_ptr, int);
          break;
      case IR_SUB:
      case IR_ADD:
      case IR_MUL:
      case IR_DIV:
          arg1 = va_arg(va_ptr, int);
          arg2 = va_arg(va_ptr, int);
          arg3 = va_arg(va_ptr, int);
          arg4 = va_arg(va_ptr, int);
          arg5 = va_arg(va_ptr, int);
          arg6 = va_arg(va_ptr, int);
          break;

      default:assert(0);

    }
    va_end(va_ptr);

    Operand x = (Operand)malloc(sizeof(Operand_));
    Operand y = (Operand)malloc(sizeof(Operand_));
    Operand z = (Operand)malloc(sizeof(Operand_));

    switch (kind) {
        case IR_FUNC:
            x->kind = OP_FUNCTION;
            x->u.no = arg1;
            ir->code.op.x = x;
            break;
        case IR_READ:
        case IR_WRITE:
        case IR_GOTO:
        case IR_LABEL:
        case IR_RET:
        case IR_ARG:
        case IR_PARAM:
            x->kind = arg1;
            x->u.no = arg2;
            ir->code.op.x = x;
            break;
        case IR_ASSIGN:
        case IR_CALL:
            x->kind = arg1;
            x->u.no = arg2;
            ir->code.biop.x = x;
            y->kind = arg3;
            y->u.no = arg4;
            ir->code.biop.y = y;
            break;
        case IR_IFGOTO:
            x->kind = arg1;
            x->u.no = arg2;
            ir->code.ifop.x = x;
            y->kind = arg3;
            y->u.no = arg4;
            ir->code.ifop.y = y;
            z->kind = arg5;
            z->u.no = arg6;
            ir->code.ifop.z = z;
            ir->code.ifop.relop = arg7;
            break;
        case IR_SUB:
        case IR_ADD:
        case IR_MUL:
        case IR_DIV:
            x->kind = arg1;
            x->u.no = arg2;
            ir->code.triop.x = x;
            y->kind = arg3;
            y->u.no = arg4;
            ir->code.triop.y = y;
            z->kind = arg5;
            z->u.no = arg6;
            ir->code.triop.z = z;
            break;
        default: assert(0);

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
    char relop[128];
    char code[128];
    FILE *fp;
    fp = fopen("out.ir", "a");
    if(fp == NULL)
        assert(0);

    switch (intercode->code.kind) {
        case IR_LABEL:
            symbolHandle(op1, intercode->code.op.x);
            sprintf(code, "LABEL %s :\n", op1);
            break;

        case IR_FUNC:
            getFuncName(func, intercode->code.op.x->u.no);
            if (intercode->code.op.x->u.no) {
                sprintf(code, "FUNCTION %s :\n", func);
            }
            else {
                sprintf(code, "\nFUNCTION %s :\n", func);
            }
            break;

        case IR_ASSIGN:
            symbolHandle(op1, intercode->code.biop.x);
            symbolHandle(op2, intercode->code.biop.y);
            sprintf(code, "%s := %s\n", op1, op2);
            break;

        case IR_ADD:
            symbolHandle(op1, intercode->code.triop.x);
            symbolHandle(op2, intercode->code.triop.y);
            symbolHandle(op3, intercode->code.triop.z);
            sprintf(code, "%s := %s + %s\n", op1, op2, op3);
            break;

        case IR_SUB:
            symbolHandle(op1, intercode->code.triop.x);
            symbolHandle(op2, intercode->code.triop.y);
            symbolHandle(op3, intercode->code.triop.z);
            sprintf(code, "%s := %s - %s\n", op1, op2, op3);
            break;

        case IR_MUL:
            symbolHandle(op1, intercode->code.triop.x);
            symbolHandle(op2, intercode->code.triop.y);
            symbolHandle(op3, intercode->code.triop.z);
            sprintf(code, "%s := %s * %s\n", op1, op2, op3);
            break;

        case IR_DIV:
            symbolHandle(op1, intercode->code.triop.x);
            symbolHandle(op2, intercode->code.triop.y);
            symbolHandle(op3, intercode->code.triop.z);
            sprintf(code, "%s := %s / %s\n", op1, op2, op3);
            break;

        case IR_GETADD:
            symbolHandle(op1, intercode->code.biop.x);
            symbolHandle(op2, intercode->code.biop.y);
            sprintf(code, "%s := &%s\n", op1, op2); break;

        case IR_GETVAL:
            symbolHandle(op1, intercode->code.biop.x);
            symbolHandle(op2, intercode->code.biop.y);
            sprintf(code, "%s := *%s\n", op1, op2);
            break;

        case IR_SETVAL:
            symbolHandle(op1, intercode->code.biop.x);
            symbolHandle(op2, intercode->code.biop.y);
            sprintf(code, "*%s := %s\n", op1, op2); break;

        case IR_GOTO:
            symbolHandle(op1, intercode->code.op.x);
            sprintf(code, "GOTO %s\n", op1); break;

        case IR_IFGOTO:
            symbolHandle(op1, intercode->code.ifop.x);
            symbolHandle(op2, intercode->code.ifop.y);
            symbolHandle(op3, intercode->code.ifop.z);
            switch (intercode->code.ifop.relop) {
                case RELGT: strcpy(relop, ">"); break;
                case RELLT: strcpy(relop, "<"); break;
                case EQUAL: strcpy(relop, "=="); break;
                case RELGE: strcpy(relop, ">="); break;
                case RELLE: strcpy(relop, "<="); break;
                default: assert(0);
            }
            sprintf(code, "IF %s %s %s GOTO %s\n", op1, relop, op2, op3); break;

        case IR_RET:
            symbolHandle(op1, intercode->code.op.x);
            sprintf(code, "RETURN %s\n", op1); break;

        case IR_DEC:
            symbolHandle(op1, intercode->code.op.x);
            sprintf(code, "DEC %s [size]", op1); break;

        case IR_ARG:
            symbolHandle(op1, intercode->code.op.x);
            sprintf(code, "ARG %s\n", op1); break;

        case IR_CALL:
            symbolHandle(op1, intercode->code.biop.x);
            getFuncName(func, intercode->code.biop.y->u.no);
            sprintf(code, "%s := CALL %s\n", op1, func); break;

        case IR_PARAM:
            symbolHandle(op1, intercode->code.op.x);
            sprintf(code, "PARAM %s\n", op1); break;

        case IR_READ:
            symbolHandle(op1, intercode->code.op.x);
            sprintf(code, "READ %s\n", op1); break;

        case IR_WRITE:
            symbolHandle(op1, intercode->code.op.x);
            sprintf(code, "WRITE %s\n", op1); break;

        default: printf("intercode.kind: %d\n", intercode->code.kind); assert(0);
    }

    printf("%s", code);
    fprintf(fp, "%s", code);
}

void symbolHandle(char *name, Operand operand) {
    switch (operand->kind) {
        case OP_VARIABLE: sprintf(name, "v%d", operand->u.no); break;
        case OP_TEMPORARY: sprintf(name, "t%d", operand->u.no); break;
        // case OP_ADDRESS: sprintf(name, "#%d", operand->u.no); break;
        case OP_LABEL: sprintf(name, "label%d", operand->u.no); break;
        case OP_CONSTANT: sprintf(name, "#%d", operand->u.no); break;
        case OP_FUNCTION: break;
        default: printf("op: %d", operand->kind); assert(0); break;
    }
}

int atoi(const char *str) {
    int value = 0;
    while (*str != '\0') {
        value = value * 10 + *str - '0'; //将数字字符转为对应的整形数
        str++;
    }
    return value;
}
