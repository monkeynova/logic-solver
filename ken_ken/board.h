#ifndef KEN_KEB_BOARD_H
#define KEN_KEB_BOARD_H

#include <cstdint>

#include "puzzle/problem.h"

namespace KenKen {

template <int64_t kWidth>
class Board : public puzzle::Problem {
 public:
  struct Box {
    int entry_id;
    int class_id;

    template <typename Sink>
    friend void AbslStringify(Sink& sink, const Box& b) {
      absl::Format(&sink, "(%v, %v)", b.entry_id, b.class_id);
    }

    template <typename H>
    friend H AbslHashValue(H h, const Box& box) {
      return H::combine(std::move(h), box.entry_id, box.class_id);
    }

    bool operator==(const Box& other) const {
      return entry_id == other.entry_id && class_id == other.class_id;
    }
  };

  struct Cage {
    int val;
    enum Op {
      kAdd = 1,
      kSub = 2,
      kMul = 3,
      kDiv = 4,
    } op;
    std::vector<Box> boxes;
  };

  Board() : puzzle::Problem(MakeEntryDescriptor()) {}

 protected:
  static int Val(const puzzle::Solution& s, int x, int y) {
    return s.Id(x).Class(y) + 1;
  }
  static int Val(const puzzle::Entry& e, int y) { return e.Class(y) + 1; }

  virtual std::vector<Cage> GetCages() const = 0;
  virtual absl::Status AddCagePredicates();

 private:
  absl::Status Setup() override;
  absl::Status AddBoardPredicates();

  static bool IsContiguous(const Cage& cage);
  absl::Status AddSumPredicate(int val, const std::vector<Box>& boxes,
                               int box_id, const std::vector<int>& classes,
                               std::optional<int> single_entry);
  absl::Status AddMulPredicate(int val, const std::vector<Box>& boxes,
                               int box_id, const std::vector<int>& classes,
                               std::optional<int> single_entry);
  absl::Status AddSubPredicate(int val, const std::vector<Box>& boxes,
                               int box_id, const std::vector<int>& classes,
                               std::optional<int> single_entry);
  absl::Status AddDivPredicate(int val, const std::vector<Box>& boxes,
                               int box_id, const std::vector<int>& classes,
                               std::optional<int> single_entry);

