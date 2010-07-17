Purpose
=======

This project is my practice of the book "Modern Compiler Implementation in C".

Progress
========

Chapter 5 - Program 1: Frames

Augment ``semantic.c`` to allocate locations for local variables, and to keep
track of the nesting level. To keep things simple, assume every variable
escapes.

Implement the ``Translate`` module as ``translate.c``.

If you are compiling for the Sparc, implement the ``SparcFrame`` module
(matching frame.h) as ``frame-sparc.c``. If compiling for the MIPS, implement
MipsFrame, and so on.
