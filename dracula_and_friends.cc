/*
From: https://www.brainzilla.com/logic/logic-grid/dracula-and-friends/

Various vampires lived in distinct regions of Romania over different centuries.
Which plant did each of them fear the most?

1. One, and only one, of the vampires had the same initials of his name and of
    his birthplace.
2. Matei wasn't from Debrogea. He hated onions or ivy.
3. The vampire from Mutenia lived 100 years after the vampire who hated
   thornbrush.
4. 100 years after Dorian's death, another vamipre rised in Buchovina, but this
   wasn't Bogdan.
5. Octavain either lived in the XVI century or hated thornbrush.
6. If Bogdan hated wolfsbane, then Matei lived in Buchovia.
7. The vampire from XIV century wasn't Octavian nor Bogdan.
8. Villagers didn't grow thornbrush against Dorian.
9. Chronicles of XVII century claimed that ivy was ineffective and that Dobrogea
   was free from vamipres.

Calculated solution:
BOGDAN: region=MUNTENIA plants=IVY century=XVI
DORIAN: region=DEBROGEA plants=WOLFSBANE century=XIV
MATEI: region=OLTENIA plants=ONION century=XVII
OCTAVIAN: region=BUCOVINA plants=THORNBRUSH century=XV
*/
#include "puzzle/solver.h"
#include "dracula_and_friends.pb.h"

using namespace DraculaAndFriends;

void AddRulePredicates(puzzle::Solver* s) {
  s->AddPredicate("1. One, and only one, of the vampires had the same initials "
		  "of his name and of his birthplace.",
		  [](const puzzle::Solution& s) {
		    return 1 == (
			(s.Id(BOGDAN).Class(REGION) == BUCOVINA ? 1 : 0) +
			(s.Id(DORIAN).Class(REGION) == DEBROGEA ? 1 : 0) +
			(s.Id(MATEI).Class(REGION) == MUNTENIA ? 1 : 0) +
			(s.Id(OCTAVIAN).Class(REGION) == OLTENIA ? 1 : 0));
		    return true;
		  },
		  REGION);
  s->AddPredicate("2. Matei wasn't from Debrogea. He hated onions or ivy.",
		  // Edit "He hated" probably should be "He hated neither".
		  [](const puzzle::Solution& s) {
		    if (s.Id(MATEI).Class(REGION) == DEBROGEA) return false;
		    return s.Id(MATEI).Class(PLANTS) == ONION ||
		      s.Id(MATEI).Class(PLANTS) == IVY;
		  },
		  {REGION, PLANTS});
  s->AddPredicate("3. The vampire from Mutenia lived 100 years after the "
		  "vampire who hated thornbrush.",
		  [](const puzzle::Solution& s) {
		    int from_mutenia_century = s.Find(
		        [](const puzzle::Entry& e) {
			  return e.Class(REGION) == MUNTENIA;
			}).Class(CENTURY);
		    int hated_thornbrush_century = s.Find(
		        [](const puzzle::Entry& e) {
			  return e.Class(PLANTS) == THORNBRUSH;
			}).Class(CENTURY);
		    return hated_thornbrush_century == from_mutenia_century - 1;
		  },
		  {REGION, PLANTS, CENTURY});
  s->AddPredicate("4. 100 years after Dorian's death, another vamipre rised "
		  "in Bucovina, but this wasn't Bogdan.",
		  [](const puzzle::Solution& s) {
		    const puzzle::Entry& from_bucovina = s.Find(
		        [](const puzzle::Entry& e) {
			  return e.Class(REGION) == BUCOVINA;
			});
		    return from_bucovina.id() != BOGDAN &&
		      (from_bucovina.Class(CENTURY) ==
		       s.Id(DORIAN).Class(CENTURY) + 1);
		  },
		  {REGION, PLANTS, CENTURY});
  s->AddPredicate("5. Octavain either lived in the XVI century or hated "
		  "thornbrush.",
		  [](const puzzle::Solution& s) {
		    const bool in_xvi = s.Id(OCTAVIAN).Class(CENTURY) == XVI;
		    const bool hated_thornbrush =
		      s.Id(OCTAVIAN).Class(PLANTS) == THORNBRUSH;
		    return (in_xvi || hated_thornbrush) &&
		      !(in_xvi && hated_thornbrush);
		  },
		  {PLANTS, CENTURY});
  s->AddPredicate("6. If Bogdan hated wolfsbane, then Matei lived in Buchovia.",
		  [](const puzzle::Solution& s) {
		    if (s.Id(BOGDAN).Class(PLANTS) == WOLFSBANE) {
		      return s.Id(MATEI).Class(REGION) == BUCOVINA;
		    }
		    return true;
		  },
		  {REGION, PLANTS});
  s->AddPredicate("7. The vampire from XIV century wasn't Octavian nor Bogdan.",
		  [](const puzzle::Solution& s) {
		    return s.Id(OCTAVIAN).Class(CENTURY) != XIV &&
		      s.Id(BOGDAN).Class(CENTURY) != XIV;
		  },
		  CENTURY);
  s->AddPredicate("8. Villagers didn't grow thornbrush against Dorian.",
		  [](const puzzle::Solution& s) {
		    return s.Id(DORIAN).Class(PLANTS) != THORNBRUSH;
		  },
		  PLANTS);
  s->AddPredicate("9. Chronicles of XVII century claimed that ivy was "
		  "ineffective and that Debrogea was free from vamipres.",
		  [](const puzzle::Solution& s) {
		    const puzzle::Entry& e = s.Find([](const puzzle::Entry& e) {
			return e.Class(CENTURY) == XVII;
		      });
		    return e.Class(PLANTS) != IVY &&
		      e.Class(REGION) != DEBROGEA;
		  },
		  {REGION, PLANTS, CENTURY});
}

puzzle::Solution ProblemSolution(const puzzle::Solver& s) {
  std::vector<puzzle::Entry> entries;
  // BOGDAN: region=MUNTENIA plants=IVY century=XVI
  entries.emplace_back(
      BOGDAN,
      std::vector<int>{MUNTENIA, IVY, XVI},
      s.entry_descriptor());

  // DORIAN: region=DEBROGEA plants=WOLFSBANE century=XIV
  entries.emplace_back(
      DORIAN,
      std::vector<int>{DEBROGEA, WOLFSBANE, XIV},
      s.entry_descriptor());

  // MATEI: region=OLTENIA plants=ONION century=XVII
  entries.emplace_back(
      MATEI,
      std::vector<int>{OLTENIA, ONION, XVII},
      s.entry_descriptor());

  // OCTAVIAN: region=BUCOVINA plants=THORNBRUSH century=XV
  entries.emplace_back(
      OCTAVIAN,
      std::vector<int>{BUCOVINA, THORNBRUSH, XV},
      s.entry_descriptor());

  return puzzle::Solution(s.entry_descriptor(), &entries).Clone();
}

void SetupProblem(puzzle::Solver* s) {
  s->SetIdentifiers(s->AddDescriptor(
      new puzzle::ProtoEnumDescriptor(Who_descriptor())));
  s->AddClass(REGION, "region",
              s->AddDescriptor(
                  new puzzle::ProtoEnumDescriptor(Region_descriptor())));
  s->AddClass(PLANTS, "plants",
              s->AddDescriptor(
                  new puzzle::ProtoEnumDescriptor(Plants_descriptor())));
  s->AddClass(CENTURY, "century",
              s->AddDescriptor(
                  new puzzle::ProtoEnumDescriptor(Century_descriptor())));

  AddRulePredicates(s);
}
