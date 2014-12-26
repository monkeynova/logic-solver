all: dirs bin/opt/six_fearsome_heroes

debug: bin/dbg/six_fearsome_heroes

CCOPTS=-Wall -Werror --std=c++11
CCOPTIMIZE=-O2
CCDEBUG=-g

dirs:
	@mkdir -p bin/opt
	@mkdir -p bin/dbg

bin/opt/six_fearsome_heroes: bin/opt/six_fearsome_heroes.o bin/opt/solver.o
	g++ -o $@ $^

bin/dbg/six_fearsome_heroes: bin/dbg/six_fearsome_heroes.o bin/dbg/solver.o
	g++ -o $@ $^

bin/opt/%.o:
	g++ -c $< -o $@ $(CCOPTS) $(CCOPT)

bin/dbg/%.o:
	g++ -c $< -o $@ $(CCOPTS) $(CCDEBUG)

bin/opt/six_fearsome_heroes.o: six_fearsome_heroes.cc solver.h Makefile
bin/dbg/six_fearsome_heroes.o: six_fearsome_heroes.cc solver.h Makefile
bin/opt/solver.o: solver.cc solver.h Makefile
bin/dbg/solver.o: solver.cc solver.h Makefile
