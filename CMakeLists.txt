cmake_minimum_required(VERSION 3.5)

project(pratt-parser VERSION 1.0.0 LANGUAGES C)

add_library(parser-core
    src/scanner.c
    src/parser.c
)

target_include_directories(parser-core PUBLIC include)
set_property(TARGET parser-core PROPERTY C_STANDARD 99)

add_executable(pratt-parser src/main.c)
target_link_libraries(pratt-parser PRIVATE parser-core)

add_executable(parser-tests tests/tests.c)
target_link_libraries(parser-tests PRIVATE parser-core)
