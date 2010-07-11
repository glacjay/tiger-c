Purpose
=======

This project is my practice of the book "Modern Compiler Implementation in C".

Progress
========

Chapter 5 - Program 1: Type Checking

Write a type-checking phase for your compiler, a module ``semantic.c`` matching
the following header file::

    /* semantic.h */
    void sem_trans_prog(ast_expr_t expr);

that type-checks an abstract syntax tree and produces any appropriate error
messages about mismatching types or undeclared identifiers.

Also provider the implementation of the env module described in this chapter.
Make a module main that calls the parser, yielding an ``ast_expr_t``, and then
calls ``sem_trans_prog`` on this expression.
