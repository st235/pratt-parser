#include "scanner.h"

#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>

Scanner scanner;

static char peek() {
    return *scanner.current;
}

static void advance() {
    scanner.current += 1;
}

static bool isAtEnd() {
    return peek() == '\0';
}

static bool isWhitespace() {
    char c = peek();
    return c == ' ' || c == '\t' || c == '\n';
}

static void skipWhitespace() {
    while (isWhitespace()) {
        advance();
    }
}

static bool isDigit() {
    char c = peek();
    return c >= '0' && c <= '9';
}

static bool isAlpha() {
    char c = peek();
    return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z');
}

static Token makeToken(TokenType type, char* begin, uint8_t length) {
    Token token;
    token.type = type;
    token.start = begin;
    token.length = length;
    return token;
}

static Token makeTokenAtPrevious(TokenType type) {
    return makeToken(type, scanner.current - 1, 1);
}

static Token makeTokenAtCurrent(TokenType type) {
    return makeToken(type, scanner.current, 1);
}

void describe(Token* token, char* buffer) {
    switch (token->type) {
        case TOKEN_TYPE_PLUS: sprintf(buffer, "+"); break;
        case TOKEN_TYPE_MINUS: sprintf(buffer, "-"); break;
        case TOKEN_TYPE_SLASH: sprintf(buffer, "/"); break;
        case TOKEN_TYPE_STAR: sprintf(buffer, "*"); break;
        case TOKEN_TYPE_BANG: sprintf(buffer, "!"); break;
        case TOKEN_TYPE_QUESTION: sprintf(buffer, "?"); break;
        case TOKEN_TYPE_DOT: sprintf(buffer, "."); break;
        case TOKEN_TYPE_PERCENT: sprintf(buffer, "%%"); break;
        case TOKEN_TYPE_OPEN_SQUARE: sprintf(buffer, "["); break;
        case TOKEN_TYPE_CLOSE_SQUARE: sprintf(buffer, "]"); break;
        case TOKEN_TYPE_OPEN_PAREN: sprintf(buffer, "("); break;
        case TOKEN_TYPE_CLOSE_PAREN: sprintf(buffer, ")"); break;
        case TOKEN_TYPE_LITERAL: sprintf(buffer, "L%.*s", token->length, token->start); break;
        case TOKEN_TYPE_IDENTIFIER: sprintf(buffer, "id(%.*s)", token->length, token->start); break;
        case TOKEN_TYPE_SEMICOLON: sprintf(buffer, ":"); break;
        case TOKEN_TYPE_EOF: sprintf(buffer, "eof"); break;
        case TOKEN_TYPE_ERROR: sprintf(buffer, "error"); break;
    }
}

void initScanner(char* expression) {
    scanner.origin = expression;
    scanner.current = expression;
}

Token scan() {
    skipWhitespace();

    if (isAtEnd()) {
        return makeTokenAtCurrent(TOKEN_TYPE_EOF);
    }

    switch (peek()) {
        case '+':
            advance();
            return makeTokenAtPrevious(TOKEN_TYPE_PLUS);
        case '-':
            advance();
            return makeTokenAtPrevious(TOKEN_TYPE_MINUS);
        case '*':
            advance();
            return makeTokenAtPrevious(TOKEN_TYPE_STAR);
        case '/':
            advance();
            return makeTokenAtPrevious(TOKEN_TYPE_SLASH);
        case '.':
            advance();
            return makeTokenAtPrevious(TOKEN_TYPE_DOT);
        case '%':
            advance();
            return makeTokenAtPrevious(TOKEN_TYPE_PERCENT);
        case '?':
            advance();
            return makeTokenAtPrevious(TOKEN_TYPE_QUESTION);
        case '[':
            advance();
            return makeTokenAtPrevious(TOKEN_TYPE_OPEN_SQUARE);
        case ']':
            advance();
            return makeTokenAtPrevious(TOKEN_TYPE_CLOSE_SQUARE);
        case '(':
            advance();
            return makeTokenAtPrevious(TOKEN_TYPE_OPEN_PAREN);
        case ')':
            advance();
            return makeTokenAtPrevious(TOKEN_TYPE_CLOSE_PAREN);
        case ':':
            advance();
            return makeTokenAtPrevious(TOKEN_TYPE_SEMICOLON);
        case '!':
            advance();
            return makeTokenAtPrevious(TOKEN_TYPE_BANG);
    }

    if (isDigit()) {
        char* start = scanner.current;
        while(isDigit()) {
            advance();
        }
        uint8_t length = (uint8_t)(scanner.current - start);
        return makeToken(TOKEN_TYPE_LITERAL, start, length);
    }

    if (isAlpha()) {
        char* start = scanner.current;
        while(isAlpha() || isDigit()) {
            advance();
        }
        uint8_t length = (uint8_t)(scanner.current - start);
        return makeToken(TOKEN_TYPE_IDENTIFIER, start, length);
    }

    return makeTokenAtCurrent(TOKEN_TYPE_ERROR);
}
