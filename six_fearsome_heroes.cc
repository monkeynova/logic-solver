/*
From: http://io9.com/can-you-solve-this-extremely-difficult-star-trek-puzzle-1667991339

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

#include "puzzle/solver.h"

enum Who { 
    PICARD = 0,
    RIKER = 1,
    TROI = 2,
    GEORDI = 3,
    DATA = 4,
    WORF = 5
};

enum Classes {
    HERO = 0,
    FEAR = 1,
    TRID = 2,
    FIZZBIN = 3
};

void AddProblemPredicates(puzzle::Solver* s) {
    s->AddPredicate("Nobody either fears him/herself ...",
                    [](const puzzle::Entry& e) {
		      return e.Class(FEAR) != e.id();
		    }, 
		    FEAR);
    s->AddPredicate("... nor counts him/herself as a hero.",
		    [](const puzzle::Entry& e) {
		      return e.Class(HERO) != e.id();
		    },
		    HERO);

    s->AddPredicate("Nobody fears his/her own hero",
		    [](const puzzle::Entry& e) {
		      return e.Class(HERO) != e.Class(FEAR);
		    },
		    {HERO, FEAR});
}

void AddRulePredicates(puzzle::Solver* s) {
    s->AddPredicate("1. Geordi ranks 2 at Tri-D Chess",
		    [](const puzzle::Solution& s) {
		      return s.Id(GEORDI).Class(TRID) == 2;
		    },
		    TRID);
    
    s->AddPredicate("2. Picard ranks two positions behind Troi at Fizzbin.",
		    [](const puzzle::Solution& s) {
		      return s.Id(PICARD).Class(FIZZBIN) == 
			s.Id(TROI).Class(FIZZBIN) - 2;
		    },
		    FIZZBIN);

    s->AddPredicate("3. Troi is feared by the person Geordi fears.",
		    [](const puzzle::Solution& s) {
		      return s.Id(s.Id(GEORDI).Class(FEAR)).Class(FEAR) == TROI;
		    },
		    FEAR);

    s->AddPredicate("4. Worf's hero ranks 3 times lower at Tri-D Chess than "
		    "the crew member who is best at Fizzbin.",
		    [](const puzzle::Solution& s) {
		      return s.Id(s.Id(WORF).Class(HERO)).Class(TRID) * 3 == 
			s.Find([](const puzzle::Entry& e) {
			    return e.Class(FIZZBIN) == 6;
			  }).Class(TRID);
		    },
		    {HERO, TRID, FIZZBIN});

    s->AddPredicate("5. Picard's hero fears Geordi.",
		    [](const puzzle::Solution& s) {
		      return s.Id(s.Id(PICARD).Class(HERO)).Class(FEAR) == 
			GEORDI;
		    },
		    {HERO, FEAR});

    s->AddPredicate("6. Data's hero is not Geordi.",
		    [](const puzzle::Solution& s) {
		      return s.Id(DATA).Class(HERO) != GEORDI;
		    },
		    HERO);

    s->AddPredicate("7. Data is the hero of Riker's hero.",
		    [](const puzzle::Solution& s) {
		      return s.Id(s.Id(RIKER).Class(HERO)).Class(HERO) == DATA;
		    },
		    HERO);

    s->AddPredicate("8. The person who is worst at Fizzbin is better than Troi "
		    "at Tri-D Chess.",
		    [](const puzzle::Solution& s) {
		      return s.Id(TROI).Class(TRID) < 
			s.Find([](const puzzle::Entry& e) {
			    return e.Class(FIZZBIN) == 1;
			  }).Class(TRID);
		    },
		    {TRID, FIZZBIN});

    s->AddPredicate("9. The person ranked number 3 at Tri-D Chess is ranked 4 "
		    "positions higher than Data at Fizzbin.",
		    [](const puzzle::Solution& s) {
		      return s.Find([](const puzzle::Entry& e) {
			  return e.Class(TRID) == 3;
			}).Class(FIZZBIN) == 4 + s.Id(DATA).Class(FIZZBIN);
		    },
		    {TRID, FIZZBIN});

    s->AddPredicate("10. Riker is feared by the person Picard fears...",
		    [](const puzzle::Solution& s) {
		      return s.Id(s.Id(PICARD).Class(FEAR)).Class(FEAR) == 
			RIKER;
		    },
		    FEAR);

    s->AddPredicate("10(cont). ... and is the hero of Worf's hero.",
		    [](const puzzle::Solution& s) {
		      return s.Id(s.Id(WORF).Class(HERO)).Class(HERO) ==
			RIKER;
		    },
		    HERO);

    s->AddPredicate("11. Riker is ranked 2 lower at Tri-D Chess than the crew "
		    "member ranked 2 at Fizzbin.",
		    [](const puzzle::Solution& s) {
		      return s.Id(RIKER).Class(TRID) + 2 ==
			s.Find([](const puzzle::Entry& e) {
			    return e.Class(FIZZBIN) == 2;
			  }).Class(TRID);
		    },
		    {TRID, FIZZBIN});
}

puzzle::Solution ProblemSolution(const puzzle::Solver& s) {
  std::vector<puzzle::Entry> entries_;
  // Picard: hero=Data fear=Troi trid=5 fizzbin=2
  entries_.emplace_back(
      PICARD,
      std::vector<int>{DATA, TROI, 5, 2},
      s.entry_descriptor());
  
  // Riker: hero=Picard fear=Worf trid=3 fizzbin=5
  entries_.emplace_back(
      RIKER,
      std::vector<int>{PICARD, WORF, 3, 5},
      s.entry_descriptor());

  // Troi: hero=Worf fear=Riker trid=1 fizzbin=4
  entries_.emplace_back(
      TROI,
      std::vector<int>{WORF, RIKER, 1, 4},
      s.entry_descriptor());

  // Geordi: hero=Riker fear=Picard trid=2 fizzbin=3
  entries_.emplace_back(
      GEORDI,
      std::vector<int>{RIKER, PICARD, 2, 3},
      s.entry_descriptor());
  
  // Data: hero=Troi fear=Geordi trid=4 fizzbin=1
  entries_.emplace_back(
      DATA,
      std::vector<int>{TROI, GEORDI, 4, 1},
      s.entry_descriptor());
  
  // Worf: hero=Geordi fear=Data trid=6 fizzbin=6
  entries_.emplace_back(
      WORF,
      std::vector<int>{GEORDI, DATA, 6, 6},
      s.entry_descriptor());
  
  return puzzle::Solution(&entries_).Clone();
}

void SetupProblem(puzzle::Solver* s) {
  puzzle::StringDescriptor *who_descriptor = s->AddDescriptor(
      new puzzle::StringDescriptor());

  who_descriptor->SetDescription(PICARD, "Picard");
  who_descriptor->SetDescription(RIKER, "Riker");
  who_descriptor->SetDescription(TROI, "Troi");
  who_descriptor->SetDescription(GEORDI, "Geordi");
  who_descriptor->SetDescription(DATA, "Data");
  who_descriptor->SetDescription(WORF, "Worf");
  
  s->SetIdentifiers(who_descriptor);
  s->AddClass(HERO, "hero", who_descriptor);
  s->AddClass(FEAR, "fear", who_descriptor);
  
  puzzle::IntRangeDescriptor *ranking_descriptor = s->AddDescriptor(
      new puzzle::IntRangeDescriptor(1, 6));
  
  s->AddClass(TRID, "trid", ranking_descriptor);
  s->AddClass(FIZZBIN, "fizzbin", ranking_descriptor);

  AddProblemPredicates(s);
  AddRulePredicates(s);
}
