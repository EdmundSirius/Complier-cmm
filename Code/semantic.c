#include "semantic.h"
#include "parse.h"

int FLAG;
int PARA;
int IN_STRUCT;

void semanticAnalysis() {
    // initSymbolTable();
    /* unnecessary cuz struct varaibles are initialized impicitly
    the same way as objects that have static storage duration. */
    if (root == NULL) return;
    if (!strcmp(root->name, "Program")) {
        handleExtDefList(Child(0), 0);
    }
    #ifdef PHASE_2
    printSymbolTable();
    printStructList();
    #endif
}

// ExtDefList -> ExtDef ExtDefList | ϵ
void handleExtDefList(Node* root, int flag) {
    if (Childsum == 0) return;
    // Iteration is better than recursion.
    while (root != NULL) {
        handleExtDef(Child(0), 0);
        root = Child(1);
    }

    #ifdef HANDLE_DEBUG
    printf("handleExtDefList bingo\n");
    #endif
}

// ExtDef -> Specifier ExtDecList SEMI
// | Specifier SEMI | Specifier FunDec CompSt
void handleExtDef(Node* root, int flag) {
    int specifierNo;
    if (!strcmp(Child(1)->name, "ExtDecList")) {
        FLAG = 3;
        flag = 3;
        specifierNo = handleSpecifier(Child(0), flag);
        handleExtDecList(Child(1), flag, specifierNo);
    }
    else if (!strcmp(Child(1)->name, "FunDec")) {
        FLAG = 2;
        specifierNo = handleSpecifier(Child(0), flag);
        char functionName[128];
        Function thisfunction;
        thisfunction.argsum = 0;
        handleFunDec(Child(1), flag, &thisfunction, functionName);
        // assert (specifierNo == 0 || specifierNo == 1);

        // if (specifierNo == 0 || specifierNo == 1) {
            thisfunction.returnvalue = specifierNo;

            if (findSymbolTable(functionName)) {
                PrintSemErrorMsg(4, Child(1)->lineno, functionName);
            }
            else {
                insertFuncSymbolTable(functionName, &thisfunction);
            }
            handleCompSt(Child(2), flag, specifierNo);
        // }

    }
    else {
        FLAG = 1;

        specifierNo = handleSpecifier(Child(0), flag);
        printf("%d\n", specifierNo);

    }

    FLAG = 0;

    #ifdef HANDLE_DEBUG
    printf("handleExtDef bingo\n");
    #endif
}

// Specifier -> TYPE | StructSpecifier
int handleSpecifier(Node* root, int flag) {
    if (!strcmp(Child(0)->name, "StructSpecifier")) {
        return handleStructSpecifier(Child(0), flag);
    }
    if (!strcmp(Child(0)->text, "int")) {
        return INT;
    }
    else {
        return FLOAT;
    }
}


// StructSpecifier -> STRUCT OptTag LC DefList RC | STRUCT Tag
int handleStructSpecifier(Node* root, int flag) {

    if (Childsum == 5) {
        // OptTag -> ID | ϵ
        if (Child(1) != NULL) {
            assert (Child(1)->childsum == 1);
            if (getStructNo(Child(1, 0)->text)) {
                PrintSemErrorMsg(16, Child(1, 0)->lineno, Child(1, 0)->text);
            }
            else {
                strcpy(specifierStructName, Child(1, 0)->text);
                // printf("name:%s\n", Child(1, 0)->text);
                createStructNode(Child(1, 0)->text);
            }
        }
        ++IN_STRUCT;
        handleDefList(Child(3), flag);
        --IN_STRUCT;
        return getStructNo(Child(1, 0)->text);
    }

    else if (Childsum == 2) {
        if (!getStructNo(Child(1, 0)->text)) {
            PrintSemErrorMsg(17, Child(1, 0)->lineno, Child(1, 0)->text);
        }
        else {
            strcpy(specifierStructName, Child(1, 0)->text);
        }
        // Tag -> ID
        return getStructNo(Child(1, 0)->text);
    }
    return -1;
}

