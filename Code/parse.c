#include "parse.h"

Node createNode(char* name, char* text) {

    Node p = (Node)malloc(sizeof(Node_));

    strcpy(p->name, name);
    strcpy(p->text, text);
    p->lineno = yylineno;
    int i = 0;
	  for (; i < MAX_CHILD_NUM; ++i)
		    p->child[i] = NULL;

	  p->childsum = 0;

    return p;
}

void insertNode(int childsum, Node parent, ...) {

    va_list ap;
    va_start(ap, parent);

    int i = 0;
    for (; i < childsum; ++i) {
        parent->child[i] = va_arg(ap, Node);
    }

    parent->lineno = parent->child[0]->lineno;
    parent->childsum = childsum;

    va_end(ap);

}

void printNode(char* name, char* text) {
    if (!strcmp(name, "ID") || !strcmp(name, "TYPE"))
        printf("%s: %s\n", name, text);
    else if (!strcmp(name, "INT"))
        printf("%s: %d\n", name, atoi(text));
    else if (!strcmp(name, "FLOAT"))
        printf("%s %f\n", name, atof(text));
    else
        printf("%s\n", name);
}


void printTree(Node parent, int blank) {
    if (parent == NULL) return;
    int i = 0;
    for(; i < blank; ++i) printf(" ");

    /* Recursion */
    if (parent->childsum != 0) {
        printf("%s (%d)\n", parent->name, parent->lineno);
        int i = 0;
        for (; i < parent->childsum; ++i)
            printTree(parent->child[i], blank + 2);
        return;
    }

    printNode(parent->name, parent->text);

}

char* oct2Dec(char *text) {
    int tmp = strtol(text, NULL, 8);
    sprintf(text, "%d", tmp);
    return text;
}

char* hex2Dec(char *text) {
    int tmp = strtol(text, NULL, 16);
    sprintf(text, "%d", tmp);
    return text;
}
