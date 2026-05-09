#include "parser.h"

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    Token current;
    Token next;
} Parser;

Parser parser;

static void initParser(char* expression) {
    initScanner(expression);
    parser.next = scan();
}

static Token peekParser() {
    return parser.next;
}

static Token advanceParser() {
    parser.current = parser.next;
    parser.next = scan();
    return parser.current;
}

void initExpr(Expr* expr, Token token) {
    expr->token = token;
    expr->size = 0;
    expr->capacity = 10;
    expr->exprs = (Expr*) malloc(sizeof(Expr*) * expr->capacity);
}

void addSubexpr(Expr* expr, Expr* subExpr) {
    if (expr->size + 1 > expr->capacity) {
        expr->capacity *= 2;
        expr->exprs = (Expr*) realloc(expr->exprs, sizeof(Expr) * expr->capacity);
    }

    expr->exprs[expr->size] = *subExpr;
    expr->size += 1;
}

void freeExpr(Expr* expr) {
    if (expr == NULL) {
        return;
    }

    for (int i = 0; i < expr->size; i++) {
        freeExpr(&expr->exprs[i]);
    }

    expr->size = 0;
    free(expr->exprs);
}

void toS(Expr* expr, char* out) {
    char name[10];
    sprintf(name, "%.*s", expr->token.length, expr->token.start);

    if (expr->size == 0) {
        strcat(out, name);
        return;
    }

    strcat(out, "(");
    strcat(out, name);

    for (int i = 0; i < expr->size; i++) {
        strcat(out, " ");
        toS(&expr->exprs[i], out);
    }

    strcat(out, ")");
}

static bool isAtom(Token* token) {
    return token->type == TOKEN_TYPE_LITERAL || token->type == TOKEN_TYPE_IDENTIFIER;
}

static bool isUnaryPrefixOperator(Token* token) {
    return token->type == TOKEN_TYPE_PLUS ||
        token->type == TOKEN_TYPE_MINUS;
}

static bool isUnaryPostfixOperator(Token* token) {
    return token->type == TOKEN_TYPE_BANG ||
           token->type == TOKEN_TYPE_OPEN_SQUARE;
}

static bool isBinaryOp(Token* token) {
    return token->type == TOKEN_TYPE_PLUS ||
        token->type == TOKEN_TYPE_MINUS ||
        token->type == TOKEN_TYPE_STAR ||
        token->type == TOKEN_TYPE_SLASH ||
        token->type == TOKEN_TYPE_PERCENT ||
        token->type == TOKEN_TYPE_DOT ||
        token->type == TOKEN_TYPE_QUESTION;
}

typedef struct {
    int8_t left;
    int8_t right;
} BindingPower;

// Should be synced with binary ops.
static BindingPower getUnaryBindingPower(Token* token) {
    switch (token->type) {
        case TOKEN_TYPE_PLUS:
        case TOKEN_TYPE_MINUS:
            return (BindingPower){ 0, 8 };
        case TOKEN_TYPE_BANG:
            return (BindingPower){ 8, 0 };
        case TOKEN_TYPE_OPEN_SQUARE:
            return (BindingPower){ 10, 0 };
        default:
            return (BindingPower){ -1, -1 };
    }
}

static BindingPower getBinaryBindingPower(Token* token) {
    switch (token->type) {
        case TOKEN_TYPE_QUESTION:
            return (BindingPower){ 2, 1 };
        case TOKEN_TYPE_PLUS:
        case TOKEN_TYPE_MINUS:
            return (BindingPower){ 3, 4 };
        case TOKEN_TYPE_STAR:
        case TOKEN_TYPE_SLASH:
        case TOKEN_TYPE_PERCENT:
            return (BindingPower){ 5, 6 };
        case TOKEN_TYPE_DOT:
            return (BindingPower){ 12, 11 };
            default:
            return (BindingPower){ -1, -1 };
    }
}

static Expr* expr_bp(uint8_t power) {
    Token current = advanceParser();
    if (!isAtom(&current) && !isUnaryPrefixOperator(&current) &&
            current.type != TOKEN_TYPE_OPEN_PAREN) {
        fprintf(stderr, "Unexpected token '%.*s'.\n", current.length, current.start);
        exit(-1);
    }

    Expr* lhs = NULL;

    if (current.type == TOKEN_TYPE_OPEN_PAREN) {
        lhs = expr_bp(0);
        current = advanceParser();
        if (current.type != TOKEN_TYPE_CLOSE_PAREN) {
            fprintf(stderr, "Unbalanced '(' bracket found.\n");
            exit(-1);
            return NULL;
        }
    } else {
        lhs = (Expr*) malloc(sizeof(Expr));
        initExpr(lhs, current);
    }

    if (isUnaryPrefixOperator(&current)) {
        BindingPower bp = getUnaryBindingPower(&current);
        Expr* rhs = expr_bp(bp.right);

        Expr* composition = (Expr*) malloc(sizeof(Expr));
        initExpr(composition, current);
        addSubexpr(composition, rhs);
        lhs = composition;
    }

    current = peekParser();
    while (current.type != TOKEN_TYPE_EOF) {
        if (!isBinaryOp(&current) && !isUnaryPostfixOperator(&current) &&
            current.type != TOKEN_TYPE_CLOSE_PAREN &&
            current.type != TOKEN_TYPE_CLOSE_SQUARE &&
            current.type != TOKEN_TYPE_SEMICOLON) {
            fprintf(stderr,
                "Found '%.*s' which is neither binary or postfix unary op.\n",
                current.length, current.start);
            exit(-1);
            return NULL;
        }

        if (isUnaryPostfixOperator(&current)) {
            bool isOpenSquare = current.type == TOKEN_TYPE_OPEN_SQUARE;

            BindingPower bp = getUnaryBindingPower(&current);
            if (bp.left < power) {
                break;
            }

            advanceParser();
            Expr* composition = (Expr*) malloc(sizeof(Expr));
            initExpr(composition, current);
            addSubexpr(composition, lhs);

            if (isOpenSquare) {
                Expr* inner = expr_bp(0);
                addSubexpr(composition, inner);

                current = advanceParser();
                if (current.type != TOKEN_TYPE_CLOSE_SQUARE) {
                    fprintf(stderr, "Unbalanced '[' bracket found.\n");
                    exit(-1);
                    return NULL;
                }
            }

            lhs = composition;

            current = peekParser();
            continue;
        }

        BindingPower bp = getBinaryBindingPower(&current);
        if (bp.left == -1 && bp.right == -1) {
            break;
        }

        if (bp.left < power) {
            break;
        }

        // Consuming operator.
        advanceParser();
        bool isTernary = current.type == TOKEN_TYPE_QUESTION;

        Expr* composition = (Expr*) malloc(sizeof(Expr));
        initExpr(composition, current);
        addSubexpr(composition, lhs);

        if (isTernary) {
            Expr* condition = expr_bp(0);
            addSubexpr(composition, condition);

            current = advanceParser();
            if (current.type != TOKEN_TYPE_SEMICOLON) {
                fprintf(stderr, "Uncomplete ternary operator.\n");
                exit(-1);
                return NULL;
            }
        }


        Expr* rhs = expr_bp(bp.right);
        addSubexpr(composition, rhs);

        lhs = composition;
        current = peekParser();
    }

    return lhs;
}

Expr* parse(char* expression) {
    initParser(expression);
    return expr_bp(0);
}
