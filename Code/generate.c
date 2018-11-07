#include "translate.h"
#include "ir.h"
#include "semantic.h"
#include "generate.h"
#include <stdarg.h>
#include <stdlib.h>

int reg_no = 0;
int tCounter = 0;
int pCounter = 0;
char func_name[128];
int getRegNo(Operand operand);
int curFuncNo = -1;
int argNo = 0;
extern void setTempVar(Operand);
extern void setArrayVar(Operand, int);

void setupTable() {
  int reg1_no;
  int reg2_no;
  int i, j, key;
  struct list_head *plist;
  list_for_each(plist, &head) {
          InterCodes *node = list_entry(plist, InterCodes, list);
          InterCode intercode = node->intercode;
          switch (intercode.kind) {
              case IR_DEC:
                  setArrayVar(intercode.biop.x, intercode.biop.y->u.no);
                  break;
              case IR_CALL:
                  setTempVar(intercode.op.x);
                  break;
              case IR_ARG:
                  functionTable[key].argno = ++argNo;
                  break;
              case IR_PARAM:
                  functionTable[key].para.no[pCounter] = intercode.op.x->u.no;
                  functionTable[key].para.type[pCounter] = intercode.op.x->kind;
                  ++pCounter;
                  functionTable[key].para.size = pCounter;
                  break;
              case IR_ASSIGN:
                  if (intercode.biop.y->kind == OP_CONSTANT) {
                      setTempVar(intercode.biop.x);
                  }
                  else {
                      setTempVar(intercode.biop.x);
                      setTempVar(intercode.biop.y);
                  }
                  break;
              case IR_ADD:
              case IR_SUB:
              case IR_MUL:
              case IR_DIV:
                  setTempVar(intercode.triop.x);
                  break;
              case IR_IFGOTO:
                  if (intercode.ifop.y->kind == OP_CONSTANT) {
                      setTempVar(intercode.ifop.x);
                  }
                  break;
              case IR_READ:
                  setTempVar(intercode.op.x);
                  break;
              case IR_FUNC:
                  ++curFuncNo;
                  getFuncName(func_name, intercode.op.x->u.no);
              #ifdef PRINT_GENERATE
                  printf("NAME: (%s)", func_name);
              #endif
                  for (i = 0; i < 128; ++i) {
                      if (!strcmp(func_name, functionTable[i].symboltable.name)) {
                          key = i;
                      #ifdef PRINT_GENERATE
                          printf("(%d)\n", key);
                      #endif
                          break;
                      }
                  }
                  assert(i != 128);
                  pCounter = 0;
                  tCounter = 0;
                  argNo = 0;
                  break;
              default:
                  break;
          }
      }
}

