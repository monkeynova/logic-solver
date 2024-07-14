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
#include "proto/dracula_and_friends.pb.h"
#include "proto/proto_problem.h"

class DraculaAndFriendsProblem
    : public puzzle::ProtoProblem<DraculaAndFriendsInfo> {
 private:
  using P = DraculaAndFriendsInfo::Entry;
  enum Classes {
    REGION = 0,
    PLANT = 1,
    CENTURY = 2,
  };

  absl::Status AddPredicates() override;
  std::string solution_textproto() const override;
};

absl::Status DraculaAndFriendsProblem::AddPredicates() {
  RETURN_IF_ERROR(AddPredicate(
      "1. One, and only one, of the vampires had the same initials "
      "of his name and of his birthplace.",
      [](const puzzle::SolutionView& s) {
        return 1 == ((s.Id(P::BOGDAN).Class(REGION) == P::BUCOVINA ? 1 : 0) +
                     (s.Id(P::DORIAN).Class(REGION) == P::DEBROGEA ? 1 : 0) +
                     (s.Id(P::MATEI).Class(REGION) == P::MUNTENIA ? 1 : 0) +
                     (s.Id(P::OCTAVIAN).Class(REGION) == P::OLTENIA ? 1 : 0));
        return true;
      },
      {REGION}));
  RETURN_IF_ERROR(AddSpecificEntryPredicate(
      "2. Matei wasn't from Debrogea. He hated onions or ivy.",
      // Edit "He hated" probably should be "He hated neither".
      [](const puzzle::Entry& e) {
        if (e.Class(REGION) == P::DEBROGEA) return false;
        return e.Class(PLANT) == P::ONION || e.Class(PLANT) == P::IVY;
      },
      {REGION, PLANT}, P::MATEI));
  RETURN_IF_ERROR(AddPredicate(
      "3. The vampire from Mutenia lived 100 years after the "
      "vampire who hated thornbrush.",
      [](const puzzle::SolutionView& s) {
        int from_mutenia_century = s.Find([](const puzzle::Entry& e) {
                                      return e.Class(REGION) == P::MUNTENIA;
                                    }).Class(CENTURY);
        int hated_thornbrush_century =
            s.Find([](const puzzle::Entry& e) {
               return e.Class(PLANT) == P::THORNBRUSH;
             }).Class(CENTURY);
        return hated_thornbrush_century == from_mutenia_century - 1;
      },
      {REGION, PLANT, CENTURY}));
  RETURN_IF_ERROR(AddPredicate(
      "4. 100 years after Dorian's death, another vamipre rised "
      "in Bucovina, but this wasn't Bogdan.",
      [](const puzzle::SolutionView& s) {
        const puzzle::Entry& from_bucovina = s.Find([](const puzzle::Entry& e) {
          return e.Class(REGION) == P::BUCOVINA;
        });
        return from_bucovina.id() != P::BOGDAN &&
               (from_bucovina.Class(CENTURY) ==
                s.Id(P::DORIAN).Class(CENTURY) + 1);
      },
      {REGION, CENTURY}));
  RETURN_IF_ERROR(AddSpecificEntryPredicate(
      "5. Octavain either lived in the XVI century or hated "
      "thornbrush.",
      [](const puzzle::Entry& e) {
        const bool in_xvi = e.Class(CENTURY) == P::XVI;
        const bool hated_thornbrush = e.Class(PLANT) == P::THORNBRUSH;
        return (in_xvi || hated_thornbrush) && !(in_xvi && hated_thornbrush);
      },
      {PLANT, CENTURY}, P::OCTAVIAN));
  RETURN_IF_ERROR(AddPredicate(
      "6. If Bogdan hated wolfsbane, then Matei lived in Buchovia.",
      [](const puzzle::SolutionView& s) {
        if (s.Id(P::BOGDAN).Class(PLANT) == P::WOLFSBANE) {
          return s.Id(P::MATEI).Class(REGION) == P::BUCOVINA;
        }
        return true;
      },
      {{REGION, P::MATEI}, {PLANT, P::BOGDAN}}));
  RETURN_IF_ERROR(AddSpecificEntryPredicate(
      "7a. The vampire from XIV century wasn't Octavian nor Bogdan. (Octavian)",
      [](const puzzle::Entry& e) { return e.Class(CENTURY) != P::XIV; },
      {CENTURY}, P::OCTAVIAN));
  RETURN_IF_ERROR(AddSpecificEntryPredicate(
      "7b. The vampire from XIV century wasn't Octavian nor Bogdan. (Bogdan)",
      [](const puzzle::Entry& e) { return e.Class(CENTURY) != P::XIV; },
      {CENTURY}, P::BOGDAN));
  RETURN_IF_ERROR(AddSpecificEntryPredicate(
      "8. Villagers didn't grow thornbrush against Dorian.",
      [](const puzzle::Entry& e) { return e.Class(PLANT) != P::THORNBRUSH; },
      {PLANT}, P::DORIAN));
  RETURN_IF_ERROR(AddPredicate(
      "9. Chronicles of XVII century claimed that ivy was "
      "ineffective and that Debrogea was free from vamipres.",
      [](const puzzle::SolutionView& s) {
        const puzzle::Entry& e = s.Find(
            [](const puzzle::Entry& e) { return e.Class(CENTURY) == P::XVII; });
        return e.Class(PLANT) != P::IVY && e.Class(REGION) != P::DEBROGEA;
      },
      {REGION, PLANT, CENTURY}));

  return absl::OkStatus();
}

std::string DraculaAndFriendsProblem::solution_textproto() const {
  return R"pb(
    entry { id: BOGDAN region: MUNTENIA plant: IVY century: XVI }
    entry { id: DORIAN region: DEBROGEA plant: WOLFSBANE century: XIV }
    entry { id: MATEI region: OLTENIA plant: ONION century: XVII }
    entry { id: OCTAVIAN region: BUCOVINA plant: THORNBRUSH century: XV }
  )pb";
}

REGISTER_PROBLEM(DraculaAndFriendsProblem);
