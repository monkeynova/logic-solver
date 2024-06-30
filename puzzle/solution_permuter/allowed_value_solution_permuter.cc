#include "puzzle/solution_permuter/allowed_value_solution_permuter.h"

#include <bit>
#include "absl/flags/flag.h"

ABSL_FLAG(bool, puzzle_allow_only_propagate, false, "...");

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

AllowedValueGrid::AllowedValueGrid(MutableSolution* mutable_solution)
 : mutable_solution_(mutable_solution),
   testable_solution_(mutable_solution_->TestableSolution()) {
  int entry_size = mutable_solution_->descriptor()->AllIds()->size();
  bv_.resize(entry_size);
  vals_.resize(entry_size);
  assigned_.resize(entry_size);
  solution_filters_.resize(entry_size);
  int class_size = mutable_solution_->descriptor()->num_classes();
  int all_bv = (1 << entry_size) - 1;
  for (int i = 0; i < entry_size; ++i) {
    bv_[i].resize(class_size, all_bv);
    vals_[i].resize(class_size, -1);
    assigned_[i].resize(class_size, false);
    solution_filters_[i].resize(class_size);
  }
}

Position AllowedValueGrid::position() const {
  double position = 0;
  double count = 1;
  for (int entry_id = 0; entry_id < vals_.size(); ++entry_id) {
    int class_count = vals_[entry_id].size();
    for (int class_id = 0; class_id < vals_[entry_id].size(); ++class_id) {
      position = position * class_count + vals_[entry_id][class_id];
      count *= class_count;
    }
  }
  return {.position = position, .count = count};
}

int AllowedValueGrid::FirstVal(Box box) const {
  int bv = bv_[box.entry_id][box.class_id];
  for (int bit = 1, value = 0; bit <= bv; ++value, bit <<= 1) {
    if (bit & bv) return value;
  }
  return -1;
}

AllowedValueGrid::Undo AllowedValueGrid::Empty(Box box) const {
  Undo ret;
  ret.box_ = box;
  ret.val_ = -1;
  ret.bv_ = bv_[box.entry_id][box.class_id];
  return ret;
}

std::pair<AllowedValueGrid::Undo, bool> AllowedValueGrid::Assign(Box box,
                                                                 int value) {
  Undo ret;
  ret.box_ = box;
  ret.val_ = value;
  ret.bv_ = bv_[box.entry_id][box.class_id];

  CHECK(!assigned_[box.entry_id][box.class_id]);
  assigned_[box.entry_id][box.class_id] = true;

  CHECK(bv_[box.entry_id][box.class_id] & (1 << value));

  bv_[box.entry_id][box.class_id] = 1 << value;
  mutable_solution_->SetClass(box.entry_id, box.class_id, value);
  bool allowed = OnSingleAllowed(ret);
  if (!allowed) return {std::move(ret), false};

  CHECK_EQ(vals_[box.entry_id][box.class_id], -1);
  vals_[box.entry_id][box.class_id] = value;
  return {std::move(ret), true};
}

void AllowedValueGrid::UnAssign(const Undo& undo) {
  assigned_[undo.entry_id()][undo.class_id()] = false;
  vals_[undo.entry_id()][undo.class_id()] = -1;
  bv_[undo.entry_id()][undo.class_id()] = undo.bv_;
  for (const auto& [box, bv] : undo.restore) {
    if ((bv & (bv - 1)) == 0) {
      CHECK_EQ(1 << testable_solution_.Id(undo.entry_id()).Class(undo.class_id()), bv);
    }
    bv_[box.entry_id][box.class_id] = bv;
  }
}

uint16_t AllowedValueGrid::CheckAllowed(SolutionFilter filter, Box box) const {
  uint16_t bv = bv_[box.entry_id][box.class_id];
  uint16_t ret = 0;
  CHECK(!assigned_[box.entry_id][box.class_id]);
  for (int bit = 1, value = 0; bit <= bv; ++value, bit <<= 1) {
    if (!(bit & bv)) continue;
    mutable_solution_->SetClass(box.entry_id, box.class_id, value);
    if (filter(testable_solution_)) {
      ret |= bit;
    }
  }
  return ret;
}

