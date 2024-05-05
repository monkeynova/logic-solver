#ifndef KEN_KEB_BOARD_H
#define KEN_KEB_BOARD_H

#include <cstdint>

#include "absl/strings/str_split.h"
#include "puzzle/problem.h"
#include "re2/re2.h"

namespace ken_ken {

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

  absl::StatusOr<std::vector<Cage>> GetCages() const;
  virtual absl::StatusOr<std::string_view> GetCageBoard() const = 0;

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
  for (int i = 0; i < kWidth; ++i) {
    for (int j = i + 1; j < kWidth; ++j) {
      absl::Status st = AddAllEntryPredicate(
          absl::StrCat("No row dupes (", i + 1, ", ", j + 1, ")"),
          [i, j](const puzzle::Entry& e) { return e.Class(i) != e.Class(j); },
          {i, j});
      if (!st.ok()) return st;
    }
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
  // kMinSums[i] = SUM(i) for i IN {1 .. i};
  constexpr int kMinSums[9] = {0, 1, 3, 6, 10, 15, 21, 28, 36};
  // kMaxSums[i] = SUM(i) for i IN {9 - i .. 9};
  constexpr int kMaxSums[9] = {0, 9, 17, 24, 30, 35, 39, 42, 44};

  int max_cage_val = val - kMinSums[boxes.size() - 1];
  if (max_cage_val < 9) {
    for (const Box& box : boxes) {
      RETURN_IF_ERROR(AddSpecificEntryPredicate(
          absl::StrCat("Cage max for ", box, " = ", max_cage_val),
          [box, max_cage_val](const puzzle::Entry& e) {
            // Value is 0 indexed.
            return e.Class(box.class_id) <= max_cage_val - 1;
          },
          {box.class_id}, box.entry_id));
    }
  }

  int min_cage_val = val - kMaxSums[boxes.size() - 1];
  if (min_cage_val > 1) {
    for (const Box& box : boxes) {
      RETURN_IF_ERROR(AddSpecificEntryPredicate(
          absl::StrCat("Cage min for ", box, " = ", min_cage_val),
          [box, min_cage_val](const puzzle::Entry& e) {
            // Value is 0 indexed.
            return e.Class(box.class_id) >= min_cage_val - 1;
          },
          {box.class_id}, box.entry_id));
    }
  }

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
  for (const Box& box : boxes) {
    if (boxes.size() < 2) continue;
    RETURN_IF_ERROR(AddSpecificEntryPredicate(
        absl::StrCat("Cage factors for ", box, " = ", val),
        [box, val](const puzzle::Entry& e) {
          // Value is 0 indexed.
          int factor = e.Class(box.class_id) + 1;
          return val % factor == 0;
        },
        {box.class_id}, box.entry_id));

    if (boxes.size() < 3) continue;
    for (const Box& box2 : boxes) {
      if (box.entry_id == box2.entry_id) {
        if (box.class_id == box2.class_id) continue;
        RETURN_IF_ERROR(AddSpecificEntryPredicate(
            absl::StrCat("Cage factors for ", box, " = ", val),
            [box, box2, val](const puzzle::Entry& e) {
              // Value is 0 indexed.
              int factor = e.Class(box.class_id) + 1;
              factor *= e.Class(box2.class_id) + 1;
              return val % factor == 0;
            },
            {box.class_id, box2.class_id}, box.entry_id));
      } else {
        std::vector<int> cols = {box.class_id};
        if (box2.class_id != box.class_id) cols.push_back(box2.class_id);
        RETURN_IF_ERROR(AddPredicate(
            absl::StrCat("Cage factors for ", box, " * ", box2, " = ", val),
            [box, box2, val](const puzzle::Solution& s) {
              // Value is 0 indexed.
              int factor = s.Id(box.entry_id).Class(box.class_id) + 1;
              factor *= s.Id(box2.entry_id).Class(box2.class_id) + 1;
              return val % factor == 0;
            },
            cols));
      }
    }
  }

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
  ASSIGN_OR_RETURN(std::vector<Cage> cages, GetCages());

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

template <int64_t kWidth>
absl::StatusOr<std::vector<typename Board<kWidth>::Cage>>
Board<kWidth>::GetCages() const {
  ASSIGN_OR_RETURN(std::string_view parsable, GetCageBoard());

  std::vector<Cage> ret;
  std::optional<int> board_line;
  for (std::string_view line : absl::StrSplit(parsable, "\n")) {
    if (line.empty()) {
      board_line = 0;
    } else if (board_line) {
      if (line.size() != kWidth) {
        return absl::InvalidArgumentError(
            absl::StrCat("Line too narrow: ", line));
      }
      for (int i = 0; i < kWidth; ++i) {
        int val = -1;
        if (line[i] >= '0' && line[i] <= '9') val = line[i] - '0';
        if (line[i] >= 'A' && line[i] <= 'Z') val = line[i] - 'A' + 10;
        if (val < 0) {
          return absl::InvalidArgumentError(
              absl::StrCat("Index too small: ", line, " @", i));
        }
        if (val > ret.size()) {
          return absl::InvalidArgumentError(
              absl::StrCat("Index too big: ", line, " @", i));
        }
        ret[val].boxes.push_back({*board_line, i});
      }
      ++*board_line;
    } else {
      int val;
      char op_char;
      if (!RE2::FullMatch(line, "(\\d+)([\\+\\-\\*\\/])", &val, &op_char)) {
        return absl::InvalidArgumentError(absl::StrCat("Bad line: ", line));
      }
      typename Cage::Op op;
      switch (op_char) {
        case '+':
          op = Cage::kAdd;
          break;
        case '-':
          op = Cage::kSub;
          break;
        case '*':
          op = Cage::kMul;
          break;
        case '/':
          op = Cage::kDiv;
          break;
        default:
          LOG(FATAL) << "Bad op_char";
      }
      ret.push_back(Cage{.val = val, .op = op});
    }
  }
  if (!board_line || *board_line != kWidth) {
    return absl::InvalidArgumentError("Board too short");
  }

  return ret;
}

}  // namespace ken_ken

#endif  // KEN_KEB_BOARD_H