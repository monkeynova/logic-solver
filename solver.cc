#include "solver.h"

#include <sys/time.h>

using namespace Puzzle;

Entry Entry::invalid_(-1);

static vector<Entry> empty_entry_vector;
Solution Solution::invalid_(empty_entry_vector);

void ClassPermuter::iterator::BuildCurrent() {
    if (position_ >= max_) {
        current_.resize(0);
    } else {
        int tmp = position_;
        vector<int> choose = values_;
        for (unsigned int i = 0; i < choose.size(); ++i) {
            int next = tmp % (choose.size() - i);
            tmp /= (choose.size() - i);
            current_[i] = choose[next];
            choose[next] = choose[choose.size() - i - 1];
        }
    }
}

SolutionPermuter::iterator::iterator(const EntryDescriptor* entry_descriptor)
    : entry_descriptor_(entry_descriptor), current_(Solution::Invalid()) {
    if (entry_descriptor_ == nullptr) {
        return;
    }

    class_types_ = entry_descriptor->AllClasses()->Values();
    
    vector<int> bad_classes(class_types_.size(),-1);

    for (auto id: entry_descriptor_->AllIds()->Values()) {
        entries_.push_back(Entry(id,bad_classes,entry_descriptor_));
    }
    permuters_.resize(class_types_.size(),nullptr);
    iterators_.resize(class_types_.size());
    for (auto class_int: class_types_) {
        const Descriptor* class_descriptor = entry_descriptor_->AllClassValues(class_int);
        permuters_[class_int] = ClassPermuter(class_descriptor);
        iterators_[class_int] = permuters_[class_int].begin();

        const vector<int>& class_values = *(iterators_[class_int]);
        for (unsigned int j = 0; j < class_values.size(); j++ ) {
            entries_[j].SetClass(class_int, class_values[j]);
        }
    }

    current_ = Solution(entries_);
}

void SolutionPermuter::iterator::Advance() {
    bool at_end = true;
    for (int class_int: class_types_) {
        ++iterators_[class_int];
        
        bool carry = false;
        if (iterators_[class_int] == permuters_[class_int].end()) {
            iterators_[class_int] = permuters_[class_int].begin();
            carry = true;
        }

        const vector<int>& class_values = *(iterators_[class_int]);
        for (unsigned int j = 0; j < class_values.size(); j++ ) {
            entries_[j].SetClass(class_int, class_values[j]);
        }

        if (!carry) {
            at_end = false;
            break;
        }
    }
    if (at_end) {
        current_ = Solution::Invalid();
    } else {
        current_ = Solution(entries_);
    }
}

long long SolutionPermuter::permutation_count() const {
    long long count = 1;
    for (auto class_int: entry_descriptor_->AllClasses()->Values()) {
        int value_count = entry_descriptor_->AllClassValues(class_int)->Values().size();
        for (int i = 2; i <= value_count; i++) {
            count *= i;
        }
    }

    return count;
}

Solution Solver::Solve() {
    SolutionPermuter permuter(&entry_descriptor_);
    long long total = permuter.permutation_count();
    long long attempts = 0;
    struct timeval start;
    gettimeofday(&start,nullptr);
    auto it = find_if(permuter.begin(),
                      permuter.end(),
                      [this,&attempts,total,start](const Solution& s) {
                          if (++attempts % 7777 == 0) {
                              struct timeval end;
                              gettimeofday(&end,nullptr);
                              double qps = attempts / (end.tv_sec - start.tv_sec + 1e-6 * (end.tv_usec - start.tv_usec));
                              cout << "\033[1K\rTrying " << (100 * attempts / static_cast<double>(total)) << "%, " << qps/1000 << "Kqps" << flush;
                          }
#ifdef PROFILE
                          if (attempts > 1e7) {
                              return true;
                          }
#endif
                          return all_of(onSolution.begin(),
                                        onSolution.end(),
                                        [s](const function<bool(const Solution&)>& p) { return p(s); } );
                      });
    cout << endl;
    if (it != permuter.end()) {
        return *it;
    }
    return Solution::Invalid();
}