// ExtDecList -> VarDec | VarDec COMMA ExtDecList
void handleExtDecList(Node* root, int flag, int specifierNo) {

    handleVarDec(Child(0), flag, specifierNo);
    if (Childsum > 1) {
        handleExtDecList(Child(2), flag, specifierNo);
    }

    #ifdef HANDLE_DEBUG
    printf("handleExtDecList bingo\n");
    #endif
}

// VarDec -> ID | VarDec LB INT RB
void handleVarDec(Node *root, int flag, int specifierNo) {
    Type thistype = (Type)malloc(sizeof(Type_));
    char specifierName[128];
    if (Childsum == 4) {
        if (findSymbolTable(Child(0)->text)) {
            PrintSemErrorMsg(15, Child(0)->lineno, Child(0)->text);
        }

        if (!strcmp(Child(0, 0)->name, "ID")) {
            thistype->kind = ARRAY;

            Type elemtype = (Type)malloc(sizeof(Type_));
            elemtype->kind = BASIC;
            elemtype->u.basic = specifierNo;

            thistype->u.array.elem = elemtype;
            thistype->u.array.size = atoi(Child(2)->text);
            insertVarSymbolTable(Child(0, 0)->text, thistype);

            FieldList thisfield = (FieldList)malloc(sizeof(FieldList_));
            thisfield->type = thistype;
            strcpy(thisfield->name, Child(0, 0)->text);
            insertStructList(thisfield);
        }
        else {
            assert(!strcmp(Child(0, 0, 0)->name, "ID"));

            Type elemchildtype = (Type)malloc(sizeof(Type_));
            elemchildtype->kind = BASIC;
            elemchildtype->u.basic = specifierNo;

            Type elemtype = (Type)malloc(sizeof(Type_));
            elemtype->kind = ARRAY;
            elemtype->u.array.elem = elemchildtype;
            elemtype->u.array.size = atoi(Child(0, 2)->text);

            thistype->kind = ARRAY;
            thistype->u.array.elem = elemtype;
            thistype->u.array.size = atoi(Child(2)->text);

            FieldList thisfield = (FieldList)malloc(sizeof(FieldList_));
            thisfield->type = thistype;
            strcpy(thisfield->name, Child(0, 0, 0)->text);
            insertVarSymbolTable(Child(0, 0, 0)->text, thistype);
            insertStructList(thisfield);
        }

        return;
    }

    switch(FLAG) {

      case 2:
            if (specifierNo == 0 || specifierNo == 1) {
                thistype->kind = BASIC;
                thistype->u.basic = specifierNo;
            }
            else if (specifierNo >= 10) {
                thistype->kind = STRUCTURE;
                FieldList fieldlist = (FieldList)malloc(sizeof(FieldList_));
                strcpy(fieldlist->name, specifierStructName);
                thistype->u.structure = fieldlist;
            }

            if (PARA == 0) {
                if (findSymbolTable(Child(0)->text)) {
                    PrintSemErrorMsg(3, Child(0)->lineno, Child(0)->text);
                }
            }

            if (getStructNo(Child(0)->text)) {
                PrintSemErrorMsg(3, Child(0)->lineno, Child(0)->text);
            }

            else {
                // assert(thistype->kind != STRUCTURE);
                // if (IN_STRUCT) assert(0);
                insertVarSymbolTable(Child(0)->text, thistype);
            }
            break;

      case 1:
      case 3:

            if (findSymbolTable(Child(0)->text)) {
                PrintSemErrorMsg(15, Child(0)->lineno, Child(0)->text);
            }
            if (specifierNo == 0 || specifierNo == 1) {
                thistype->kind = BASIC;
                thistype->u.basic = specifierNo;
                if (getStructNo(Child(0)->text)) {
                    PrintSemErrorMsg(3, Child(0)->lineno, Child(0)->text);
                }
                insertVarSymbolTable(Child(0)->text, thistype);

                FieldList thisfield = (FieldList)malloc(sizeof(FieldList_));
                thisfield->type = thistype;
                strcpy(thisfield->name, Child(0)->text);
                insertStructList(thisfield);
            }
            else {
                thistype->kind = STRUCTURE;
                char specifierName[128];

                FieldList thisfield = (FieldList)malloc(sizeof(FieldList_));
                getSpecifierName(specifierNo - 10, specifierName, &thisfield);
                thisfield->type = thistype;
                strcpy(thisfield->name, specifierStructName);
                thistype->u.structure = thisfield;
                insertVarSymbolTable(Child(0)->text, thistype);
                // if (IN_STRUCT == 1) assert(0);
                // assert(0);
                // insertStructList(thisfield);

            }
            break;

// default: printf("FLAG:%d\n", flag); assert(0);
    }

    #ifdef HANDLE_DEBUG
    printf("handleVarDec bingo\n");
    #endif
}


