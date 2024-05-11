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

class DraculaAndFriendsProblem : public puzzle::ProtoProblem {
 public:
  DraculaAndFriendsProblem()
      : puzzle::ProtoProblem(DraculaAndFriendsInfo::descriptor()) {}

 private:
  enum Who {
    BOGDAN = DraculaAndFriendsInfo::Entry::BOGDAN,
    DORIAN = DraculaAndFriendsInfo::Entry::DORIAN,
    MATEI = DraculaAndFriendsInfo::Entry::MATEI,
    OCTAVIAN = DraculaAndFriendsInfo::Entry::OCTAVIAN
  };
  enum Region {
    BUCOVINA = DraculaAndFriendsInfo::Entry::BUCOVINA,
    DEBROGEA = DraculaAndFriendsInfo::Entry::DEBROGEA,
    MUNTENIA = DraculaAndFriendsInfo::Entry::MUNTENIA,
    OLTENIA = DraculaAndFriendsInfo::Entry::OLTENIA
  };
  enum Plant {
    IVY = DraculaAndFriendsInfo::Entry::IVY,
    ONION = DraculaAndFriendsInfo::Entry::ONION,
    THORNBRUSH = DraculaAndFriendsInfo::Entry::THORNBRUSH,
    WOLFSBANE = DraculaAndFriendsInfo::Entry::WOLFSBANE
  };
  enum Century {
    XIV = DraculaAndFriendsInfo::Entry::XIV,
    XV = DraculaAndFriendsInfo::Entry::XV,
    XVI = DraculaAndFriendsInfo::Entry::XVI,
    XVII = DraculaAndFriendsInfo::Entry::XVII,
  };
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
      [](const puzzle::Solution& s) {
        return 1 == ((s.Id(BOGDAN).Class(REGION) == BUCOVINA ? 1 : 0) +
                     (s.Id(DORIAN).Class(REGION) == DEBROGEA ? 1 : 0) +
                     (s.Id(MATEI).Class(REGION) == MUNTENIA ? 1 : 0) +
                     (s.Id(OCTAVIAN).Class(REGION) == OLTENIA ? 1 : 0));
        return true;
      },
      {REGION}));
  RETURN_IF_ERROR(AddSpecificEntryPredicate(
      "2. Matei wasn't from Debrogea. He hated onions or ivy.",
      // Edit "He hated" probably should be "He hated neither".
      [](const puzzle::Entry& e) {
        if (e.Class(REGION) == DEBROGEA) return false;
        return e.Class(PLANT) == ONION || e.Class(PLANT) == IVY;
      },
      {REGION, PLANT}, MATEI));
  RETURN_IF_ERROR(AddPredicate(
      "3. The vampire from Mutenia lived 100 years after the "
      "vampire who hated thornbrush.",
      [](const puzzle::Solution& s) {
        int from_mutenia_century = s.Find([](const puzzle::Entry& e) {
                                      return e.Class(REGION) == MUNTENIA;
                                    }).Class(CENTURY);
        int hated_thornbrush_century = s.Find([](const puzzle::Entry& e) {
                                          return e.Class(PLANT) == THORNBRUSH;
                                        }).Class(CENTURY);
        return hated_thornbrush_century == from_mutenia_century - 1;
      },
      {REGION, PLANT, CENTURY}));
  RETURN_IF_ERROR(AddPredicate(
      "4. 100 years after Dorian's death, another vamipre rised "
      "in Bucovina, but this wasn't Bogdan.",
      [](const puzzle::Solution& s) {
        const puzzle::Entry& from_bucovina = s.Find(
            [](const puzzle::Entry& e) { return e.Class(REGION) == BUCOVINA; });
        return from_bucovina.id() != BOGDAN &&
               (from_bucovina.Class(CENTURY) ==
                s.Id(DORIAN).Class(CENTURY) + 1);
      },
      {REGION, CENTURY}));
  RETURN_IF_ERROR(AddSpecificEntryPredicate(
      "5. Octavain either lived in the XVI century or hated "
      "thornbrush.",
      [](const puzzle::Entry& e) {
        const bool in_xvi = e.Class(CENTURY) == XVI;
        const bool hated_thornbrush = e.Class(PLANT) == THORNBRUSH;
        return (in_xvi || hated_thornbrush) && !(in_xvi && hated_thornbrush);
      },
      {PLANT, CENTURY}, OCTAVIAN));
  RETURN_IF_ERROR(AddPredicate(
      "6. If Bogdan hated wolfsbane, then Matei lived in Buchovia.",
      [](const puzzle::Solution& s) {
        if (s.Id(BOGDAN).Class(PLANT) == WOLFSBANE) {
          return s.Id(MATEI).Class(REGION) == BUCOVINA;
        }
        return true;
      },
      {{REGION, MATEI}, {PLANT, BOGDAN}}));
  RETURN_IF_ERROR(AddSpecificEntryPredicate(
      "7a. The vampire from XIV century wasn't Octavian nor Bogdan. (Octavian)",
      [](const puzzle::Entry& e) { return e.Class(CENTURY) != XIV; }, {CENTURY},
      OCTAVIAN));
  RETURN_IF_ERROR(AddSpecificEntryPredicate(
      "7b. The vampire from XIV century wasn't Octavian nor Bogdan. (Bogdan)",
      [](const puzzle::Entry& e) { return e.Class(CENTURY) != XIV; }, {CENTURY},
      BOGDAN));
  RETURN_IF_ERROR(AddSpecificEntryPredicate(
      "8. Villagers didn't grow thornbrush against Dorian.",
      [](const puzzle::Entry& e) { return e.Class(PLANT) != THORNBRUSH; },
      {PLANT}, DORIAN));
  RETURN_IF_ERROR(AddPredicate(
      "9. Chronicles of XVII century claimed that ivy was "
      "ineffective and that Debrogea was free from vamipres.",
      [](const puzzle::Solution& s) {
        const puzzle::Entry& e = s.Find(
            [](const puzzle::Entry& e) { return e.Class(CENTURY) == XVII; });
        return e.Class(PLANT) != IVY && e.Class(REGION) != DEBROGEA;
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
