#include <stdio.h>
#include "parse.h"
#include "semantic.h"
#include "translate.h"
#include "ir.h"
#include "generate.h"

extern void yyparse();
extern void yyrestart(FILE*);
char outputFile[128];

int main(int argc, char** argv) {
    if (argc <= 1)
        return 1;
    FILE* f = fopen(argv[1], "r");
    if (!f) {
        perror(argv[1]);
        return 1;
    }
    yyrestart(f);
    yyparse();

#ifndef PRINT_TRACE
    if (!errorNum)
#endif

    {
#ifdef PRINT_TRACE
        printf("errorNum: %d\n", errorNum);
#endif

#ifdef PRINT_TRACE
        printTree(root, 0);
#endif
        INIT_LIST_HEAD(&head);
        preInterCodeGenerate();
        semanticAnalysis();
        fp = fopen(argv[2], "w");
        if(fp == NULL) {
            fp = fopen("out.s", "w");
            strcpy(outputFile, "out.s");
        }
        else {
            strcpy(outputFile, argv[2]);
        }
        interCodeGenerate();
        interCodeOptimize();
        codeGenerate();
        fclose(fp);
    }

    return 0;
}
