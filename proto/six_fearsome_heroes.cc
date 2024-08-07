/*
From:
http://io9.com/can-you-solve-this-extremely-difficult-star-trek-puzzle-1667991339

Grobly Grizik is planning to write a novel fashioned after Star Trek: The Next
Generation. in this novel, six of the crew members compete both at Fizzbin and
at Tridimensional chess. Each crew member gets two independent rankings for
ability at these games, with 1 ranked lowest and 6 highest. Every crew member
has a personal hero among the crew, and every crew member is afraid of some crew
member. Everyone is the hero of somebody, and everyone is feared by somebody.
Nobody either fears him/herself nor counts him/herself as a hero. Nobody fears
his/her own hero. From the given clues, discover every crew member's ranking at
Fizzbin and at Tri-D chess, as well as whom he/she fears and whom he/she counts
as a hero:

1. Geordi ranks 2 at Tri-D Chess.
2. Picard ranks two positions behind Troi at Fizzbin.
3. Troi is feared by the person Geordi fears.
4. Worf's hero ranks 3 times lower at Tri-D Chess than the crew member who is
   best at Fizzbin.
5. Picard's hero fears Geordi.
6. Data's hero is not Geordi.
7. Data is the hero of Riker's hero.
8. The person who is worst at Fizzbin is better than Troi at Tri-D Chess.
9. The person ranked number 3 at Tri-D Chess is ranked 4 positions higher than
   Data at Fizzbin.
10. Riker is feared by the person Picard fears and is the hero of Worf's hero.
11. Riker is ranked 2 lower at Tri-D Chess than the crew member ranked 2 at
    Fizzbin.

Calculated solution:
Picard: hero=Data fear=Troi trid=5 fizzbin=2
Riker: hero=Picard fear=Worf trid=3 fizzbin=5
Troi: hero=Worf fear=Riker trid=1 fizzbin=4
Geordi: hero=Riker fear=Picard trid=2 fizzbin=3
Data: hero=Troi fear=Geordi trid=4 fizzbin=1
Worf: hero=Geordi fear=Data trid=6 fizzbin=6

 */

#include <iostream>
#include <memory>

#include "proto/proto_problem.h"
#include "proto/six_fearsome_heroes.pb.h"

class SixFearsomeHeroes : public puzzle::ProtoProblem<SixFearsomeHeroesInfo> {
 private:
  using P = SixFearsomeHeroesInfo::Entry;
  enum Classes { HERO = 0, FEAR = 1, TRID = 2, FIZZBIN = 3 };

  absl::Status AddGeneralPredicates();
  absl::Status AddStatementPredicates();

  std::string solution_textproto() const override;
  absl::Status AddPredicates() override;
};

REGISTER_PROBLEM(SixFearsomeHeroes);

absl::Status SixFearsomeHeroes::AddGeneralPredicates() {
  RETURN_IF_ERROR(AddAllEntryPredicate(
      "Nobody either fears him/herself ...",
      [](const puzzle::Entry& e) { return e.Class(FEAR) != e.id(); }, {FEAR}));
  RETURN_IF_ERROR(AddAllEntryPredicate(
      "... nor counts him/herself as a hero.",
      [](const puzzle::Entry& e) { return e.Class(HERO) != e.id(); }, {HERO}));

  RETURN_IF_ERROR(AddAllEntryPredicate(
      "Nobody fears his/her own hero",
      [](const puzzle::Entry& e) { return e.Class(HERO) != e.Class(FEAR); },
      {HERO, FEAR}));

  return absl::OkStatus();
}

