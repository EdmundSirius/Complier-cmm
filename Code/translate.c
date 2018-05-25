#include "translate.h"
#include "ir.h"
#include "semantic.h"
#include <stdarg.h>
#include <stdlib.h>

int label_no = 0;
int temp_no = 0;
int func_no = 0;
int arg_no = 0;
int isParam[4096];
FILE *fp;
struct list_head head;
char outputFile[128];

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
}

void printInterCodes() {
    FILE *fp = fopen("out.ir", "wt");
    if(fp == NULL)
        assert(0);
    struct list_head *plist;
    list_for_each(plist, &head) {
        InterCodes *node = list_entry(plist, InterCodes, list);
        printInterCode(node->intercode);
    }
    fclose(fp);
}

void interCodeGenerate() {
    if (root == NULL) return;
    if (!strcmp(root->name, "Program")) {
        translate_ExtDefList(Child(0));
    }
    writeFile("original.ir");
#ifdef PRINT_IR
    printInterCodes();
#endif
}

// ExtDefList -> ExtDef ExtDefList | ϵ
void translate_ExtDefList(Node root) {
    if (Childsum == 0) return;
    while (root != NULL) {
        translate_ExtDef(Child(0));
        root = Child(1);
    }
}

// ExtDef -> Specifier ExtDecList SEMI
// | Specifier SEMI | Specifier FunDec CompSt

void translate_ExtDef(Node root) {
    assert(!strcmp(Child(1)->name, "FunDec"));
    translate_FunDec(Child(1));
    translate_CompSt(Child(2));

}

// ParamDec -> Specifier VarDec
// Specifier -> TYPE | StructSpecifier
// VarDec -> ID | VarDec LB INT RB

void translate_ParamDec(Node root) {
    assert(!strcmp(Child(0, 0)->name, "TYPE"));
    if (Child(1)->childsum == 1) {
        CREATE_IR(IR_PARAM, OP_VARIABLE, getOpVarNo(Child(1, 0)->text) + 1);
        isParam[getOpVarNo(Child(1, 0)->text) + 1] = 1;
    }
    else {
        assert(Child(1)->childsum == 4);
        CREATE_IR(IR_PARAM, OP_VARIABLE, getOpVarNo(Child(1, 0, 0)->text) + 1);
        isParam[getOpVarNo(Child(1, 0, 0)->text) + 1] = 1;
    }
}

// VarList -> ParamDec COMMA VarList | ParamDec
void translate_VarList(Node root) {
    translate_ParamDec(Child(0));
    if (Childsum == 3) {
        translate_VarList(Child(2));
    }
}

// FunDec -> ID LP VarList RP | ID LP RP
void translate_FunDec(Node root) {
    CREATE_IR(IR_FUNC, getFuncNo(Child(0)->text));
    if (Childsum == 4) {
        translate_VarList(Child(2));
    }
}

// VarDec -> ID (| VarDec LB INT RB)
void translate_VarDec(Node root) {
    if (Childsum != 1) {
      Array *array = getArray(root);
      int size = array->dim;
      // printf("[size:%d]\n", size);
      Operand var = (Operand)malloc(sizeof(Operand_));
      Operand operand = (Operand)malloc(sizeof(Operand_));
      operand->kind = OP_SIZE;
      var->kind = OP_VARIABLE;
      int i = 0;
      operand->u.no = 8;
      for (; i < size; ++i) {
          root = Child(0);
          operand->u.no = operand->u.no * array->size[i];
      }
      var->u.no = getOpVarNo(Child(0)->text) + 1;
      CREATE_IR(IR_DEC, var->kind, var->u.no, operand->kind, operand->u.no);
    }
}

