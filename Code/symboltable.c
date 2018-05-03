#include "semantic.h"

char specifierStructName[128] = "";
int structNodeNo = -1;
StructNode structlist;

void insertVarSymbolTable(char* text, Type type) {

    unsigned int key = hashPJW(text);
    // Assert NO collisions.
    if (symboltable[key].occupied) {
        return;
    }

    Type thistype = (Type)malloc(sizeof(Type_));
    thistype->kind = type->kind;
    thistype->u = type->u;
    symboltable[key].type = thistype;
    symboltable[key].occupied = true;

    strcpy(symboltable[key].name, text);

#ifdef PHASE_2
    if (symboltable[key].type->kind == BASIC) {
        printf("insert BASIC[%d %s]: kind:", key, text);
        printf("%d\n", symboltable[key].type->u.basic);
    }
    else if (symboltable[key].type->kind == ARRAY) {
        printf("insert ARRAY[%d %s]: ", key, text);
        printf("%d\n", symboltable[key].type->u.array.size);
    }
    else if (symboltable[key].type->kind == STRUCTURE) {
        printf("insert STRUCTURE[%d %s]: ", key, text);
        printf("name: %s \n", symboltable[key].type->u.structure->name);
        FieldList fieldlist = symboltable[key].type->u.structure;
        while (fieldlist->tail != NULL) {
            fieldlist = fieldlist->tail;
            printf(" %s; ", fieldlist->name);
        }

    }
    else {
        printf("WTF INSERT!!!\n");
    }
#endif

}

void insertFuncSymbolTable(char* name, Function *function) {

    unsigned int key = hashPJW(name);
    if (symboltable[key].occupied) {
        assert("WTF\n");
    }
    symboltable[key].occupied = true;

    Type thistype = (Type)malloc(sizeof(Type_));
    thistype->kind = FUNCTION;
    thistype->u.function = *function;

    symboltable[key].type = thistype;
    strcpy(symboltable[key].name, name);

#ifdef PHASE_2
    printf("insert FUNCTION[%d %s]: ", key, name);
    printf("ret[%d]; ", symboltable[key].type->u.function.returnvalue);
    printf("argsum[%d]\n", symboltable[key].type->u.function.argsum);
#endif

}

bool findSymbolTable(char* text) {
    unsigned int key = hashPJW(text);
    if (symboltable[key].occupied) {
        #ifdef HANDLE_DEBUG
        printf("%s defined.\n", text);
        #endif
        return true;
    }
    else {
        #ifdef HANDLE_DEBUG
        printf("%s not defined.\n", text);
        #endif
        return false;
    }
}

unsigned int hashPJW(char* name) {
    unsigned int val = 0, i;
    for (; *name; ++name) {
        val = (val << 2) + *name;
        i = val & ~0x3fff;
        if (i != 0)
            val = (val ^ (i >> 12)) & 0x3fff;
    }
    return val;
}

void createStructNode(char* text) {
    StructNode structnode = (StructNode)malloc(sizeof(StructNode_));
    strcpy(structnode->name, text);
    strcpy(specifierStructName, text);
    structnode->no = ++structNodeNo;
    structnode->children = NULL;
    structnode->next = NULL;

    if (structlist == NULL) {
        structlist = structnode;
    }

    else if (structlist->next == NULL) {
        structlist->next = structnode;
    }
    else {
        StructNode tmp = structlist;
        while (tmp->next != NULL) {
            tmp = tmp->next;
        }
        tmp->next = structnode;
    }
}

void insertStructList(FieldList fieldlist) {
    StructNode tmp = structlist;
    if (tmp == NULL) {
        return;
    }
    else if (tmp->no == structNodeNo) {
        FieldList tmpfield = tmp->children;
        FieldList thisfield = (FieldList)malloc(sizeof(FieldList_));
        thisfield = fieldlist;
        if (tmpfield == NULL) {
            tmp->children = thisfield;
        }
        else if (tmpfield->tail == NULL) {
            tmp->children->tail = thisfield;
        }
        else {
            while (tmpfield->tail != NULL) {
                tmpfield = tmpfield->tail;
            }
            tmpfield->tail = thisfield;
        }
    }
    else {
        tmp = tmp->next;
        FieldList thisfield = (FieldList)malloc(sizeof(FieldList_));
        thisfield = fieldlist;
        while (tmp != NULL) {
            if (tmp->no == structNodeNo) {
                FieldList tmpfield = tmp->children;
                if (tmpfield == NULL) {
                    tmp->children = thisfield;
                }
                else if (tmpfield->tail == NULL) {
                    tmpfield->tail = thisfield;
                }
                else {
                    while (tmpfield->tail != NULL) {
                        tmpfield = tmpfield->tail;
                    }
                    tmpfield->tail = thisfield;
                }
                break;
           }
           tmp = tmp->next;
        }
    }
}

