#include <iostream>

#include "solver.h"

using namespace std;

int main(void) {
    Puzzle::EntryDescriptor ed;
    Puzzle::IntRangeDescriptor id(3, 5);
    Puzzle::IntRangeDescriptor cd1(6, 8);
    Puzzle::IntRangeDescriptor cd2(11, 13);

    ed.SetIds(&id);
    ed.SetClass(0,"foo",&cd1);
    ed.SetClass(1,"bar",&cd2);

    Puzzle::SolutionPermuter p(&ed);
    for (auto it = p.begin(); it != p.end(); ++it) {
        cout << it->ToStr() << endl;
    }
}
