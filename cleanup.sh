#!/bin/sh -x

clang-format --style=Google -i `find . -name \*.h -o -name \*.cc`
rm -f *~ puzzle/*~ sudoku/*~
