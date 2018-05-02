#include <stdio.h>
#include "parse.h"
#include "semantic.h"
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

#ifndef PRINT_DEBUG
    if (!errorNum)
#endif

    {
#ifdef PRINT_DEBUG
        printf("errorNum: %d\n", errorNum);
#endif

#ifdef PHASE_1
        printTree(root, 0);
#endif

        semanticAnalysis();

    }

    return 0;
}
