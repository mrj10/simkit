#include "clockable.hh"
#include "clock_domain.hh"
#include "clocked_block.hh"

#include <iostream>
#include <vector>
#include <map>
#include <cassert>

#define DEBUG

void ClockDomain::addBlock(ClockedBlock *_cb) {
  cb.push_back(_cb);
  for(std::vector<Clockable *>::iterator it = _cb->outputs.begin(), end = _cb->outputs.end(); it != end; ++it) {
    outputs.push_back(*it);
  }
}

void ClockDomain::clock() {
  for(std::vector<ClockedBlock *>::iterator it = cb.begin(), end = cb.end(); it != end; ++it)
    (*it)->clock();
  for(std::vector<Clockable *>::iterator it = outputs.begin(), end = outputs.end(); it != end; ++it)
    (*it)->clock();
}

void ClockDomain::toposort() {
  //This maps from the block that holds 1 or more 0-cycle Outputs to the set of blocks that
  //receive those Outputs.  This constitutes an adjacency list for a 0-cycle dependency graph.
  //This method sorts the list of ClockedBlock *s such that we can clock them in order and all
  //0-cycle dependencies will be satisfied.
  //NOTE: We assume there are *no* 0-cycle "cycles" in the graph (as in asynchronous logic or glitchy feedback).
  std::map< ClockedBlock *, std::set<ClockedBlock *> > forward_adjacency, backward_adjacency;

  for(std::vector<ClockedBlock *>::iterator it = cb.begin(), end = cb.end(); it != end; ++it) {
    for(std::vector<Clockable *>::iterator itOuts = (*it)->outputs.begin(), endOuts = (*it)->outputs.end(); itOuts != endOuts; ++itOuts) {
      for(std::set<Clockable *>::const_iterator itZCD = (*itOuts)->getZeroCycleDepsBegin(), endZCD = (*itOuts)->getZeroCycleDepsEnd(); itZCD != endZCD; ++itZCD) {
        for(std::vector<ClockedBlock *>::iterator it2 = cb.begin(), end2 = cb.end(); it2 != end2; ++it2) {
          for(std::vector<Clockable *>::iterator itIns = (*it2)->inputs.begin(), endIns = (*it2)->inputs.end(); itIns != endIns; ++itIns) {
            if(*itZCD == *itIns) { //Insert an edge
#ifdef DEBUG
              std::cerr << "toposort() inserting edge b/w " << (*it)->getName() << "::" << (*itOuts)->getName() << " and " << (*it2)->getName() << "::" << (*itIns)->getName() << std::endl;
#endif              
              forward_adjacency[*it].insert(*it2);
              backward_adjacency[*it2].insert(*it);
            }
          }
        }
      }
    }
  }

#ifdef DEBUG
  std::cerr << "toposort() before sort:" << std::endl;
  for(std::vector<ClockedBlock *>::iterator it = cb.begin(), end = cb.end(); it != end; ++it) {
    std::cerr << (*it)->getName() << " ";
  }
  std::cerr << std::endl;
#endif

  std::vector<ClockedBlock *> sorted;
  std::set<ClockedBlock *> visited;
  for(std::vector<ClockedBlock *>::iterator it = cb.begin(), end = cb.end(); it != end; ++it) {
    if(forward_adjacency.count(*it) == 0) { //No incoming edges (in reversed sense)
#ifdef DEBUG
  std::cerr << "toposort() starting visit() chain with " << (*it)->getName() << std::endl;
#endif
      std::set<ClockedBlock *> visited_this_call_chain;
      toposort_visitor(*it, sorted, backward_adjacency, visited, visited_this_call_chain);
    }
  }

  //Use the assignment operator to clear out the existing cb and replace it with the topologically-sorted version.
  cb = sorted; 

#ifdef DEBUG
  std::cerr << "toposort() after sort:" << std::endl;
  for(std::vector<ClockedBlock *>::iterator it = cb.begin(), end = cb.end(); it != end; ++it) {
    std::cerr << (*it)->getName() << " ";
  }
  std::cerr << std::endl;
#endif
}

  //  From Wikipedia:
  // L <- Empty list that will contain the sorted nodes
  // S <- Set of all nodes with no incoming edges
  // for each node n in S do
  //     visit(n) 
  // function visit(node n)
  //     if n has not been visited yet then
  //         mark n as visited
  //         for each node m with an edge from n to m do
  //             visit(m)
  //         add n to L

void ClockDomain::toposort_visitor(ClockedBlock *cb,
                                   std::vector<ClockedBlock *> &sorted,
                                   std::map< ClockedBlock *, std::set<ClockedBlock *> > &backward_adjacency,
                                   std::set<ClockedBlock *> &visited,
                                   std::set<ClockedBlock *> &visited_this_call_chain) {
  if(visited_this_call_chain.insert(cb).second == false) { //Already visited this block this call chain
    std::cerr << "Error in ClockDomain::toposort_visitor: 0-cycle feedback loop in ClockedBlocks" << std::endl;
    assert(0);
  }
#ifdef DEBUG
  std::cerr << "toposort_visitor(" << cb->getName() << ") : " << ((visited.count(cb) == 0) ? "not visited" : "visited") << std::endl;
#endif
  if(visited.count(cb) == 0) {
    visited.insert(cb);
    if(backward_adjacency.count(cb) != 0) { //If I have outgoing edges
      for(std::set<ClockedBlock *>::iterator it = backward_adjacency[cb].begin(), end = backward_adjacency[cb].end(); it != end; ++it) {
        toposort_visitor(*it, sorted, backward_adjacency, visited, visited_this_call_chain);
      }
    }
    sorted.push_back(cb);
  }
}
