#ifndef __CLOCK_DOMAIN_HH__
#define __CLOCK_DOMAIN_HH__

#include <vector>
#include <string>
#include <map>
#include <set>

#include "clockable.hh"

class ClockedBlock;

class ClockDomain : public Clockable {
  public:
    ClockDomain(std::string _name, cycle_t (*_getcycle)()) : Clockable(_name, _getcycle) { }
    void addBlock(ClockedBlock *_cb);
    void init();
    void clock();
  protected:
    std::vector<Clockable *> outputs;
    std::vector<ClockedBlock *> cb;
    void toposort();
    void toposort_visitor(ClockedBlock *cb,
                          std::vector<ClockedBlock *> &sorted,
                          std::map< ClockedBlock *, std::set<ClockedBlock *> > &backward_adjacency,
                          std::set<ClockedBlock *> &visited,
                          std::set<ClockedBlock *> &visited_this_call_chain);
};

#endif //#ifndef __CLOCK_DOMAIN_HH__
