# 优化处理
公共子表达式消除 (Common-subexpression Elimination)
代码移动 (Code Motion）
代数运算简化 (Algebraic Simplification)
常量折叠

# 数组的翻译模式
temp: v1
temp[5]

temp[0]
```
t1 := v1
t2 := *t1
```
temp[1]
```
t1 := v1 + #4
t2 = *t1

t1 := #1
t2 := t1 * #4
t3 := &v1 + t2

&v1 + 4 == *(v1 + 4)
```
temp[3]
```
t1 := #3
t2 := t1 * #4
t3 := &v1 + t2
```

temp[i]
```
t1 := vi
t2 := t1 * #4
t2 = &v1 + t1
```

r: v1
i: v2
j: v3
r[0][i]
```
t1 := v2 * #4
t2 := &v1 + t1
```
r[i][j]
```
t1 := #2

t2 := v2 * t1
t3 := t2 * #4
t4 := &v1 + t2

t5 := v3 * #4
t6 := t4 + t5

&v1 + i * 2 * #4 + j * #4
```

[7][8]
int array[i][j]
array: v1
i: v2
j: v3
k: v4
array + i * 8 * 4 + j * 4
```
t2 := #8

t4 := v2 * t2
t6 := t4 * #4
t10 := &v1 + t6

t7 := v3 * #4
t11 := t10 + t7

```

[7][8][9]
int array[i][j][k]
array: v1
i: v2
j: v3
k: v4
array + i * 8 * 9 * 4 + j * 9 * 4 + k * 4
```
t2 := #8
t3 := #9

t4 := v2 * t2
t5 := t4 * t3
t6 := t5 * #4
t10 := &v1 + t6

t7 := v3 * t3
t8 := t7 * #4
t11 := t10 + t8

t9 := v4 * #4
t12 := t11 + t9
```

[7][8][9][10]
int array[i][j][k][s]
array: v1
i: v2
j: v3
k: v4
s: v5
array + i * 8 * 9 * 10 * 4 + j * 9 * 10 * 4 + k * 10 * 4 + s * 4
```
t2 := #8
t3 := #9
t4 := #10

t5 := v2 * t2
t6 := t5 * t3
t7 := t6 * t4
t8 := t7 * #4
t15 := &v1 + t8

t9 := v3 * t3
t10 := t9 * t4
t11 := t10 * #4
t16 := t15 + t11

t12 := v4 * t4
t13 := t12 * #4
t17 := t16 + t13

t14 := v5 * #4
t18 := t17 + t14
```
```
void printDagOperand(Operand_ operand) {
    if (operand.kind == VARIABLE) {
        printf("t");
        printf("%d", operand.u.no);
    }
    else {
        assert(operand.kind == OPERATOR);
        switch (operand.u.no) {
          case SYM_ADD: printf("+"); break;
          case SYM_SUB: printf("-"); break;
          case SYM_MUL: printf("*"); break;
          case SYM_DIV: printf("/"); break;
          default: assert(0);
        }
    }
}

void printDagNodeNeighbor(DagNode *root) {
  printf(": ");
  if (root != NULL) {
      DagNode *p = root;
      while (p->neighbor != NULL) {
        printDagOperand(p->neighbor->operand);
        p = p->neighbor;
      }
  }
  printf("\n");
}

void printDag(DagNode *root) {
    if (root != NULL) {
        DagNode *p = root;
        printDagOperand(p->operand);
        printDagNodeNeighbor(p);
        while (p->next != NULL) {
            p = p->next;
            printDagOperand(p->operand);
            printDagNodeNeighbor(p);
        }
    }
    else {
        assert(0);
    }
}

bool isRelatedNode(DagNode *node, Operand operand) {
    assert(node->kind = OPERATOR);
    if (node->related != NULL) {
        DagNode* p = node->related;
        do {
            if (p->operand.kind == operand->kind || p->operand.u.no == operand->u.no) {
                return true;
            }
            if (p->next != NULL) {
                p = p->next;
            }
            else {
                break;
            }
        } while (p != NULL);
    }
    return false;
}

void insertDag(InterCode intercode, int i) {

    DagNode *dagNode_op = (DagNode *)malloc(sizeof(DagNode));
    assert (intercode.kind == IR_ADD || intercode.kind == IR_SUB ||
      intercode.kind == IR_MUL || intercode.kind == IR_DIV);

    DagNode *dagNode_y = (DagNode *)malloc(sizeof(DagNode));
    assert(intercode.triop.y->kind == OP_TEMPORARY);
    dagNode_y->operand.kind = intercode.triop.y->kind;
    dagNode_y->operand.u.no = intercode.triop.y->u.no;

    DagNode *dagNode_z = (DagNode *)malloc(sizeof(DagNode));
    dagNode_z->operand.kind = intercode.triop.z->kind;
    dagNode_z->operand.u.no = intercode.triop.z->u.no;

    DagNode *dagNode_x = (DagNode *)malloc(sizeof(DagNode));
    dagNode_x->operand.kind = intercode.triop.x->kind;
    dagNode_x->operand.u.no = intercode.triop.x->u.no;

    dagNode_op->operand.kind = OPERATOR;
    dagNode_op->operand.u.no = OP(intercode.kind);
    dagNode_op->related = dagNode_x;

    dagNode_y->neighbor = dagNode_op;
    dagNode_z->neighbor = dagNode_op;

    dagNode_y->next = dagNode_z;
    dagNode_z->next = dagNode_op;

    if (rootDagNode[i] == NULL) {
        rootDagNode[i] = dagNode_y;
        printf("(%d)\n", dagNode_y->operand.u.no);
    }

    if (rootDagNode[i]->next == NULL) {
        rootDagNode[i] = dagNode_y;
        printf("<%d>\n", dagNode_y->operand.u.no);
    }
    else {
        DagNode *p = rootDagNode[i];
        while (p->next != NULL) {
            p = p->next;
        }
        p->next = dagNode_y;
        printf("[%d]\n", dagNode_y->operand.u.no);
    }



}
```
