#ifndef __PUZZLE_CROPPED_SOLUTION_PERMUTER_H
#define __PUZZLE_CROPPED_SOLUTION_PERMUTER_H

#include "puzzle/class_permuter.h"
#include "puzzle/solution.h"

namespace Puzzle {

class CroppedSolutionPermuter {
 public:
    class iterator {
    public:
        typedef std::forward_iterator_tag iterator_category;
        typedef int difference_type;
        typedef CroppedSolutionPermuter value_type;
        typedef CroppedSolutionPermuter& reference;
        typedef CroppedSolutionPermuter* pointer;

        iterator(const CroppedSolutionPermuter& permuter, 
                 const EntryDescriptor* entry_descriptor);

        bool operator!=(const iterator& other) {
            return !(*this == other);
        }
        bool operator==(const iterator& other) {
            return current_ == other.current_;
        }
        const Solution& operator*() {
            return current_;
        }
        const Solution* operator->() {
            return &current_;
        }
        iterator& operator++() {
            Advance();
            return *this;
        }

    private:
        void Advance();

        const CroppedSolutionPermuter& permuter_;
        const EntryDescriptor* entry_descriptor_;
        vector<Entry> entries_;
        vector<int> class_types_;
        vector<ClassPermuter::iterator> iterators_;
        Solution current_;
    };

    CroppedSolutionPermuter(const EntryDescriptor* e);
    ~CroppedSolutionPermuter() {}

    iterator begin() const { return iterator(*this, entry_descriptor_); }
    iterator end() const { return iterator(*this, nullptr); }

    long long permutation_count() const;
    const ClassPermuter& class_permuter(int class_int) const { return class_permuters_[class_int]; }

 private:
    const EntryDescriptor* entry_descriptor_;
    vector<ClassPermuter> class_permuters_;
};

}  // namespace Puzzle

#endif  // __PUZZLE_CROPPED_SOLUTION_PERMUTER_H
