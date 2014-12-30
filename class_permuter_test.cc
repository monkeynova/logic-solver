#include <iostream>

#include "puzzle/class_permuter.h"

using namespace std;

int main(void) {
    Puzzle::IntRangeDescriptor d(3, 5);
    Puzzle::ClassPermuter p(&d);
    for (auto it = p.begin(); it != p.end(); ++it) {
        cout << "[";
        for (auto it2 = it->begin(); it2 != it->end(); ++it2) {
            if (it2 != it->begin()) {
                cout << ", ";
            }
            cout << *it2;
        }
        cout << "]" << endl;
    }
}