void AllowedValueGrid::AddFilter(SolutionFilter solution_filter,
                                 std::vector<Box> boxes) {
  if (boxes.size() == 1) {
    uint16_t bv = CheckAllowed(solution_filter, boxes[0]);
    bv_[boxes[0].entry_id][boxes[0].class_id] = bv;
  } else {
    for (Box b : boxes) {
      solution_filters_[b.entry_id][b.class_id].push_back(
          {solution_filter, boxes});
    }
  }
}

bool AllowedValueGrid::Prepare() {
  std::vector<Undo> queue;
  for (int entry_id = 0; entry_id < bv_.size(); ++entry_id) {
    for (int class_id = 0; class_id < bv_[entry_id].size(); ++class_id) {
      int bv = bv_[entry_id][class_id];
      if (bv & (bv - 1)) continue;
      Undo undo;
      undo.box_ = {.entry_id = entry_id, .class_id = class_id};
      int value = std::countr_zero(bv_[undo.entry_id()][undo.class_id()]);
      CHECK_EQ(1 << value, bv);
      CHECK(!assigned_[entry_id][class_id]);
      mutable_solution_->SetClass(entry_id, class_id, value);
      queue.push_back(std::move(undo));
    }
  }
  for (Undo& undo : queue) {
    if (!OnSingleAllowed(undo)) return false;
  }
  return true;
}

bool AllowedValueGrid::OnSingleAllowed(Undo& undo) {
  CHECK_EQ(1 << testable_solution_.Id(undo.entry_id()).Class(undo.class_id()),
           bv_[undo.entry_id()][undo.class_id()]);
  for (const auto& [filter, boxes] :
       solution_filters_[undo.entry_id()][undo.class_id()]) {
    std::optional<Box> missing;
    bool run_check = true;
    for (const Box& b : boxes) {
      int test_bv = bv_[b.entry_id][b.class_id];
      CHECK_NE(test_bv, 0);
      if (test_bv & (test_bv - 1)) {
        CHECK(!assigned_[b.entry_id][b.class_id]);
        if (missing) {
          missing = std::nullopt;
          break;
        }
        missing = b;
        run_check = false;
      } else {
        CHECK_EQ(1 << testable_solution_.Id(b.entry_id).Class(b.class_id), test_bv)
           << b.entry_id << "," << b.class_id;
      }
    }
    if (run_check) {
      if (!filter(testable_solution_)) return false;
    } else if (missing) {
      uint16_t bv = CheckAllowed(filter, *missing);
      if (bv == 0) return false;
      if (bv != bv_[missing->entry_id][missing->class_id]) {
        CHECK((bv & bv_[missing->entry_id][missing->class_id]) == bv);
        undo.restore.push_back({*missing, bv_[missing->entry_id][missing->class_id]});
        bv_[missing->entry_id][missing->class_id] = bv;
        if ((bv & (bv - 1)) == 0) {
          int value = std::countr_zero(bv);
          CHECK(!assigned_[missing->entry_id][missing->class_id]);
          mutable_solution_->SetClass(missing->entry_id, missing->class_id, value);
          if (absl::GetFlag(FLAGS_puzzle_allow_only_propagate)) {
            Box save = undo.box_;
            undo.box_ = *missing;
            bool possible = OnSingleAllowed(undo);
            undo.box_ = save;
            if (!possible) return false;
          }
        }
      }
    }
  }
  return true;
}