// FunDec -> ID LP VarList RP
// | ID LP RP
void handleFunDec(Node* root, int flag, Function *function, char* functionName) {
    strcpy(functionName, Child(0)->text);
    if (Childsum == 4) {
        handleVarList(Child(2), flag, function);
        // ParamDec -> Specifier VarDec
        // Specifier -> TYPE (| StructSpecifier)
        // VarDec -> ID (| VarDec LB INT RB)
    }
    else {
        function->argsum = 0;
    }

    #ifdef HANDLE_DEBUG
    printf("handleFunDec bingo\n");
    #endif
}

// VarList -> ParamDec COMMA VarList | ParamDec
void handleVarList(Node* root, int flag, Function *function) {
    handleParamDec(Child(0), flag, function);
    if (Childsum == 3) {
        handleVarList(Child(2), flag, function);
    }

    #ifdef HANDLE_DEBUG
    printf("handleVarList bingo\n");
    #endif
}

// ParamDec -> Specifier VarDec
// Specifier -> TYPE | StructSpecifier
// VarDec -> ID (| VarDec LB INT RB)
void handleParamDec(Node* root, int flag, Function* function) {

    PARA = 1;
    if (!strcmp(Child(0, 0)->name, "TYPE")) {
        assert(!strcmp(Child(1, 0)->name, "ID"));
        int specifierNo = handleSpecifier(Child(0), flag);
        // printf("specifierNo: %d\n", specifierNo);
        Type thistype = (Type)malloc(sizeof(Type_));
        thistype->kind = BASIC;
        if (!strcmp(Child(0, 0)->text, "int")) {
            thistype->u.basic = 0;
        }
        else {
            thistype->u.basic = 1;
        }
        if (getStructNo(Child(1, 0)->text)) {
            PrintSemErrorMsg(3, Child(1, 0)->lineno, Child(1, 0)->text);
        }
        else {
            insertVarSymbolTable(Child(1, 0)->text, thistype);
        }
        function->argbasic[function->argsum++] = thistype->u.basic;

        handleVarDec(Child(1), flag, specifierNo);
    }
    else {
        int specifierNo = handleSpecifier(Child(0), flag);
        Type thistype = (Type)malloc(sizeof(Type_));
        thistype->kind = STRUCTURE;

        FieldList fieldlist = (FieldList)malloc(sizeof(FieldList_));
        strcpy(fieldlist->name, specifierStructName);
        thistype->u.structure = fieldlist;

        if (getStructNo(Child(1, 0)->text)) {
            PrintSemErrorMsg(3, Child(1, 0)->lineno, Child(1, 0)->text);
        }
        else {
            insertVarSymbolTable(Child(1, 0)->text, thistype);
            if (IN_STRUCT) assert(0);
        }
        // function->argbasic[function->argsum++] = thistype->u.basic;

        handleVarDec(Child(1), flag, specifierNo);
    }

    PARA = 0;
}