// Dec -> VarDec | VarDec ASSIGNOP Exp
// VarDec -> ID (| VarDec LB INT RB)
void translate_Dec(Node root) {
    Operand var = (Operand)malloc(sizeof(Operand_));
    Operand t1 = (Operand)malloc(sizeof(Operand_));
    Operand t2 = (Operand)malloc(sizeof(Operand_));
    if (Childsum == 3) {
        assert(!strcmp(Child(0, 0)->name, "ID"));
        t1->kind = OP_TEMPORARY;
        t1->u.no = ++temp_no;

        var->kind = OP_VARIABLE;
        var->u.no = getOpVarNo(Child(0, 0)->text) + 1;
        assert(var->u.no != 0);

        t2->kind = OP_TEMPORARY;
        t2->u.no = ++temp_no;
        translate_Exp(Child(2), t2, 1);
        CREATE_IR(IR_ASSIGN, var->kind, var->u.no, t2->kind, t2->u.no);
    }
    else {
        assert(Childsum == 1);
        translate_VarDec(Child(0));
    }
}

// DecList -> Dec | Dec COMMA DecList
void translate_DecList(Node root) {
    translate_Dec(Child(0));
    if (Childsum == 3) {
        translate_DecList(Child(2));
    }
}

// Def -> Specifier DecList SEMI
void translate_Def(Node root) {
    translate_DecList(Child(1));
}

// DefList -> Def DefList | ϵ
void translate_DefList(Node root) {
    if (root == NULL) return;
    while (root != NULL) {
        translate_Def(Child(0));
        root = Child(1);
    }
}

// CompSt -> LC DefList StmtList RC
void translate_CompSt(Node root) {
    translate_DefList(Child(1));
    translate_StmtList(Child(2));
}

// StmtList -> Stmt StmtList | ϵ
void translate_StmtList(Node root) {
    while (root != NULL) {
        assert(Childsum == 2);
        translate_Stmt(Child(0));
        root = Child(1);
    }
}

// Stmt -> Exp SEMI
// | CompSt
// | RETURN Exp SEMI
// | IF LP Exp RP Stmt
// | IF LP Exp RP Stmt ELSE Stmt
// | WHILE LP Exp RP Stmt
void translate_Stmt(Node root) {
    if (!strcmp(Child(0)->name, "Exp")) {
        translate_Exp(Child(0), NULL, -1);
    }
    else if (!strcmp(Child(0)->name, "CompSt")) {
        translate_CompSt(Child(0));
    }

    else if (!strcmp(Child(0)->name, "RETURN")) {
        Operand t1 = CREATE_TEMP_OP();
        t1->kind = OP_TEMPORARY;
        t1->u.no = ++temp_no;
        translate_Exp(Child(1), t1, 1);
        CREATE_IR(IR_RET, t1->kind, t1->u.no);
    }

    else if (!strcmp(Child(0)->name, "IF")) {
        if (Childsum == 5) {
            int label1 = ++label_no;
            int label2 = ++label_no;
            translate_Cond(Child(2), label1, label2);
            CREATE_IR(IR_LABEL, OP_LABEL, label1);
            translate_Stmt(Child(4));
            CREATE_IR(IR_LABEL, OP_LABEL, label2);
        }
        if (Childsum == 7) {
            int label1 = ++label_no;
            int label2 = ++label_no;
            int label3 = ++label_no;
            translate_Cond(Child(2), label1, label2);
            CREATE_IR(IR_LABEL, OP_LABEL, label1);
            translate_Stmt(Child(4));
            CREATE_IR(IR_GOTO, OP_LABEL, label3);
            CREATE_IR(IR_LABEL, OP_LABEL, label2);
            translate_Stmt(Child(6));
            CREATE_IR(IR_LABEL, OP_LABEL, label3);
        }
    }

    else {
        assert(!strcmp(Child(0)->name, "WHILE"));
        int label1 = ++label_no;
        int label2 = ++label_no;
        int label3 = ++label_no;
        CREATE_IR(IR_LABEL, OP_LABEL, label1);
        translate_Cond(Child(2), label2, label3);
        CREATE_IR(IR_LABEL, OP_LABEL, label2);
        translate_Stmt(Child(4));
        CREATE_IR(IR_GOTO, OP_LABEL, label1);
        CREATE_IR(IR_LABEL, OP_LABEL, label3);
    }
}

int get_relop(Node root) {
    assert(!strcmp(root->name, "RELOP"));
    if (!strcmp(root->text, ">")) {
        return RELGT;
    }
    else if (!strcmp(root->text, "<")) {
        return RELLT;
    }
    else if (!strcmp(root->text, "==")) {
        return EQUAL;
    }
    else if (!strcmp(root->text, "!=")) {
        return NOTEQ;
    }
    else if (!strcmp(root->text, ">=")) {
        return RELGE;
    }
    else if (!strcmp(root->text, "<=")) {
        return RELLE;
    }
    else {
        assert(0);
    }
    return 0;
}

