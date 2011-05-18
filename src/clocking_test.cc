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
    UnpipelinedAdder(std::string _basename, std::string _name, cycle_t (*_getcycle)(), size_t lat) :
      ClockedBlock(_basename, _name, _getcycle), 
      stallin(getName(), "StallIn", _getcycle, false, false, MDCT_OR),
      in1(getName(), "In1", _getcycle, false, false, MDCT_DISALLOWED),
      in2(getName(), "In2", _getcycle, false, false, MDCT_DISALLOWED),
      out(getName(), "Out", _getcycle),
      stallout(getName(), "StallOut", _getcycle),
      latency(lat),
      cycleready(0)
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
        std::cerr << "Error: UnpipelinedAdder " << getName() << " has invalid stallin input" << std::endl;
        assert(0);
      }

      //bool internalStall = cycleready != 0 && getcycle() < cycleready;
      //if(internalStall)
      //{
        //std::cerr << getName() << " internal stalling" << std::endl;
      //}
      //stallout.setNext(stallvalue || internalStall);
      stallout.setNext(false);

      if(!stallvalue) { //Don't stall
        //std::cerr << getName() << " not stalling" << std::endl;
        unsigned int in1val, in2val;

        
        if(cycleready != 0 && getcycle() >= cycleready) {
          //std::cerr << getName() << " setting out to " << result << std::endl;
          out.setNext(result);
          cycleready = 0;
        }

        if(cycleready == 0) {
          bool in1valid = in1.getValue(in1val), in2valid = in2.getValue(in2val);
          if(in1valid && in2valid) {
            //std::cerr << getName() << " taking in 2 values (" << in1val << "," << in2val << ")" << std::endl;
            result = in1val + in2val;
            if(latency == 0)
              out.setNext(result);
            else
              cycleready = getcycle() + latency;
          }
          else {
            //std::cerr << getName() << "(in1valid, in2valid) = (" << in1valid << "," << in2valid << ")" << std::endl;
          }
        }
      }
      else {
        //std::cerr << getName() << " stalled" << std::endl;
      }
    }

    size_t latency;
    cycle_t cycleready;
    unsigned int result;
};

class TestHarness : public ClockedBlock
{
  public:
    TestHarness(std::string _basename, std::string _name, cycle_t (*_getcycle)()) :
      ClockedBlock(_basename, _name, _getcycle),
      result(getName(), "result", _getcycle, false, false, MDCT_DISALLOWED),
      in1(getName(), "in1", _getcycle),
      in2(getName(), "in2", _getcycle),
      in3(getName(), "in3", _getcycle),
      in4(getName(), "in4", _getcycle),
      nostall(getName(), "NOSTALL", _getcycle),
      stall1(getName(), "stall1", _getcycle, false, false, MDCT_DISALLOWED),
      stall2(getName(), "stall2", _getcycle, false, false, MDCT_DISALLOWED)
    {
      outputs.push_back(&in1);
      outputs.push_back(&in2);
      outputs.push_back(&in3);
      outputs.push_back(&in4);
      outputs.push_back(&nostall);
      inputs.push_back(&result);
      inputs.push_back(&stall1);
      inputs.push_back(&stall2);
    }

    Input<unsigned int> result;
    Output<unsigned int> in1;
    Output<unsigned int> in2;
    Output<unsigned int> in3;
    Output<unsigned int> in4;
    Output<bool> nostall;
    Input<bool> stall1;
    Input<bool> stall2;

    virtual void init()
    {
      ClockedBlock::init();
      nostall.setNext(false);
    }

  protected:
    virtual void update() {
      unsigned int i1 = rand() % 100;
      unsigned int i2 = rand() % 100;
      unsigned int i3 = rand() % 100;
      unsigned int i4 = rand() % 100;
      printf("Cycle %2"PRIu64": ", getCycle());
      bool s1, s2;
      if(!stall1.getValue(s1) || !stall2.getValue(s2)) {
        std::cerr << "Warning: " << getName() << " stall1 or stall2 was not valid." << std::endl;
        s1 = true;
        s2 = true;
        //assert(0);
      }
      if(!s1) {
        in1.setNext(i1);
        in2.setNext(i2);
      }
      if(!s2) {
        in3.setNext(i3);
        in4.setNext(i4);
      }
      nostall.setNext(false);
    
      unsigned int res;
      bool resvalid = result.getValue(res);
      printf("(In1, In2, In3, In4) = (%2d, %2d, %2d, %2d), Result = (%01d, %3u), (Stall1, Stall2) : (%01d %01d)\n", (!s1 ? i1 : 0), (!s1 ? i2 : 0), (!s2 ? i3 : 0), (!s2 ? i4 : 0), resvalid, (resvalid ? res : 0U), s1, s2);
    }
};

int main(int argc, char *argv[])
{
  ClockDomain cd1("", "ClockDomain1", &getCycle);
  UnpipelinedAdder ua1("", "UnpipelinedAdder1", &getCycle, 2);
  UnpipelinedAdder ua2("", "UnpipelinedAdder2", &getCycle, 2);
  UnpipelinedAdder ua3("", "UnpipelinedAdder3", &getCycle, 2);
  TestHarness th("", "TestHarness", &getCycle);
  //printf("%p %p %p %p %p\n", result.getcycle, in1.getcycle, in2.getcycle, in3.getcycle, in4.getcycle);

  ua1.out.registerInput(&ua3.in1, 1, 1);
  ua2.out.registerInput(&ua3.in2, 1, 1);
  th.in1.registerInput(&ua1.in1, 0, 1);
  th.in2.registerInput(&ua1.in2, 0, 1);
  th.in3.registerInput(&ua2.in1, 0, 1);
  th.in4.registerInput(&ua2.in2, 0, 1);
  ua3.out.registerInput(&th.result, 0, 1);
  th.nostall.registerInput(&ua3.stallin, 0, 1);
  ua3.stallout.registerInput(&ua2.stallin, 0, 1);
  ua3.stallout.registerInput(&ua1.stallin, 0, 1);
  ua1.stallout.registerInput(&th.stall1, 0, 1);
  ua2.stallout.registerInput(&th.stall2, 0, 1);

  //Adding the blocks 1-2-3 so that we can see that toposort() reorders them appropriately (3-1-2 or 3-2-1).
  cd1.addBlock(&ua1);
  cd1.addBlock(&ua2);
  cd1.addBlock(&ua3);

  cd1.init();
  th.init();

  for(int i = 0; i < 100; i++) {
    cd1.clock();
    th.clock();
    CURR_CYCLE++;
  }
  return 0;
}  
