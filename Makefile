BINS=six_fearsome_heroes class_permuter_test solution_permuter_test

all: opt

opt: dirs $(addprefix bin/opt/,$(BINS))
profile: dirs $(addprefix bin/prof/,$(BINS))
debug: dirs $(addprefix bin/dbg/,$(BINS))

CCOPTS=-Wall -Werror --std=c++11
CCOPTIMIZE=-O2
LDOPTIMIZE=
CCDEBUG=-g -ggdb
LDDEBUG=
CCPROF=-g -ggdb -O2 -DPROFILE
LDPROF=-lprofiler

dirs:
	@mkdir -p bin/opt
	@mkdir -p bin/dbg
	@mkdir -p bin/prof

.PRECIOUS: bin/opt/%.o bin/dbg/%.o bin/prof/%.o

bin/opt/%: bin/opt/%.o bin/opt/solver.o
	g++ -o $@ $^ $(LDOPTIMIZE)

bin/prof/%: bin/prof/%.o bin/prof/solver.o
	g++ -o $@ $^ $(LDPROF)

bin/dbg/%: bin/dbg/%.o bin/dbg/solver.o
	g++ -o $@ $^ $(LDDEBUG)

bin/opt/%.o: %.cc solver.h Makefile
	g++ -c $< -o $@ $(CCOPTS) $(CCOPTIMIZE)

bin/prof/%.o: %.cc solver.h Makefile
	g++ -c $< -o $@ $(CCOPTS) $(CCPROF)

bin/dbg/%.o: %.cc solver.h Makefile
	g++ -c $< -o $@ $(CCOPTS) $(CCDEBUG)