void getSpecifierName(int no, char* name, FieldList *fieldlist) {
    StructNode tmp = structlist;
    if (tmp == NULL) {
        return;
    }
    else {
        while (tmp != NULL) {
            if (tmp->no == no) {
                strcpy(name, tmp->name);
                fieldlist = &tmp->children;
                return;
            }
            if (tmp->next != NULL)
                tmp = tmp->next;
            else {
                return;
            }
        }
    }
    return;
}

int getStructNo(char* name) {
      StructNode tmp = structlist;
      if (tmp == NULL) {
         return 0;
      }
      else {
          while (tmp != NULL) {
              if (!strcmp(tmp->name, name)) {
                  return (10 + tmp->no);
              }
              if (tmp->next != NULL)
                  tmp = tmp->next;
              else {
                  break;
              }
          }
      }
      return 0;
}

bool isInStructure(int no, char* name) {
    StructNode tmp = structlist;
    if (tmp == NULL) {
        return false;
    }
    else {
        while (tmp != NULL) {
            if (tmp->no == no) {
                FieldList child = tmp->children;
                if (child == NULL) {
                    return false;
                }

                while (child != NULL) {
                    if (!strcmp(child->name, name)) {
                        return true;
                    }
                    if (child->tail != NULL)
                        child = child->tail;
                    else {
                        break;
                    }
                }
            }
            if (tmp->next != NULL)
                tmp = tmp->next;
            else {
                break;
            }
        }
    }
    return false;
}

void printSymbolTable() {
    printf("------------------------------------------------\n");
    int i = 0;
    for (; i < SYMBOL_TABLE_SIZE; ++i) {
        if (symboltable[i].occupied) {
            printf("*%s\t\t%d*\n", symboltable[i].name, symboltable[i].type->kind);
        }
    }
}

void printStructChild(FieldList tmpfield) {
    if (tmpfield->type->kind == BASIC) {
        printf("[%s][%d]", tmpfield->name, tmpfield->type->u.basic);
    }
    else if (tmpfield->type->kind == ARRAY) {
        printf("[%s]:[%d] ", tmpfield->name, tmpfield->type->u.array.size);

    }
    printf("\n");
}

void printStructNode(StructNode tmp) {
    int counter = 0;
    printf("%s(%d)\n", tmp->name, tmp->no);
    FieldList tmpfield = tmp->children;
    if (tmpfield != NULL) {
        if (tmpfield->tail == NULL) {
            ++counter;
            printStructChild(tmpfield);
        }
        else {
          ++counter;
          printStructChild(tmpfield);
          do {
              tmpfield = tmpfield->tail;
              ++counter;
              printStructChild(tmpfield);
          } while (tmpfield->tail != NULL);
      }
    }
    printf("Total %d children\n", counter);
}

void printStructList() {
    printf("------------------------------------------------\n");
    StructNode tmp = structlist;
    if (tmp != NULL) {
        if (tmp->next == NULL) {
            printStructNode(tmp);
        }
        else {
            printStructNode(tmp);
            do {
                tmp = tmp->next;
                printStructNode(tmp);
            } while (tmp->next != NULL);
        }
    }
}

 bool isStructVar(char* name) {
      unsigned int key = hashPJW(name);
      if (symboltable[key].occupied && symboltable[key].type->kind == STRUCTURE) {
          return true;
      }
      return false;
 }

 bool isArray(char* name) {
      unsigned int key = hashPJW(name);
      if (symboltable[key].occupied && symboltable[key].type->kind == ARRAY) {
          return true;
      }
      return false;
 }

