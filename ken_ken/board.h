#ifndef KEN_KEB_BOARD_H
#define KEN_KEB_BOARD_H

#include <cstdint>

#include "puzzle/problem.h"

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
  absl::StatusOr<std::vector<Cage>> GetCages() const;
  virtual absl::StatusOr<std::string_view> GetCageBoard() const = 0;

  virtual absl::Status AddCagePredicates();

 private:
  absl::Status Setup() final;
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

extern template class Board<4>;
extern template class Board<6>;
extern template class Board<9>;

}  // namespace ken_ken

#endif  // KEN_KEB_BOARD_H