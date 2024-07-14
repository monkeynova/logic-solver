/*
From: https://www.brainzilla.com/logic/logic-grid/swimming-pool/

There are four swimmers, each training their favorite style. Each one is from an
different country and is swimming alone in their lane. What kind of swimming
style is the Canadian practicing?

1. Betty is swimming next to the athlete from the UK. Neither of them is
   swimming Butterfly.
2. Among Emily and the Backstroker, one is from the UK and the other is in the
   fourth lane.
3. Carol is not swimming Backstroke nor Dolphin. She is not Australian, and is
   not swiming in lates #2 nor #4.
4. The Freestyler is next to both Daisy and the American swimmer.
5. The American swimmer is next to Carol.
6. Daisy is not swimming in lane #2.

Calculated solution:
Betty: style=Dolphin country=US lane=2
Carol: style=Butterfly country=CA lane=1
Daisy: style=Backstroke country=AU lane=4
Emily: style=Freestyle country=UK lane=3
 */
#include "proto/proto_problem.h"
#include "proto/swimming_pool.pb.h"

class SwimmingPoolProblem
    : public puzzle::ProtoProblem<SwimmingPoolProblemInfo> {
 private:
  using P = SwimmingPoolProblemInfo::Entry;
  enum Classes { LANE = 0, COUNTRY = 1, STYLE = 2 };

  static bool IsNextTo(const puzzle::Entry& e, const puzzle::Entry& b);

  absl::Status AddPredicates() override;

  std::string solution_textproto() const override;
};

// static
bool SwimmingPoolProblem::IsNextTo(const puzzle::Entry& a,
                                   const puzzle::Entry& b) {
  return fabs(a.Class(LANE) - b.Class(LANE)) == 1;
};

absl::Status SwimmingPoolProblem::AddPredicates() {
  RETURN_IF_ERROR(AddPredicate(
      "1. Betty is swimming next to the athlete from the UK. "
      "Neither of them is swimming Butterfly.",
      [](const puzzle::SolutionView& s) {
        const puzzle::Entry& betty = s.Id(P::BETTY);
        const puzzle::Entry& from_uk = s.Find(
            [](const puzzle::Entry& e) { return e.Class(COUNTRY) == P::UK; });
        return IsNextTo(betty, from_uk) && betty.Class(STYLE) != P::BUTTERFLY &&
               from_uk.Class(STYLE) != P::BUTTERFLY;
      },
      {COUNTRY, STYLE, LANE}));

  RETURN_IF_ERROR(AddPredicate(
      "2. Among Emily and the Backstroker, one is from the UK "
      "and the other is in the fourth lane.",
      [](const puzzle::SolutionView& s) {
        const puzzle::Entry& emily = s.Id(P::EMILY);
        if (emily.Class(STYLE) == P::BACKSTROKE) return false;
        const puzzle::Entry& backstroker = s.Find([](const puzzle::Entry& e) {
          return e.Class(STYLE) == P::BACKSTROKE;
        });
        if (emily.Class(COUNTRY) == P::UK &&
            backstroker.Class(LANE) == P::FOUR) {
          return true;
        }
        if (backstroker.Class(COUNTRY) == P::UK &&
            emily.Class(LANE) == P::FOUR) {
          return true;
        }
        return false;
      },
      {COUNTRY, STYLE, LANE}));

  RETURN_IF_ERROR(AddSpecificEntryPredicate(
      "3. Carol is not swimming Backstroke nor Dolphin. She is "
      "not Australian, and is not swiming in lates #2 nor #4.",
      [](const puzzle::Entry& e) {
        return e.Class(STYLE) != P::BACKSTROKE &&
               e.Class(STYLE) != P::DOLPHIN &&
               e.Class(COUNTRY) != P::AUSTRALIA && e.Class(LANE) != P::TWO &&
               e.Class(LANE) != P::FOUR;
      },
      {STYLE, COUNTRY, LANE}, P::CAROL));

  RETURN_IF_ERROR(AddPredicate(
      "4. The Freestyler is next to both Daisy and the American "
      "swimmer.",
      [](const puzzle::SolutionView& s) {
        return IsNextTo(s.Id(P::DAISY), s.Find([](const puzzle::Entry& e) {
                 return e.Class(STYLE) == P::FREESTYLE;
               })) &&
               IsNextTo(s.Find([](const puzzle::Entry& e) {
                 return e.Class(COUNTRY) == P::USA;
               }),
                        s.Find([](const puzzle::Entry& e) {
                          return e.Class(STYLE) == P::FREESTYLE;
                        }));
      },
      {STYLE, COUNTRY, LANE}));

  RETURN_IF_ERROR(AddPredicate(
      "5. The American swimmer is next to Carol.",
      [](const puzzle::SolutionView& s) {
        return IsNextTo(s.Id(P::CAROL), s.Find([](const puzzle::Entry& e) {
          return e.Class(COUNTRY) == P::USA;
        }));
      },
      {COUNTRY, LANE}));

  RETURN_IF_ERROR(AddSpecificEntryPredicate(
      "6. Daisy is not swimming in lane #2.",
      [](const puzzle::Entry& e) { return e.Class(LANE) != P::TWO; }, {LANE},
      P::DAISY));

  return absl::OkStatus();
}

std::string SwimmingPoolProblem::solution_textproto() const {
  return R"pb(
    entry { id: BETTY lane: TWO country: USA style: DOLPHIN }
    entry { id: CAROL lane: ONE country: CANADA style: BUTTERFLY }
    entry { id: DAISY lane: FOUR country: AUSTRALIA style: BACKSTROKE }
    entry { id: EMILY lane: THREE country: UK style: FREESTYLE }
  )pb";
}

REGISTER_PROBLEM(SwimmingPoolProblem);
