#include "translate.h"
#include "ir.h"
#include "semantic.h"
#include <stdarg.h>
#include <stdlib.h>

int label_no = 0;
int temp_no = 0;
// int var_no = 0;

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
    INIT_LIST_HEAD(&interCodes_head);
}

void printInterCodes(InterCode head) {
    InterCode pos;
    list_for_each(pos, head) {
        printInterCode(pos);
    }
}

void interCodeGenerate() {

    if (root == NULL) return;
    if (!strcmp(root->name, "Program")) {
        translate_ExtDefList(Child(0));
    }

    // InterCode ir[64];

    // ir[0] = intercodeConstruct(IR_PARAM, OP_VARIABLE, 1);
    // ir[1] = intercodeConstruct(IR_ARG, OP_TEMPORARY, 1);
    // ir[2] = intercodeConstruct(IR_CALL, OP_TEMPORARY, 5, OP_FUNCTION, 2);
/*
    ir[0] = intercodeConstruct(IR_FUNC, 2);
    ir[1] = intercodeConstruct(IR_READ, OP_TEMPORARY, 1);
    ir[2] = intercodeConstruct(IR_ASSIGN, OP_VARIABLE, 1, OP_TEMPORARY, 1);
    ir[3] = intercodeConstruct(IR_ASSIGN, OP_TEMPORARY, 2, OP_ADDRESS, 0);
    ir[4] = intercodeConstruct(IR_IFGOTO, OP_VARIABLE, 1, OP_TEMPORARY, 2, OP_LABEL, 1, RELGT);
    ir[5] = intercodeConstruct(IR_GOTO, OP_LABEL, 2);
    ir[6] = intercodeConstruct(IR_LABEL, OP_LABEL, 1);
    ir[7] = intercodeConstruct(IR_ASSIGN, OP_TEMPORARY, 3, OP_ADDRESS, 1);
    ir[8] = intercodeConstruct(IR_WRITE, OP_TEMPORARY, 3);
    ir[9] = intercodeConstruct(IR_GOTO, OP_LABEL, 3);
    ir[10] = intercodeConstruct(IR_LABEL, OP_LABEL, 2);
    ir[11] = intercodeConstruct(IR_ASSIGN, OP_TEMPORARY, 4, OP_ADDRESS, 0);
    ir[12] = intercodeConstruct(IR_IFGOTO, OP_VARIABLE, 1, OP_TEMPORARY, 4, OP_LABEL, 4, RELLT);
    ir[13] = intercodeConstruct(IR_GOTO, OP_LABEL, 5);
    ir[14] = intercodeConstruct(IR_LABEL, OP_LABEL, 4);
    ir[15] = intercodeConstruct(IR_ASSIGN, OP_TEMPORARY, 5, OP_ADDRESS, 1);
    ir[16] = intercodeConstruct(IR_SUB, OP_TEMPORARY, 6, OP_ADDRESS, 0, OP_TEMPORARY, 5);
    ir[17] = intercodeConstruct(IR_WRITE, OP_TEMPORARY, 6);
    ir[18] = intercodeConstruct(IR_GOTO, OP_LABEL, 6);
    ir[19] = intercodeConstruct(IR_LABEL, OP_LABEL, 5);
    ir[20] = intercodeConstruct(IR_ASSIGN, OP_TEMPORARY, 7, OP_ADDRESS, 0);
    ir[21] = intercodeConstruct(IR_WRITE, OP_TEMPORARY, 7);
    ir[22] = intercodeConstruct(IR_LABEL, OP_LABEL, 6);
    ir[23] = intercodeConstruct(IR_LABEL, OP_LABEL, 3);
    ir[24] = intercodeConstruct(IR_ASSIGN, OP_TEMPORARY, 8, OP_ADDRESS, 0);
    ir[25] = intercodeConstruct(IR_RET, OP_TEMPORARY, 8);
*/

    printInterCodes(&interCodes_head);

}

// ExtDefList -> ExtDef ExtDefList | ϵ
void translate_ExtDefList(Node root) {
    InterCode intercode;
    if (Childsum == 0) return;
    while (root != NULL) {
        intercode = translate_ExtDef(Child(0));
        assert(intercode != NULL);
        // insertInterCode(intercode);
        root = Child(1);
    }
}


//
// void insertInterCodes(InterCodes codes) {
//     InterCodes *pos;
//     list_for_each(pos, &codes) {
//         list_add_tail(pos, &interCodes_head);
//     }
// }

// ExtDef -> Specifier ExtDecList SEMI
// | Specifier SEMI | Specifier FunDec CompSt

InterCode translate_ExtDef(Node root) {
    InterCode intercode;
    assert(!strcmp(Child(1)->name, "FunDec"));
    // intercode = translate_Specifier(Child(0));
    // assert(intercode != NULL);
    intercode = translate_FunDec(Child(1));
    /*TODO:*/
    assert(intercode != NULL);
    intercode = translate_CompSt(Child(2));
    assert(intercode != NULL);
    return intercode;
}

