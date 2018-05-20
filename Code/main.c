#include <stdio.h>
#include "parse.h"
#include "semantic.h"
#include "translate.h"
extern void yyparse();
extern void yyrestart(FILE*);

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

        preInterCodeGenerate();
        semanticAnalysis();
        fp = fopen("out.ir", "w");
        if(fp == NULL) {
            assert(0);
        }
        interCodeGenerate();
        fclose(fp);
    }

    return 0;
}
