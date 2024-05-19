#ifndef KEN_KEN_GRID_H
#define KEN_KEN_GRID_H

#include <cstdint>

#include "puzzle/problem.h"

namespace ken_ken {

template <int64_t kWidth>
class Grid : public puzzle::Problem {
 public:
  struct Box {
    int entry_id;
    int class_id;

    void Transpose() {
      std::swap(entry_id, class_id);
    }

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

  Grid() : puzzle::Problem(MakeEntryDescriptor()) {}

 protected:
  enum class Orientation {
    kDefault = 0,
    kTranspose = 1,
  };
  virtual absl::Status AddGridPredicates(Orientation o) = 0;

 private:
  absl::StatusOr<puzzle::Solution> TransformAlternate(puzzle::Solution in, AlternateId alternate) const final;

  absl::Status Setup() final;

  static puzzle::EntryDescriptor MakeEntryDescriptor();

  puzzle::Solver::AlternateId default_id_;
  puzzle::Solver::AlternateId transpose_id_;
};

extern template class Grid<4>;
extern template class Grid<6>;
extern template class Grid<9>;

}  // namespace ken_ken

#endif  // KEN_KEN_GRID_H