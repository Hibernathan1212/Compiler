# Compiler

A small compiler for a simple imperative language that targets x86-64 Linux assembly. The compiler tokenizes source, parses it into an AST, and generates assembly that can be assembled and linked into an executable.

## Features
- Lexer/tokenizer for basic keywords and symbols: `let`, `if`/`elif`/`else`, `exit`, identifiers, integer literals, `+ - * /`, parentheses `()`, braces `{}` and semicolons `;`.
- Pratt-style expression parser with operator precedence and associativity for binary expressions.
- Statement support:
  - Variable declarations with `let` and assignment.
  - Block scopes with `{ ... }` and automatic stack cleanup when leaving scope.
  - Conditional statements with `if` / `elif` / `else` chains.
  - Program termination via `exit <expr>;` which maps to Linux `sys_exit`.
- Code generation to x86-64 Linux assembly using a simple virtual stack on `rsp`.

## Project structure
- `main.cpp` — Entry point: reads a `.newton` source filename from stdin, orchestrates tokenization, parsing, and code generation, and writes `out.asm`.
- `Tokenization.hpp` — Token definitions and `Tokenizer` implementation.
- `Parser.hpp` — AST node types and `Parser` interface for terms, expressions, statements, scopes, and conditionals.
- `Generation.hpp` / `Generation.cpp` — Assembly code generator and helpers for stack/scopes/labels.
- `Arena.hpp` — Simple arena allocator used by the parser (see file for details).

## Getting started

### Prerequisites
- A C++20-capable compiler (e.g., clang++ or g++).
- An x86-64 Linux environment (or cross tools) to run the generated assembly, plus `nasm` and `ld` to assemble/link.
- Alternatively, you can generate the assembly on any platform and inspect it without running.

### Build
Compile the project with your preferred toolchain.
