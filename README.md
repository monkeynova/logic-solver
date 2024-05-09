# Logic Puzzle Solver

An attempt to work through a logic puzzle solver by being able
to translate logic puzzle statements into predicates and then
using them to test the problem space.

## Citations

Puzzles in this repo are included from (among other locations):

* http://io9.com/can-you-solve-this-extremely-difficult-star-trek-puzzle-1667991339
* https://www.brainzilla.com/logic/logic-grid/swimming-pool/ (More at: https://www.brainzilla.com/logic/logic-grid/)

## Structure

This repository is split into a few pieces.

 * At the top-level are a few logic puzzles that have been converted from
   natural English to lambda expression that can then be solved
 * puzzle/... contains all the library code to iterate and find a solution to a
   logic puzzle
 * sudoku/... maps sudoku problems into a logic puzzle like lambda space
 * ken_ken/... does the same as sudoku, but for 'KenKen' problems

## Purpose

I created this project as a place to learn. The lessons have come in phases
and have been more varied and enudringly useful than I thought they would be.

### C++ Lambdas

My original goal was to gain experience and familiarity with these new-fangled
C++ lambdas. The idea of transformaing a logic puzzle into a series of lambda
expressions and then passing that to a library for solving felt like a great
opportunity to play with the basics, understand the syntax, and start
developing intuition for their behavior.

### API Performance

After a model for describing the problems and handing it to a solving library
came the need to see what I could do for performance. A single lambda on
"is this solution right" acts as a challenging black box, and finding the
right way to expose deeper information about the lambda, without exposing its
entire workings became a valuable set of lessons within API design.

### Thinking in columns

Within databse query execution there's a model of columnar or vectorized
execution, where functions need to be applied to batches of results and
if you have enough functions and results, it's more efficient to store
the data not as a vector of structured rows, but as a structured row of
vectors. Thinking through filter application on the set of all combinations
(and especially vertical slices of those combinations) offered a number
of interesting performance opportunities.

### Larger Problem Representation

And after grinding down the performance on filter application, and hitting
complexity limits on storing combinations of column filters (pairs worked
well, triples started to become challenging from code structure and
memory bandwidth), the next win (specifically for sudoku) came from the
representation of the problem. Breaking down a monolithic "no dupes from
col# in any other column" into the N^2 pairwise checks allowed those
specialized filter applications to take greater affect.
