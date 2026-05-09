# Pratt Parser

A lightweight expression parser built around [the Pratt parsing technique](https://en.wikipedia.org/wiki/Operator-precedence_parser).

`pratt-parser` focuses on clear and extensible operator precedence handling while also providing a complete lexer and scanner implementation for tokenization.

## Supported Operators

Operators are listed from **lowest precedence** to **highest precedence**.

| Precedence | Operators            | Associativity |
| ---------- | -------------------- | ------------- |
| 1          | `?:` (ternary)       | Right         |
| 2          | `+`, `-`             | Left          |
| 3          | `*`, `/`, `%`        | Left          |
| 4          | Unary `+`, Unary `-` | Right         |
| 5          | `!` (factorial)      | Left          |
| 6          | `[]` (array access)  | Left          |
| 7          | `.` (member access)  | Right         |

## Example

>[!INFO]
>See [`tests.c`](./tests/tests.c) for complete tests set.

Running the tests suit should yeild something similar to the output below.

```txt
==== SCANNER ====
1                              PASSED
-91                            PASSED
1 +   2                        PASSED
14  / 3                        PASSED
3+15*-7                        PASSED
a%5                            PASSED
120!                           PASSED
a ? b : c                      PASSED
a[5]                           PASSED
a().b(-3) + 31                 PASSED

==== PARSER ====
2                              PASSED
1+2+3                          PASSED
11 + 2 - 3   * 7               PASSED
3 * 5 - 4                      PASSED
2 - 3 / 4                      PASSED
1 + 44 - 7 * 21 % 3            PASSED
2 + a.b.c                      PASSED
 1 + 2 + f . g . h * 3 * 4     PASSED
-1+3*-2                        PASSED
--1 * 2                        PASSED
--f . g                        PASSED
3*12 - 4!/6                    PASSED
5!!!*3+3-2                     PASSED
(1+2)*3                        PASSED
(((0)))+(3*2)                  PASSED
a[(5+3)*2]                     PASSED
7*a.b[2+3]-4                   PASSED
a[2+b[4]-3*d[4-3]]             PASSED
x[0][1]                        PASSED
a ? 2 : 3 + 4                  PASSED
a + 3*2 ? 2 - 1/6 : 3!-4       PASSED
```