void translate_Cond(Node root, int label_true, int label_false) {
    int type = getTranslateExpType(root), op, label1;
    Operand t1 = (Operand)malloc(sizeof(Operand_));
    Operand t2 = (Operand)malloc(sizeof(Operand_));
    switch(type) {
        case 60:
            t1->kind = OP_TEMPORARY;
            t1->u.no = ++temp_no;
            translate_Exp(Child(0), t1, 1);

            t2->kind = OP_TEMPORARY;
            t2->u.no = ++temp_no;
            translate_Exp(Child(2), t2, 1);

            op = get_relop(Child(1));
            CREATE_IR(IR_IFGOTO, t1->kind, t1->u.no, t2->kind, t2->u.no, OP_LABEL, label_true, op);
            CREATE_IR(IR_GOTO, OP_LABEL, label_false);
            break;
        case 61:
            translate_Cond(Child(1), label_false, label_true);
            break;
        case 62:
            label1 = ++label_no;
            translate_Cond(Child(0), label1, label_false);
            CREATE_IR(IR_LABEL, OP_LABEL, label1);
            translate_Cond(Child(2), label_true, label_false);
            break;
        case 63:
            label1 = ++label_no;
            translate_Cond(Child(0), label_true, label1);
            CREATE_IR(IR_LABEL, OP_LABEL, label1);
            translate_Cond(Child(2), label_true, label_false);
            break;
        default:
            t1->kind = OP_TEMPORARY;
            t1->u.no = ++temp_no;
            translate_Exp(root, t1, 1);
            // code2 = [IF t1 != #0 GOTO label_true]
            CREATE_IR(IR_IFGOTO, t1->kind, t1->u.no, OP_CONSTANT, 0, OP_LABEL, label_true, NOTEQ);
            CREATE_IR(IR_GOTO, OP_LABEL, label_false);
    }
}

// Exp ->
//   Exp ASSIGNOP Exp 3
// | Exp AND Exp 62
// | Exp OR Exp 63
// | Exp RELOP Exp 60
// | Exp PLUS Exp 4
// | Exp MINUS Exp 4
// | Exp STAR Exp 4
// | Exp DIV Exp 4
// | LP Exp RP 9
// | MINUS Exp 5
// | NOT Exp 61
// | ID LP Args RP 8
// | ID LP RP 7
// | Exp LB Exp RB 81
// | Exp DOT ID 0
// | ID 2
// | INT 1
// | FLOAT 0

int getTranslateExpType(Node root) {
    if (Childsum == 1) {
        if (!strcmp(Child(0)->name, "INT")) return 1;
        else if (!strcmp(Child(0)->name, "ID")) return 2;
    }
    else if (Childsum == 2) {
        if (!strcmp(Child(0)->name, "MINUS")) return 5;
    }
    else if (Childsum == 3) {
        if (!strcmp(Child(1)->name, "ASSIGNOP")) return 3;
        if (!strcmp(Child(1)->name, "PLUS")) return 40;
        if (!strcmp(Child(1)->name, "MINUS")) return 41;
        if (!strcmp(Child(1)->name, "STAR")) return 42;
        if (!strcmp(Child(1)->name, "DIV")) return 43;
        if (!strcmp(Child(1)->name, "AND")) return 62;
        if (!strcmp(Child(1)->name, "OR")) return 63;
        if (!strcmp(Child(1)->name, "RELOP")) return 60;
        if (!strcmp(Child(0)->name, "ID")) return 7;
        if (!strcmp(Child(0)->name, "LP")) return 9;
    }
    else if (Childsum == 4) {
        if (!strcmp(Child(0)->name, "ID")) return 8;
        if (!strcmp(Child(1)->name, "LB")) return 81;
    }

    return 0;
}

