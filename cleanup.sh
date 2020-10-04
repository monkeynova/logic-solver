#!/bin/sh -x

clang-format --style=Google -i `find . -name \*.h -o -name \*.cc`
buildifier `find . -name BUILD`
rm -f *~ puzzle/*~ sudoku/*~