// Specifier -> TYPE | StructSpecifier
InterCode translate_Specifier(Node root) {
    InterCode intercode;
    assert(!strcmp(Child(0)->text, "int"));
    return intercode;
}

// FunDec -> ID LP VarList RP | ID LP RP
InterCode translate_FunDec(Node root) {
    if (Childsum == 3) {
        InterCode intercode = intercodeConstruct(IR_FUNC, getFuncNo(Child(0)->text));
        insertInterCode(intercode);
        return intercode;
    }
    else {
        assert(Childsum == 4);
        InterCode intercode = intercodeConstruct(IR_FUNC, getFuncNo(Child(0)->text));
        insertInterCode(intercode);
        intercode = intercodeConstruct(IR_PARAM, OP_VARIABLE, getOpVarNo(Child(2, 0, 1, 0)->text) + 1);
        /*TODO:*/
        insertInterCode(intercode);
        // translate_VarList(Child(2));
        return intercode;
    }

}

// CompSt -> LC DefList StmtList RC
InterCode translate_CompSt(Node root) {
    // translate_DefList
    return translate_StmtList(Child(2));
}

// StmtList -> Stmt StmtList | ϵ
InterCode translate_StmtList(Node root) {
    InterCode intercode;
    while (root != NULL) {
        intercode = translate_Stmt(Child(0));
        // insertInterCode(intercode);
        root = Child(1);
    }
    return intercode;
}

// Stmt -> Exp SEMI
// | CompSt
// | RETURN Exp SEMI
// | IF LP Exp RP Stmt
// | IF LP Exp RP Stmt ELSE Stmt
// | WHILE LP Exp RP Stmt
InterCode translate_Stmt(Node root) {
    InterCode intercode = (InterCode)malloc(sizeof(InterCode_));
    Operand t1 = (Operand)malloc(sizeof(Operand_));
    if (!strcmp(Child(0)->name, "Exp")) {
        translate_Exp(Child(0), NULL);
    }
    else if (!strcmp(Child(0)->name, "CompSt")) {
        intercode = translate_CompSt(Child(0));
    }

    else if (!strcmp(Child(0)->name, "RETURN")) {
        t1->kind = OP_TEMPORARY;
        t1->u.no = ++temp_no;
        translate_Exp(Child(1), t1);
        intercode = intercodeConstruct(IR_RET, OP_TEMPORARY, temp_no);
        insertInterCode(intercode);
    }

    else if (!strcmp(Child(0)->name, "IF")) {

        if (Childsum == 5) {
            assert(0);
        }
        if (Childsum == 7) {
            int label1 = ++label_no;
            int label2 = ++label_no;
            int label3 = ++label_no;
            // intercode = translate_Exp(Child(2), NULL);
            translate_Cond(Child(2), label1, label2);
            intercode = intercodeConstruct(IR_LABEL, OP_LABEL, label1);
            insertInterCode(intercode);

            translate_Stmt(Child(4));
            intercode = intercodeConstruct(IR_GOTO, OP_LABEL, label3);
            insertInterCode(intercode);

            intercode = intercodeConstruct(IR_LABEL, OP_LABEL, label2);
            insertInterCode(intercode);

            translate_Stmt(Child(6));
            intercode = intercodeConstruct(IR_LABEL, OP_LABEL, label3);
            insertInterCode(intercode);
        }

    }

    else {
        assert(0);
    }
    return intercode;
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
    else {
        assert(0);
    }
    return 0;
}

InterCode translate_Cond(Node root, int label_true, int label_false) {
    int type = getTranslateExpType(root);
    Operand t1 = (Operand)malloc(sizeof(Operand_));
    Operand t2 = (Operand)malloc(sizeof(Operand_));
    int op;
    InterCode intercode, code1, code2, code3, ir;
    code1 = (InterCode)malloc(sizeof(InterCode_));
    code2 = (InterCode)malloc(sizeof(InterCode_));
    ir = (InterCode)malloc(sizeof(InterCode_));
    switch(type) {
        case 60:
            // assert(0);
            t1->kind = OP_TEMPORARY;
            t1->u.no = ++temp_no;
            /*code1 = */translate_Exp(Child(0), t1);

            t2->kind = OP_TEMPORARY;
            t2->u.no = ++temp_no;
            /*code2 = */translate_Exp(Child(2), t2);

            op = get_relop(Child(1));
            code3 = intercodeConstruct(IR_IFGOTO, t1->kind, t1->u.no, t2->kind, t2->u.no, OP_LABEL, label_true, op);
            insertInterCode(code3);
            insertInterCode(intercodeConstruct(IR_GOTO, OP_LABEL, label_false));
            break;
    }
    return intercode;
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
// | Exp LB Exp RB
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
    }

    return 0;
}

InterCode translate_Args(Node root, Operand *arg_list) {
    if (Childsum == 1) {
        Operand t1 = (Operand)malloc(sizeof(Operand_));
        t1->kind = OP_TEMPORARY;
        t1->u.no = ++temp_no;
        InterCode code1 = translate_Exp(Child(0), t1);
        *arg_list = t1;
        // insertInterCode(code1);
        return code1;
    }
    assert(0);
}

