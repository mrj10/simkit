#ifndef __CLOCKED_BLOCK_HH__
#define __CLOCKED_BLOCK_HH__

#include <vector>
#include "clockable.hh"

class ClockDomain;

class ClockedBlock : public Clockable {
  public:
    ClockedBlock(std::string _name, cycle_t (*_getcycle)()) : 
      Clockable(_name, _getcycle)
    {
    }
    virtual void init();
    void clock();
    friend class ClockDomain;
  protected:
    virtual void update() = 0;

    std::vector<Clockable *> inputs;
    std::vector<Clockable *> outputs;
};

#endif //#ifndef __CLOCKED_BLOCK_HH__