// Args -> Exp COMMA Args | Exp
void translate_Args(Node root, Operand *arg_list) {
    Operand t1 = CREATE_TEMP_OP();
    translate_Exp(Child(0), t1, 1);
    arg_list[arg_no++] = t1;
    if (Childsum != 1) {
        translate_Args(Child(2), arg_list);
    }
}

void translate_Array(Node root, Operand operand, int direction) {
    Node tmp = root;
    Array *array = getArray(root);
    int size = array->dim;
    int opVarNo1;

    root = Child(0);
    while (strcmp(Child(0)->name, "ID")) {
        root = Child(0);
    }

    assert(!strcmp(Child(0)->name, "ID"));
    opVarNo1 = getOpVarNo(Child(0)->text) + 1;
    root = tmp;

#ifdef TRANSLATE
    printf("ARRAY:");
    int i = 0;
    for (; i < size; ++i) {
        printf("[%d]", array->size[i]);
    }
    printf("\n");
#endif

    int n = 3 * size - 1;
    int no1 = size - 1, no2 = no1 + 1, no3 = no2 + 1, i;
    Operand tmp1, tmp2, tmp3;
    Operand *t = malloc(n * sizeof(Operand));

    for (i = 0; i < n; ++i) {
        t[i] = CREATE_TEMP_OP();
    }
    for (i = 0; i < size; ++i) {
        translate_Exp(Child(2), t[i], direction);
        root = Child(0);
    }

    for (i = 0; i < size - 1; ++i) {
        tmp1 = t[no1];
        tmp2 = t[no2];
        tmp3 = t[no3];
        CREATE_IR(IR_MUL, tmp2->kind, tmp2->u.no, tmp1->kind, tmp1->u.no, OP_CONSTANT, array->size[size - i - 2]);
        CREATE_IR(IR_ADD, tmp3->kind, tmp3->u.no, tmp2->kind, tmp2->u.no, t[size - i - 2]->kind, t[size - i - 2]->u.no);
        no1 += 2;
        no2 += 2;
        no3 += 2;
    }

    CREATE_IR(IR_MUL, t[n - 1]->kind, t[n - 1]->u.no, t[n - 2]->kind, t[n - 2]->u.no, OP_CONSTANT, 4);
    if (isParam[opVarNo1] == 1) {
        CREATE_IR(IR_ADD, operand->kind, operand->u.no, OP_VARIABLE, opVarNo1, t[n - 1]->kind, t[n - 1]->u.no);
    }
    else {
        CREATE_IR(IR_ADD, operand->kind, operand->u.no, OP_ADDRESS, opVarNo1, t[n - 1]->kind, t[n - 1]->u.no);
    }
    Operand var = (Operand)malloc(sizeof(Operand_));
    if (direction == 1) {
        var->kind = OP_TEMPORARY;
        var->u.no = ++temp_no;
        CREATE_IR(IR_ASSIGN, var->kind, var->u.no, OP_VALUE, operand->u.no);
        operand->u.no = var->u.no;
    }
}

