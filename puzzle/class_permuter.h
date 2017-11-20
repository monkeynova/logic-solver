#ifndef __PUZZLE_CLASS_PERMUTER_H
#define __PUZZLE_CLASS_PERMUTER_H

#include "puzzle/solution.h"

namespace Puzzle {

class ClassPermuter {
 public:
    class iterator {
    public:
        typedef std::forward_iterator_tag iterator_category;
        typedef int difference_type;
        typedef ClassPermuter value_type;
        typedef ClassPermuter& reference;
        typedef ClassPermuter* pointer;

        iterator() : iterator(nullptr) {}
        iterator(const Descriptor* descriptor) {
            if (descriptor != nullptr) {
                values_ = descriptor->Values();
            }
            int entries = values_.size();
            if (entries > 0) {
                max_ = 1;
                for (int i = 2; i <= entries; i++ ) {
                    max_ *= i;
                }
            }
            position_ = 0;
            current_.resize(values_.size());
            BuildCurrent();
        }

        bool operator!=(const iterator& other) {
            return !(*this == other);
        }
        bool operator==(const iterator& other) {
            return current_ == other.current_;
        }
        const std::vector<int>& operator*() {
            return current_;
        }
        const std::vector<int>* operator->() {
            return &current_;
        }
        iterator& operator++() {
            ++position_;
            BuildCurrent();
            return *this;
        }

        long long position() const { return position_; }
        double completion() const { return static_cast<double>(position_) / max_; }

    private:
        void BuildCurrent();

        std::vector<int> values_;
        std::vector<int> current_;
        int position_;
        int max_;
    };

    ClassPermuter(const Descriptor* d) : descriptor_(d) {}
    ~ClassPermuter() {}

    iterator begin() const { return iterator(descriptor_); }
    iterator end() const { return iterator(); }

    long long permutation_count() const;

 private:
    const Descriptor* descriptor_;
};

}  // namespace Puzzle

#endif  // __PUZZLE_CLASS_PERMUTER_H
