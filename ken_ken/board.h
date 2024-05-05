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
absl::Status Board<kWidth>::AddCagePredicates() {
  std::vector<Cage> cages = GetCages();

  std::vector<std::vector<bool>> found(kWidth,
                                       std::vector<bool>(kWidth, false));

  int box_id = 0;
  for (const Cage& c : cages) {
    ++box_id;
    if (c.boxes.empty()) {
      return absl::InvalidArgumentError("Empty boxes");
    }
    std::optional<int> single_entry = c.boxes[0].entry_id;
    std::vector<int> classes;
    for (const Box& b : c.boxes) {
      if (found[b.entry_id][b.class_id]) {
        return absl::InvalidArgumentError(absl::StrCat("Dup box: ", b));
      }
      found[b.entry_id][b.class_id] = true;
      if (single_entry && *single_entry != b.entry_id) {
        single_entry = std::nullopt;
      }
      if (!absl::c_linear_search(classes, b.class_id)) {
        classes.push_back(b.class_id);
      }
    }
    // TODO: Could also validate contiguous.
    absl::c_sort(classes);
    switch (c.op) {
      case Cage::kAdd: {
        // TODO: KillerSudoku puts some bounds on elements from totals.
        if (single_entry) {
          RETURN_IF_ERROR(AddSpecificEntryPredicate(
              absl::StrCat("Box #", box_id),
              [c](const puzzle::Entry& e) {
                int sum = 0;
                for (const Box& b : c.boxes) {
                  sum += e.Class(b.class_id) + 1;
                }
                return sum == c.val;
              },
              classes, *single_entry));
        } else {
          RETURN_IF_ERROR(AddPredicate(
              absl::StrCat("Box #", box_id),
              [c](const puzzle::Solution& s) {
                int sum = 0;
                for (const Box& b : c.boxes) {
                  sum += s.Id(b.entry_id).Class(b.class_id) + 1;
                }
                return sum == c.val;
              },
              classes));
        }
        break;
      }
      case Cage::kMul: {
        // TODO: We can put requirements based on factorization.
        if (single_entry) {
          RETURN_IF_ERROR(AddSpecificEntryPredicate(
              absl::StrCat("Box #", box_id),
              [c](const puzzle::Entry& e) {
                int product = 1;
                for (const Box& b : c.boxes) {
                  product *= e.Class(b.class_id) + 1;
                }
                return product == c.val;
              },
              classes, *single_entry));

        } else {
          RETURN_IF_ERROR(AddPredicate(
              absl::StrCat("Box #", box_id),
              [c](const puzzle::Solution& s) {
                int product = 1;
                for (const Box& b : c.boxes) {
                  product *= s.Id(b.entry_id).Class(b.class_id) + 1;
                }
                return product == c.val;
              },
              classes));
        }
        break;
      }
      case Cage::kSub: {
        if (c.boxes.size() != 2) {
          return absl::InvalidArgumentError(
              absl::StrCat("Subtraction only for 2 boxes"));
        }
        if (single_entry) {
          RETURN_IF_ERROR(AddSpecificEntryPredicate(
              absl::StrCat("Box #", box_id),
              [c](const puzzle::Entry& e) {
                int b1 = e.Class(c.boxes[0].class_id) + 1;
                int b2 = e.Class(c.boxes[1].class_id) + 1;
                return b2 - b1 == c.val || b1 - b2 == c.val;
              },
              classes, *single_entry));

        } else {
          RETURN_IF_ERROR(AddPredicate(
              absl::StrCat("Box #", box_id),
              [c](const puzzle::Solution& s) {
                int b1 =
                    s.Id(c.boxes[0].entry_id).Class(c.boxes[0].class_id) + 1;
                int b2 =
                    s.Id(c.boxes[1].entry_id).Class(c.boxes[1].class_id) + 1;
                return b2 - b1 == c.val || b1 - b2 == c.val;
              },
              classes));
        }
        break;
      }
      case Cage::kDiv: {
        if (c.boxes.size() != 2) {
          return absl::InvalidArgumentError(
              absl::StrCat("Division only for 2 boxes"));
        }
        if (single_entry) {
          RETURN_IF_ERROR(AddSpecificEntryPredicate(
              absl::StrCat("Box #", box_id),
              [c](const puzzle::Entry& e) {
                int b1 = e.Class(c.boxes[0].class_id) + 1;
                int b2 = e.Class(c.boxes[1].class_id) + 1;
                return b1 == c.val * b2 || b2 == c.val * b1;
              },
              classes, *single_entry));

        } else {
          RETURN_IF_ERROR(AddPredicate(
              absl::StrCat("Box #", box_id),
              [c](const puzzle::Solution& s) {
                int b1 =
                    s.Id(c.boxes[0].entry_id).Class(c.boxes[0].class_id) + 1;
                int b2 =
                    s.Id(c.boxes[1].entry_id).Class(c.boxes[1].class_id) + 1;
                return b1 == c.val * b2 || b2 == c.val * b1;
              },
              classes));
        }
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