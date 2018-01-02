#!/bin/sh -x

perl -spi -e 's/\t/" "x8/ge' {puzzle/,sudoku/,}*.{cc,h}
perl -spi -e 's/\s+\n/\n/' {puzzle/,sudoku/,}*.{cc,h}
rm *~ puzzle/*~ sudoku/*~
