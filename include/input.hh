#ifndef __INPUT_HH__
#define __INPUT_HH__

#include <vector>
#include <string>
#include <iostream>
#include <cassert>
#include <cstdio>
#include <inttypes.h>
#include "clocking_util.hh"
#include "clockable.hh"
#include "identity_elements.hh"

//#define DEBUG

#define INPUT_TEMPLATE_PARAM template<class INPUTPAYLOAD>

//Forward decl
template<class OUTPUTPAYLOAD>
class Output;

class InputBase : public Clockable {
  public:
    InputBase(std::string _basename, std::string _name, cycle_t (*_getcycle)(), bool _required = true, bool _required_every_cycle = true, 
              MultiDriverCombType_t _mdct = MDCT_DISALLOWED) :
              Clockable(_basename, _name, _getcycle),
	            drive_count(0),
              required(_required), required_every_cycle(_required_every_cycle),
              mdct(_mdct)
    {
    }

    friend class ClockDomain;
    friend class ClockedBlock;
  protected:
    virtual void resetDriveCount() = 0;
    int drive_count;
    bool required;
    bool required_every_cycle;
    MultiDriverCombType_t mdct;
};

INPUT_TEMPLATE_PARAM
class Input : public InputBase {
  public:
    Input(std::string _basename, std::string _name, cycle_t (*_getcycle)(), bool required = true, bool required_every_cycle = true, 
          MultiDriverCombType_t mdct = MDCT_DISALLOWED) :
          InputBase(_basename, _name, _getcycle, required, required_every_cycle, mdct)
    {
    }

    bool getValue(INPUTPAYLOAD &val) {
      val = value;
      return (drive_count != 0);
    }
  
    friend class Output<INPUTPAYLOAD>;

  protected:
    struct driver_tracker_t {
      driver_tracker_t(Output<INPUTPAYLOAD> *o) : driver(o), last_driven(0) { }
      Output<INPUTPAYLOAD> *driver;
      cycle_t last_driven;
    };

    void addDriver(Output<INPUTPAYLOAD> *o);
    void putValue(INPUTPAYLOAD p, cycle_t cycle_written);
    virtual void resetDriveCount();
    std::vector<driver_tracker_t> drivers;
    INPUTPAYLOAD value;
};

INPUT_TEMPLATE_PARAM
void Input<INPUTPAYLOAD>::addDriver(Output<INPUTPAYLOAD> *o) {
  if(mdct == MDCT_DISALLOWED && !drivers.empty()) {
    std::cerr << "Error: Adding second driver " << o->getName() << " to " << getName() << std::endl;
    assert(0);
  }
  driver_tracker_t dt(o);
  drivers.push_back(dt);
}

INPUT_TEMPLATE_PARAM
void Input<INPUTPAYLOAD>::putValue(INPUTPAYLOAD p, cycle_t cycle_written) {
#ifdef DEBUG
  std::cerr << "Input " << getName() << " driven to " << p << ", written @ Cycle " << cycle_written << std::endl;
#endif
  switch(mdct) {
    case MDCT_ONE:
    case MDCT_DISALLOWED:
      if(drive_count != 0) {
        //printf("getCycle = %p\n", getCycle);
        printf("&getcycle = %p\n", &getcycle);
        printf("getcycle = %p\n", getcycle);
        printf("drive_count = %d\n", drive_count);
        printf("getcycle() = %"PRIu64"\n", getCycle());
        std::cerr << getcycle << std::endl;
        std::cerr << getCycle() << " " << drive_count << std::endl;
        std::cerr << "Error: driving Input " << getName() << " of mdct type " << mdct << " twice" << std::endl;
        assert(0);
      }
    case MDCT_LAST: //fallthrough
      value = p;
      break;
    case MDCT_AND:
      value = value & p;
      break;
    case MDCT_OR:
      value = value | p;
      break;
    default:
      std::cerr << "Error in Input::putValue(): Unknown mdct type " << mdct << std::endl;
      assert(0);
      break;
  }
  drive_count++;
}

INPUT_TEMPLATE_PARAM
void Input<INPUTPAYLOAD>::resetDriveCount() {
  drive_count = 0;
  switch(mdct) {
    case MDCT_AND:
      value = getANDIdentityElement<INPUTPAYLOAD>();
      break;
    case MDCT_OR:
      value = getORIdentityElement<INPUTPAYLOAD>();
      break;
    default:
      break;
  }
}

#endif //#ifndef __INPUT_HH__
