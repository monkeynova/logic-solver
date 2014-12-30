/*
Grobly Grizik is planning to write a novel fashioned after Star Trek: The Next Generation.
In this novel, six of the crew members compete both at Fizzbin and at Tridimensional chess.
Each crew member gets two independent rankings for ability at these games, with 1 ranked
lowest and 6 highest. Every crew member has a personal hero among the crew, and every crew
member is afraid of some crew member. Everyone is the hero of somebody, and everyone is
feared by somebody. Nobody either fears him/herself nor counts him/herself as a hero.
Nobody fears his/her own hero. From the given clues, discover every crew member's ranking
at Fizzbin and at Tri-D chess, as well as whom he/she fears and whom he/she counts as a hero:

1. Geordi ranks 2 at Tri-D Chess.
2. Picard ranks two positions behind Troi at Fizzbin.
3. Troi is feared by the person Geordi fears.
4. Worf's hero ranks 3 times lower at Tri-D Chess than the crew member who is best at Fizzbin.
5. Picard's hero fears Geordi.
6. Data's hero is not Geordi.
7. Data is the hero of Riker's hero.
8. The person who is worst at Fizzbin is better than Troi at Tri-D Chess.
9. The person ranked number 3 at Tri-D Chess is ranked 4 positions higher than Data at Fizzbin.
10. Riker is feared by the person Picard fears and is the hero of Worf's hero.
11. Riker is ranked 2 lower at Tri-D Chess than the crew member ranked 2 at Fizzbin.
 */
#include <iostream>
#include <memory>

#include "solver.h"

enum WHO { 
    Picard = 0,
    Riker = 1,
    Troi = 2,
    Geordi = 3,
    Data = 4,
    Worf = 5};

enum CLASSES {
    HERO = 0,
    FEAR = 1,
    TRID = 2,
    FIZZBIN = 3};

void SetupProblem(Puzzle::Solver& s, vector<std::unique_ptr<Puzzle::Descriptor>> *descriptors) {
    Puzzle::StringDescriptor *who_descriptor = new Puzzle::StringDescriptor();
    descriptors->push_back(std::unique_ptr<Puzzle::Descriptor>(who_descriptor));

    who_descriptor->SetDescription(Picard, "Picard");
    who_descriptor->SetDescription(Riker, "Riker");
    who_descriptor->SetDescription(Troi, "Troi");
    who_descriptor->SetDescription(Geordi, "Geordi");
    who_descriptor->SetDescription(Data, "Data");
    who_descriptor->SetDescription(Worf, "Worf");
    
    s.SetIdentifiers(who_descriptor);
    s.AddClass(HERO, "hero", who_descriptor);
    s.AddClass(FEAR, "fear", who_descriptor);

    Puzzle::IntRangeDescriptor *ranking_descriptor = new Puzzle::IntRangeDescriptor(1, 6);
    descriptors->push_back(std::unique_ptr<Puzzle::Descriptor>(ranking_descriptor));

    s.AddClass(TRID, "trid", ranking_descriptor);
    s.AddClass(FIZZBIN, "fizzbin", ranking_descriptor);
}

void AddProblemPredicates(Puzzle::Solver& s) {
    // Nobody either fears him/herself nor counts him/herself as a hero.
    s.AddPredicate([](const Puzzle::Entry& e) {return e.Class(HERO) != e.id(); }, HERO );
    s.AddPredicate([](const Puzzle::Entry& e) {return e.Class(FEAR) != e.id(); }, FEAR );

    // Nobody fears his/her own hero.
    s.AddPredicate([](const Puzzle::Entry& e) {return e.Class(HERO) != e.Class(FEAR); }, {HERO, FEAR} );
}

void AddRulePredicates(Puzzle::Solver& s) {
    // 1. Geordi ranks 2 at Tri-D Chess.
    s.AddPredicate([](const Puzzle::Solution& s) {return s.Id(Geordi).Class(TRID) == 2;}, TRID);

    // 2. Picard ranks two positions behind Troi at Fizzbin.
    s.AddPredicate([](const Puzzle::Solution& s) {
            return s.Id(Picard).Class(FIZZBIN) == s.Id(Troi).Class(FIZZBIN) - 2; }, FIZZBIN);

    // 3. Troi is feared by the person Geordi fears.
    s.AddPredicate([](const Puzzle::Solution& s) {
            return s.Id(s.Id(Geordi).Class(FEAR)).Class(FEAR) == Troi;}, FEAR);

    // 4. Worf's hero ranks 3 times lower at Tri-D Chess than the crew member who is best at Fizzbin.
    s.AddPredicate([](const Puzzle::Solution& s) {
            return s.Id(s.Id(Worf).Class(HERO)).Class(TRID) * 3 == 
                s.Find([](const Puzzle::Entry& e) {return e.Class(FIZZBIN) == 6;}).Class(TRID); }, {HERO, TRID, FIZZBIN});

    // 5. Picard's hero fears Geordi.
    s.AddPredicate([](const Puzzle::Solution& s) {
            return s.Id(s.Id(Picard).Class(HERO)).Class(FEAR) == Geordi;}, {HERO, FEAR});

    // 6. Data's hero is not Geordi.
    s.AddPredicate([](const Puzzle::Solution& s) {return s.Id(Data).Class(HERO) != Geordi;}, HERO);

    // 7. Data is the hero of Riker's hero.
    s.AddPredicate([](const Puzzle::Solution& s) {
            return s.Id(s.Id(Riker).Class(HERO)).Class(HERO) == Data;}, HERO);

    // 8. The person who is worst at Fizzbin is better than Troi at Tri-D Chess.
    s.AddPredicate([](const Puzzle::Solution& s) {
            return s.Id(Troi).Class(TRID) < s.Find([](const Puzzle::Entry& e) {return e.Class(FIZZBIN) == 1;}).Class(TRID);},
        {TRID, FIZZBIN});

    // 9. The person ranked number 3 at Tri-D Chess is ranked 4 positions higher than Data at Fizzbin.
    s.AddPredicate([](const Puzzle::Solution& s) {
            return s.Find([](const Puzzle::Entry& e) {return e.Class(TRID) == 3;}).Class(FIZZBIN) == 4 + s.Id(Data).Class(FIZZBIN);},
        {TRID, FIZZBIN});

    // 10. Riker is feared by the person Picard fears and is the hero of Worf's hero.
    s.AddPredicate([](const Puzzle::Solution& s) {
            return s.Id(s.Id(Worf).Class(HERO)).Class(HERO) == Riker &&
                s.Id(s.Id(Picard).Class(FEAR)).Class(FEAR) == Riker;},
        {HERO, FEAR});

    // 11. Riker is ranked 2 lower at Tri-D Chess than the crew member ranked 2 at Fizzbin.
    s.AddPredicate([](const Puzzle::Solution& s) {
            return s.Id(Riker).Class(TRID) + 2 == s.Find([](const Puzzle::Entry& e) {return e.Class(FIZZBIN) == 2;}).Class(TRID);},
        {TRID, FIZZBIN});
}

int main( void ) {
    Puzzle::Solver s;
    vector<std::unique_ptr<Puzzle::Descriptor>> descriptors;

    SetupProblem(s, &descriptors);
    AddProblemPredicates(s);
    AddRulePredicates(s);

    Puzzle::Solution answer = s.Solve();

    cout << answer.ToStr();

    return answer.IsValid() ? 0 : 1;
}