// CompSt -> LC DefList StmtList RC
void handleCompSt(Node* root, int flag, int specifierNo) {
    // FLAG = 1;
    handleDefList(Child(1), flag);
    handleStmtList(Child(2), flag, specifierNo);

    #ifdef HANDLE_DEBUG
    printf("handleCompSt bingo\n");
    #endif
}

// StmtList -> Stmt StmtList | ϵ
void handleStmtList(Node* root, int flag, int specifierNo) {
    if (root == NULL) return;
    while (root != NULL) {
        handleStmt(Child(0), flag, specifierNo);
        root = Child(1);
    }

    #ifdef HANDLE_DEBUG
    printf("handleStmtList bingo\n");
    #endif
}


// Stmt -> Exp SEMI
// | CompSt
// | RETURN Exp SEMI
// | IF LP Exp RP Stmt
// | IF LP Exp RP Stmt ELSE Stmt
// | WHILE LP Exp RP Stmt
void handleStmt(Node* root, int flag, int specifierNo) {

    if (!strcmp(Child(0)->name, "Exp")) {
        handleExp(Child(0), flag);
    }

    else if(!strcmp(Child(0)->name, "CompSt")) {
        handleCompSt(Child(0), flag, 0);
    }

    else if (!strcmp(Child(0)->name, "RETURN")) {
        int nodetype = -1;

        nodetype = handleExp(Child(1), flag);

        int funcreturntype = specifierNo;

    #ifdef PHASE_2
        printf("Defined returnType: %d; ", funcreturntype);
        printf("Actual return \"%s\" Type: %d\n", Child(1, 0)->text, nodetype);
    #endif

        if (funcreturntype != nodetype) {
            PrintSemErrorMsg(8, Child(0)->lineno, "");
        }
    }

    // | IF LP Exp RP Stmt
    // | IF LP Exp RP Stmt ELSE Stmt
    // | WHILE LP Exp RP Stmt

    else if (!strcmp(Child(0)->name, "IF") || (!strcmp(Child(0)->name, "WHILE"))) {
        handleExp(Child(2), flag);
        handleStmt(Child(4), flag, specifierNo);
        if (Childsum == 7) {
            handleStmt(Child(6), flag, specifierNo);
        }
    }
}



// DefList -> Def DefList | ϵ
void handleDefList(Node* root, int flag) {
    if (root == NULL) return;
    while (root != NULL) {
        handleDef(Child(0), flag);
        root = Child(1);
    }

    #ifdef HANDLE_DEBUG
    printf("handleDefList bingo\n");
    #endif
}

// Def -> Specifier DecList SEMI
void handleDef(Node* root, int flag) {

    int specifierNo =  handleSpecifier(Child(0), flag);
    handleDecList(Child(1), flag, specifierNo);

    #ifdef HANDLE_DEBUG
    printf("handleDef bingo\n");
    #endif
}

// DecList -> Dec | Dec COMMA DecList
// Dec, Dec, Dec, Dec ...
void handleDecList(Node* root, int flag, int specifierNo) {
    handleDec(Child(0), flag, specifierNo);
    if (Childsum == 3) {
        handleDecList(Child(2), flag, specifierNo);
    }

    #ifdef HANDLE_DEBUG
    printf("handleDecList bingo\n");
    #endif
}

