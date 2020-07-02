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

#include "puzzle/proto_problem.h"
#include "six_fearsome_heroes.pb.h"

class SixFearsomeHeroes : public puzzle::ProtoProblem {
 private:
  enum Who {
    PICARD = SixFearsomeHeroesInfo::Entry::PICARD,
    RIKER = SixFearsomeHeroesInfo::Entry::RIKER,
    TROI = SixFearsomeHeroesInfo::Entry::TROI,
    GEORDI = SixFearsomeHeroesInfo::Entry::GEORDI,
    DATA = SixFearsomeHeroesInfo::Entry::DATA,
    WORF = SixFearsomeHeroesInfo::Entry::WORF
  };

  // RANK_1 is considered "worst" with RANK_6 considered "best".
  enum Ranking {
    RANK_1 = SixFearsomeHeroesInfo::Entry::RANK_1,
    RANK_2 = SixFearsomeHeroesInfo::Entry::RANK_2,
    RANK_3 = SixFearsomeHeroesInfo::Entry::RANK_3,
    RANK_4 = SixFearsomeHeroesInfo::Entry::RANK_4,
    RANK_5 = SixFearsomeHeroesInfo::Entry::RANK_5,
    RANK_6 = SixFearsomeHeroesInfo::Entry::RANK_6,
  };

  enum Classes { HERO = 0, FEAR = 1, TRID = 2, FIZZBIN = 3 };

  void AddGeneralPredicates();
  void AddStatementPredicates();

  const google::protobuf::Descriptor* problem_descriptor() const override {
    return SixFearsomeHeroesInfo::descriptor();
  }
  std::string solution_textproto() const override;
  void AddPredicates() override;
};

REGISTER_PROBLEM(SixFearsomeHeroes);

void SixFearsomeHeroes::AddGeneralPredicates() {
  AddAllEntryPredicate(
      "Nobody either fears him/herself ...",
      [](const puzzle::Entry& e) { return e.Class(FEAR) != e.id(); }, {FEAR});
  AddAllEntryPredicate(
      "... nor counts him/herself as a hero.",
      [](const puzzle::Entry& e) { return e.Class(HERO) != e.id(); }, {HERO});

  AddAllEntryPredicate(
      "Nobody fears his/her own hero",
      [](const puzzle::Entry& e) { return e.Class(HERO) != e.Class(FEAR); },
      {HERO, FEAR});
}

void SixFearsomeHeroes::AddStatementPredicates() {
  AddSpecificEntryPredicate(
      "1. Geordi ranks 2 at Tri-D Chess",
      [](const puzzle::Entry& e) { return e.Class(TRID) == RANK_2; }, {TRID},
      GEORDI);

  AddPredicate("2. Picard ranks two positions behind Troi at Fizzbin.",
               [](const puzzle::Solution& s) {
                 return s.Id(PICARD).Class(FIZZBIN) ==
                        s.Id(TROI).Class(FIZZBIN) - 2;
               },
               {FIZZBIN});

  AddPredicate("3. Troi is feared by the person Geordi fears.",
               [](const puzzle::Solution& s) {
                 return s.Id(s.Id(GEORDI).Class(FEAR)).Class(FEAR) == TROI;
               },
               {FEAR});

  AddPredicate(
      "4. Worf's hero ranks 3 times lower at Tri-D Chess than "
      "the crew member who is best at Fizzbin.",
      [](const puzzle::Solution& s) {
        return (s.Id(s.Id(WORF).Class(HERO)).Class(TRID) + 1) * 3 ==
               s.Find([](const puzzle::Entry& e) {
                  return e.Class(FIZZBIN) == RANK_6;
                }).Class(TRID) + 1;
      },
      {HERO, TRID, FIZZBIN});

  AddPredicate("5. Picard's hero fears Geordi.",
               [](const puzzle::Solution& s) {
                 return s.Id(s.Id(PICARD).Class(HERO)).Class(FEAR) == GEORDI;
               },
               {HERO, FEAR});

  AddSpecificEntryPredicate(
      "6. Data's hero is not Geordi.",
      [](const puzzle::Entry& e) { return e.Class(HERO) != GEORDI; }, {HERO},
      DATA);

  AddPredicate("7. Data is the hero of Riker's hero.",
               [](const puzzle::Solution& s) {
                 return s.Id(s.Id(RIKER).Class(HERO)).Class(HERO) == DATA;
               },
               {HERO});

  AddPredicate(
      "8. The person who is worst at Fizzbin is better than Troi "
      "at Tri-D Chess.",
      [](const puzzle::Solution& s) {
        return s.Id(TROI).Class(TRID) < s.Find([](const puzzle::Entry& e) {
                                           return e.Class(FIZZBIN) == RANK_1;
                                         }).Class(TRID);
      },
      {TRID, FIZZBIN});

  AddPredicate(
      "9. The person ranked number 3 at Tri-D Chess is ranked 4 "
      "positions higher than Data at Fizzbin.",
      [](const puzzle::Solution& s) {
        return s.Find([](const puzzle::Entry& e) {
		  return e.Class(TRID) == RANK_3;
                }).Class(FIZZBIN) == 4 + s.Id(DATA).Class(FIZZBIN);
      },
      {TRID, FIZZBIN});

  AddPredicate("10. Riker is feared by the person Picard fears...",
               [](const puzzle::Solution& s) {
                 return s.Id(s.Id(PICARD).Class(FEAR)).Class(FEAR) == RIKER;
               },
               {FEAR});

  AddPredicate("10(cont). ... and is the hero of Worf's hero.",
               [](const puzzle::Solution& s) {
                 return s.Id(s.Id(WORF).Class(HERO)).Class(HERO) == RIKER;
               },
               {HERO});

  AddPredicate(
      "11. Riker is ranked 2 lower at Tri-D Chess than the crew "
      "member ranked 2 at Fizzbin.",
      [](const puzzle::Solution& s) {
        return s.Id(RIKER).Class(TRID) + 2 ==
               s.Find([](const puzzle::Entry& e) {
                  return e.Class(FIZZBIN) == RANK_2;
                }).Class(TRID);
      },
      {TRID, FIZZBIN});
}

std::string SixFearsomeHeroes::solution_textproto() const {
  return R"PROTO(
    entry { id: PICARD hero: DATA fear: TROI trid: RANK_5 fizzbin: RANK_2 }
    entry { id: RIKER hero: PICARD fear: WORF trid: RANK_3 fizzbin: RANK_5 }
    entry { id: TROI hero: WORF fear: RIKER trid: RANK_1 fizzbin: RANK_4 }
    entry { id: GEORDI hero: RIKER fear: PICARD trid: RANK_2 fizzbin: RANK_3 }
    entry { id: DATA hero: TROI fear: GEORDI trid: RANK_4 fizzbin: RANK_1 }
    entry { id: WORF hero: GEORDI fear: DATA trid: RANK_6 fizzbin: RANK_6 }
  )PROTO";
}

void SixFearsomeHeroes::AddPredicates() {
  AddGeneralPredicates();
  AddStatementPredicates();
}