absl::Status SixFearsomeHeroes::AddStatementPredicates() {
  RETURN_IF_ERROR(AddSpecificEntryPredicate(
      "1. Geordi ranks 2 at Tri-D Chess",
      [](const puzzle::Entry& e) { return e.Class(TRID) == P::RANK_2; }, {TRID},
      P::GEORDI));

  RETURN_IF_ERROR(
      AddPredicate("2. Picard ranks two positions behind Troi at Fizzbin.",
                   [](const puzzle::SolutionView& s) {
                     return s.Id(P::PICARD).Class(FIZZBIN) ==
                            s.Id(P::TROI).Class(FIZZBIN) - 2;
                   },
                   {FIZZBIN}));

  RETURN_IF_ERROR(AddPredicate(
      "3. Troi is feared by the person Geordi fears.",
      [](const puzzle::SolutionView& s) {
        return s.Id(s.Id(P::GEORDI).Class(FEAR)).Class(FEAR) == P::TROI;
      },
      {FEAR}));

  RETURN_IF_ERROR(AddPredicate(
      "4. Worf's hero ranks 3 times lower at Tri-D Chess than "
      "the crew member who is best at Fizzbin.",
      [](const puzzle::SolutionView& s) {
        return (s.Id(s.Id(P::WORF).Class(HERO)).Class(TRID) + 1) * 3 ==
               s.Find([](const puzzle::Entry& e) {
                  return e.Class(FIZZBIN) == P::RANK_6;
                }).Class(TRID) +
                   1;
      },
      {HERO, TRID, FIZZBIN}));

  RETURN_IF_ERROR(AddPredicate(
      "5. Picard's hero fears Geordi.",
      [](const puzzle::SolutionView& s) {
        return s.Id(s.Id(P::PICARD).Class(HERO)).Class(FEAR) == P::GEORDI;
      },
      {HERO, FEAR}));

  RETURN_IF_ERROR(AddSpecificEntryPredicate(
      "6. Data's hero is not Geordi.",
      [](const puzzle::Entry& e) { return e.Class(HERO) != P::GEORDI; }, {HERO},
      P::DATA));

  RETURN_IF_ERROR(AddPredicate(
      "7. Data is the hero of Riker's hero.",
      [](const puzzle::SolutionView& s) {
        return s.Id(s.Id(P::RIKER).Class(HERO)).Class(HERO) == P::DATA;
      },
      {HERO}));

  RETURN_IF_ERROR(
      AddPredicate("8. The person who is worst at Fizzbin is better than Troi "
                   "at Tri-D Chess.",
                   [](const puzzle::SolutionView& s) {
                     return s.Id(P::TROI).Class(TRID) <
                            s.Find([](const puzzle::Entry& e) {
                               return e.Class(FIZZBIN) == P::RANK_1;
                             }).Class(TRID);
                   },
                   {TRID, FIZZBIN}));

  RETURN_IF_ERROR(AddPredicate(
      "9. The person ranked number 3 at Tri-D Chess is ranked 4 "
      "positions higher than Data at Fizzbin.",
      [](const puzzle::SolutionView& s) {
        return s.Find([](const puzzle::Entry& e) {
                  return e.Class(TRID) == P::RANK_3;
                }).Class(FIZZBIN) == 4 + s.Id(P::DATA).Class(FIZZBIN);
      },
      {TRID, FIZZBIN}));

  RETURN_IF_ERROR(AddPredicate(
      "10. Riker is feared by the person Picard fears...",
      [](const puzzle::SolutionView& s) {
        return s.Id(s.Id(P::PICARD).Class(FEAR)).Class(FEAR) == P::RIKER;
      },
      {FEAR}));

  RETURN_IF_ERROR(AddPredicate(
      "10(cont). ... and is the hero of Worf's hero.",
      [](const puzzle::SolutionView& s) {
        return s.Id(s.Id(P::WORF).Class(HERO)).Class(HERO) == P::RIKER;
      },
      {HERO}));

  RETURN_IF_ERROR(
      AddPredicate("11. Riker is ranked 2 lower at Tri-D Chess than the crew "
                   "member ranked 2 at Fizzbin.",
                   [](const puzzle::SolutionView& s) {
                     return s.Id(P::RIKER).Class(TRID) + 2 ==
                            s.Find([](const puzzle::Entry& e) {
                               return e.Class(FIZZBIN) == P::RANK_2;
                             }).Class(TRID);
                   },
                   {TRID, FIZZBIN}));

  return absl::OkStatus();
}

std::string SixFearsomeHeroes::solution_textproto() const {
  return R"pb(
    entry { id: PICARD hero: DATA fear: TROI trid: RANK_5 fizzbin: RANK_2 }
    entry { id: RIKER hero: PICARD fear: WORF trid: RANK_3 fizzbin: RANK_5 }
    entry { id: TROI hero: WORF fear: RIKER trid: RANK_1 fizzbin: RANK_4 }
    entry { id: GEORDI hero: RIKER fear: PICARD trid: RANK_2 fizzbin: RANK_3 }
    entry { id: DATA hero: TROI fear: GEORDI trid: RANK_4 fizzbin: RANK_1 }
    entry { id: WORF hero: GEORDI fear: DATA trid: RANK_6 fizzbin: RANK_6 }
  )pb";
}

absl::Status SixFearsomeHeroes::AddPredicates() {
  RETURN_IF_ERROR(AddGeneralPredicates());
  RETURN_IF_ERROR(AddStatementPredicates());
  return absl::OkStatus();
}
