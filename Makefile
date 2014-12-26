all: bin/six_fearsome_heroes dirs

dirs:
	@mkdir -p bin

bin/six_fearsome_heroes: bin/six_fearsome_heroes.o bin/solver.o
	g++ -o $@ $^

bin/six_fearsome_heroes.o: six_fearsome_heroes.cc solver.h
	g++ -c -o $@ $< --std=c++11 -Wall -Werror

bin/solver.o: solver.cc solver.h
	g++ -c -o $@ $< --std=c++11 -Wall -Werror
