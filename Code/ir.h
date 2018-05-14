#ifndef IR_H
#define IR_H

#define MAX_LINE 1024

typedef struct Operand_* Operand;
typedef struct InterCodes* InterCode;

typedef struct Operand_ {
    enum { OP_VARIABLE, OP_TEMPORARY, OP_CONSTANT, OP_ADDRESS, OP_LABEL, OP_FUNCTION } kind;
    union {
        int no;
        char value[128];
    } u;
} Operand_;

typedef enum { RELGT, RELLT, EQUAL } Relop;

typedef struct InterCode_ {
    enum {
      IR_ADD, IR_SUB, IR_MUL, IR_DIV, IR_IFGOTO,
      IR_LABEL, IR_FUNC, IR_GOTO, IR_RET, IR_DEC, IR_ARG, IR_PARAM, IR_READ, IR_WRITE,
      IR_ASSIGN, IR_GETADD, IR_GETVAL, IR_SETVAL, IR_CALL
    } kind;

    union {
        struct { Operand x; } op;
        struct { Operand x, y; } biop;
        struct { Operand x, y, z; } triop;
        struct { Operand x, y, z; Relop relop; } ifop;
    };

} InterCode_;


typedef struct InterCodes {
    struct InterCode_ code;
    struct InterCodes *prev, *next;
} InterCodes;

static inline void INIT_LIST_HEAD(struct InterCodes *list) {
    list->next = list;
    list->prev = list;
}

static inline void list_insert(struct InterCodes *newhead, struct InterCodes *prev, struct InterCodes *next) {
    next->prev = newhead;
    newhead->next = next;
    newhead->prev = prev;
    prev->next = newhead;
}

static inline void list_add(struct InterCodes *newhead, struct InterCodes *head) {
    list_insert(newhead, head, head->next);
}

static inline void list_add_tail(struct InterCodes *newhead, struct InterCodes *head) {
    list_insert(newhead, head->prev, head);
}

static inline void list_replace(struct InterCodes *old,struct InterCodes *newhead) {
    newhead->next = old->next;
    newhead->next->prev = newhead;
    newhead->prev = old->prev;
    newhead->prev->next = newhead;
}

static inline int list_empty(const struct InterCodes *head) {
    return head->next == head;
}

/*8.遍历节点
list_for_each(pos, head)通常用于获取节点，而不能用到删除节点的场景。
*/

#define list_for_each(pos, head) \
for (pos = (head)->next; pos != (head); pos = pos->next)

struct InterCodes interCodes_head;

int no_code;

#endif
