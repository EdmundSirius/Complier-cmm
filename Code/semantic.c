#include "semantic.h"
#include "parse.h"
#include "ir.h"

int FLAG;
int PARA;
int IN_STRUCT;

void semanticAnalysis() {
    // initSymbolTable();
    /* unnecessary cuz struct varaibles are initialized impicitly
    the same way as objects that have static storage duration. */
    if (root == NULL) return;
    if (!strcmp(root->name, "Program")) {
        handleExtDefList(Child(0));
    }
#ifdef PRINT_TABLE
    printSymbolTable();
    printStructList();
#endif
}

// ExtDefList -> ExtDef ExtDefList | ϵ
void handleExtDefList(Node root) {
    if (Childsum == 0) return;
    // Iteration is better than recursion.
    while (root != NULL) {
        handleExtDef(Child(0));
        root = Child(1);
    }

    #ifdef HANDLE_DEBUG
    printf("handleExtDefList bingo\n");
    #endif
}

// ExtDef -> Specifier ExtDecList SEMI
// | Specifier SEMI | Specifier FunDec CompSt
void handleExtDef(Node root) {
    int specifierNo;
    if (!strcmp(Child(1)->name, "ExtDecList")) {
        FLAG = 3;
        specifierNo = handleSpecifier(Child(0));
        handleExtDecList(Child(1), specifierNo);
    }
    else if (!strcmp(Child(1)->name, "FunDec")) {
        FLAG = 2;
        specifierNo = handleSpecifier(Child(0));
        char functionName[128];
        Function thisfunction;
        thisfunction.argsum = 0;
        handleFunDec(Child(1), &thisfunction, functionName);
        thisfunction.returnvalue = specifierNo;

        if (findSymbolTable(functionName)) {
            PrintSemErrorMsg(4, Child(1)->lineno, functionName);
        }

        else {
            insertFuncSymbolTable(functionName, &thisfunction);
        }
        handleCompSt(Child(2), specifierNo);

    }
    else {
        FLAG = 1;
        specifierNo = handleSpecifier(Child(0));
    }
    FLAG = 0;

    #ifdef HANDLE_DEBUG
    printf("handleExtDef bingo\n");
    #endif
}

// Specifier -> TYPE | StructSpecifier
int handleSpecifier(Node root) {
    if (!strcmp(Child(0)->name, "StructSpecifier")) {
        return handleStructSpecifier(Child(0));
    }
    if (!strcmp(Child(0)->text, "int")) {
        return INT;
    }
    else {
        return FLOAT;
    }
}

