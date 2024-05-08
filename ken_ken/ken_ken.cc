#include "ken_ken/ken_ken.h"

#include "absl/strings/str_split.h"
#include "re2/re2.h"

namespace ken_ken {

template <int64_t kWidth>
absl::Status KenKen<kWidth>::AddGridPredicates() {
  return AddCagePredicates();
}

template <int64_t kWidth>
bool KenKen<kWidth>::IsContiguous(const Cage& c) {
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
absl::Status KenKen<kWidth>::AddSumPredicate(int val,
                                             const std::vector<Box>& boxes,
                                             int box_id,
                                             const std::vector<int>& classes,
                                             std::optional<int> single_entry) {
  if (true) {
    // TODO: This is copy&paste-d from killer_sudoku.cc. This should be common,
    // but that kinda requires that they use the same definition of 'Box' and
    // maybe the same problem.
    std::vector<int> count_by_entry(kWidth, 0);
    std::vector<int> count_by_class(kWidth, 0);
    for (const Box& box : boxes) {
      ++count_by_entry[box.entry_id];
      ++count_by_class[box.class_id];
    }
    int min_by_entry = 0;
    int min_by_class = 0;
    int max_by_entry = 0;
    int max_by_class = 0;
    for (int i = 0; i < kWidth; ++i) {
      min_by_entry += count_by_entry[i] * (count_by_entry[i] + 1) / 2;
      min_by_class += count_by_class[i] * (count_by_class[i] + 1) / 2;
      max_by_entry += count_by_entry[i] * (count_by_entry[i] + 1) / 2 +
                      (kWidth - count_by_entry[i]) * count_by_entry[i];
      max_by_class += count_by_class[i] * (count_by_class[i] + 1) / 2 +
                      (kWidth - count_by_class[i]) * count_by_class[i];
    }
    int min_cage = std::min(min_by_entry, min_by_class);
    int max_cage = std::min(max_by_entry, max_by_class);

    for (const Box& box : boxes) {
      int biggest_remove =
          std::max(count_by_entry[box.entry_id], count_by_class[box.class_id]);
      int max_cage_val = val - (min_cage - biggest_remove);
      if (max_cage_val < 9) {
        RETURN_IF_ERROR(AddSpecificEntryPredicate(
            absl::StrCat("Cage max for ", box, " = ", max_cage_val),
            [box, max_cage_val](const puzzle::Entry& e) {
              // Value is 0 indexed.
              return e.Class(box.class_id) <= max_cage_val - 1;
            },
            {box.class_id}, box.entry_id));
      }

      int smallest_remove = kWidth + 1 - biggest_remove;
      int min_cage_val = val - (max_cage - smallest_remove);
      if (min_cage_val > 1) {
        RETURN_IF_ERROR(AddSpecificEntryPredicate(
            absl::StrCat("Cage min for ", box, " = ", min_cage_val),
            [box, min_cage_val](const puzzle::Entry& e) {
              // Value is 0 indexed.
              return e.Class(box.class_id) >= min_cage_val - 1;
            },
            {box.class_id}, box.entry_id));
      }
    }

    // TODO(@monkeynova): There are likely other restrictions possible. For
    // example a SUM of 4 precludes the value 2 from either given the sudoku
    // constraints.
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
absl::Status KenKen<kWidth>::AddMulPredicate(int val,
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
absl::Status KenKen<kWidth>::AddSubPredicate(int val,
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
absl::Status KenKen<kWidth>::AddDivPredicate(int val,
                                             const std::vector<Box>& boxes,
                                             int box_id,
                                             const std::vector<int>& classes,
                                             std::optional<int> single_entry) {
  if (boxes.size() != 2) {
    return absl::InvalidArgumentError(
        absl::StrCat("Division only for 2 boxes"));
  }
  // Bitmap of values that can be either numerator or denominator.
  int64_t allowed_by_bit = (1 << kWidth) - 1;
  for (int i = 0; i < kWidth; ++i) {
    if ((i + 1) * val > kWidth && (i + 1) % val != 0) {
      allowed_by_bit &= ~(1 << i);
    }
  }
  RETURN_IF_ERROR(AddSpecificEntryPredicate(
      absl::StrCat("Div Allowed (1) #", box_id),
      [allowed_by_bit, boxes](const puzzle::Entry& e) {
        return allowed_by_bit & (1 << e.Class(boxes[0].class_id));
      },
      {boxes[0].class_id}, boxes[0].entry_id));
  RETURN_IF_ERROR(AddSpecificEntryPredicate(
      absl::StrCat("Div Allowed (2) #", box_id),
      [allowed_by_bit, boxes](const puzzle::Entry& e) {
        return allowed_by_bit & (1 << e.Class(boxes[1].class_id));
      },
      {boxes[1].class_id}, boxes[1].entry_id));

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
absl::Status KenKen<kWidth>::AddCagePredicates() {
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
absl::StatusOr<std::vector<typename KenKen<kWidth>::Cage>>
KenKen<kWidth>::GetCages() const {
  ASSIGN_OR_RETURN(std::string_view parsable, GetCageKenKen());

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
    return absl::InvalidArgumentError("KenKen too short");
  }

  return ret;
}

template class KenKen<4>;
template class KenKen<6>;
template class KenKen<9>;

}  // namespace ken_ken
