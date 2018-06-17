#include "translate.h"
#include "ir.h"
#include "semantic.h"
#include "generate.h"
#include <stdarg.h>
#include <stdlib.h>

int reg_no = 0;
int depth = 12;
int tCounter = 0;
int vCounter = 0;
char func_name[128];
char name[128];
Operand regTable[1024];
int getRegNo(Operand operand);
int curFuncNo = -1;


void setupTable() {
  int reg1_no;
  int reg2_no;
  int i, j, key;
  struct list_head *plist;
  list_for_each(plist, &head) {
          InterCodes *node = list_entry(plist, InterCodes, list);
          InterCode intercode = node->intercode;
          switch (intercode.kind) {
              case IR_PARAM:
                  functionTable[key].para.no[vCounter] = intercode.op.x->u.no;
                  functionTable[key].para.type[vCounter] = intercode.op.x->kind;
                  ++vCounter;
                  functionTable[key].para.size = vCounter;
                  break;
              case IR_ASSIGN:
                  if (intercode.biop.y->kind == OP_CONSTANT) {
                      setTempVar(intercode.biop.x);
                      reg1_no = getRegNo(intercode.biop.x);
                      if (!reg1_no) {
                          regTable[tCounter++] = intercode.biop.y;
                      }
                  }
                  else {
                      reg1_no = getRegNo(intercode.biop.x);
                      setTempVar(intercode.biop.x);
                      reg2_no = getRegNo(intercode.biop.y);
                      setTempVar(intercode.biop.y);
                      printf("::%s %d %d\n", name, reg1_no, reg2_no);
                      if (!reg1_no) {

                          regTable[tCounter++] = intercode.biop.x;
                      }
                      if (!reg2_no) {
                          regTable[tCounter++] = intercode.biop.y;
                      }
                  }
                  break;
              case IR_ADD:
              case IR_SUB:
              case IR_MUL:
              case IR_DIV:
                  reg1_no = getRegNo(intercode.triop.x);
                  if (reg1_no == -1) {
                      regTable[tCounter++] = intercode.triop.x;
                  }
                  break;
              case IR_IFGOTO:
                  if (intercode.ifop.y->kind == OP_CONSTANT) {
                      regTable[tCounter++] = intercode.ifop.x;
                  }
                  break;
              case IR_READ:
                  regTable[tCounter++] = intercode.op.x;
                  break;
              case IR_FUNC:
                  ++curFuncNo;
                  getFuncName(name, intercode.op.x->u.no);
                  for (i = 0; i < 128; ++i) {
                      if (!strcmp(name, functionTable[i].name)) {
                          key = i;
                          break;
                      }
                  }
                  if (curFuncNo) {
                      // printVar(func_name, tCounter);
                      for (j = 0; j < functionTable[key].para.size; ++j) {
                          assert(0);
                          printf("v:%d\t", j);
                          if (functionTable[key].temp.type[j] == OP_TEMPORARY) {
                              printf("t");
                          }
                          else {
                              printf("v");
                          }
                          printf("%d\n", functionTable[key].para.no[j]);
                      }
                  }
                  getFuncName(func_name, intercode.op.x->u.no);
                  printf("%s: ", func_name);
                  vCounter = 0;
                  tCounter = 0;
                  // functionTable[key].para.size = 0;
                  break;
              default:
                  break;
          }
      }
     // printVar(func_name, tCounter);
}

void printPara(){
  printf("**----------------------------------\n");
  int i, j;
  for (i = 2; i < curFuncNo + 2; ++i) {
      for (j = 0; j < functionTable[i].para.size; ++j) {
          printf("%s PARAM%d\t", functionTable[i].name, j + 1);
          if (functionTable[i].para.type[j] == OP_TEMPORARY) {
              printf("t");
          }
          else {
              printf("v");
          }
          printf("%d\n", functionTable[i].para.no[j]);
      }
  }
}

