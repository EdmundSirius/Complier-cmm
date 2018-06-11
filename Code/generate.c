#include "translate.h"
#include "ir.h"
#include "semantic.h"
#include "generate.h"
#include <stdarg.h>
#include <stdlib.h>

int reg_no = 0;

void CodeGenerate() {
    FILE *fp = fopen("out.s", "wt");
    if(fp == NULL)
        assert(0);
    char *preCodeGenerate = ".data\n_prompt: .asciiz \"Enter an integer:\"\n_ret: .asciiz \"\\n\"\n.globl main\n.text\nread:\n\tli $v0, 4\n\tla $a0, _prompt\n\tsyscall\n\tli $v0, 5\n\tsyscall\n\tjr $ra\n\nwrite:\n\tli $v0, 1\n\tsyscall\n\tli $v0, 4\n\tla $a0, _ret\n\tsyscall\n\tmove $v0, $0\n\tjr $ra\n\n";
    fprintf(fp, "%s", preCodeGenerate);
    char func[128];
    char op1[128];
    char op2[128];
    char op3[128];
    char relop[128];
    char code[128];
    char reg[128];
    char reg1[128];
    char reg2[128];
    char reg3[128];
    int reg1_no;
    int reg2_no;
    struct list_head *plist;
    list_for_each(plist, &head) {
        InterCodes *node = list_entry(plist, InterCodes, list);
        InterCode intercode = node->intercode;
        switch (intercode.kind) {
            case IR_LABEL:
                symbolHandle(op1, intercode.op.x);
                sprintf(code, "%s :\n", op1);
                break;

            case IR_FUNC:
                getFuncName(func, intercode.op.x->u.no);
                if (intercode.op.x->u.no) {
                    sprintf(code, "%s:\n", func);
                }
                else {
                    sprintf(code, "\n%s:\n", func);
                }
                break;

            case IR_ASSIGN:
              symbolHandle(op1, intercode.biop.x);
              symbolHandle(op2, intercode.biop.y);
                if (intercode.biop.y->kind == OP_CONSTANT) {
                    regTable[reg_no++].op = intercode.biop.x;
                    sprintf(code, "\tli $t%d, %d\n", reg_no, intercode.biop.y->u.no);
                }
                else {
                    reg1_no = getRegNo(intercode.biop.x);
                    reg2_no = getRegNo(intercode.biop.y);
                    if (!reg1_no) {
                        regTable[reg_no++].op = intercode.biop.x;
                        reg1_no = reg_no;
                    }
                    if (!reg2_no) {
                        regTable[reg_no++].op = intercode.biop.y;
                        reg2_no = reg_no;
                    }
                    // sprintf(code, "\tla $t0, %s\n\tmove $t1, $t0\n\tsw $t1, %s\n", op2, op1);
                    sprintf(code, "\tmove $t%d, $t%d\n", reg1_no, reg2_no);
                }
                break;

            case IR_ADD:
                symbolHandle(op1, intercode.triop.x);
                symbolHandle(op2, intercode.triop.y);
                symbolHandle(op3, intercode.triop.z);

                reg1_no = getRegNo(intercode.biop.x);
                if (!reg1_no) {
                    regTable[reg_no++].op = intercode.biop.x;
                    reg1_no = reg_no;
                }
                reg2_no = getRegNo(intercode.triop.y);
                assert(reg2_no > 0);

                if (intercode.triop.z->kind == OP_CONSTANT) {

                    sprintf(code, "\taddi $t%d, $t%d, %d\n", reg1_no, reg2_no, intercode.triop.z->u.no);
                    // sprintf(code, "\tla $t0, %s\n", op2);
                    // strcat(code, "\taddi $t1, $t0, ");
                    // sprintf(reg3, "%d", intercode.triop.z->u.no);
                    // strcat(code, reg3);
                    // strcat(code, "\n\tsw $t1, ");
                    // strcat(code, op2);
                    // strcat(code, "\n");
                }
                else {
                    // sprintf(code, "\tla $t0, %s\n", op2);
                    // strcpy(reg3, op3);
                    // strcat(code, "\tla $t1, ");
                    // strcat(code, reg3);
                    // strcat(code, "\n\tadd $t2, $t0, $t1\n");
                    // strcat(code, "\tsw $t2, ");
                    // strcat(code, op3);
                    // strcat(code, "\n");
                    int reg3_no = getRegNo(intercode.triop.z);
                    sprintf(reg3, "%d", reg3_no);
                    sprintf(code, "\tadd $t%d, $t%d, $t%s\n", reg1_no, reg2_no, reg3);
                }
                break;

                /*

            case IR_SUB:
                symbolHandle(op1, intercode.triop.x);
                symbolHandle(op2, intercode.triop.y);
                symbolHandle(op3, intercode.triop.z);
                sprintf(code, "%s := %s - %s\n", op1, op2, op3);
                break;

            case IR_MUL:
                symbolHandle(op1, intercode.triop.x);
                symbolHandle(op2, intercode.triop.y);
                symbolHandle(op3, intercode.triop.z);
                sprintf(code, "%s := %s * %s\n", op1, op2, op3);
                break;

            case IR_DIV:
                symbolHandle(op1, intercode.triop.x);
                symbolHandle(op2, intercode.triop.y);
                symbolHandle(op3, intercode.triop.z);
                sprintf(code, "%s := %s / %s\n", op1, op2, op3);
                break;

            case IR_GETADD:
                symbolHandle(op1, intercode.biop.x);
                symbolHandle(op2, intercode.biop.y);
                sprintf(code, "%s := &%s\n", op1, op2); break;

            case IR_GETVAL:
                symbolHandle(op1, intercode.biop.x);
                symbolHandle(op2, intercode.biop.y);
                sprintf(code, "%s := *%s\n", op1, op2);
                break;

            case IR_SETVAL:
                symbolHandle(op1, intercode.biop.x);
                symbolHandle(op2, intercode.biop.y);
                sprintf(code, "*%s := %s\n", op1, op2); break;
*/
            case IR_GOTO:
                symbolHandle(op1, intercode.op.x);
                sprintf(code, "\tj %s\n", op1); break;

            case IR_IFGOTO:
                symbolHandle(op1, intercode.ifop.x);
                symbolHandle(op2, intercode.ifop.y);
                symbolHandle(op3, intercode.ifop.z);
                reg1_no = getRegNo(intercode.ifop.x);
                reg2_no = getRegNo(intercode.ifop.y);
                sprintf(reg1, "$t%d", reg1_no);
                if (intercode.ifop.y->kind == OP_CONSTANT) {
                    regTable[reg_no++].op = intercode.biop.x;
                    sprintf(code, "\tli $t%d, %d\n", reg_no, intercode.ifop.y->u.value);
                    reg2_no = reg_no;
                }
                sprintf(reg2, "$t%d", reg2_no);

                switch (intercode.ifop.relop) {
                    case RELGT: strcat(code, "\tbgt "); break;
                    case RELLT: strcat(code, "\tblt "); break;
                    case EQUAL: strcat(code, "\tbeq "); break;
                    case NOTEQ: strcat(code, "\tbne "); break;
                    case RELGE: strcat(code, "\tbge "); break;
                    case RELLE: strcat(code, "\tble "); break;
                    default: assert(0);
                }
                strcat(code, reg1);
                strcat(code, ", ");
                strcat(code, reg2);
                strcat(code, ", ");
                strcat(code, op3);
                strcat(code, "\n");
                break;

            case IR_RET:
                symbolHandle(op1, intercode.op.x);
                if (intercode.op.x->kind == OP_CONSTANT && intercode.op.x->u.no == 0) {
                    strcpy(code, "\tmove $v0, $0\n\tjr $ra\n");
                }
                else {
                    sprintf(code, "\tmove $v0, reg(%s)\n\tjr $ra\n", op1);
                }

                break;
/*
            case IR_DEC:
                symbolHandle(op1, intercode.biop.x);
                symbolHandle(op2, intercode.biop.y);
                sprintf(code, "DEC %s %s\n", op1, op2); break;
*/
            case IR_ARG:
                // symbolHandle(op1, intercode.op.x);
                strcpy(code, "\taddi $sp, $sp, -4\n");
                strcat(code, "\tsw $ra, 0($sp)\n");
                break;


            case IR_CALL:
                // symbolHandle(op1, intercode.biop.x);
                getFuncName(func, intercode.biop.y->u.no);
                sprintf(code, "\tjal %s\n", func);
                sprintf(reg1, "\tmove $t%d ", getRegNo(intercode.biop.x));
                strcat(code, reg1);
                strcat(code, ", $v0\n");
                break;


            case IR_PARAM:
                // symbolHandle(op1, intercode.op.x);
                // sprintf(code, "PARAM %s\n", op1);
                sprintf(code, "\tmove $a0, $t%d", getRegNo(intercode.biop.x));
                break;


            case IR_READ:
                // symbolHandle(op1, intercode.op.x);
                strcpy(code, "\taddi $sp, $sp, -4\n\tsw $ra, 0($sp)\n");
                strcat(code, "\tjal read\n\tlw $ra, 0($sp)\n\taddi $sp, $sp, 4\n");
                strcat(code, "\tmove $t");
                regTable[reg_no++].op = intercode.op.x;
                sprintf(reg, "%d", reg_no);
                strcat(code, reg);
                strcat(code, ", $v0\n");
                break;

            case IR_WRITE:
                symbolHandle(op1, intercode.op.x);
                reg1_no = getRegNo(intercode.op.x);
                sprintf(reg1, "%d", reg1_no);
                strcpy(code, "\tla $t0, ");
                strcat(code, op1);
                strcat(code, "\n\tmove $a0, $t0");
                strcat(code, "\n\taddi $sp, $sp, -4\n");
                strcat(code, "\tsw $ra, 0($sp)\n");
                strcat(code, "\tjal write\n");
                strcat(code, "\tlw $ra, 0($sp)\n");
                strcat(code, "\taddi $sp, $sp, 4\n");
                break;

            default:
              strcpy(code, "Hello\n");
              printf("interkind: %d\n", intercode.kind);
              // assert(0);
        }

        fprintf(fp, "%s", code);
    }
    fclose(fp);
    printRegTable();
}

int getRegNo(Operand operand) {
    int i = 0;
    for (; i < reg_no; ++i) {
        if (regTable[i].op->kind == operand->kind && regTable[i].op->u.no == operand->u.no) {
            return i + 1;
        }
    }
    // assert(0);
    return 0;
}

void printRegTable() {
    int i = 0;
    for (; i < reg_no; ++i) {
        printf("$t%d\t", i + 1);
        if (regTable[i].op->kind == OP_TEMPORARY) printf("t");
        else printf("v");
        printf("%d\n", regTable[i].op->u.no);
    }
}