// StructSpecifier -> STRUCT OptTag LC DefList RC | STRUCT Tag
int handleStructSpecifier(Node root) {

    if (Childsum == 5) {
        // OptTag -> ID | ϵ
        if (Child(1) != NULL) {
            assert (Child(1)->childsum == 1);
            if (getStructNo(Child(1, 0)->text)) {
                PrintSemErrorMsg(16, Child(1, 0)->lineno, Child(1, 0)->text);
            }
            else {
                if (IN_STRUCT == 1) {
                    strcpy(specifierSubStructName, Child(1, 0)->text);
                }
                else {
                    strcpy(specifierStructName, Child(1, 0)->text);
                }
                createStructNode(Child(1, 0)->text);
            }
        }
        ++IN_STRUCT;
        handleDefList(Child(3));
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
void handleExtDecList(Node root, int specifierNo) {

    handleVarDec(Child(0), specifierNo);
    if (Childsum > 1) {
        handleExtDecList(Child(2), specifierNo);
    }

    #ifdef HANDLE_DEBUG
    printf("handleExtDecList bingo\n");
    #endif
}

// VarDec -> ID | VarDec LB INT RB
void handleVarDec(Node root, int specifierNo) {
    Type thistype = (Type)malloc(sizeof(Type_));
    Type thischildtype = (Type)malloc(sizeof(Type_));
    char specifierName[128];
    if (Childsum == 4) {
        if (findSymbolTable(Child(0)->text)) {
            PrintSemErrorMsg(15, Child(0)->lineno, Child(0)->text);
        }

        if (!strcmp(Child(0, 0)->name, "ID")) {
            thistype->kind = ARRAY;

            Type elemtype = (Type)malloc(sizeof(Type_));
            if (specifierNo == 0 || specifierNo == 1) {
                elemtype->kind = BASIC;
                elemtype->u.basic = specifierNo;
            }
            else {
                elemtype->kind = STRUCTURE;
            }

            thistype->u.array.elem = elemtype;
            thistype->u.array.size = atoi(Child(2)->text);

            insertVarSymbolTable(Child(0, 0)->text, thistype);

            FieldList thisfield = (FieldList)malloc(sizeof(FieldList_));
            thisfield->type = thistype;
            strcpy(thisfield->name, Child(0, 0)->text);

            #ifdef PHASE_SEM
            printf("insert_1: %d %s, %d\n", IN_STRUCT, Child(0, 0)->text, specifierNo);
            #endif

            insertStructList(thisfield);

        }
        else if (!strcmp(Child(0, 0, 0)->name, "ID")) {
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

            #ifdef PHASE_SEM
            printf("insert_5: %d\n", IN_STRUCT);
            #endif
            insertStructList(thisfield);
        }
        else {
            assert(!strcmp(Child(0, 0, 0, 0)->name, "ID"));
            Type elemchildtype = (Type)malloc(sizeof(Type_));
            elemchildtype->kind = BASIC;
            elemchildtype->u.basic = specifierNo;

            Type elemtype = (Type)malloc(sizeof(Type_));
            elemtype->kind = ARRAY;
            elemtype->u.array.elem = elemchildtype;
            elemtype->u.array.size = atoi(Child(0, 0, 2)->text);

            thistype->kind = ARRAY;
            thistype->u.array.elem = elemtype;
            thistype->u.array.size = atoi(Child(0, 2)->text);

            thischildtype->kind = ARRAY;
            thischildtype->u.array.elem = thistype;
            thischildtype->u.array.size = atoi(Child(2)->text);

            FieldList thisfield = (FieldList)malloc(sizeof(FieldList_));
            thisfield->type = thischildtype;
            strcpy(thisfield->name, Child(0, 0, 0, 0)->text);
            insertVarSymbolTable(Child(0, 0, 0, 0)->text, thischildtype);

            #ifdef PHASE_SEM
            printf("insert_6: %d\n", IN_STRUCT);
            #endif
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
            else if (specifierNo >= 20) {
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
                // assert(0);
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

                #ifdef PHASE_SEM
                printf("insert_2: %d\n", IN_STRUCT);
                #endif

                PrintInterCodeErrorMsg();

                FieldList thisfield = (FieldList)malloc(sizeof(FieldList_));
                thisfield->type = thistype;
                strcpy(thisfield->name, Child(0)->text);


                insertStructList(thisfield);

            }
            else {
                thistype->kind = STRUCTURE;
                char specifierName[128];

                FieldList thisfield = (FieldList)malloc(sizeof(FieldList_));
                getSpecifierName(specifierNo - 20, specifierName, &thisfield);
                thisfield->type = thistype;

                if (IN_STRUCT == 1) {
                    strcpy(thisfield->name, specifierSubStructName);
                }
                else {
                    strcpy(thisfield->name, specifierStructName);
                }

                thistype->u.structure = thisfield;

                #ifdef PHASE_SEM
                printf("insert_3: %d (%s) %s\n", IN_STRUCT, specifierSubStructName, specifierStructName);
                #endif

                assert(0);
                insertVarSymbolTable(Child(0)->text, thistype);

                insertStructList(thisfield);

            }
            break;
            default: assert(0);
    }

    #ifdef HANDLE_DEBUG
    printf("handleVarDec bingo\n");
    #endif
}


// FunDec -> ID LP VarList RP
// | ID LP RP
void handleFunDec(Node root, Function *function, char* functionName) {
    strcpy(functionName, Child(0)->text);
    if (Childsum == 4) {
        handleVarList(Child(2), function);
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
void handleVarList(Node root, Function *function) {
    handleParamDec(Child(0), function);
    if (Childsum == 3) {
        handleVarList(Child(2), function);
    }

    #ifdef HANDLE_DEBUG
    printf("handleVarList bingo\n");
    #endif
}

// ParamDec -> Specifier VarDec
// Specifier -> TYPE | StructSpecifier
// VarDec -> ID (| VarDec LB INT RB)
void handleParamDec(Node root, Function* function) {

    PARA = 1;
    if (!strcmp(Child(0, 0)->name, "TYPE")) {
        if (Child(1)->childsum == 1) {
            assert(!strcmp(Child(1, 0)->name, "ID"));
            int specifierNo = handleSpecifier(Child(0));
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
            handleVarDec(Child(1), specifierNo);
        }
        else {
            assert(Child(1)->childsum == 4);
            int specifierNo = handleSpecifier(Child(0));
            Type thistype = (Type)malloc(sizeof(Type_));
            thistype->kind = ARRAY;
            // ParamDec -> Specifier VarDec
            // Specifier -> TYPE | StructSpecifier
            // VarDec -> ID (| VarDec LB INT RB)
            Type elemtype = (Type)malloc(sizeof(Type_));
            if (specifierNo == 0 || specifierNo == 1) {
                elemtype->kind = BASIC;
                elemtype->u.basic = specifierNo;
            }
            else {
                assert(0);
            }

            thistype->u.array.elem = elemtype;
            thistype->u.array.size = atoi(Child(1, 2)->text);

            if (getStructNo(Child(1, 0, 0)->text)) {
                PrintSemErrorMsg(3, Child(1, 0, 0)->lineno, Child(1, 0, 0)->text);
            }
            else {
                insertVarSymbolTable(Child(1, 0, 0)->text, thistype);
            }

            function->argbasic[function->argsum++] = thistype->u.array.elem->u.basic;
            // handleVarDec(Child(1), specifierNo);
        }
    }
    else {
        int specifierNo = handleSpecifier(Child(0));
        Type thistype = (Type)malloc(sizeof(Type_));
        thistype->kind = STRUCTURE;

        FieldList fieldlist = (FieldList)malloc(sizeof(FieldList_));
        strcpy(fieldlist->name, specifierStructName);
        thistype->u.structure = fieldlist;

        if (getStructNo(Child(1, 0)->text)) {
            PrintSemErrorMsg(3, Child(1, 0)->lineno, Child(1, 0)->text);
        }
        else {
            assert(0);
            insertVarSymbolTable(Child(1, 0)->text, thistype);
        }

        function->argbasic[function->argsum++] = specifierNo;
        handleVarDec(Child(1), specifierNo);
    }

    PARA = 0;
}


// CompSt -> LC DefList StmtList RC
void handleCompSt(Node root, int specifierNo) {
    handleDefList(Child(1));
    handleStmtList(Child(2), specifierNo);

    #ifdef HANDLE_DEBUG
    printf("handleCompSt bingo\n");
    #endif
}

// StmtList -> Stmt StmtList | ϵ
void handleStmtList(Node root, int specifierNo) {
    if (root == NULL) return;
    while (root != NULL) {
        handleStmt(Child(0), specifierNo);
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
void handleStmt(Node root, int specifierNo) {

    if (!strcmp(Child(0)->name, "Exp")) {
        handleExp(Child(0));
    }

    else if(!strcmp(Child(0)->name, "CompSt")) {
        handleCompSt(Child(0), 0);
    }

    else if (!strcmp(Child(0)->name, "RETURN")) {
        int nodetype = -1;
        nodetype = handleExp(Child(1));
        int funcreturntype = specifierNo;

    #ifdef PHASE_SEM
        printf("Defined returnType: %d; ", funcreturntype);
        printf("Actual return Type: %d\n", nodetype);
    #endif

        if (funcreturntype != nodetype) {
            PrintSemErrorMsg(8, Child(0)->lineno, "");
        }
    }

    // | IF LP Exp RP Stmt
    // | IF LP Exp RP Stmt ELSE Stmt
    // | WHILE LP Exp RP Stmt

    else if (!strcmp(Child(0)->name, "IF") || (!strcmp(Child(0)->name, "WHILE"))) {
        handleExp(Child(2));
        handleStmt(Child(4), specifierNo);
        if (Childsum == 7) {
            handleStmt(Child(6), specifierNo);
        }
    }
}



// DefList -> Def DefList | ϵ
void handleDefList(Node root) {
    if (root == NULL) return;
    while (root != NULL) {
        handleDef(Child(0));
        root = Child(1);
    }

    #ifdef HANDLE_DEBUG
    printf("handleDefList bingo\n");
    #endif
}

// Def -> Specifier DecList SEMI
void handleDef(Node root) {

    int specifierNo =  handleSpecifier(Child(0));
    handleDecList(Child(1), specifierNo);

    #ifdef HANDLE_DEBUG
    printf("handleDef bingo\n");
    #endif
}

// DecList -> Dec | Dec COMMA DecList
void handleDecList(Node root, int specifierNo) {
    handleDec(Child(0), specifierNo);
    if (Childsum == 3) {
        handleDecList(Child(2), specifierNo);
    }

    #ifdef HANDLE_DEBUG
    printf("handleDecList bingo\n");
    #endif
}

// Dec -> VarDec | VarDec ASSIGNOP Exp
// VarDec -> ID (| VarDec LB INT RB)
void handleDec(Node root, int specifierNo) {
    handleVarDec(Child(0), specifierNo);
    if (Childsum == 3) {
        handleExp(Child(2));

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
                getSpecifierName(specifierNo - 20, specifierName, &thisfield);
                thisfield->type = thistype;

                strcpy(thisfield->name, specifierStructName);
                thistype->u.structure = thisfield;
                assert(0);
                insertVarSymbolTable(Child(0)->text, thistype);
                #ifdef PHASE_SEM
                printf("insert_4: %d\n", IN_STRUCT);
                #endif
                insertStructList(thisfield);
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

int handleExp(Node root) {

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
                  return getStructNo(symboltable[key].type->u.structure->name);
              }
              else if (symboltable[key].type->kind == ARRAY) {
                  if (symboltable[key].type->u.array.elem->kind == ARRAY) {
                      return symboltable[key].type->u.array.elem->u.array.elem->kind;
                  }
                  else {
                      return symboltable[key].type->u.array.elem->kind;
                  }
              }
              else {
                  assert(0);
              }
        }
    }
    break;

    case 2:

        return handleExp(Child(1));
        break;

    case 4:

      // | Exp LB Exp RB
      if (!strcmp(Child(2, 0)->name, "FLOAT")) {
        assert(0);
          PrintSemErrorMsg(12, Child(2, 0)->lineno, Child(2, 0)->text);
          return -1;
      }

      else if (!strcmp(Child(0, 0)->name, "ID")) {
          if (!isArray(Child(0, 0)->text)) {
              PrintSemErrorMsg(10, Child(0, 0)->lineno, Child(0, 0)->text);
              return -1;
          }
          else {
              return getArrayBasicType(Child(0, 0)->text);
          }
      }

      else {
          return handleExp(Child(0));
      }

      break;

      case 3:
      // Exp -> ID LP Args RP | | ID LP RP
      // Args -> Exp COMMA Args | Exp

      if (!findSymbolTable(Child(0)->text)) {
          PrintSemErrorMsg(2, Child(0)->lineno, Child(0)->text);
          return -1;
      }
      else if (!isFunction(Child(0)->text)) {
          PrintSemErrorMsg(11, Child(0)->lineno, Child(0)->text);
          return -1;
      }
      else {
          int argsum = 0;
          if (Childsum != 3) {
              argsum = getVarSum(Child(2));
          }

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

      // | Exp DOT ID 5
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
          return getNodeType(Child(2));
      }
      break;

      case 1:

        if (!strcmp(Child(1)->name, "ASSIGNOP")) {
            Node node1 = Child(0);
            Node node2 = Child(2);

            int node1_type = handleExp(Child(0));
            int node2_type = handleExp(Child(2));

            #ifdef PHASE_SEM
            printf("node1_type: %d %s\n", node1_type, Child(0)->name);
            printf("node2_type: %d %s\n", node2_type, Child(2)->name);
            #endif

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
                Node root = node1;
                if (!strcmp(Child(0)->name, "INT") || !strcmp(Child(0)->name, "FLOAT")) {
                    PrintSemErrorMsg(6, node1->lineno, "");
                    return -1;
                }
            }
            if (node1_type != node2_type) {
                if (node1_type >= 20 && node2_type >= 20) {
                    if (!isEqualStruct(node1_type - 20, node1_type - 20)) {
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
                        assert(0);
                    }
              }
              return node1_type;
           }
        }

        else if (!strcmp(Child(1)->name, "PLUS") || !strcmp(Child(1)->name, "MINUS") ||
        !strcmp(Child(1)->name, "STAR") || !strcmp(Child(1)->name, "DIV")) {
            int node1_type = handleExp(Child(0));
            int node2_type = handleExp(Child(2));

            if (node1_type != -1 && node1_type == node2_type) {
                return node1_type;
            }
            else {
                PrintSemErrorMsg(7, Child(0)->lineno, "");
                printf("%d %d\n", node1_type, node2_type);
                return -1;
            }
        }
        else if (!strcmp(Child(0)->name, "Exp")) {
            handleExp(Child(0));
            handleExp(Child(2));
        }
        else if (!strcmp(Child(1)->name , "Exp")) {
            handleExp(Child(1));
        }
        break;
      }
      return -1;
}

// Args -> Exp COMMA Args
// | Exp

void handleArgs(Node root) {
    handleExp(Child(0));
    if (Childsum == 3) {
        handleArgs(Child(2));
    }
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

// VarList -> ParamDec COMMA VarList | ParamDec

// ParamDec -> Specifier VarDec

// VarDec -> ID | VarDec LB INT RB

int getArgSum(Node root) {
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
void getArgType(int* argType, Node root) {
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
int getVarSum(Node root) {
    int argsum = 1;
    while (root->childsum == 3) {
        ++argsum;
        root = Child(2);
    }
    return argsum;
}

void getVarType(int* p, int argsum, Node root) {
    int i = 0;
    for (; i < argsum; ++i) {
        p[i] = getNodeType(Child(0));
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

int getHandleExpType(Node root) {
    if (Childsum == 1) {
        return 6;
    }
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

void getFuncPrototype(char* name, char prototype[]) {
    unsigned int key = hashPJW(name);
    if (!symboltable[key].occupied) {
        return;
    }

    #ifdef PHASE_SEM
    printf("FUNCTION: name: %s; argsum: %d\n", name, symboltable[key].type->u.function.argsum);
    #endif

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
            // assert(symboltable[key].type->u.function.argbasic[i] >= 20);
            char name[128];
            getStructName(symboltable[key].type->u.function.argbasic[i] - 20, name);
            strcat(prototype, name);
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
            if (pargtype[i] >= 20) {
                char structName[128];
                getStructName(pargtype[i] - 20, structName);
                strcat(arguments, structName);
            }
            else {
                printf("T: %d\n", pargtype[i]);
            }

        }
        if (i != argsum - 1) {
            strcat(arguments, ", ");
        }
    }
    strcat(arguments, ")");
}

bool isLegalField(Node var, Node child) {
    if (!strcmp(var->name, "ID")) {
        if (!strcmp(child->name, "ID")) {
            unsigned int key = hashPJW(var->text);
            if (symboltable[key].occupied && symboltable[key].type->kind == STRUCTURE) {
                  int no = getStructNo(symboltable[key].type->u.structure->name);
                  if (isInStructure(no - 20, child->text)) {
                      return true;
                  }
            }
        }
    }
    return false;
}


int getNodeType(Node root) {
    int node_type = -1;
    if (!strcmp(root->name, "ID")) {
        unsigned int key = hashPJW(root->text);
        if (symboltable[key].occupied) {
            node_type = symboltable[key].type->kind;
            if (node_type == BASIC) {
                node_type = symboltable[key].type->u.basic;
            }
            else if (node_type == FUNCTION){
                node_type = FUNCTION * 10 + symboltable[key].type->u.function.returnvalue;
            }
            else if (node_type == ARRAY) {
                node_type = ARRAY * 10 + symboltable[key].type->u.array.elem->u.basic;
            }
        }
        else {
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
        node_type = handleExp(root);
    }
    else {
        assert(0);
    }
    return node_type;
}