void CodeGenerate() {
    setupTable();
    printPara();
    curFuncNo = -1;
    FILE *fp = fopen("out.s", "wt");
    if(fp == NULL)
        assert(0);
    char *preCodeGenerate = ".data\n_prompt: .asciiz \"Enter an integer:\"\n_ret: .asciiz \"\\n\"\n.globl main\n.text\nread:\n\tli $v0, 4\n\tla $a0, _prompt\n\tsyscall\n\tli $v0, 5\n\tsyscall\n\tjr $ra\n\nwrite:\n\tli $v0, 1\n\tsyscall\n\tli $v0, 4\n\tla $a0, _ret\n\tsyscall\n\tmove $v0, $0\n\tjr $ra\n";
    fprintf(fp, "%s", preCodeGenerate);
    int frameSize, paraSize;
    char func[128], op1[128], op3[128], code[128];
    int reg1_no, reg2_no, reg3_no;
    int funcSum = -1, i;
    struct list_head *plist;
    list_for_each(plist, &head) {
        InterCodes *node = list_entry(plist, InterCodes, list);
        InterCode intercode = node->intercode;
        printInterCode(intercode);
        switch (intercode.kind) {
            case IR_LABEL:
                symbolHandle(op1, intercode.op.x);
                sprintf(code, "%s:\n", op1);
                fputs(code, fp);
                break;
            case IR_FUNC:
                ++curFuncNo;
                getFuncName(func, intercode.op.x->u.no);
                for (; i < 128; ++i) {
                    if (!strcmp(func, functionTable[i].name)) {
                        printf("%d %s: ", i, func);
                        frameSize = functionTable[i].temp.size * 4;
                        paraSize = functionTable[i].para.size * 4;
                        printf("frameSize: %d ", frameSize);
                        printf("paraSize: %d\n", paraSize);
                        break;
                    }
                }
                assert(i != 128);
                sprintf(code, "\n%s:\n", func);
                fputs(code, fp);
                sprintf(code, "\tsubu $sp, $sp, %d\n", frameSize);
                fputs(code, fp);
                break;

            case IR_ASSIGN:
                if (intercode.biop.y->kind == OP_CONSTANT) {
                    regTable[reg_no++] = intercode.biop.x;
                    sprintf(code, "\tli $t1, %d\n", intercode.biop.y->u.no);
                    fputs(code, fp);
                    reg1_no = getRegNo(intercode.biop.x);
                    sprintf(code, "\tsw $t1, %d($fp)\n", frameSize - 4 * reg1_no);
                    fputs(code, fp);
                }
                else {
                    reg1_no = getRegNo(intercode.biop.x);
                    reg2_no = getRegNo(intercode.biop.y);
                    sprintf(code, "\tlw $t1, %d($fp)\n", frameSize - 4 * reg2_no);
                    fputs(code, fp);
                    strcpy(code, "\tmove $t0, $t1\n");
                    fputs(code, fp);
                    sprintf(code, "\tsw $t0, %d($fp)\n", frameSize - 4 * reg1_no);
                    fputs(code, fp);

                }
                break;

            case IR_ADD:
                reg1_no = getRegNo(intercode.triop.x);
                if (!reg1_no) {
                    regTable[reg_no++] = intercode.triop.x;
                    reg1_no = reg_no;
                }
                reg2_no = getRegNo(intercode.triop.y);
                // assert(reg2_no > 0);

                if (intercode.triop.z->kind == OP_CONSTANT) {
                    sprintf(code, "\taddi $t%d, $t%d, %d\n", reg1_no, reg2_no, intercode.triop.z->u.no);
                    fputs(code, fp);
                }
                else {
                    int reg3_no = getRegNo(intercode.triop.z);
                    sprintf(code, "\tlw $t1, %d($fp)\n", depth + 4 * reg2_no);
                    fputs(code, fp);
                    sprintf(code, "\tlw $t2, %d($fp)\n", depth + 4 * reg3_no);
                    fputs(code, fp);
                    sprintf(code, "\tadd $t3, $t1, $t2\n");
                    fputs(code, fp);
                    sprintf(code, "\tsw $t3, %d($fp)\n", depth + 4 * reg1_no);
                    fputs(code, fp);
                }
                break;

            case IR_SUB:
                reg1_no = getRegNo(intercode.triop.x);
                // if (!reg1_no) {
                //     regTable[reg_no++] = intercode.triop.x;
                //     reg1_no = reg_no;
                // }
                reg2_no = getRegNo(intercode.triop.y);
                // assert(reg2_no > 0);

                if (intercode.triop.z->kind == OP_CONSTANT) {
                    sprintf(code, "\taddi $t%d, $t%d, -%d\n", reg1_no, reg2_no, intercode.triop.z->u.no);
                    fputs(code, fp);
                }
                else {
                    reg3_no = getRegNo(intercode.triop.z);
                    sprintf(code, "\tsub $t%d, $t%d, $t%d\n", reg1_no, reg2_no, reg3_no);
                    fputs(code, fp);
                }
                break;

            case IR_MUL:
                reg1_no = getRegNo(intercode.triop.x);
                if (!reg1_no) {
                    regTable[reg_no++] = intercode.triop.x;
                    reg1_no = reg_no;
                }
                reg2_no = getRegNo(intercode.triop.y);
                reg3_no = getRegNo(intercode.triop.z);
                sprintf(code, "\tlw $t1, %d($fp)\n", depth + 4 * reg2_no);
                fputs(code, fp);
                sprintf(code, "\tlw $t2, %d($fp)\n", depth + 4 * reg3_no);
                fputs(code, fp);
                sprintf(code, "\tmul $t3, $t1, $t2\n");
                fputs(code, fp);
                sprintf(code, "\tsw $t3, %d($fp)\n", depth + 4 * reg1_no);
                fputs(code, fp);
                break;

            case IR_DIV:
                reg1_no = getRegNo(intercode.biop.x);
                if (!reg1_no) {
                    regTable[reg_no++] = intercode.biop.x;
                    reg1_no = reg_no;
                }
                reg2_no = getRegNo(intercode.triop.y);
                // assert(reg2_no > 0);
                reg3_no = getRegNo(intercode.triop.z);
                sprintf(code, "\tdiv $t%d, $t%d\n", reg2_no, reg3_no);
                fputs(code, fp);
                sprintf(code, "\tmflo $t%d\n", reg1_no);
                fputs(code, fp);
                break;
/*
            case IR_GETADD:
                symbolHandle(op1, intercode.biop.x);
                symbolHandle(op2, intercode.biop.y);
                sprintf(code, "%s := &%s\n", op1, op2); break;
*/
            case IR_GETVAL:
                reg1_no = getRegNo(intercode.biop.x);
                reg2_no = getRegNo(intercode.biop.y);
                assert(reg1_no > 0);
                assert(reg2_no > 0);
                sprintf(code, "\tlw $t%d, 0(%d)\n", reg1_no, reg2_no);
                fputs(code, fp);
                break;

            case IR_SETVAL:
                reg1_no = getRegNo(intercode.biop.x);
                reg2_no = getRegNo(intercode.biop.y);
                assert(reg1_no > 0);
                assert(reg2_no > 0);
                sprintf(code, "\tsw $t%d, 0(%d)\n", reg2_no, reg1_no);
                fputs(code, fp);
                break;

            case IR_GOTO:
                symbolHandle(op1, intercode.op.x);
                sprintf(code, "\tj %s\n", op1);
                fputs(code, fp);
                break;

            case IR_IFGOTO:
                symbolHandle(op3, intercode.ifop.z);
                reg1_no = getRegNo(intercode.ifop.x);
                reg2_no = getRegNo(intercode.ifop.y);
                if (intercode.ifop.y->kind == OP_CONSTANT) {
                    regTable[reg_no++] = intercode.ifop.x;
                    sprintf(code, "\tli $t%d, %d\n", reg_no, intercode.ifop.y->u.value);
                    fputs(code, fp);
                    reg2_no = reg_no;
                }
                sprintf(code, "\tlw $t1, %d($fp)\n", depth + 4 * reg1_no);
                fputs(code, fp);
                sprintf(code, "\tlw $t2, %d($fp)\n", depth + 4 * reg2_no);
                fputs(code, fp);

                switch (intercode.ifop.relop) {
                    case RELGT: fputs("\tbgt ", fp); break;
                    case RELLT: fputs("\tblt ", fp); break;
                    case EQUAL: fputs("\tbeq ", fp); break;
                    case NOTEQ: fputs("\tbne ", fp); break;
                    case RELGE: fputs("\tbge ", fp); break;
                    case RELLE: fputs("\tble ", fp); break;
                    default: assert(0);
                }
                sprintf(code, "$t1, $t2, %s\n", op3);
                fputs(code, fp);
                break;

            case IR_RET:
                reg1_no = getRegNo(intercode.op.x);
                sprintf(code, "\tsw $t1, %d($fp)\n", depth + 4 * reg1_no);
                fputs(code, fp);
                sprintf(code, "\tlw $v0, %d($fp)\n", depth + 4 * reg1_no);
                fputs(code, fp);
                sprintf(code, "\tmove $sp, $fp\n\tlw $ra, %d($sp)\n\tlw $fp, %d($sp)\n\taddiu $sp, $sp, %d\n\tjr $ra\n", frameSize - 4, frameSize - 8, frameSize);
                fputs(code, fp);
                break;
/*
            case IR_DEC:
                symbolHandle(op1, intercode.biop.x);
                symbolHandle(op2, intercode.biop.y);
                sprintf(code, "DEC %s %s\n", op1, op2); break;
*/
            case IR_ARG:
                reg1_no = getRegNo(intercode.op.x);
                sprintf(code, "\tlw $a0, %d($fp)\n", depth + 4 * reg1_no);
                fputs(code, fp);
                break;

            case IR_CALL:
                getFuncName(func, intercode.biop.y->u.no);
                // funcSum = getFuncArgSum(func);
                sprintf(code, "\tjal %s\n", func);
                fputs(code, fp);
                reg1_no = getRegNo(intercode.biop.x);
                sprintf(code, "\tsw $v0, %d($fp)\n", depth + 4 * reg1_no);
                fputs(code, fp);
                break;

            case IR_PARAM:
                reg1_no = getRegNo(intercode.op.x);
                // sprintf(code, "\tsw $a0, %d($fp)\n", depth + 4 * reg1_no);
                // fputs(code, fp);
                // sprintf(code, "\tlw $t1, %d($fp)\n", depth + 4 * reg1_no);
                // fputs(code, fp);
                break;

            case IR_READ:
                strcpy(code, "\tjal read\n");
                fputs(code, fp);
                regTable[reg_no++] = intercode.op.x;
                reg1_no = getRegNo(intercode.op.x);
                sprintf(code, "\tsw $v0, %d($fp)\n", depth + 4 * reg1_no);
                fputs(code, fp);
                break;

            case IR_WRITE:
                symbolHandle(op1, intercode.op.x);
                reg1_no = getRegNo(intercode.op.x);
                sprintf(code, "\tlw $a0, %d($fp)\n", depth + 4 * reg1_no);
                fputs(code, fp);
                strcat(code, "\tjal write\n");
                fputs(code, fp);
                break;

            default:
                assert(0);
        }

    }
    fclose(fp);
    // printRegTable();
}


