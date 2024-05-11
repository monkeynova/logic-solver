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

class SwimmingPoolProblem : public puzzle::ProtoProblem<SwimmingPoolProblemInfo> {
 private:
  enum Who {
    BETTY = SwimmingPoolProblemInfo::Entry::BETTY,
    CAROL = SwimmingPoolProblemInfo::Entry::CAROL,
    DAISY = SwimmingPoolProblemInfo::Entry::DAISY,
    EMILY = SwimmingPoolProblemInfo::Entry::EMILY
  };
  enum Lane {
    ONE = SwimmingPoolProblemInfo::Entry::ONE,
    TWO = SwimmingPoolProblemInfo::Entry::TWO,
    THREE = SwimmingPoolProblemInfo::Entry::THREE,
    FOUR = SwimmingPoolProblemInfo::Entry::FOUR
  };
  enum Style {
    BACKSTROKE = SwimmingPoolProblemInfo::Entry::BACKSTROKE,
    BUTTERFLY = SwimmingPoolProblemInfo::Entry::BUTTERFLY,
    DOLPHIN = SwimmingPoolProblemInfo::Entry::DOLPHIN,
    FREESTYLE = SwimmingPoolProblemInfo::Entry::FREESTYLE
  };
  enum Country {
    USA = SwimmingPoolProblemInfo::Entry::USA,
    AUSTRALIA = SwimmingPoolProblemInfo::Entry::AUSTRALIA,
    UK = SwimmingPoolProblemInfo::Entry::UK,
    CANADA = SwimmingPoolProblemInfo::Entry::CANADA
  };
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
      [](const puzzle::Solution& s) {
        const puzzle::Entry& betty = s.Id(BETTY);
        const puzzle::Entry& from_uk = s.Find(
            [](const puzzle::Entry& e) { return e.Class(COUNTRY) == UK; });
        return IsNextTo(betty, from_uk) && betty.Class(STYLE) != BUTTERFLY &&
               from_uk.Class(STYLE) != BUTTERFLY;
      },
      {COUNTRY, STYLE, LANE}));

  RETURN_IF_ERROR(AddPredicate(
      "2. Among Emily and the Backstroker, one is from the UK "
      "and the other is in the fourth lane.",
      [](const puzzle::Solution& s) {
        const puzzle::Entry& emily = s.Id(EMILY);
        if (emily.Class(STYLE) == BACKSTROKE) return false;
        const puzzle::Entry& backstroker = s.Find([](const puzzle::Entry& e) {
          return e.Class(STYLE) == BACKSTROKE;
        });
        if (emily.Class(COUNTRY) == UK && backstroker.Class(LANE) == FOUR) {
          return true;
        }
        if (backstroker.Class(COUNTRY) == UK && emily.Class(LANE) == FOUR) {
          return true;
        }
        return false;
      },
      {COUNTRY, STYLE, LANE}));

  RETURN_IF_ERROR(AddSpecificEntryPredicate(
      "3. Carol is not swimming Backstroke nor Dolphin. She is "
      "not Australian, and is not swiming in lates #2 nor #4.",
      [](const puzzle::Entry& e) {
        return e.Class(STYLE) != BACKSTROKE && e.Class(STYLE) != DOLPHIN &&
               e.Class(COUNTRY) != AUSTRALIA && e.Class(LANE) != TWO &&
               e.Class(LANE) != FOUR;
      },
      {STYLE, COUNTRY, LANE}, CAROL));

  RETURN_IF_ERROR(AddPredicate(
      "4. The Freestyler is next to both Daisy and the American "
      "swimmer.",
      [](const puzzle::Solution& s) {
        return IsNextTo(s.Id(DAISY), s.Find([](const puzzle::Entry& e) {
                 return e.Class(STYLE) == FREESTYLE;
               })) &&
               IsNextTo(s.Find([](const puzzle::Entry& e) {
                 return e.Class(COUNTRY) == USA;
               }),
                        s.Find([](const puzzle::Entry& e) {
                          return e.Class(STYLE) == FREESTYLE;
                        }));
      },
      {STYLE, COUNTRY, LANE}));

  RETURN_IF_ERROR(AddPredicate(
      "5. The American swimmer is next to Carol.",
      [](const puzzle::Solution& s) {
        return IsNextTo(s.Id(CAROL), s.Find([](const puzzle::Entry& e) {
          return e.Class(COUNTRY) == USA;
        }));
      },
      {COUNTRY, LANE}));

  RETURN_IF_ERROR(AddSpecificEntryPredicate(
      "6. Daisy is not swimming in lane #2.",
      [](const puzzle::Entry& e) { return e.Class(LANE) != TWO; }, {LANE},
      DAISY));

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