// Dec -> VarDec | VarDec ASSIGNOP Exp
// VarDec -> ID (| VarDec LB INT RB)
void handleDec(Node* root, int flag, int specifierNo) {
    handleVarDec(Child(0), flag, specifierNo);
    if (Childsum == 3) {
        handleExp(Child(2), flag);
        if (IN_STRUCT) {
            PrintSemErrorMsg(15, Child(0, 0)->lineno, Child(0, 0)->text);
            Type thistype = (Type)malloc(sizeof(Type_));
            thistype->kind = BASIC;
            if (!strcmp(Child(0, 0)->text, "int")) {
                thistype->u.basic = 0;
            }
            else {
                thistype->u.basic = 1;
            }
            if (getStructNo(Child(0, 0)->text)) {
                PrintSemErrorMsg(3, Child(0, 0)->lineno, Child(0, 0)->text);
            }
            else {

                thistype->kind = STRUCTURE;
                char specifierName[128];
                FieldList thisfield = (FieldList)malloc(sizeof(FieldList_));
                getSpecifierName(specifierNo - 10, specifierName, &thisfield);
                thisfield->type = thistype;

                strcpy(thisfield->name, specifierStructName);
                thistype->u.structure = thisfield;
                insertVarSymbolTable(Child(0)->text, thistype);
                insertStructList(thisfield);
                // if (IN_STRUCT == 1) assert(0);
            }
        }
    }

    #ifdef HANDLE_DEBUG
    printf("handleDec bingo\n");
    #endif
}



// Exp ->
//   Exp ASSIGNOP Exp 1
// | Exp AND Exp 1
// | Exp OR Exp 1
// | Exp RELOP Exp 1
// | Exp PLUS Exp 1
// | Exp MINUS Exp 1
// | Exp STAR Exp 1
// | Exp DIV Exp 1
// | LP Exp RP 2
// | MINUS Exp 2
// | NOT Exp 2
// | ID LP Args RP 3
// | ID LP RP 3
// | Exp LB Exp RB 4
// | Exp DOT ID 5
// | ID 6
// | INT 6
// | FLOAT 6