bool isEqualStruct(int node1, int node2) {
    FieldList fieldlist1 = NULL;
    FieldList fieldlist2 = NULL;
    int no1 = 0;
    int no2 = 0;
    int no1_type[10];
    int no2_type[10];
    int no1_subtype[10];
    int no2_subtype[10];

    StructNode tmp = structlist;
    assert (tmp != NULL);

    while (tmp != NULL) {
        if (tmp->no == node1) {
            fieldlist1 = tmp->children;
            if (fieldlist1 != NULL) {
                if (fieldlist1->tail == NULL) {
                    no1_type[no1] = fieldlist1->type->kind;
                    if (fieldlist1->type->kind == BASIC) {
                        no1_subtype[no1++] = fieldlist1->type->u.basic;
                    }
                    else if (fieldlist1->type->kind == ARRAY) {
                        no1_subtype[no1++] = fieldlist1->type->u.array.size;
                    }
                    else if (fieldlist1->type->kind == STRUCTURE) {
                        no1_subtype[no1++] = getStructNo(fieldlist1->type->u.structure->name);
                    }
                    else {
                        assert(0);
                    }
                }
                else {
                  no1_type[no1] = fieldlist1->type->kind;
                  if (fieldlist1->type->kind == BASIC) {
                      no1_subtype[no1++] = fieldlist1->type->u.basic;
                  }
                  else if (fieldlist1->type->kind == ARRAY) {
                      no1_subtype[no1++] = fieldlist1->type->u.array.size;
                  }
                  else if (fieldlist1->type->kind == STRUCTURE) {
                      no1_subtype[no1++] = getStructNo(fieldlist1->type->u.structure->name);
                  }
                  else {
                      assert(0);
                  }
                  do {
                      fieldlist1 = fieldlist1->tail;
                      no1_type[no1] = fieldlist1->type->kind;
                      if (fieldlist1->type->kind == BASIC) {
                          no1_subtype[no1++] = fieldlist1->type->u.basic;
                      }
                      else if (fieldlist1->type->kind == ARRAY) {
                          no1_subtype[no1++] = fieldlist1->type->u.array.size;
                      }
                      else if (fieldlist1->type->kind == STRUCTURE) {
                          no1_subtype[no1++] = getStructNo(fieldlist1->type->u.structure->name);
                      }
                      else {
                          assert(0);
                      }
                  } while (fieldlist1->tail != NULL);
              }
            }
            break;
        }
        if (tmp->next != NULL)
            tmp = tmp->next;
        else {
            break;
        }
    }

    tmp = structlist;
    while (tmp != NULL) {
        if (tmp->no == node2) {
            fieldlist2 = tmp->children;
            if (fieldlist2 != NULL) {
                if (fieldlist2->tail == NULL) {
                  no2_type[no2] = fieldlist2->type->kind;
                  if (fieldlist2->type->kind == BASIC) {
                      no2_subtype[no2++] = fieldlist2->type->u.basic;
                  }
                  else if (fieldlist2->type->kind == ARRAY) {
                      no2_subtype[no2++] = fieldlist2->type->u.array.size;
                  }
                  else if (fieldlist2->type->kind == STRUCTURE) {
                      no2_subtype[no2++] = getStructNo(fieldlist2->type->u.structure->name);
                  }
                  else {
                      assert(0);
                  }
                }
                else {
                  no2_type[no2] = fieldlist2->type->kind;
                  if (fieldlist2->type->kind == BASIC) {
                      no2_subtype[no2++] = fieldlist2->type->u.basic;
                  }
                  else if (fieldlist2->type->kind == ARRAY) {
                      no2_subtype[no2++] = fieldlist2->type->u.array.size;
                  }
                  else if (fieldlist2->type->kind == STRUCTURE) {
                      no2_subtype[no2++] = getStructNo(fieldlist2->type->u.structure->name);
                  }
                  else {
                      assert(0);
                  }
                  do {
                      fieldlist2 = fieldlist2->tail;
                      no2_type[no2] = fieldlist2->type->kind;
                      if (fieldlist2->type->kind == BASIC) {
                          no2_subtype[no2++] = fieldlist2->type->u.basic;
                      }
                      else if (fieldlist2->type->kind == ARRAY) {
                          no2_subtype[no2++] = fieldlist2->type->u.array.size;
                      }
                      else if (fieldlist2->type->kind == STRUCTURE) {
                          no2_subtype[no2++] = getStructNo(fieldlist2->type->u.structure->name);
                      }
                      else {
                          assert(0);
                      }
                  } while (fieldlist1->tail != NULL);
              }
            }
            break;
        }
        if (tmp->next != NULL)
            tmp = tmp->next;
        else {
            break;
        }
    }


    assert(fieldlist1 != NULL);
    assert(fieldlist2 != NULL);

    printf("%s %s %d %d\n", fieldlist1->name, fieldlist2->name, no1, no2);
    int i = 0;
    for (; i < no1; ++i) printf("%d(%d)", no1_type[i], no1_subtype[i]);
    printf("\n");
    i = 0;
    for (; i < no2; ++i) printf("%d(%d)", no2_type[i], no2_subtype[i]);
    printf("\n");
    return false;
}

 int getRetValue(char* name) {
      unsigned int key = hashPJW(name);
      assert(symboltable[key].occupied);
      return symboltable[key].type->u.function.returnvalue;
 }
