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
#include <iostream>
#include <memory>

#include "absl/memory/memory.h"
#include "puzzle/solver.h"
#include "swimming_pool.pb.h"

using namespace SwimmingPool;

void SetupProblem(
    Puzzle::Solver* s,
    std::vector<std::unique_ptr<Puzzle::Descriptor>> *descriptors) {
    auto who_descriptor = absl::make_unique<Puzzle::ProtoEnumDescriptor>(
        Who_descriptor());

    auto style_descriptor = absl::make_unique<Puzzle::ProtoEnumDescriptor>(
        Style_descriptor());

    auto lane_descriptor = absl::make_unique<Puzzle::IntRangeDescriptor>(1, 4);

    auto country_descriptor = absl::make_unique<Puzzle::ProtoEnumDescriptor>(
        Country_descriptor());

    s->SetIdentifiers(who_descriptor.get());    
    s->AddClass(LANE, "lane", lane_descriptor.get());
    s->AddClass(COUNTRY, "country",
                country_descriptor.get());
    s->AddClass(STYLE, "style", style_descriptor.get());

    descriptors->push_back(std::move(who_descriptor));
    descriptors->push_back(std::move(style_descriptor));
    descriptors->push_back(std::move(lane_descriptor));
    descriptors->push_back(std::move(country_descriptor));
}

void AddProblemPredicates(Puzzle::Solver* s) {
    // No rules here.
}

bool IsNextTo(const Puzzle::Entry& a, const Puzzle::Entry& b) {
    return fabs(a.Class(LANE) - b.Class(LANE)) == 1;
};

void AddRulePredicates(Puzzle::Solver* s) {

    s->AddPredicate("1. Betty is swimming next to the athlete from the UK. "
                    "Neither of them is swimming Butterfly.",
                    [](const Puzzle::Solution& s) {
                        const Puzzle::Entry& betty = s.Id(BETTY);
                        const Puzzle::Entry& from_uk =
                            s.Find([](const Puzzle::Entry& e) {
                                       return e.Class(COUNTRY) == UK; 
                                   });
                        // TODO(petersk): Split this into 3 separate predicates.
                        return IsNextTo(betty, from_uk) && 
                            betty.Class(STYLE) != BUTTERFLY &&
                            from_uk.Class(STYLE) != BUTTERFLY;
                    },
                    {COUNTRY, STYLE, LANE});

    s->AddPredicate("2. Among Emily and the Backstroker, one is from the UK "
                    "and the other is in the fourth lane.",
                    [](const Puzzle::Solution& s) {
                        const Puzzle::Entry& emily = s.Id(EMILY);
                        if (emily.Class(STYLE) == BACKSTROKE) return false;
                        const Puzzle::Entry& backstroker = 
                            s.Find([](const Puzzle::Entry& e) {
                                        return e.Class(STYLE) == BACKSTROKE;
                                   });
                        if (emily.Class(COUNTRY) == UK && 
                            backstroker.Class(LANE) == 4) {
                          return true;
                        }
                        if (backstroker.Class(COUNTRY) == UK && 
                            emily.Class(LANE) == 4) {
                          return true;
                        }
                        return false;
                    },
                    {COUNTRY, STYLE, LANE});
    s->AddPredicate("3. Carol is not swimming Backstroke nor Dolphin. She is "
                    "not Australian, and is not swiming in lates #2 nor #4.",
                    [](const Puzzle::Solution& s) {
                        return 
                            s.Id(CAROL).Class(STYLE) != BACKSTROKE &&
                            s.Id(CAROL).Class(STYLE) != DOLPHIN &&
                            s.Id(CAROL).Class(COUNTRY) != AUSTRALIA &&
                            s.Id(CAROL).Class(LANE) != 2 &&
                            s.Id(CAROL).Class(LANE) != 4;
                    },
                    {STYLE, COUNTRY, LANE});
    s->AddPredicate("4. The Freestyler is next to both Daisy and the American "
                    "swimmer.",
                    [](const Puzzle::Solution& s) {
                        // TODO(petersk): Split into 2 rules, and add disjoint
                        // conditions on entries.
                        return 
                            IsNextTo(s.Id(DAISY),
                                     s.Find([](const Puzzle::Entry& e){
                                              return e.Class(STYLE) == FREESTYLE;
                                            })) &&
                            IsNextTo(s.Find([](const Puzzle::Entry& e){
                                              return e.Class(COUNTRY) == USA;
                                             }),
                                     s.Find([](const Puzzle::Entry& e){
                                               return e.Class(STYLE) == FREESTYLE;
                                             }));
                    },
                    {STYLE, COUNTRY, LANE});
    s->AddPredicate("5. The American swimmer is next to Carol.",
                    [](const Puzzle::Solution& s) {
                        return IsNextTo(s.Id(CAROL),
                                        s.Find([](const Puzzle::Entry& e){
                                                 return e.Class(COUNTRY) == USA;
                                               }));
                    },
                    {COUNTRY, LANE});
    s->AddPredicate("6. Daisy is not swimming in lane #2.",
                    [](const Puzzle::Solution& s) {
                      return s.Id(DAISY).Class(LANE) != 2;
                    },
                    LANE);
}