void translate_Exp(Node root, Operand operand, int direction) {
    int type = getTranslateExpType(root);
    Operand var = (Operand)malloc(sizeof(Operand_));
    Operand t1 = (Operand)malloc(sizeof(Operand_));
    Operand t2 = (Operand)malloc(sizeof(Operand_));
    Operand arg_list[32];
    int no = -1;
    int opVarNo1;
    char name[128];
    switch (type) {
      case 8:
          arg_no = 0;
          translate_Args(Child(2), arg_list);
          if (!strcmp(Child(0)->text, "write")) {
              CREATE_IR(IR_WRITE, OP_TEMPORARY, arg_list[arg_no - 1]->u.no);
              break;
          }

          int argSum = getVarSum(Child(0));
          int i;

          for (i = arg_no - 1; i >= 0; --i) {
              CREATE_IR(IR_ARG, arg_list[i]->kind, arg_list[i]->u.no);
          }
          if (operand != NULL) {
              CREATE_IR(IR_CALL, operand->kind, operand->u.no, OP_FUNCTION, getFuncNo(Child(0)->text));
          }
          else {
              Operand t1 = CREATE_TEMP_OP();
              CREATE_IR(IR_CALL, t1->kind, t1->u.no, OP_FUNCTION, getFuncNo(Child(0)->text));
          }
          break;
      case 7:
          if (!strcmp(Child(0)->text, "read")) {
              no = getFuncNo(Child(0)->text);
              if (no == -1) assert(0);
              CREATE_IR(IR_READ, OP_TEMPORARY, operand->u.no);
          }
          else {
              CREATE_IR(IR_CALL, operand->kind, operand->u.no, OP_FUNCTION, getFuncNo(Child(0)->text));
          }
          break;
      case 3:
          // Exp -> Exp(ID) ASSIGNOP Exp
          t1->kind = OP_TEMPORARY;
          t1->u.no = ++temp_no;
          translate_Exp(Child(2), t1, 1);
          if (!strcmp(Child(0, 0)->name, "ID")) {
              var->kind = OP_VARIABLE;
              var->u.no = getOpVarNo(Child(0, 0)->text) + 1;
              CREATE_IR(IR_ASSIGN, var->kind, var->u.no, t1->kind, t1->u.no);
              if (operand != NULL)
                  CREATE_IR(IR_ASSIGN, operand->kind, operand->u.no, var->kind, var->u.no);
          }
          else {
              t2->kind = OP_TEMPORARY;
              t2->u.no = ++temp_no;
              translate_Exp(Child(0), t2, 0);
              CREATE_IR(IR_ASSIGN, OP_VALUE, t2->u.no, t1->kind, t1->u.no);
              if (operand != NULL)
                  CREATE_IR(IR_ASSIGN, operand->kind, operand->u.no, OP_VALUE, t2->u.no);
          }
          break;
      case 40:
      case 41:
      case 42:
      case 43:
          t1->kind = OP_TEMPORARY;
          t1->u.no = ++temp_no;
          t2->kind = OP_TEMPORARY;
          t2->u.no = ++temp_no;
          translate_Exp(Child(0), t1, 1);
          translate_Exp(Child(2), t2, 1);
          if (type == 40) {
              CREATE_IR(IR_ADD, operand->kind, operand->u.no, t1->kind, t1->u.no, t2->kind, t2->u.no);
          }
          else if (type == 41) {
              CREATE_IR(IR_SUB, operand->kind, operand->u.no, t1->kind, t1->u.no, t2->kind, t2->u.no);
          }
          else if (type == 42) {
              CREATE_IR(IR_MUL, operand->kind, operand->u.no, t1->kind, t1->u.no, t2->kind, t2->u.no);
          }
          else {
              CREATE_IR(IR_DIV, operand->kind, operand->u.no, t1->kind, t1->u.no, t2->kind, t2->u.no);
          }
          break;
      case 6:
          assert(0);
          break;
      case 2:
          assert(!strcmp(Child(0)->name, "ID"));
          var->u.no = getOpVarNo(Child(0)->text) + 1;
          if (isArray(Child(0)->text)) {
              if (isParam[var->u.no] == 1) {
                  var->kind = OP_VARIABLE;
              }
              else {
                  var->kind = OP_ADDRESS;
              }
          }
          else {
              var->kind = OP_VARIABLE;
          }
          CREATE_IR(IR_ASSIGN, operand->kind, operand->u.no, var->kind, var->u.no);
          break;
      case 1:
          assert(!strcmp(Child(0)->name, "INT"));
          if (operand != NULL) {
              var->kind = OP_CONSTANT;
              var->u.no = atoi(Child(0)->text);
              CREATE_IR(IR_ASSIGN, operand->kind, operand->u.no, var->kind, var->u.no);
          }
          else {
              assert(0);
          }
          break;
      case 5:
          t1->kind = OP_TEMPORARY;
          t1->u.no = ++temp_no;
          translate_Exp(Child(1), t1, 1);
          CREATE_IR(IR_SUB, operand->kind, operand->u.no, OP_CONSTANT, 0, t1->kind, t1->u.no);
          break;
      case 60:
          assert(0);
          break;
      case 9:
          assert(!strcmp(Child(1)->name, "Exp"));
          translate_Exp(Child(1), operand, direction);
          break;
      case 81:
          translate_Array(root, operand, direction);
          break;
      default: printf("%d\n", type); assert(0);
    }
}