  static puzzle::EntryDescriptor MakeEntryDescriptor();
};

template <int64_t kWidth>
puzzle::EntryDescriptor Board<kWidth>::MakeEntryDescriptor() {
  std::vector<std::string> id_names(kWidth);
  for (int i = 0; i < kWidth; ++i) {
    id_names[i] = ("");
  }

  std::vector<std::unique_ptr<const puzzle::Descriptor>> class_descriptors;
  for (int i = 0; i < kWidth; ++i) {
    std::vector<std::string> class_names(kWidth);
    for (int j = 0; j < kWidth; ++j) {
      class_names[j] = absl::StrCat(j + 1);
    }
    class_descriptors.push_back(
        absl::make_unique<puzzle::StringDescriptor>(std::move(class_names)));
  }

  return puzzle::EntryDescriptor(
      absl::make_unique<puzzle::StringDescriptor>(id_names),
      /*class_descriptor=*/
      absl::make_unique<puzzle::StringDescriptor>(
          std::vector<std::string>(kWidth, "")),
      std::move(class_descriptors));
}

template <int64_t kWidth>
absl::Status Board<kWidth>::Setup() {
  RETURN_IF_ERROR(AddBoardPredicates());
  RETURN_IF_ERROR(AddCagePredicates());
  return absl::OkStatus();
}

template <int64_t kWidth>
absl::Status Board<kWidth>::AddBoardPredicates() {
  std::vector<int> cols = {0};
  for (int i = 1; i < kWidth; ++i) {
    cols.push_back(i);
    RETURN_IF_ERROR(AddAllEntryPredicate(
        absl::StrCat("No row dupes ", i + 1),
        [i](const puzzle::Entry& e) {
          for (int j = 0; j < i; ++j) {
            if (e.Class(i) == e.Class(j)) return false;
          }
          return true;
        },
        cols));
  }

  return absl::OkStatus();
}

template <int64_t kWidth>
bool Board<kWidth>::IsContiguous(const Cage& c) {
  std::array<std::bitset<kWidth>, kWidth> box_found;
  for (const Box& b : c.boxes) {
    box_found[b.entry_id][b.class_id] = true;
  }

  int b0_contiguous_size = 0;
  for (std::deque<Box> queue = {c.boxes[0]}; !queue.empty();
       queue.pop_front()) {
    ++b0_contiguous_size;
    Box cur = queue.front();
    box_found[cur.entry_id][cur.class_id] = false;
    if (cur.entry_id > 0 && box_found[cur.entry_id - 1][cur.class_id]) {
      queue.push_back({cur.entry_id - 1, cur.class_id});
    }
    if (cur.entry_id < kWidth - 1 &&
        box_found[cur.entry_id + 1][cur.class_id]) {
      queue.push_back({cur.entry_id + 1, cur.class_id});
    }
    if (cur.class_id > 0 && box_found[cur.entry_id][cur.class_id - 1]) {
      queue.push_back({cur.entry_id, cur.class_id - 1});
    }
    if (cur.class_id < kWidth - 1 &&
        box_found[cur.entry_id][cur.class_id + 1]) {
      queue.push_back({cur.entry_id, cur.class_id + 1});
    }
  }
  return b0_contiguous_size == c.boxes.size();
}

template <int64_t kWidth>
absl::Status Board<kWidth>::AddSumPredicate(int val,
                                            const std::vector<Box>& boxes,
                                            int box_id,
                                            const std::vector<int>& classes,
                                            std::optional<int> single_entry) {
  // TODO: KillerSudoku puts some bounds on elements from totals.
  if (single_entry) {
    return AddSpecificEntryPredicate(
        absl::StrCat("Box #", box_id),
        [val, boxes](const puzzle::Entry& e) {
          int sum = 0;
          for (const Box& b : boxes) {
            sum += e.Class(b.class_id);
          }
          return sum + boxes.size() == val;  // Fenceposts.
        },
        classes, *single_entry);
  }

  return AddPredicate(
      absl::StrCat("Box #", box_id),
      [val, boxes](const puzzle::Solution& s) {
        int sum = 0;
        for (const Box& b : boxes) {
          sum += s.Id(b.entry_id).Class(b.class_id);
        }
        return sum + boxes.size() == val;  // Fenceposts.
      },
      classes);
}

template <int64_t kWidth>
absl::Status Board<kWidth>::AddMulPredicate(int val,
                                            const std::vector<Box>& boxes,
                                            int box_id,
                                            const std::vector<int>& classes,
                                            std::optional<int> single_entry) {
  // TODO: We can put requirements based on factorization.
  if (single_entry) {
    return AddSpecificEntryPredicate(
        absl::StrCat("Box #", box_id),
        [val, boxes](const puzzle::Entry& e) {
          int product = 1;
          for (const Box& b : boxes) {
            product *= e.Class(b.class_id) + 1;
          }
          return product == val;
        },
        classes, *single_entry);
  }

  return AddPredicate(
      absl::StrCat("Box #", box_id),
      [val, boxes](const puzzle::Solution& s) {
        int product = 1;
        for (const Box& b : boxes) {
          product *= s.Id(b.entry_id).Class(b.class_id) + 1;
        }
        return product == val;
      },
      classes);
}

template <int64_t kWidth>
absl::Status Board<kWidth>::AddSubPredicate(int val,
                                            const std::vector<Box>& boxes,
                                            int box_id,
                                            const std::vector<int>& classes,
                                            std::optional<int> single_entry) {
  if (boxes.size() != 2) {
    return absl::InvalidArgumentError(
        absl::StrCat("Subtraction only for 2 boxes"));
  }
  if (single_entry) {
    return AddSpecificEntryPredicate(
        absl::StrCat("Box #", box_id),
        [val, boxes](const puzzle::Entry& e) {
          int b1 = e.Class(boxes[0].class_id);
          int b2 = e.Class(boxes[1].class_id);
          return b2 - b1 == val || b1 - b2 == val;
        },
        classes, *single_entry);
  }

  return AddPredicate(
      absl::StrCat("Box #", box_id),
      [val, boxes](const puzzle::Solution& s) {
        int b1 = s.Id(boxes[0].entry_id).Class(boxes[0].class_id);
        int b2 = s.Id(boxes[1].entry_id).Class(boxes[1].class_id);
        return b2 - b1 == val || b1 - b2 == val;
      },
      classes);
}

template <int64_t kWidth>
absl::Status Board<kWidth>::AddDivPredicate(int val,
                                            const std::vector<Box>& boxes,
                                            int box_id,
                                            const std::vector<int>& classes,
                                            std::optional<int> single_entry) {
  if (boxes.size() != 2) {
    return absl::InvalidArgumentError(
        absl::StrCat("Division only for 2 boxes"));
  }
  if (single_entry) {
    return AddSpecificEntryPredicate(
        absl::StrCat("Box #", box_id),
        [val, boxes](const puzzle::Entry& e) {
          int b1 = e.Class(boxes[0].class_id) + 1;
          int b2 = e.Class(boxes[1].class_id) + 1;
          return b1 == val * b2 || b2 == val * b1;
        },
        classes, *single_entry);
  }
  return AddPredicate(
      absl::StrCat("Box #", box_id),
      [val, boxes](const puzzle::Solution& s) {
        int b1 = s.Id(boxes[0].entry_id).Class(boxes[0].class_id) + 1;
        int b2 = s.Id(boxes[1].entry_id).Class(boxes[1].class_id) + 1;
        return b1 == val * b2 || b2 == val * b1;
      },
      classes);
}

template <int64_t kWidth>
absl::Status Board<kWidth>::AddCagePredicates() {
  std::vector<Cage> cages = GetCages();

  std::array<std::bitset<kWidth>, kWidth> found;

  int box_id = 0;
  for (const Cage& c : cages) {
    ++box_id;
    if (c.boxes.empty()) {
      return absl::InvalidArgumentError("Empty boxes");
    }
    std::optional<int> single_entry = c.boxes[0].entry_id;
    std::vector<int> classes;
    std::array<std::bitset<kWidth>, kWidth> box_found;
    for (const Box& b : c.boxes) {
      if (found[b.entry_id][b.class_id]) {
        return absl::InvalidArgumentError(absl::StrCat("Dup box: ", b));
      }
      found[b.entry_id][b.class_id] = true;
      box_found[b.entry_id][b.class_id] = true;
      if (single_entry && *single_entry != b.entry_id) {
        single_entry = std::nullopt;
      }
      if (!absl::c_linear_search(classes, b.class_id)) {
        classes.push_back(b.class_id);
      }
    }

    if (!IsContiguous(c)) {
      return absl::InvalidArgumentError("Cage isn't contiguous");
    }

    absl::c_sort(classes);
    switch (c.op) {
      case Cage::kAdd: {
        RETURN_IF_ERROR(
            AddSumPredicate(c.val, c.boxes, box_id, classes, single_entry));
        break;
      }
      case Cage::kMul: {
        RETURN_IF_ERROR(
            AddMulPredicate(c.val, c.boxes, box_id, classes, single_entry));
        break;
      }
      case Cage::kSub: {
        RETURN_IF_ERROR(
            AddSubPredicate(c.val, c.boxes, box_id, classes, single_entry));
        break;
      }
      case Cage::kDiv: {
        RETURN_IF_ERROR(
            AddDivPredicate(c.val, c.boxes, box_id, classes, single_entry));
        break;
      }
      default:
        return absl::InvalidArgumentError("Bad Op");
    }
  }

  for (int x = 0; x < kWidth; ++x) {
    for (int y = 0; y < kWidth; ++y) {
      if (!found[x][y]) {
        return absl::InvalidArgumentError(
            absl::StrCat("Missing box: ", x, ",", y));
      }
    }
  }

  return absl::OkStatus();
}

}  // namespace KenKen

#endif  // KEN_KEB_BOARD_H