int handleExp(Node *root, int flag) {
    switch (getHandleExpType(root)) {
      case 6:
      if (!strcmp(Child(0)->name, "INT")) {
          return INT;
      }
      else if (!strcmp(Child(0)->name, "FLOAT")) {
          return FLOAT;
      }
      else {
          unsigned int key = hashPJW(Child(0)->text);
          if (symboltable[key].occupied == false) {
              PrintSemErrorMsg(1, Child(0)->lineno, Child(0)->text);
              return -1;
          }
          else {
              if (symboltable[key].type->kind == BASIC) {
                  return symboltable[key].type->u.basic;
              }
              else if (symboltable[key].type->kind == STRUCTURE) {
                  return STRUCTURE * 10 + getStructNo(symboltable[key].type->u.structure->name);
              }
              else if (symboltable[key].type->kind == ARRAY) {
                  return ARRAY * 10 + symboltable[key].type->u.array.elem->u.basic;
              }
        }
    }
    break;

    case 2:
        return handleExp(Child(1), flag);
        break;

    case 4:
      if (!strcmp(Child(0)->name, "Exp")) {
          // | Exp LB Exp RB
          if (!strcmp(Child(2, 0)->name, "FLOAT")) {
              PrintSemErrorMsg(12, Child(2, 0)->lineno, Child(2, 0)->text);
              return -1;
          }

          else if (!strcmp(Child(0, 0)->name, "ID")) {
              unsigned int key = hashPJW(Child(0, 0)->text);
              assert (symboltable[key].occupied);
              if (symboltable[key].type->kind != ARRAY) {
                  PrintSemErrorMsg(10, Child(0, 0)->lineno, Child(0, 0)->text);
                  return -1;
              }
              else {
                  return symboltable[key].type->u.array.elem->u.basic;
              }
          }

          else {
              // handleExp(Child(2), flag);
              return handleExp(Child(0), flag);
          }
      }
      break;

      case 3:
      // Exp -> ID LP Args RP | | ID LP RP
      // Args -> Exp COMMA Args | Exp
      if ((getType(Child(0)->text) == 0) || (getType(Child(0)->text) == 1)) {
          PrintSemErrorMsg(11, Child(0)->lineno, Child(0)->text);
          return -1;
      }
      else if (!findSymbolTable(Child(0)->text)) {
          PrintSemErrorMsg(2, Child(0)->lineno, Child(0)->text);
          return -1;
      }
      else {
          int argsum;
          if (Childsum == 3) argsum = 0;
          else argsum = getVarSum(Child(2));
          int* pargtype = (int *)malloc(argsum * sizeof(int));
          getVarType(pargtype, argsum, Child(2));

          char funcprototype[128];
          char arguments[128];
          char funcprototype_title[128];
          char msg[128];

          getFuncPrototype(Child(0)->text, funcprototype);
          getArguments(arguments, pargtype, argsum);
          if (strcmp(funcprototype, arguments)) {
              strcpy(funcprototype_title, Child(0)->text);
              strcat(funcprototype_title, funcprototype);
              sprintf(msg, "Function \"%s\" is not applicable for arguments \"%s\"",
                  funcprototype_title, arguments);
              PrintSemErrorMsg(9, Child(0)->lineno, msg);
          }
          return getRetValue(Child(0)->text);
      }
      break;

      case 5:
      if (!isStructVar(Child(0, 0)->text)) {
          PrintSemErrorMsg(13, Child(0, 0)->lineno, "");
          return -1;
      }
      else if (!isLegalField(Child(0, 0), Child(2))) {
          PrintSemErrorMsg(14, Child(0, 0)->lineno, Child(2)->text);
          return -1;
      }
      else {
          return getType(Child(2)->text);
      }
      break;

      case 1:
        if (!strcmp(Child(1)->name, "ASSIGNOP")) {
            Node* node1 = Child(0);
            Node* node2 = Child(2);

            int node1_type = handleExp(Child(0), flag);
            // printf("node1_type: %d %s\n", node1_type, Child(0)->name);
            int node2_type = handleExp(Child(2), flag);
            // printf("node2_type: %d %s\n", node2_type, Child(2)->name);

            if (node1_type == -1 || node2_type == -1) {
                return -1;
            }
            if (!strcmp(node1->name, "INT") || !strcmp(node1->name, "FLOAT")) {
                PrintSemErrorMsg(6, node1->lineno, "");
                return -1;
            }

            if (!strcmp(node1->name, "Exp")) {
                if (getHandleExpType(Child(0))== 3) {
                    PrintSemErrorMsg(6, node1->lineno, "");
                    return -1;
                }
                Node* root = node1;
                if (!strcmp(Child(0)->name, "INT") || !strcmp(Child(0)->name, "FLOAT")) {
                    PrintSemErrorMsg(6, node1->lineno, "");
                    return -1;
                }
            }
            if (node1_type != node2_type) {
                if (node1_type >= 30 && node2_type >= 30) {
                    if (!isEqualStruct(node1_type - 30, node1_type - 30)) {
                        PrintSemErrorMsg(5, node1->lineno, "");
                        return -1;
                    }
                }
                else {
                    PrintSemErrorMsg(5, node1->lineno, "");
                    return -1;
                }
            }
            else {
                if (node1_type == STRUCTURE) {
                    char* node1_name = node1->text;
                    char* node2_name = node2->text;
                    unsigned int key1 = hashPJW(node1_name);
                    unsigned int key2 = hashPJW(node2_name);
                    if (strcmp(symboltable[key1].type->u.structure->name, symboltable[key2].type->u.structure->name)) {
                        printf("Let me see.\n");
                    }
              }
              return node1_type;
           }
        }

        else if (!strcmp(Child(1)->name, "PLUS") || !strcmp(Child(1)->name, "MINUS") ||
        !strcmp(Child(1)->name, "STAR") || !strcmp(Child(1)->name, "DIV")) {
            int node_1_type = handleExp(Child(0), flag);
            int node_2_type = handleExp(Child(2), flag);
            // printf("TYPE: %d; %d\n", node_1_type, node_2_type);

            if (node_1_type != -1 && node_1_type == node_2_type) {
                return node_1_type;
            }

            else {
                PrintSemErrorMsg(7, Child(0)->lineno, "");
                return -1;
            }
        }
        else if (!strcmp(Child(0)->name, "Exp")) {
            handleExp(Child(0), flag);
            handleExp(Child(2), flag);
        }
        else if (!strcmp(Child(1)->name , "Exp")) {
            handleExp(Child(1), flag);
        }
        break;
      }
      return -1;
}

