#include "puzzle/solution_permuter/allowed_value_solution_permuter.h"

namespace puzzle {

int AllowedValueGrid::Undo::NextVal() const {
  int test = val_ + 1;
  int bit = 1 << test;
  while (true) {
    if (bit > bv_) return -1;
    if (bit & bv_) return test;
    bit <<= 1;
    ++test;
  }
}

AllowedValueGrid::AllowedValueGrid(const EntryDescriptor* e) {
  int entry_size = e->AllIds()->size();
  bv_.resize(entry_size);
  vals_.resize(entry_size);
  assigned_.resize(entry_size);
  int class_size = e->num_classes();
  for (int i = 0; i < entry_size; ++i) {
    int all_bv = (1 << (class_size + 1)) - 1;
    bv_[i].resize(class_size, all_bv);
    vals_[i].resize(class_size, -1);
    assigned_[i].resize(class_size, false);
  }
}

int AllowedValueGrid::FirstVal(int entry_id, int class_id) {
  int test = 0;
  int bit = 1 << test;
  int bv = bv_[entry_id][class_id];
  while (true) {
    if (bit > bv) return -1;
    if (bit & bv) return test;
    bit <<= 1;
    ++test;
  }  
}

AllowedValueGrid::Undo AllowedValueGrid::Assign(int entry_id, int class_id, int value) {
  Undo ret;
  ret.entry_id_ = entry_id;
  ret.class_id_ = class_id;
  ret.val_ = value;
  ret.bv_ = bv_[entry_id][class_id];

  bv_[entry_id][class_id] = 1 << value;
  CHECK(!assigned_[entry_id][class_id]);
  assigned_[entry_id][class_id] = true;
  CHECK_EQ(vals_[entry_id][class_id], -1);
  vals_[entry_id][class_id] = value;
  return ret;
}

void AllowedValueGrid::UnAssign(Undo undo) {
  assigned_[undo.entry_id()][undo.class_id()] = false;
  vals_[undo.entry_id()][undo.class_id()] = -1;
  bv_[undo.entry_id()][undo.class_id()] = undo.bv_;
}

void AllowedValueGrid::AddFilter(SolutionFilter solution_filter) {
  solution_filters_.push_back(solution_filter);
}

bool AllowedValueGrid::CheckFilters(const Solution& solution) const {
  return AllMatch(solution_filters_, solution);
}

AllowedValueAdvancer::AllowedValueAdvancer(
    const AllowedValueSolutionPermuter* permuter,
    const EntryDescriptor* entry_descriptor)
    : AdvancerBase(entry_descriptor) {
  allowed_grid_ = permuter->allowed_grid_;
  int entry_size = entry_descriptor->AllIds()->size();
  int class_size = entry_descriptor->num_classes();
  undos_.reserve(entry_size * class_size);
  reassign_.reserve(entry_size * class_size);
   for (int entry_id = 0; entry_id < entry_size; ++entry_id) {
    for (int class_id = 0; class_id < class_size; ++class_id) {
      int first_val = allowed_grid_.FirstVal(entry_id, class_id);
      CHECK_NE(first_val, -1);
      undos_.push_back(allowed_grid_.Assign(entry_id, class_id, first_val));
      mutable_solution().SetClass(entry_id, class_id, first_val);
    }
  }
  if (!allowed_grid_.CheckFilters(current())) {
    Advance();
  }
}

bool AllowedValueAdvancer::Undo2Reassign() {
  while (!undos_.empty()) {
    allowed_grid_.UnAssign(undos_.back());
    reassign_.push_back(undos_.back());
    undos_.pop_back();
    if (reassign_.back().NextVal() != -1) break;
  }
  return !undos_.empty() || reassign_.back().NextVal() != -1;
}

bool AllowedValueAdvancer::Reassign2Undo() {
  CHECK(!reassign_.empty());
  int entry_id = reassign_.back().entry_id();
  int class_id = reassign_.back().class_id();
  int next_val = reassign_.back().NextVal();
  while (true) {
    mutable_solution().SetClass(entry_id, class_id, next_val);
    undos_.push_back(allowed_grid_.Assign(entry_id, class_id, next_val));
    reassign_.pop_back();
    if (reassign_.empty()) return true;
    entry_id = reassign_.back().entry_id();
    class_id = reassign_.back().class_id();
    next_val = allowed_grid_.FirstVal(entry_id, class_id);
    if (next_val == -1) return false;
  }
  LOG(FATAL) << "Left infinite loop";
}

void AllowedValueAdvancer::Advance() {
  CHECK(reassign_.empty());
  int test = 0;
  while (Undo2Reassign()) {
    if (Reassign2Undo()) {
      ++test;
      if (allowed_grid_.CheckFilters(current())) {
        VLOG(1) << current();
        break;
      }
    } else {
      LOG(FATAL) << "Should not happen without value pruning";
    }
  }
  VLOG(1) << test << " tests";
  if (undos_.empty()) {
    set_done();
  }
  set_position(position());
}

Position AllowedValueAdvancer::position() const {
  return {.position = 0, .count = 0};
}

AllowedValueSolutionPermuter::AllowedValueSolutionPermuter(
    const EntryDescriptor* e)
  : SolutionPermuter(e) {
  allowed_grid_ = AllowedValueGrid(entry_descriptor());
  int entry_size = e->AllIds()->size();
  int class_size = e->num_classes();
  for (int class_id = 0; class_id < class_size; ++class_id) {
    for (int entry_id1 = 0; entry_id1 < entry_size; ++entry_id1) {
      for (int entry_id2 = entry_id1 + 1; entry_id2 < entry_size; ++entry_id2) {
        CHECK(AddFilter(SolutionFilter(
            absl::StrFormat("ClassPermutation %d/{%d,%d}", class_id, entry_id1, entry_id2),
            // Capture by value.
            [entry_id1, entry_id2, class_id](const Solution& s) {
              return s.Id(entry_id1).Class(class_id) != s.Id(entry_id2).Class(class_id);
            },
            {class_id})).ok());
      }
    }
  }
}

absl::StatusOr<bool> AllowedValueSolutionPermuter::AddFilter(
    SolutionFilter solution_filter) {
  allowed_grid_.AddFilter(solution_filter);
  return false;
}

absl::Status AllowedValueSolutionPermuter::PrepareCheap() {
  return absl::OkStatus();
}

absl::Status AllowedValueSolutionPermuter::PrepareFull() {
  return absl::OkStatus();
  return absl::UnimplementedError(
      "AllowedValueSolutionPermuter not implemented");
}

double AllowedValueSolutionPermuter::permutation_count() const {
  double permutations_count = 1;
  const EntryDescriptor* e = entry_descriptor();
  for (int i = 0; i < e->num_classes(); ++i) {
    permutations_count *= e->Class(i).size();
  }
  return permutations_count;
}

AllowedValueSolutionPermuter::iterator AllowedValueSolutionPermuter::begin() const {
  return iterator(absl::make_unique<AllowedValueAdvancer>(this, entry_descriptor()));
}

}  // namespace puzzle
