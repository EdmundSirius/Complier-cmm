#include "semantic.h"
#include "translate.h"
#include "ir.h"
#include <stdarg.h>
#include <stdlib.h>

void interCodeOptimize() {
    struct list_head *plist;
    int codeSum = 0, newCodeSum = 0;
    // divideBasicBlock();
    list_for_each(plist, &head) {
        ++newCodeSum;
        InterCodes *node = list_entry(plist, InterCodes, list);
    }
    do {
        codeSum = newCodeSum;
        calculateConstExp();
        eliminateConstant();
        newCodeSum = 0;
        list_for_each(plist, &head) {
            ++newCodeSum;
            InterCodes *node = list_entry(plist, InterCodes, list);
        }
    } while(newCodeSum != codeSum);

    writeFile(outputFile);
}

void writeFile(char *name) {
    fp = fopen(name, "w");
    struct list_head *plist;
    if(fp == NULL) assert(0);
    list_for_each(plist, &head) {
        InterCodes *node = list_entry(plist, InterCodes, list);
        printInterCode(node->intercode);
    }
    fclose(fp);
}

void divideBasicBlock() {
  int i = 0;
  struct list_head *plist;
  list_for_each(plist, &head) {
      InterCodes *node = list_entry(plist, InterCodes, list);
      if (node->intercode.kind == IR_GOTO || node->intercode.kind == IR_IFGOTO) {
          if (plist->next != &head) {
              node->isLeader = true;
              i++;
          }
          int no;
          if (node->intercode.kind == IR_GOTO) {
              no = node->intercode.op.x->u.no;
          }
          else {
              no = node->intercode.triop.z->u.no;
          }
          struct list_head *subplist;
          list_for_each(subplist, &head) {
              InterCodes *subnode = list_entry(subplist, InterCodes, list);
              if (subnode->intercode.kind == IR_LABEL) {
                  if (subnode->intercode.op.x->u.no == no) {
                      subnode->isLeader = true;
                      i++;
                  }
              }
          }
      }
  }
}

void eliminateConstant() {
  int codeSum = 0;
  struct list_head *p, *plist, *qlist;
  list_for_each(plist, &head) {
      ++codeSum;
      InterCodes *node = list_entry(plist, InterCodes, list);
  }
  int mode = 0;
  int t_no = 0;
  int t_value = 0;

  while (codeSum--) {
      list_for_each_safe(plist, qlist, &head) {
          InterCodes *node = list_entry(plist, InterCodes, list);
          if (mode == 0) {
              if (node->intercode.kind == IR_ASSIGN && node->intercode.biop.y->kind == OP_CONSTANT &&
                node->intercode.biop.x->kind == OP_TEMPORARY) {
                    mode = 1;
                    t_no = node->intercode.biop.x->u.no;
                    t_value = node->intercode.biop.y->u.no;
                    p = plist;
                    list_del(p);
              }
          }
          else {
              int kind = node->intercode.kind;
              switch(kind) {
                  case IR_ASSIGN:
                      if (node->intercode.biop.y->kind == OP_TEMPORARY && node->intercode.biop.y->u.no == t_no) {
                          node->intercode.biop.y->kind = OP_CONSTANT;
                          node->intercode.biop.y->u.no = t_value;
                          mode = 0;
                      }
                      break;
                  case IR_ADD:
                  case IR_SUB:
                  case IR_MUL:
                  case IR_DIV:
                      if (node->intercode.triop.y->kind == OP_TEMPORARY && node->intercode.triop.y->u.no == t_no) {
                          node->intercode.triop.y->kind = OP_CONSTANT;
                          node->intercode.triop.y->u.no = t_value;
                          mode = 0;
                      }
                      else if (node->intercode.triop.z->kind == OP_TEMPORARY && node->intercode.triop.z->u.no == t_no) {
                          node->intercode.triop.z->kind = OP_CONSTANT;
                          node->intercode.triop.z->u.no = t_value;
                          mode = 0;
                      }
                      break;
                  case IR_READ:
                  case IR_WRITE:
                  case IR_ARG:
                  case IR_RET:
                      if (node->intercode.op.x->kind == OP_TEMPORARY && node->intercode.op.x->u.no == t_no) {
                          node->intercode.op.x->kind = OP_CONSTANT;
                          node->intercode.op.x->u.no = t_value;
                          mode = 0;
                      }
                      break;
                  case IR_IFGOTO:
                      if (node->intercode.ifop.x->kind == OP_TEMPORARY && node->intercode.ifop.x->u.no == t_no) {
                          node->intercode.ifop.x->kind = OP_CONSTANT;
                          node->intercode.ifop.x->u.no = t_value;
                          mode = 0;
                      }
                      else if (node->intercode.ifop.y->kind == OP_TEMPORARY && node->intercode.ifop.y->u.no == t_no) {
                          node->intercode.ifop.y->kind = OP_CONSTANT;
                          node->intercode.ifop.y->u.no = t_value;
                          mode = 0;
                      }
                      break;

                  default: ;
              }
          }
      }
  }
}

void calculateConstExp() {
  struct list_head *plist;
  list_for_each(plist, &head) {
      InterCodes *node = list_entry(plist, InterCodes, list);
      int sum;
      if (node->intercode.kind == IR_ADD) {
          if (node->intercode.triop.y->kind == OP_CONSTANT && node->intercode.triop.z->kind == OP_CONSTANT) {
              node->intercode.kind = IR_ASSIGN;
              node->intercode.biop.x->kind = node->intercode.triop.x->kind;
              node->intercode.biop.x->u.no = node->intercode.triop.x->u.no;
              node->intercode.biop.x->kind = node->intercode.triop.x->kind;
              node->intercode.biop.y->u.no = node->intercode.triop.y->u.no + node->intercode.triop.z->u.no;
          }
      }
      else if (node->intercode.kind == IR_SUB) {
          if (node->intercode.triop.y->kind == OP_CONSTANT && node->intercode.triop.z->kind == OP_CONSTANT) {
              node->intercode.kind = IR_ASSIGN;
              node->intercode.biop.x->kind = node->intercode.triop.x->kind;
              node->intercode.biop.x->u.no = node->intercode.triop.x->u.no;
              node->intercode.biop.x->kind = node->intercode.triop.x->kind;
              node->intercode.biop.y->u.no = node->intercode.triop.y->u.no - node->intercode.triop.z->u.no;
          }
      }
      else if (node->intercode.kind == IR_MUL) {
          if (node->intercode.triop.y->kind == OP_CONSTANT && node->intercode.triop.z->kind == OP_CONSTANT) {
              node->intercode.kind = IR_ASSIGN;
              node->intercode.biop.x->kind = node->intercode.triop.x->kind;
              node->intercode.biop.x->u.no = node->intercode.triop.x->u.no;
              node->intercode.biop.x->kind = node->intercode.triop.x->kind;
              node->intercode.biop.y->u.no = node->intercode.triop.y->u.no * node->intercode.triop.z->u.no;
          }
      }
      else if (node->intercode.kind == IR_DIV) {
          if (node->intercode.triop.y->kind == OP_CONSTANT && node->intercode.triop.z->kind == OP_CONSTANT) {
              node->intercode.kind = IR_ASSIGN;
              node->intercode.biop.x->kind = node->intercode.triop.x->kind;
              node->intercode.biop.x->u.no = node->intercode.triop.x->u.no;
              node->intercode.biop.x->kind = node->intercode.triop.x->kind;
              node->intercode.biop.y->u.no = node->intercode.triop.y->u.no / node->intercode.triop.z->u.no;
          }
      }
  }
}