InterCode translate_Exp(Node root, Operand operand) {
    int type = getTranslateExpType(root);
    InterCode intercode = (InterCode)malloc(sizeof(InterCode_));
    InterCode code1, code2;
    Operand var = (Operand)malloc(sizeof(Operand_));
    Operand t1 = (Operand)malloc(sizeof(Operand_));
    Operand t2 = (Operand)malloc(sizeof(Operand_));
    Operand *arg_list = (Operand *)malloc(sizeof(Operand));
    int no = -1;
    switch (type) {
      case 8:
          code1 = translate_Args(Child(2), arg_list);
          // insertInterCode(code1);
          if (!strcmp(Child(0)->text, "write")) {
              intercode = intercodeConstruct(IR_WRITE, OP_TEMPORARY, (*arg_list)->u.no);
              insertInterCode(intercode);
              assert(arg_list != NULL);
          }
          // | ID LP Args RP 8
          /*TODO:*/
          if (operand != NULL) {
              intercode = intercodeConstruct(IR_ARG, OP_VARIABLE, getOpVarNo(Child(2, 0, 0)->text));
              insertInterCode(intercode);
              intercode = intercodeConstruct(IR_CALL, OP_TEMPORARY, operand->u.no, OP_FUNCTION, getFuncNo(Child(0)->text));
              insertInterCode(intercode);
          }

          break;
      case 7:
          if (!strcmp(Child(0)->text, "read")) {
              intercode = intercodeConstruct(IR_READ, OP_TEMPORARY, operand->u.no);
              no = getFuncNo(Child(0)->text);
              if (no == -1) assert(0);
              functionTable[no].t_no = temp_no;
              insertInterCode(intercode);
          }
          break;
      case 3:
          // Exp -> Exp(ID) ASSIGNOP Exp
          t1->kind = OP_TEMPORARY;
          t1->u.no = ++temp_no;
          code1 = translate_Exp(Child(2), t1);
          // insertInterCode(code1);
          code2 = intercodeConstruct(IR_ASSIGN, OP_VARIABLE, getOpVarNo(Child(0, 0)->text) + 1, OP_TEMPORARY, t1->u.no);
          insertInterCode(code2);
          if (operand != NULL) assert(0);
          break;
      case 40:
      case 41:
      case 42:
      case 43:
          t1->kind = OP_TEMPORARY;
          t1->u.no = ++temp_no;
          t2->kind = OP_TEMPORARY;
          t2->u.no = ++temp_no;
          translate_Exp(Child(0), t1);
          translate_Exp(Child(2), t2);
          if (type == 40) {
              intercode = intercodeConstruct(IR_ADD, operand->kind, operand->u.no,
                t1->kind, t1->u.no, t2->kind, t2->u.no);
          }
          else if (type == 41) {
              intercode = intercodeConstruct(IR_SUB, operand->kind, operand->u.no,
                t1->kind, t1->u.no, t2->kind, t2->u.no);
          }
          else if (type == 42) {
              intercode = intercodeConstruct(IR_MUL, operand->kind, operand->u.no,
                t1->kind, t1->u.no, t2->kind, t2->u.no);
          }
          else {
              intercode = intercodeConstruct(IR_DIV, operand->kind, operand->u.no,
                t1->kind, t1->u.no, t2->kind, t2->u.no);
          }
          insertInterCode(intercode);
          break;
      case 6:
          var->kind = OP_VARIABLE;
          var->u.no = getOpVarNo(Child(0, 0)->text) + 1;
          intercode->code.kind = IR_ASSIGN;
          intercode->code.biop.x = operand;
          intercode->code.biop.y = var;
          break;
      case 2:
          assert(!strcmp(Child(0)->name, "ID"));
          var->kind = OP_VARIABLE;
          var->u.no = functionTable[getOpVarNo(Child(0)->text)].t_no + 1;
          intercode->code.kind = IR_ASSIGN;
          intercode->code.biop.x = operand;
          intercode->code.biop.y = var;
          insertInterCode(intercode);
          break;
      case 1:
          assert(!strcmp(Child(0)->name, "INT"));
          if (operand != NULL) {
              // assert(0);
              var->kind = OP_CONSTANT;
              // strcpy(var->u.value, Child(0)->text);
              var->u.no = atoi(Child(0)->text);
              intercode->code.kind = IR_ASSIGN;
              intercode->code.biop.x = operand;
              intercode->code.biop.y = var;
              insertInterCode(intercode);
          }
          break;
      case 5:
          t1->kind = OP_TEMPORARY;
          t1->u.no = ++temp_no;
          code1 = translate_Exp(Child(1), t1);
          // insertInterCode(code1);
          code2 = intercodeConstruct(IR_SUB, operand->kind, operand->u.no, OP_CONSTANT, 0, t1->kind, t1->u.no);
          insertInterCode(code2);
          break;
      case 60:
          break;
      case 9:
          assert(!strcmp(Child(1)->name, "Exp"));
          translate_Exp(Child(1), t1);
          break;
      default: printf("%d\n", type); assert(0);

    }
    return intercode;
}