void printPara(){
  printf("----------------------------------\n");
  int i, j;
  for (i = 2; i < curFuncNo + 2 + 1; ++i) {
      printf("%s TempNo: %d ArgNo: %d\n", functionTable[i].symboltable.name, functionTable[i].symboltable.temp.size, functionTable[i].symboltable.argno);
      for (j = 0; j < functionTable[i].para.size; ++j) {
          printf("PARAM%d\t", j + 1);
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

void printTemp(){
  printf("----------------------------------\n");
  int i, j;
  for (i = 2; i < curFuncNo + 2 + 1; ++i) {
      printf("%s TempNo: %d ArgNo: %d\n", functionTable[i].symboltable.name, functionTable[i].symboltable.temp.size, functionTable[i].symboltable.argno);
      for (j = 0; j < functionTable[i].symboltable.temp.size; ++j) {
          printf("TEMP%d\t", j + 1);
          if (functionTable[i].symboltable.temp.type[j] == OP_TEMPORARY) {
              printf("t");
          }
          else {
              printf("v");
          }
          printf("%d ", functionTable[i].symboltable.temp.no[j]);
          printf("%d\n", functionTable[i].symboltable.temp.scale[j]);
      }
  }
}

void codeGenerate() {
    setupTable();
  #ifdef PRINT_GENERATE
    printPara();
    printTemp();
  #endif
    curFuncNo = -1;
    FILE *fp = fopen(outputFile, "wt");
    if(fp == NULL)
        assert(0);
    char *preCodeGenerate = ".data\n_prompt: .asciiz \"Enter an integer:\"\n_ret: .asciiz \"\\n\"\n.globl main\n.text\nread:\n\tli $v0, 4\n\tla $a0, _prompt\n\tsyscall\n\tli $v0, 5\n\tsyscall\n\tjr $ra\n\nwrite:\n\tli $v0, 1\n\tsyscall\n\tli $v0, 4\n\tla $a0, _ret\n\tsyscall\n\tmove $v0, $0\n\tjr $ra\n";
    fprintf(fp, "%s", preCodeGenerate);
    int frameSize = 0, paraSize, frameCur;
    char func[128], op1[128], op3[128], code[128];
    int reg1_no, reg2_no, reg3_no;
    int funcSum = -1, i, j, flag = 0;
    struct list_head *plist;
    list_for_each(plist, &head) {
        InterCodes *node = list_entry(plist, InterCodes, list);
        InterCode intercode = node->intercode;
        fputs("# ", fp);
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
                for (i = 0; i < 128; ++i) {
                    if (!strcmp(func, functionTable[i].symboltable.name)) {
                        for (j = 0; j < functionTable[i].temp.size; ++j) {
                              frameSize += functionTable[i].temp.scale[j];
                        }
                        frameSize *= 4;
                        frameCur = frameSize;
                        paraSize = functionTable[i].para.size * 4;
                        #ifdef PRINT_GENERATE
                        printf("frameSize: %d ", frameSize);
                        printf("paraSize: %d\n", paraSize);
                        #endif
                        break;
                    }
                }
                assert(i != 128);
                sprintf(code, "\n%s:\n", func);
                fputs(code, fp);
                sprintf(code, "\taddi $sp, $sp, -%d\n", frameSize);
                fputs(code, fp);
                break;

            case IR_ASSIGN:
                if (intercode.biop.y->kind == OP_CONSTANT) {
                    sprintf(code, "\tli $t1, %d\n", intercode.biop.y->u.no);
                    if (intercode.biop.x->kind != OP_VALUE) {
                        fputs(code, fp);
                        strcpy(code, "\tmove $t0, $t1\n");
                    }
                    else {
                        flag = 1;
                    }
                }
                else if (intercode.biop.y->kind == OP_ADDRESS) {
                    reg2_no = getRegNo(intercode.biop.y);
                    sprintf(code, "\tla $t0, %d($sp)\n", frameCur - 4 * reg2_no);
                }
                else if (intercode.biop.y->kind == OP_VALUE) {
                    reg2_no = getRegNo(intercode.biop.y);
                    sprintf(code, "\tlw $t1, %d($sp)\n", frameCur - 4 * reg2_no);
                    fputs(code, fp);
                    strcpy(code, "\tlw $t0, 0($t1)\n");
                }
                else {
                    reg2_no = getRegNo(intercode.biop.y);
                    sprintf(code, "\tlw $t1, %d($sp)\n", frameCur - 4 * reg2_no);
                    if (intercode.biop.x->kind != OP_VALUE) {
                        fputs(code, fp);
                        strcpy(code, "\tmove $t0, $t1\n");
                    }
                    else {
                        flag = 1;
                    }
                }
                fputs(code, fp);

                if (flag == 1) {
                    strcpy(code, "\tsw $t1, 0($t0)\n");
                    flag = 0;
                }
                else {
                    reg1_no = getRegNo(intercode.biop.x);
                    sprintf(code, "\tsw $t0, %d($sp)\n", frameCur - 4 * reg1_no);
                }
                fputs(code, fp);
                break;

            case IR_ADD:
                reg1_no = getRegNo(intercode.triop.x);
                reg2_no = getRegNo(intercode.triop.y);
                reg3_no = getRegNo(intercode.triop.z);
                if (intercode.triop.y->kind == OP_CONSTANT) {
                    sprintf(code, "\tli $t1, %d\n", intercode.triop.y->u.no);
                }
                else if (intercode.triop.y->kind == OP_ADDRESS) {
                    sprintf(code, "\tla $t1, %d($sp)\n", frameCur - 4 * reg2_no);
                }
                else {
                    sprintf(code, "\tlw $t1, %d($sp)\n", frameCur - 4 * reg2_no);
                }
                fputs(code, fp);

                if (intercode.triop.z->kind == OP_CONSTANT) {
                    sprintf(code, "\tli $t2, %d\n", intercode.triop.z->u.no);
                }
                else if (intercode.triop.z->kind == OP_ADDRESS) {
                    sprintf(code, "\tla $t2, %d($sp)\n", frameCur - 4 * reg3_no);
                }
                else {
                    sprintf(code, "\tlw $t2, %d($sp)\n", frameCur - 4 * reg3_no);
                }
                fputs(code, fp);
                strcpy(code, "\tadd $t0, $t1, $t2\n");
                fputs(code, fp);
                sprintf(code, "\tsw $t0, %d($sp)\n", frameCur - 4 * reg1_no);
                fputs(code, fp);
                break;

            case IR_SUB:
                reg1_no = getRegNo(intercode.triop.x);
                reg2_no = getRegNo(intercode.triop.y);
                reg3_no = getRegNo(intercode.triop.z);
                if (intercode.triop.y->kind == OP_CONSTANT) {
                    sprintf(code, "\tli $t1, %d\n", intercode.triop.y->u.no);
                }
                else {
                    sprintf(code, "\tlw $t1, %d($sp)\n", frameCur - 4 * reg2_no);
                }
                fputs(code, fp);
                if (intercode.triop.z->kind == OP_CONSTANT) {
                    sprintf(code, "\tli $t2, %d\n", intercode.triop.z->u.no);
                    fputs(code, fp);
                    sprintf(code, "\taddi $t0, $t1, -%d\n", intercode.triop.z->u.no);
                }
                else {
                    sprintf(code, "\tlw $t2, %d($sp)\n", frameCur - 4 * reg3_no);
                    fputs(code, fp);
                    strcpy(code, "\tsub $t0, $t1, $t2\n");
                }
                fputs(code, fp);
                sprintf(code, "\tsw $t0, %d($sp)\n", frameCur - 4 * reg1_no);
                fputs(code, fp);
                break;

            case IR_MUL:
                reg1_no = getRegNo(intercode.triop.x);
                reg2_no = getRegNo(intercode.triop.y);
                reg3_no = getRegNo(intercode.triop.z);
                if (intercode.triop.y->kind == OP_CONSTANT) {
                    sprintf(code, "\tli $t1, %d\n", intercode.triop.y->u.no);
                }
                else {
                    sprintf(code, "\tlw $t1, %d($sp)\n", frameCur - 4 * reg2_no);
                }
                fputs(code, fp);
                if (intercode.triop.z->kind == OP_CONSTANT) {
                    sprintf(code, "\tli $t2, %d\n", intercode.triop.z->u.no);
                }
                else {
                    sprintf(code, "\tlw $t2, %d($sp)\n", frameCur - 4 * reg3_no);
                }
                fputs(code, fp);
                strcpy(code, "\tmul $t0, $t1, $t2\n");
                fputs(code, fp);
                sprintf(code, "\tsw $t0, %d($sp)\n", frameCur - 4 * reg1_no);
                fputs(code, fp);
                break;

            case IR_DIV:
                reg1_no = getRegNo(intercode.triop.x);
                reg2_no = getRegNo(intercode.triop.y);
                reg3_no = getRegNo(intercode.triop.z);
                if (intercode.triop.y->kind == OP_CONSTANT) {
                    sprintf(code, "\tli $t1, %d\n", intercode.triop.y->u.no);
                }
                else {
                    sprintf(code, "\tlw $t1, %d($sp)\n", frameCur - 4 * reg2_no);
                }
                fputs(code, fp);
                if (intercode.triop.z->kind == OP_CONSTANT) {
                    sprintf(code, "\tli $t2, %d\n", intercode.triop.z->u.no);
                }
                else {
                    sprintf(code, "\tlw $t2, %d($sp)\n", frameCur - 4 * reg3_no);
                }
                fputs(code, fp);
                strcpy(code, "\tdiv $t1, $t2\n");
                fputs(code, fp);
                strcpy(code, "\tmflo $t0\n");
                fputs(code, fp);
                sprintf(code, "\tsw $t0, %d($sp)\n", frameCur - 4 * reg1_no);
                fputs(code, fp);
                break;

            case IR_GETADD:
                reg2_no = getRegNo(intercode.biop.y);
                sprintf(code, "\tla $t0, %d($sp)\n", frameCur - 4 * reg2_no);
                fputs(code, fp);
                reg1_no = getRegNo(intercode.biop.x);
                sprintf(code, "\tsw $t0, %d($sp)\n", frameCur - 4 * reg1_no);
                fputs(code, fp);
                break;

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
                sprintf(code, "\tlw $t1, %d($sp)\n", frameCur - 4 * reg1_no);
                fputs(code, fp);
                if (intercode.ifop.y->kind == OP_CONSTANT) {
                    sprintf(code, "\tli $t2, %d\n", intercode.ifop.y->u.value);
                }
                else {
                    sprintf(code, "\tlw $t2, %d($sp)\n", frameCur - 4 * reg2_no);
                }
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
                if (intercode.op.x->kind == OP_TEMPORARY) {
                    reg1_no = getRegNo(intercode.op.x);
                    sprintf(code, "\tlw $t0, %d($sp)\n", frameCur - 4 * reg1_no);
                }
                else {
                    assert(intercode.op.x->kind == OP_CONSTANT);
                    sprintf(code, "\tli $t0, %d\n", intercode.op.x->u.no);
                }
                fputs(code, fp);
                strcpy(code, "\tmove $v0, $t0\n");
                fputs(code, fp);
                sprintf(code, "\taddi $sp, $sp, %d\n", frameSize);
                fputs(code, fp);
                strcpy(code, "\tjr $ra\n");
                fputs(code, fp);
                break;

            case IR_DEC:
            case IR_PARAM:
                break;

            case IR_ARG:
                if (intercode.op.x->kind == OP_CONSTANT) {
                    sprintf(code, "\tli $t0, %d\n", intercode.op.x->u.no);
                }
                else {
                    reg1_no = getRegNo(intercode.op.x);
                    sprintf(code, "\tlw $t0, %d($sp)\n", frameCur - 4 * reg1_no);
                }
                fputs(code, fp);
                sprintf(code, "\taddi $sp, $sp, -4\n");
                frameCur += 4;
                fputs(code, fp);
                strcpy(code, "\tsw $t0, 0($sp)\n");
                fputs(code, fp);
                break;

            case IR_CALL:
                getFuncName(func, intercode.biop.y->u.no);
                paraSize =  getFuncArgSum(func);
                reg1_no = getRegNo(intercode.biop.x);
                strcpy(code, "\taddi $sp, $sp, -4\n");
                frameCur += 4;
                fputs(code, fp);
                strcpy(code, "\tsw $ra, 0($sp)\n");
                fputs(code, fp);
                sprintf(code, "\tjal %s\n", func);
                fputs(code, fp);
                strcpy(code, "\tlw $ra, 0($sp)\n");
                fputs(code, fp);
                sprintf(code, "\taddi $sp, $sp, %d\n", 4 * paraSize + 4);
                frameCur -= (4 * paraSize + 4);
                fputs(code, fp);
                strcpy(code, "\tmove $t0, $v0\n");
                fputs(code, fp);
                sprintf(code, "\tsw $t0, %d($sp)\n", frameCur - 4 * reg1_no);
                fputs(code, fp);
                break;

            case IR_READ:
                strcpy(code, "\taddi $sp, $sp, -4\n");
                fputs(code, fp);
                strcpy(code, "\tsw $ra, 0($sp)\n");
                fputs(code, fp);
                strcpy(code, "\tjal read\n");
                fputs(code, fp);
                strcpy(code, "\tlw $ra, 0($sp)\n");
                fputs(code, fp);
                strcpy(code, "\taddi $sp, $sp, 4\n");
                fputs(code, fp);
                strcpy(code, "\tmove $t0, $v0\n");
                fputs(code, fp);
                reg1_no = getRegNo(intercode.op.x);
                sprintf(code, "\tsw $t0, %d($sp)\n", frameCur - 4 * reg1_no);
                fputs(code, fp);
                break;

            case IR_WRITE:
                if (intercode.op.x->kind == OP_CONSTANT) {
                    sprintf(code, "\tli $t0, %d\n", intercode.op.x->u.no);
                }
                else {
                    reg1_no = getRegNo(intercode.op.x);
                    sprintf(code, "\tlw $t0, %d($sp)\n", frameCur - 4 * reg1_no);
                }
                fputs(code, fp);
                strcpy(code, "\tmove $a0, $t0\n");
                fputs(code, fp);
                strcpy(code, "\taddi $sp, $sp, -4\n");
                fputs(code, fp);
                strcpy(code, "\tsw $ra, 0($sp)\n");
                fputs(code, fp);
                strcpy(code, "\tjal write\n");
                fputs(code, fp);
                strcpy(code, "\tlw $ra, 0($sp)\n");
                fputs(code, fp);
                strcpy(code, "\taddi $sp, $sp, 4\n");
                fputs(code, fp);
                break;

            default:
                assert(0);
        }

    }
    fclose(fp);
}


void setTempVar(Operand operand) {
    int i, j, k, kind = operand->kind;
    if (operand->kind == OP_ADDRESS) kind = OP_VARIABLE;
    if (operand->kind == OP_VALUE) kind = OP_TEMPORARY;
    for (i = 0; i < 128; ++i) {
        if (!strcmp(func_name, functionTable[i].symboltable.name)) {
            for (k = 0; k < tCounter; ++k) {
                if (functionTable[i].temp.no[k] == operand->u.no) {
                    if (functionTable[i].temp.type[k] == kind) {
                        return;
                    }
                }
            }
            for (k = 0; k < pCounter; ++k) {
                if (functionTable[i].para.no[k] == operand->u.no) {
                    if (functionTable[i].para.type[k] == kind) {
                        return;
                    }
                }
            }
            functionTable[i].temp.no[tCounter] = operand->u.no;
            functionTable[i].temp.type[tCounter] = kind;
            functionTable[i].temp.scale[tCounter] = 1;
            tCounter++;
            functionTable[i].temp.size = tCounter;
            break;
        }
    }
    assert(i != 128);
}

void setArrayVar(Operand operand, int size) {
  int i, j, k;
  for (i = 0; i < 128; ++i) {
      if (!strcmp(func_name, functionTable[i].symboltable.name)) {
          for (k = 0; k < tCounter; ++k) {
              if (functionTable[i].temp.no[k] == operand->u.no) {
                  if (functionTable[i].temp.type[k] == operand->kind) {
                      assert(0);
                  }
              }
          }
          for (k = 0; k < pCounter; ++k) {
              if (functionTable[i].para.no[k] == operand->u.no) {
                  if (functionTable[i].para.type[k] == operand->kind) {
                      assert(0);
                  }
              }
          }

      #ifdef PRINT_GENERATE
          printf("%d %d: (ARRAY)", i, tCounter);
          if (operand->kind == OP_TEMPORARY)
              printf("t");
          else
              printf("v");
          printf("%d\n", operand->u.no);
      #endif

          functionTable[i].temp.no[tCounter] = operand->u.no;
          functionTable[i].temp.type[tCounter] = operand->kind;
          functionTable[i].temp.scale[tCounter] = size / 4;
          functionTable[i].temp.size = ++tCounter;
          break;
      }
  }
  assert(i != 128);
}
int getFuncArgSum(char* text) {
    int i;
    for (i = 0; i < 128; ++i) {
        if (!strcmp(text, functionTable[i].symboltable.name)) {
            return functionTable[i].para.size;
        }
    }
    return -1;
}

int getRegNo(Operand operand) {
    int ret = 0;
    int i = curFuncNo + 2, j, k, kind = operand->kind;
    if (operand->kind == OP_ADDRESS) {
        kind = OP_VARIABLE;
    }
    if (operand->kind == OP_VALUE) {
        kind = OP_TEMPORARY;
    }
    if (operand->kind == OP_CONSTANT) {
        return 0;
    }
    for (j = 0; j < functionTable[i].temp.size; ++j) {
        if (operand->u.no == functionTable[i].temp.no[j] && kind == functionTable[i].temp.type[j]) {
            for (k = 0; k <= j; ++k)
                ret += functionTable[i].temp.scale[k];

        #ifdef PRINT_GENERATE
            if (kind == OP_TEMPORARY) printf("t"); else printf("v");
            printf("%d :%d\n", operand->u.no, ret);
        #endif

            return ret;
       }
    }

    for (j = 0; j < functionTable[i].para.size; ++j) {
        if (operand->u.no == functionTable[i].para.no[j] && kind == functionTable[i].para.type[j]) {
            ret = j + 1;

        #ifdef PRINT_GENERATE
            printf("-ret :%d\n", -ret);
        #endif

            return -ret;
        }
    }
    return 0;
}
