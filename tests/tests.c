#include "scanner.h"
#include "parser.h"

#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <stdlib.h>

#ifdef __APPLE__
#define ANSI_COLOR_RED     "\x1b[31m"
#define ANSI_COLOR_GREEN   "\x1b[32m"
#define ANSI_COLOR_RESET   "\x1b[0m"
#endif

static void UnfoldTokens(char* expression, char* out) {
    initScanner(expression);

    Token token;
    do {
        char buffer[10];
        token = scan();
        if (token.type == TOKEN_TYPE_ERROR) {
            return;
        }

        if (token.type != TOKEN_TYPE_EOF) {
            describe(&token, buffer);
            strcat(out, buffer);
            strcat(out, ", ");
        } else {
            strcat(out, "eof");
        }
    } while (token.type != TOKEN_TYPE_EOF);
}

static void AssertTokenSequence(char* expression, char* expected) {
    char out[512];
    out[0] = '\0';
    UnfoldTokens(expression, out);

    if (strcmp(expected, out) == 0) {
        printf("%-30s %sPASSED%s\n", expression, ANSI_COLOR_GREEN, ANSI_COLOR_RESET);
    } else {
        printf("%-30s %sFAILED%s\n\texpected '%s', but got '%s'.\n", expression, ANSI_COLOR_RED, ANSI_COLOR_RESET, expected, out);
        exit(-1);
    }
}

static void AssertParsedTree(char* expression, char* expected) {
    char out[512];
    out[0] = '\0';

    Expr* expr = parse(expression);
    toS(expr, out);

    freeExpr(expr);
    free(expr);

    if (strcmp(expected, out) == 0) {
        printf("%-30s %sPASSED%s\n", expression, ANSI_COLOR_GREEN, ANSI_COLOR_RESET);
    } else {
        printf("%-30s %sFAILED%s\n\texpected '%s', but got '%s'.\n", expression, ANSI_COLOR_RED, ANSI_COLOR_RESET, expected, out);
        exit(-1);
    }
}

int main() {
    printf("==== SCANNER ====\n");

    AssertTokenSequence("1", "L1, eof");
    AssertTokenSequence("-91", "-, L91, eof");
    AssertTokenSequence("1 +   2", "L1, +, L2, eof");
    AssertTokenSequence("14  / 3 ", "L14, /, L3, eof");
    AssertTokenSequence("3+15*-7", "L3, +, L15, *, -, L7, eof");
    AssertTokenSequence("a%5", "id(a), %, L5, eof");
    AssertTokenSequence("120!", "L120, !, eof");
    AssertTokenSequence("a ? b : c", "id(a), ?, id(b), :, id(c), eof");
    AssertTokenSequence("a[5]", "id(a), [, L5, ], eof");
    AssertTokenSequence("a().b(-3) + 31", "id(a), (, ), ., id(b), (, -, L3, ), +, L31, eof");

    printf("\n==== PARSER ====\n");

    AssertParsedTree("2", "2");
    AssertParsedTree("1+2+3", "(+ (+ 1 2) 3)");
    AssertParsedTree("11 + 2 - 3   * 7", "(- (+ 11 2) (* 3 7))");
    AssertParsedTree("3 * 5 - 4", "(- (* 3 5) 4)");
    AssertParsedTree("2 - 3 / 4", "(- 2 (/ 3 4))");
    AssertParsedTree("1 + 44 - 7 * 21 % 3", "(- (+ 1 44) (% (* 7 21) 3))");
    AssertParsedTree("2 + a.b.c", "(+ 2 (. a (. b c)))"); // Right associativity.
    AssertParsedTree(" 1 + 2 + f . g . h * 3 * 4", "(+ (+ 1 2) (* (* (. f (. g h)) 3) 4))");
    AssertParsedTree("-1+3*-2", "(+ (- 1) (* 3 (- 2)))");
    AssertParsedTree("--1 * 2", "(* (- (- 1)) 2)");
    AssertParsedTree("--f . g", "(- (- (. f g)))");
    AssertParsedTree("3*12 - 4!/6", "(- (* 3 12) (/ (! 4) 6))");
    AssertParsedTree("5!!!*3+3-2", "(- (+ (* (! (! (! 5))) 3) 3) 2)");
    AssertParsedTree("(1+2)*3", "(* (+ 1 2) 3)");
    AssertParsedTree("(((0)))+(3*2)", "(+ 0 (* 3 2))");
    AssertParsedTree("a[(5+3)*2]", "([ a (* (+ 5 3) 2))");
    AssertParsedTree("7*a.b[2+3]-4", "(- (* 7 ([ (. a b) (+ 2 3))) 4)");
    AssertParsedTree("a[2+b[4]-3*d[4-3]]", "([ a (- (+ 2 ([ b 4)) (* 3 ([ d (- 4 3)))))");
    AssertParsedTree("x[0][1]", "([ ([ x 0) 1)");
    AssertParsedTree("a ? 2 : 3 + 4", "(? a 2 (+ 3 4))");
    AssertParsedTree("a + 3*2 ? 2 - 1/6 : 3!-4", "(? (+ a (* 3 2)) (- 2 (/ 1 6)) (- (! 3) 4))");

    printf("\n");

    return 0;
}
