#include "parser.h"

#include <stdio.h>
#include <stdlib.h>

int main(int argc, char* argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Expected usage: pratt-parser \"expression\"");
        exit(65);
        return 0;
    }

    Expr* expr = parse(argv[1]);

    char buffer[512];
    buffer[0] = '\0';
    toS(expr, buffer);
    freeExpr(expr);
    free(expr);

    printf("%s\n", buffer);

    return 0;
}
