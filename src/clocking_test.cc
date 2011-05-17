#include "clock_domain.hh"
#include "input.hh"
#include "output.hh"
#include "clocked_block.hh"
#include "clocking_util.hh"

#include <cstdlib>
#include <cstdio>
#include <inttypes.h>

cycle_t CURR_CYCLE;
cycle_t getCycle() { return CURR_CYCLE; }

class UnpipelinedAdder : public ClockedBlock
{
  public:
    UnpipelinedAdder(std::string _name, cycle_t (*_getcycle)(), size_t lat) :
      ClockedBlock(_name, _getcycle), 
      stallin("StallIn", _getcycle, false, false, MDCT_OR),
      in1("In1", _getcycle, false, false, MDCT_DISALLOWED),
      in2("In2", _getcycle, false, false, MDCT_DISALLOWED),
      out("Out", _getcycle),
      stallout("StallOut", _getcycle),
      latency(lat)
    {
      //TODO We might pass a ClockedBlock * or a vector<Clockable *>
      //into Input<> and Output<> constructors so they can add themselves to the back
      //of the list when they're constructed, rather than requiring us to repeat ourselves
      //in the block constructor.  We'd need to allow passing in NULL in the case that the Input
      //or Output is not part of a block (as in a primary input/output of the overall circuit)
      inputs.push_back(&in1);
      inputs.push_back(&in2);
      inputs.push_back(&stallin);
      outputs.push_back(&out);
      outputs.push_back(&stallout);
    }

    Input<bool> stallin;
    Input<unsigned int> in1, in2;
    Output<unsigned int> out;
    Output<bool> stallout;

  protected:
    virtual void update() {
      bool stallvalue;
      bool stallvalid = stallin.getValue(stallvalue);
      if(!stallvalid) {
        std::cerr << "Error: UnpipelinedAdder " << name << " has invalid stallin input" << std::endl;
        assert(0);
      }
      if(!stallvalue) { //Don't stall
        unsigned int in1val, in2val;

        if(cycleready != 0 && getcycle() >= cycleready) {
          out.setNext(result);
        }

        if(in1.getValue(in1val) && in2.getValue(in2val)) {
          result = in1val + in2val;
          if(latency == 0)
            out.setNext(result);
          else
            cycleready = getcycle() + latency;
        }
      }
      stallout.setNext(stallvalue || (rand() % 2 == 0));
    }
    size_t latency;
    cycle_t cycleready;
    unsigned int result;
};

int main(int argc, char *argv[])
{
  printf("&getCycle = %p\n", &getCycle);
  ClockDomain cd1("ClockDomain1", &getCycle);
  UnpipelinedAdder ua1("UnpipelinedAdder1", &getCycle, 2);
  UnpipelinedAdder ua2("UnpipelinedAdder2", &getCycle, 2);
  UnpipelinedAdder ua3("UnpipelinedAdder3", &getCycle, 2);

  Input<unsigned int> result("result", &getCycle, false, false, MDCT_DISALLOWED);
  Output<unsigned int> in1("in1", &getCycle);
  Output<unsigned int> in2("in2", &getCycle);
  Output<unsigned int> in3("in3", &getCycle);
  Output<unsigned int> in4("in4", &getCycle);
  Output<bool> nostall("NOSTALL", &getCycle);
  Input<bool> stall1("stall1", &getCycle, false, false, MDCT_DISALLOWED);
  Input<bool> stall2("stall2", &getCycle, false, false, MDCT_DISALLOWED);
  //printf("%p %p %p %p %p\n", result.getcycle, in1.getcycle, in2.getcycle, in3.getcycle, in4.getcycle);

  ua1.out.registerInput(&ua3.in1, 1, 1);
  ua2.out.registerInput(&ua3.in2, 1, 1);
  in1.registerInput(&ua1.in1, 0, 1);
  in2.registerInput(&ua1.in2, 0, 1);
  in3.registerInput(&ua2.in1, 0, 1);
  in4.registerInput(&ua2.in2, 0, 1);
  ua3.out.registerInput(&result, 0, 1);
  nostall.registerInput(&ua3.stallin, 0, 1);
  ua3.stallout.registerInput(&ua2.stallin, 0, 1);
  ua3.stallout.registerInput(&ua1.stallin, 0, 1);
  ua1.stallout.registerInput(&stall1, 0, 1);
  ua2.stallout.registerInput(&stall2, 0, 1);

  //Adding the blocks 1-2-3 so that we can see that toposort() reorders them appropriately (3-1-2 or 3-2-1).
  cd1.addBlock(&ua1);
  cd1.addBlock(&ua2);
  cd1.addBlock(&ua3);
  cd1.toposort();

  for(int i = 0; i < 100; i++) {
    unsigned int i1 = rand() % 100;
    unsigned int i2 = rand() % 100;
    unsigned int i3 = rand() % 100;
    unsigned int i4 = rand() % 100;
    in1.setNext(i1);
    in2.setNext(i2);
    in3.setNext(i3);
    in4.setNext(i4);
    nostall.setNext(false);
    cd1.clock();
    unsigned int res;
    bool resvalid = result.getValue(res);
    bool s1, s2;
    bool s1valid = stall1.getValue(s1);
    bool s2valid = stall2.getValue(s2);
    printf("Cycle %2"PRIu64": Result = (%01d, %u), In1-In4 = %2d %2d %2d %2d (%01d %01d) (%01d %01d)\n", getCycle(), resvalid, res, i1, i2, i3, i4, s1valid, s1, s2valid, s2);
    CURR_CYCLE++;
  }
  return 0;
}  
