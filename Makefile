BINS=six_fearsome_heroes class_permuter_test solution_permuter_test

PUZZLE_LIB=brute_solution_permuter.o class_permuter.o cropped_solution_permuter.o solver.o
PUZZLE_LIB:=$(addprefix puzzle/,$(PUZZLE_LIB))

PUZZLE_HEADERS=brute_solution_permuter.h class_permuter.h cropped_solution_permuter.h solver.h solution.h
PUZZLE_HEADERS:=$(addprefix puzzle/,$(PUZZLE_HEADERS))

all: opt

opt: dirs $(addprefix bin/opt/,$(BINS))
profile: dirs $(addprefix bin/prof/,$(BINS))
debug: dirs $(addprefix bin/dbg/,$(BINS))

CCOPTS=-Wall -Werror --std=c++11 -I.
CCOPTIMIZE=-O2
LDOPTIMIZE=
CCDEBUG=-g -ggdb
LDDEBUG=
CCPROF=-g -ggdb -O2 -DPROFILE
LDPROF=-lprofiler

dirs:
	@mkdir -p bin/opt/puzzle
	@mkdir -p bin/dbg/puzzle
	@mkdir -p bin/prof/puzzle

.PRECIOUS: bin/opt/%.o bin/dbg/%.o bin/prof/%.o

bin/opt/%: bin/opt/%.o $(addprefix bin/opt/,$(PUZZLE_LIB))
	g++ -o $@ $^ $(LDOPTIMIZE)

bin/prof/%: bin/prof/%.o $(addprefix bin/opt/,$(PUZZLE_LIB))
	g++ -o $@ $^ $(LDPROF)

bin/dbg/%: bin/dbg/%.o $(addprefix bin/opt/,$(PUZZLE_LIB))
	g++ -o $@ $^ $(LDDEBUG)

bin/opt/%.o: %.cc $(PUZZLE_HEADERS) Makefile
	g++ -c $< -o $@ $(CCOPTS) $(CCOPTIMIZE)

bin/opt/puzzle/%.o: puzzle/%.cc $(PUZZLE_HEADERS) Makefile
	g++ -c $< -o $@ $(CCOPTS) $(CCOPTIMIZE)

bin/prof/%.o: %.cc $(PUZZLE_HEADERS) Makefile
	g++ -c $< -o $@ $(CCOPTS) $(CCPROF)

bin/prof/puzzle/%.o: puzzle/%.cc $(PUZZLE_HEADERS) Makefile
	g++ -c $< -o $@ $(CCOPTS) $(CCOPTIMIZE)

bin/dbg/%.o: %.cc $(PUZZLE_HEADERS) Makefile
	g++ -c $< -o $@ $(CCOPTS) $(CCDEBUG)

bin/dbg/puzzle/%.o: puzzle/%.cc $(PUZZLE_HEADERS) Makefile
	g++ -c $< -o $@ $(CCOPTS) $(CCOPTIMIZE)