AllowedValueAdvancer::AllowedValueAdvancer(
    const AllowedValueSolutionPermuter* permuter,
    const EntryDescriptor* entry_descriptor)
    : AdvancerBase(entry_descriptor),
      allowed_grid_(&mutable_solution()) {
  int entry_size = entry_descriptor->AllIds()->size();
  int class_size = entry_descriptor->num_classes();

  for (int class_id = 0; class_id < class_size; ++class_id) {
    for (int entry_id1 = 0; entry_id1 < entry_size; ++entry_id1) {
      for (int entry_id2 = entry_id1 + 1; entry_id2 < entry_size; ++entry_id2) {
        allowed_grid_.AddFilter(
            SolutionFilter(absl::StrFormat("ClassPermutation %d/{%d,%d}",
                                           class_id, entry_id1, entry_id2),
                           // Capture by value.
                           [entry_id1, entry_id2, class_id](const Solution& s) {
                             return s.Id(entry_id1).Class(class_id) !=
                                    s.Id(entry_id2).Class(class_id);
                           },
                           {class_id}),
            {AllowedValueGrid::Box{.entry_id = entry_id1, .class_id = class_id},
             AllowedValueGrid::Box{.entry_id = entry_id2,
                                   .class_id = class_id}});
      }
    }
  }

  for (const auto& [solution_filter, boxes] : permuter->solution_filters()) {
    allowed_grid_.AddFilter(solution_filter, boxes);
  }

  if (allowed_grid_.Prepare()) {
    undos_.reserve(entry_size * class_size);
    reassign_.reserve(entry_size * class_size);
    for (int entry_id = 0; entry_id < entry_size; ++entry_id) {
      for (int class_id = 0; class_id < class_size; ++class_id) {
        reassign_.push_back(allowed_grid_.Empty({.entry_id = entry_id, .class_id = class_id}));
      }
    }
    absl::c_reverse(reassign_);
    while (!Reassign2Undo()) {
      if (!Undo2Reassign()) break;
    }
  } else {
    CHECK(undos_.empty()) << "Impossible possible not marking done?";
  }
  if (undos_.empty()) {
    set_done();
  }
  set_position(position());
}

bool AllowedValueAdvancer::Undo2Reassign() {
  while (!undos_.empty()) {
    allowed_grid_.UnAssign(undos_.back());
    reassign_.push_back(std::move(undos_.back()));
    undos_.pop_back();
    if (reassign_.back().NextVal() != -1) break;
  }
  return !undos_.empty() || reassign_.back().NextVal() != -1;
}

bool AllowedValueAdvancer::Reassign2Undo() {
  CHECK(!reassign_.empty());
  AllowedValueGrid::Box box = reassign_.back().box();
  int next_val = reassign_.back().NextVal();
  while (true) {
    auto undo_and_assigned = allowed_grid_.Assign(box, next_val);
    while (!undo_and_assigned.second) {
      next_val = undo_and_assigned.first.NextVal();
      allowed_grid_.UnAssign(undo_and_assigned.first);
      if (next_val == -1) return false;
      undo_and_assigned = allowed_grid_.Assign(box, next_val);
    }
    undos_.push_back(std::move(undo_and_assigned.first));
    reassign_.pop_back();
    if (reassign_.empty()) return true;
    box = reassign_.back().box();
    next_val = allowed_grid_.FirstVal(box);
    if (next_val == -1) return false;
  }
  LOG(FATAL) << "Left infinite loop";
}

void AllowedValueAdvancer::Advance() {
  CHECK(reassign_.empty());
  while (Undo2Reassign()) {
    if (Reassign2Undo()) {
      break;
    }
  }
  if (undos_.empty()) {
    set_done();
  }
  set_position(position());
}

Position AllowedValueAdvancer::position() const {
  return allowed_grid_.position();
}

AllowedValueSolutionPermuter::AllowedValueSolutionPermuter(
    const EntryDescriptor* e)
    : SolutionPermuter(e) {
}

absl::StatusOr<bool> AllowedValueSolutionPermuter::AddFilter(
    SolutionFilter solution_filter) {
  std::vector<AllowedValueGrid::Box> boxes;
  for (int class_id : solution_filter.classes()) {
    int entry_id = solution_filter.entry_id(class_id);
    if (entry_id == Entry::kBadId) {
      VLOG(1) << "Left " << solution_filter.name() << "  as residual";
      return false;
    }
    boxes.push_back({.entry_id = entry_id, .class_id = class_id});
  }
  solution_filters_.push_back({solution_filter, boxes});
  return true;
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

AllowedValueSolutionPermuter::iterator AllowedValueSolutionPermuter::begin()
    const {
  return iterator(
      absl::make_unique<AllowedValueAdvancer>(this, entry_descriptor()));
}

}  // namespace puzzle
