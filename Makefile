BINS=six_fearsome_heroes class_permuter_test solution_permuter_test

all: opt

opt: dirs $(addprefix bin/opt/,$(BINS))

debug: dirs $(addprefix bin/dbg/,$(BINS))

CCOPTS=-Wall -Werror --std=c++11
CCOPTIMIZE=-O2
CCDEBUG=-g

dirs:
	@mkdir -p bin/opt
	@mkdir -p bin/dbg

.PRECIOUS: bin/opt/%.o bin/dbg/%.o

bin/opt/%: bin/opt/%.o bin/opt/solver.o
	g++ -o $@ $^

bin/dbg/%: bin/dbg/%.o bin/dbg/solver.o
	g++ -o $@ $^

bin/opt/%.o: %.cc solver.h Makefile
	g++ -c $< -o $@ $(CCOPTS) $(CCOPT)

bin/dbg/%.o: %.cc solver.h Makefile
	g++ -c $< -o $@ $(CCOPTS) $(CCDEBUG)
