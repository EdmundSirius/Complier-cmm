#ifndef PARSE_H
#define PARSE_H

#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <string.h>

#define MAX_CHILD_NUM 7

// #define LEX_ANALYSE
// #define PRINT_TRACE
// #define PHASE_SEM
// #define PRINT_TABLE
// #define TRANSLATE
#define PRINT_IR
// #define PRINT_GENERATE
// #define OUTPUT_ORI_IR

#ifdef LEX_ANALYSE

#define CreateNode(node_name) \
yylval.node = createNode(node_name, yytext); printf("%s\n", node_name)

#else

#define CreateNode(node_name) \
yylval.node = createNode(node_name, yytext)

#endif

#define errorMsg(hint) \
char errormsg[128];  \
sprintf(errormsg, "%s \"%s\"", hint, yytext)

#define PrintErrorMsg(type, msg) \
printf("Error type \033[;31m%s\033[0m at Line %d: \033[;31m%s\033[0m.\n", type, yylineno, msg)

typedef struct Node_* Node;

typedef struct Node_ {
    char name[32];
    char text[32];
    int lineno;
    int childsum;
    Node child[MAX_CHILD_NUM];
} Node_;

extern Node root;

extern int yylineno;

int errorNum;

Node createNode(char*, char*);

void insertNode(int, Node, ...);

void printNode(char*, char*);

void printTree(Node, int);

char* oct2Dec(char *);

char* hex2Dec(char *);

#endif
