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
#include "dracula_and_friends.pb.h"
#include "puzzle/problem.h"

using namespace DraculaAndFriends;

class DraculaAndFriendsProblem : public puzzle::Problem {
 private:
  void Setup() override;
  puzzle::Solution GetSolution() const override;

  void AddRulePredicates();
};

REGISTER_PROBLEM(DraculaAndFriendsProblem);

void DraculaAndFriendsProblem::AddRulePredicates() {
  AddPredicate(
      "1. One, and only one, of the vampires had the same initials "
      "of his name and of his birthplace.",
      [](const puzzle::Solution& s) {
        return 1 == ((s.Id(BOGDAN).Class(REGION) == BUCOVINA ? 1 : 0) +
                     (s.Id(DORIAN).Class(REGION) == DEBROGEA ? 1 : 0) +
                     (s.Id(MATEI).Class(REGION) == MUNTENIA ? 1 : 0) +
                     (s.Id(OCTAVIAN).Class(REGION) == OLTENIA ? 1 : 0));
        return true;
      },
      {REGION});
  AddSpecificEntryPredicate(
      "2. Matei wasn't from Debrogea. He hated onions or ivy.",
      // Edit "He hated" probably should be "He hated neither".
      MATEI,
      [](const puzzle::Entry& e) {
        if (e.Class(REGION) == DEBROGEA) return false;
        return e.Class(PLANTS) == ONION || e.Class(PLANTS) == IVY;
      },
      {REGION, PLANTS});
  AddPredicate(
      "3. The vampire from Mutenia lived 100 years after the "
      "vampire who hated thornbrush.",
      [](const puzzle::Solution& s) {
        int from_mutenia_century = s.Find([](const puzzle::Entry& e) {
                                      return e.Class(REGION) == MUNTENIA;
                                    }).Class(CENTURY);
        int hated_thornbrush_century = s.Find([](const puzzle::Entry& e) {
                                          return e.Class(PLANTS) == THORNBRUSH;
                                        }).Class(CENTURY);
        return hated_thornbrush_century == from_mutenia_century - 1;
      },
      {REGION, PLANTS, CENTURY});
  AddPredicate(
      "4. 100 years after Dorian's death, another vamipre rised "
      "in Bucovina, but this wasn't Bogdan.",
      [](const puzzle::Solution& s) {
        const puzzle::Entry& from_bucovina = s.Find(
            [](const puzzle::Entry& e) { return e.Class(REGION) == BUCOVINA; });
        return from_bucovina.id() != BOGDAN &&
               (from_bucovina.Class(CENTURY) ==
                s.Id(DORIAN).Class(CENTURY) + 1);
      },
      {REGION, CENTURY});
  AddSpecificEntryPredicate(
      "5. Octavain either lived in the XVI century or hated "
      "thornbrush.",
      OCTAVIAN,
      [](const puzzle::Entry& e) {
        const bool in_xvi = e.Class(CENTURY) == XVI;
        const bool hated_thornbrush = e.Class(PLANTS) == THORNBRUSH;
        return (in_xvi || hated_thornbrush) && !(in_xvi && hated_thornbrush);
      },
      {PLANTS, CENTURY});
  AddPredicate("6. If Bogdan hated wolfsbane, then Matei lived in Buchovia.",
               [](const puzzle::Solution& s) {
                 if (s.Id(BOGDAN).Class(PLANTS) == WOLFSBANE) {
                   return s.Id(MATEI).Class(REGION) == BUCOVINA;
                 }
                 return true;
               },
               {REGION, PLANTS});
  AddPredicate("7. The vampire from XIV century wasn't Octavian nor Bogdan.",
               [](const puzzle::Solution& s) {
                 return s.Id(OCTAVIAN).Class(CENTURY) != XIV &&
                        s.Id(BOGDAN).Class(CENTURY) != XIV;
               },
               {CENTURY});
  AddSpecificEntryPredicate(
      "8. Villagers didn't grow thornbrush against Dorian.", DORIAN,
      [](const puzzle::Entry& e) { return e.Class(PLANTS) != THORNBRUSH; },
      {PLANTS});
  AddPredicate(
      "9. Chronicles of XVII century claimed that ivy was "
      "ineffective and that Debrogea was free from vamipres.",
      [](const puzzle::Solution& s) {
        const puzzle::Entry& e = s.Find(
            [](const puzzle::Entry& e) { return e.Class(CENTURY) == XVII; });
        return e.Class(PLANTS) != IVY && e.Class(REGION) != DEBROGEA;
      },
      {REGION, PLANTS, CENTURY});
}

puzzle::Solution DraculaAndFriendsProblem::GetSolution() const {
  std::vector<puzzle::Entry> entries;
  // BOGDAN: region=MUNTENIA plants=IVY century=XVI
  entries.emplace_back(BOGDAN, std::vector<int>{MUNTENIA, IVY, XVI},
                       entry_descriptor());

  // DORIAN: region=DEBROGEA plants=WOLFSBANE century=XIV
  entries.emplace_back(DORIAN, std::vector<int>{DEBROGEA, WOLFSBANE, XIV},
                       entry_descriptor());

  // MATEI: region=OLTENIA plants=ONION century=XVII
  entries.emplace_back(MATEI, std::vector<int>{OLTENIA, ONION, XVII},
                       entry_descriptor());

  // OCTAVIAN: region=BUCOVINA plants=THORNBRUSH century=XV
  entries.emplace_back(OCTAVIAN, std::vector<int>{BUCOVINA, THORNBRUSH, XV},
                       entry_descriptor());

  return puzzle::Solution(entry_descriptor(), &entries).Clone();
}

void DraculaAndFriendsProblem::Setup() {
  SetIdentifiers(
      AddDescriptor(new puzzle::ProtoEnumDescriptor(Who_descriptor())));
  AddClass(REGION, "region",
           AddDescriptor(new puzzle::ProtoEnumDescriptor(Region_descriptor())));
  AddClass(PLANTS, "plants",
           AddDescriptor(new puzzle::ProtoEnumDescriptor(Plants_descriptor())));
  AddClass(
      CENTURY, "century",
      AddDescriptor(new puzzle::ProtoEnumDescriptor(Century_descriptor())));

  AddRulePredicates();
}
