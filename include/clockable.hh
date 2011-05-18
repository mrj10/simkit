#ifndef __CLOCKABLE_HH__
#define __CLOCKABLE_HH__

#include <string>
#include <set>
#include <vector>
#include "clocking_util.hh"

//#define DEBUG

#ifdef DEBUG
#include <iostream>
#endif

class Clockable {
  public:
    Clockable(std::string _basename, std::string _name, cycle_t (*_getcycle)()) :
      name((_basename.empty() ? _basename : (_basename + "::")) + _name),
      getcycle(_getcycle)
    {
    }
    virtual void clock() { } //{ cyclenum++; }
    std::string getName() const { return name; }
    cycle_t getCycle() const { return getcycle(); }
    std::set<Clockable *>::const_iterator getZeroCycleDepsBegin() const { return zeroCycleDeps.begin(); }
    std::set<Clockable *>::const_iterator getZeroCycleDepsEnd() const { return zeroCycleDeps.end(); }
    bool addZeroCycleDep(Clockable *dep) {
#ifdef DEBUG
      std::cerr << "Adding " << dep->getName() << " as a ZCD of " << getName() << std::endl;
#endif
      return zeroCycleDeps.insert(dep).second == true;
    }
    friend class Clocker;

  protected:
    std::set<Clockable *> zeroCycleDeps;
    std::string name;
    cycle_t (*getcycle)(); //ptr to function that takes no args and returns a cycle_t
    //cycle_t internalGetCycle() const { return cyclenum; }
    //cycle_t cyclenum; //Only used if external function is NULL
};

#endif //#ifndef __CLOCKABLE_HH__
