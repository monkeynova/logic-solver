#include <iostream>

#include "solver.h"

int main( void ) {
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

    Puzzle::Solver s;

    Puzzle::StringDescriptor who_descriptor;
    who_descriptor.SetDescription( Picard, "Picard" );
    who_descriptor.SetDescription( Riker, "Riker" );
    who_descriptor.SetDescription( Troi, "Troi" );
    who_descriptor.SetDescription( Geordi, "Geordi" );
    who_descriptor.SetDescription( Data, "Data" );
    who_descriptor.SetDescription( Worf, "Worf" );

    s.SetIdentifiers( who_descriptor );
    s.AddClass( HERO, "hero", &who_descriptor );
    s.AddClass( FEAR, "fear", &who_descriptor );

    Puzzle::IntRangeDescriptor ranking_descriptor(1, 6);

    s.AddClass( TRID, "trid", &ranking_descriptor );
    s.AddClass( FIZZBIN, "fizzbin", &ranking_descriptor );
    
    s.AddPredicate([](const Puzzle::Entry& e) {return e.Class(HERO) != e.id(); } );
    s.AddPredicate([](const Puzzle::Entry& e) {return e.Class(FEAR) != e.id(); } );

    // Geordi ranks 2 at Tri-D Chess.
    s.AddPredicate([](const Puzzle::Solution& s) {return s.Id(Geordi).Class(TRID) == 2;});
    // Picard ranks two positions behind Troi at Fizzbin.
    s.AddPredicate([](const Puzzle::Solution& s) {
            return s.Id(Picard).Class(FIZZBIN) == s.Id(Troi).Class(FIZZBIN) - 2; });
    // Troi is feared by the person Geordi fears.
    s.AddPredicate([](const Puzzle::Solution& s) {
            return s.Id(s.Id(Geordi).Class(FEAR)).Class(FEAR) == Troi;});
    // Worf's hero ranks 3 times lower at Tri-D Chess than the crew member who is best at Fizzbin.
    s.AddPredicate([](const Puzzle::Solution& s) {
            return s.Id(s.Id(Worf).Class(HERO)).Class(TRID) * 3 == 
                s.Find([](const Puzzle::Entry& e) {return e.Class(FIZZBIN) == 6;}).Class(TRID); });
    // Picard's hero fears Geordi.
    s.AddPredicate([](const Puzzle::Solution& s) {
            return s.Id(s.Id(Picard).Class(HERO)).Class(FEAR) == Geordi;});
    // Data's hero is not Geordi.
    s.AddPredicate([](const Puzzle::Solution& s) {return s.Id(Data).Class(HERO) != Geordi;});
    // Data is the hero of Riker's hero.
    s.AddPredicate([](const Puzzle::Solution& s) {
            return s.Id(s.Id(Riker).Class(HERO)).Class(HERO) == Data;});
    // The person who is worst at Fizzbin is better than Troi at Tri-D Chess.
    s.AddPredicate([](const Puzzle::Solution& s) {
            return s.Id(Troi).Class(TRID) < s.Find([](const Puzzle::Entry& e) {return e.Class(FIZZBIN) == 1;}).Class(TRID);});
    // The person ranked number 3 at Tri-D Chess is ranked 4 positions higher than Data at Fizzbin.
    s.AddPredicate([](const Puzzle::Solution& s) {
            return s.Find([](const Puzzle::Entry& e) {return e.Class(TRID) == 3;}).Class(FIZZBIN) == 4 + s.Id(Data).Class(FIZZBIN);});
    // Riker is feared by the person Picard fears and is the hero of Worf's hero.
    s.AddPredicate([](const Puzzle::Solution& s) {
            return s.Id(s.Id(Worf).Class(HERO)).Class(HERO) == Riker &&
                s.Id(s.Id(Picard).Class(FEAR)).Class(FEAR) == Riker;});
    // Riker is ranked 2 lower at Tri-D Chess than the crew member ranked 2 at Fizzbin.
    s.AddPredicate([](const Puzzle::Solution& s) {
            return s.Id(Riker).Class(TRID) + 2 == s.Find([](const Puzzle::Entry& e) {return e.Class(FIZZBIN) == 2;}).Class(TRID);});

    Puzzle::Solution answer = s.Solve();

    cout << answer.ToStr();

    return answer.IsValid() ? 0 : 1;
}
