#!/bin/sh -x

perl -spi -e 's/\t/" "x8/ge' {puzzle/,}*.{cc,h}
perl -spi -e 's/\s+\n/\n/' {puzzle/,}*.{cc,h}
rm *~ puzzle/*~