// Args -> Exp COMMA Args
// | Exp

void handleArgs(Node* root, int flag) {
    handleExp(Child(0), flag);
    if (Childsum == 3) {
        handleArgs(Child(2), flag);
    }
}

int getType(char* text) {
    unsigned int key = hashPJW(text);
    if (symboltable[key].occupied == true) {
        if (symboltable[key].type->kind == BASIC) {
            return symboltable[key].type->u.basic;
        }
        else if (symboltable[key].type->kind == STRUCTURE) {
            return STRUCTURE * 10 + getStructNo(symboltable[key].type->u.structure->name);
        }
        else if (symboltable[key].type->kind == ARRAY) {
            return ARRAY * 10 + symboltable[key].type->u.array.elem->u.basic;
        }
    }
    return -1;
}

int getFuncReturnType(char* text) {
    unsigned int key = hashPJW(text);
    if (symboltable[key].occupied) {
        if (symboltable[key].type->kind == FUNCTION) {
            return symboltable[key].type->u.function.returnvalue;
        }
    }
    return -1;
}

// FunDec -> ID LP VarList RP
// | ID LP RP

// VarList -> ParamDec COMMA VarList
// | ParamDec

// ParamDec -> Specifier VarDec

// VarDec -> ID | VarDec LB INT RB

int getArgSum(Node* root) {
    int argsum = 0;
    if (root->childsum == 4) {
        root = Child(2);
        if (!strcmp(root->name, "VarList")) {
            ++argsum;
            while (root->childsum == 3) {
                ++argsum;
                root = Child(2);
            }
        }
    }
    return argsum;
}

// VarList -> ParamDec COMMA VarList | ParamDec
// ParamDec -> Specifier VarDec
// VarDec -> ID | VarDec LB INT RB
// Specifier -> TYPE | StructSpecifier
void getArgType(int* argType, Node* root) {
    int argno = 0;
    if (root->childsum == 4) {
        if (!strcmp(Child(2)->name, "VarList")) {
            do {
                root = Child(2);
                if (!strcmp(Child(0, 0, 0)->text, "int")) {
                    argType[argno] = 0;
                }
                else if (!strcmp(Child(0, 0, 0)->text, "float")) {
                    argType[argno] = 1;
                }
                ++argno;
            } while (Childsum == 3);
        }
    }
}

// Args -> Exp COMMA Args | Exp
int getVarSum(Node* root) {
    int argsum = 1;
    while (root->childsum == 3) {
        ++argsum;
        root = Child(2);
    }
    return argsum;
}

void getVarType(int* p, int argsum, Node* root) {
    int i = 0;
    while (root->childsum == 3) {
        p[i] = getExpType(Child(0));
        root = Child(2);
        ++i;
    }
}

// Exp ->
//   Exp ASSIGNOP Exp 1
// | Exp AND Exp 1
// | Exp OR Exp 1
// | Exp RELOP Exp 1
// | Exp PLUS Exp 1
// | Exp MINUS Exp 1
// | Exp STAR Exp 1
// | Exp DIV Exp 1
// | LP Exp RP 2
// | MINUS Exp 2
// | NOT Exp 2
// | ID LP Args RP 3
// | ID LP RP 3
// | Exp LB Exp RB 4
// | Exp DOT ID 5
// | ID 6
// | INT 6
// | FLOAT 6

