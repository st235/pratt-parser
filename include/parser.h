#ifndef PRATT_PARSER_PARSER_H_
#define PRATT_PARSER_PARSER_H_

#include <stdint.h>

#include "scanner.h"

typedef struct Expr Expr;

struct Expr {
    Token token;
    uint8_t size;
    uint8_t capacity;
    Expr* exprs;
};

void initExpr(Expr* expr, Token token);
void addSubexpr(Expr* expr, Expr* subExpr);
void freeExpr(Expr* expr);

void toS(Expr* expr, char* out);

Expr* parse(char* expression);

#endif  // PRATT_PARSER_PARSER_H_
