#ifndef PRATT_PARSER_SCANNER_H_
#define PRATT_PARSER_SCANNER_H_

#include <stdint.h>

typedef enum {
    TOKEN_TYPE_PLUS,
    TOKEN_TYPE_MINUS,
    TOKEN_TYPE_SLASH,
    TOKEN_TYPE_STAR,
    TOKEN_TYPE_BANG,
    TOKEN_TYPE_QUESTION,
    TOKEN_TYPE_DOT,
    TOKEN_TYPE_PERCENT,
    TOKEN_TYPE_OPEN_SQUARE,
    TOKEN_TYPE_CLOSE_SQUARE,
    TOKEN_TYPE_OPEN_PAREN,
    TOKEN_TYPE_CLOSE_PAREN,
    TOKEN_TYPE_SEMICOLON,

    TOKEN_TYPE_LITERAL,
    TOKEN_TYPE_IDENTIFIER,

    TOKEN_TYPE_EOF,

    TOKEN_TYPE_ERROR,
} TokenType;

typedef struct {
    TokenType type;
    char* start;
    uint8_t length;
} Token;

void describe(Token* token, char* buffer);

typedef struct {
    char* origin;
    char* current;
} Scanner;

void initScanner(char* expression);
Token scan();

#endif  // PRATT_PARSER_SCANNER_H_