int getHandleExpType(Node *root) {
    if (Childsum == 1) return 6;
    if (!strcmp(Child(0)->name, "LP") || !strcmp(Child(0)->name, "MINUS") || !strcmp(Child(0)->name, "NOT")) {
        return 2;
    }
    if (!strcmp(Child(0)->name, "ID")) {
        return 3;
    }
    if (!strcmp(Child(1)->name, "LB")) {
        return 4;
    }
    if (!strcmp(Child(1)->name, "DOT")) {
        return 5;
    }
    return 1;
}

int getExpType(Node *root) {
    if (Childsum == 1) {
        if (!strcmp(Child(0)->name, "ID")) {
            if (!findSymbolTable(Child(0)->text)) {
                PrintSemErrorMsg(1, Child(0)->lineno, Child(0)->text);
            }
            else {
                return getType(root->text);
            }
        }
        else if (!strcmp(Child(0)->name, "INT")) {
            return 0;
        }
        else if (!strcmp(Child(0)->name, "FLOAT")) {
            return 1;
        }
    }
    return -1;
}

void getFuncPrototype(char* name, char prototype[]) {
    unsigned int key = hashPJW(name);
    if (!symboltable[key].occupied) {
        return;
    }
    // printf("name: %s argsum: %d\n", name, symboltable[key].type->u.function.argsum);
    strcpy(prototype, "(");
    int i = 0;
    for (; i < symboltable[key].type->u.function.argsum; ++i) {
        if (symboltable[key].type->u.function.argbasic[i] == 0) {
            strcat(prototype, "int");
        }
        else if (symboltable[key].type->u.function.argbasic[i] == 1) {
            strcat(prototype, "float");
        }
        else {
            assert("WTF");
        }
        if (i != symboltable[key].type->u.function.argsum - 1) {
            strcat(prototype, ", ");
        }
    }
    strcat(prototype, ")");
}

void getArguments(char* arguments, int* pargtype, int argsum) {
    strcpy(arguments, "(");
    int i = 0;
    for (; i < argsum; ++i) {
        if (pargtype[i] == 0) {
            strcat(arguments, "int");
        }
        else if (pargtype[i] == 1) {
            strcat(arguments, "float");
        }
        else {
            assert("WTF");
        }
        if (i != argsum - 1) {
            strcat(arguments, ", ");
        }
    }
    strcat(arguments, ")");
}

bool isLegalField(Node* var, Node* child) {
    if (!strcmp(var->name, "ID")) {
        if (!strcmp(child->name, "ID")) {
            unsigned int key = hashPJW(var->text);
            // printf("IF %s has %s: \n", var->text, child->text);
            if (symboltable[key].occupied && symboltable[key].type->kind == STRUCTURE) {
                  int no = getStructNo(symboltable[key].type->u.structure->name);
                  // printf("NO: %d\n", no);
                  if (isInStructure(no - 10, child->text)) {
                      return true;
                  }
            }
        }
    }
    return false;
}


int getNodeType(Node* root, int flag) {
    int node_type = -1;
    if (!strcmp(root->name, "ID")) {
        unsigned int key = hashPJW(root->text);
        if (symboltable[key].occupied) {
            node_type = symboltable[key].type->kind;
            if (node_type == BASIC) {
                node_type = symboltable[key].type->u.basic;
            }
            else if (node_type == FUNCTION){
                node_type = symboltable[key].type->u.function.returnvalue;
            }
            else if (node_type == ARRAY) {
                node_type = symboltable[key].type->u.array.elem->u.basic;
            }
        }
        else {
            PrintSemErrorMsg(1, root->lineno, root->text);
            return -1;
        }
    }
    else if (!strcmp(root->name, "FLOAT")) {
        node_type = 1;
    }
    else if (!strcmp(root->name, "INT")) {
        node_type = 0;
    }
    else if (!strcmp(root->name, "Exp")) {
        node_type = handleExp(root, flag);
        printf("node_type: %d\n", node_type);
    }
    else {
        assert(0);
    }
    return node_type;
}