void setTempVar(Operand operand) {
    int i, j, k;
    for (i = 0; i < 128; ++i) {
        if (!strcmp(name, functionTable[i].name)) {
            for (k = 0; k < tCounter; ++k) {
                if (functionTable[i].temp.no[k] == operand->u.no) {
                    if (functionTable[i].temp.type[k] == operand->kind) {
                        return;
                    }
                }
            }
            functionTable[i].temp.no[tCounter] = operand->u.no;
            functionTable[i].temp.type[tCounter] = operand->kind;
        }
     }
}

int getRegNo(Operand operand) {
    int ret = 0;
    int i = curFuncNo + 2, j = 0;
    for (; j < functionTable[i].temp.size; ++j) {
        if (operand->u.no == functionTable[i].temp.no[j] && operand->kind == functionTable[i].temp.type[j]) {
            ret = j + 1;
            return -ret;
       }
    }
    if (ret != -1)
        return ret;
    for (j = 0; j < functionTable[i].para.size; ++j) {
        if (operand->u.no == functionTable[i].para.no[j] && operand->kind == functionTable[i].para.type[j]) {
            ret = j + 1;
            return -ret;
        }
    }
    return 0;
}

void printVar(char *name, int no) {
    printf("%s: %d %d\n", name, tCounter, vCounter);
    int i = 0, j;
    for (; i < funcNo + 1; ++i) {
        if (!strcmp(name, functionTable[i].name)) {
            for (j = 0; j < no; ++j) {
                functionTable[i].temp.no[j] = regTable[j]->u.no;
                functionTable[i].temp.type[j] = regTable[j]->kind;
                printf("%d\t", j);
                if (functionTable[i].temp.type[j] == OP_TEMPORARY) {
                    printf("t");
                }
                else {
                    printf("v");
                }
                printf("%d\n", functionTable[i].temp.no[j]);
                // ++tCounter;
            }
            functionTable[i].temp.size = no;
            printf("------------------------------------\n");

            break;
        }
    }
}
void printRegTable() {
    int i = 0;
    for (; i < reg_no; ++i) {
        printf("$t%d\t", i + 1);
        if (regTable[i]->kind == OP_TEMPORARY) printf("t");
        else printf("v");
        printf("%d\n", regTable[i]->u.no);
    }
}
