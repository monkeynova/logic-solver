#ifndef KEN_KEN_KEN_KEN_H
#define KEN_KEN_KEN_KEN_H

#include <cstdint>

#include "ken_ken/grid.h"

namespace ken_ken {

template <int64_t kWidth>
class KenKen : public Grid<kWidth> {
 public:
  using Box = Grid<kWidth>::Box;
  using ::puzzle::Solver::AddSpecificEntryPredicate;
  using ::puzzle::Solver::AddPredicate;

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

  KenKen() = default;

 protected:
  absl::StatusOr<std::vector<Cage>> GetCages() const;
  virtual absl::StatusOr<std::string_view> GetCageKenKen() const = 0;

  virtual absl::Status AddCagePredicates();

 private:
  absl::Status AddGridPredicates() final;

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
};

extern template class KenKen<4>;
extern template class KenKen<6>;
extern template class KenKen<9>;

}  // namespace ken_ken

#endif  // KEN_KEN_KEN_KEN_H