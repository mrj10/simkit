#ifndef __OUTPUT_HH__
#define __OUTPUT_HH__

#include <vector>
#include <string>
#include <queue>
#include <cassert>
#include "clocking_util.hh"
#include "clockable.hh"

//#define DEBUG

#ifdef DEBUG
#include <iostream>
#endif

#define OUTPUT_TEMPLATE_PARAM template<class OUTPUTPAYLOAD>

//Forward decl
template<class INPUTPAYLOAD>
class Input;

OUTPUT_TEMPLATE_PARAM
class Output : public Clockable{
  public:

  	Output(std::string _name, cycle_t (*_getcycle)()) :
  	  Clockable(_name, _getcycle) {
   		
  	}

  	class wire_t {
      public:
        typedef typename std::vector< Input<OUTPUTPAYLOAD> * > sinkvec;

    	  struct q_entry_t {
    	  	q_entry_t() :
  	      cycle_written(0)
    	  	{
    	  	}
    	  	OUTPUTPAYLOAD p;
  		    cycle_t cycle_written;
        }; //struct q_entry_t

        wire_t(size_t _latency, size_t _value_every_n_cycles) :
          qsize((_latency+_value_every_n_cycles-1)/_value_every_n_cycles), //ceil(latency/value_every_n_cycles)
          latency(_latency),
          value_every_n_cycles(_value_every_n_cycles)
        {
#ifdef DEBUG
          std::cerr << "Constructing wire_t with (qsize,latency,value_every_n_cycles) = (" << qsize << "," << latency << "," << value_every_n_cycles << ")" << std::endl;
#endif
          if(latency > 0) {
            q.resize(qsize);
            outidx = 0;
            inidx = (outidx+1) % qsize;
            nextvalid = false;
          }
        }
  	  	~wire_t(){
#ifdef DEBUG
          std::cerr << "Destroying wire_t with (qsize,latency,value_every_n_cycles) = (" << qsize << "," << latency << "," << value_every_n_cycles << ")" << std::endl;
#endif
  	  	}
        bool isNextValid() const {
          return nextvalid;
        }
        void setNext(OUTPUTPAYLOAD p) {
          if(latency != 0) {
            next = p;
            nextvalid = true;
          }
          else {
            for(typename sinkvec::const_iterator it = sinks.begin(), end = sinks.end(); it != end; ++it) {
              (*it)->putValue(p, 0); //TODO: Do I need to plumb in a cycle number here?
              //TODO: Check to see if the input used our last pushed value or not, warn or error out if necessary.
            }
          }
        }
        void clock(cycle_t c);
        size_t getLatency() const { return latency; }
        size_t getValueEveryNCycles() const { return value_every_n_cycles; }
        void registerInput(Input<OUTPUTPAYLOAD> * in);
        typename sinkvec::iterator getSinksBegin() const { return this->sinks.begin(); }
        typename sinkvec::iterator getSinksEnd() const { return this->sinks.end(); }
        size_t getNumSinks() const { return sinks.size(); }
      private:
        sinkvec sinks;
      	std::vector<q_entry_t> q;
        size_t qsize; //Logically const, but can't declare it so b/c we want to make a std::vector<> of wire_t's
      	size_t inidx;
      	size_t outidx;
        OUTPUTPAYLOAD next;
        bool nextvalid;
        size_t latency; //Logically const, but can't declare it so b/c we want to make a std::vector<> of wire_t's
        size_t value_every_n_cycles; //Logically const, but can't declare it so b/c we want to make a std::vector<> of wire_t's
    }; //class wire_t

    typedef typename std::vector<wire_t> wirevec;

    void registerInput(Input<OUTPUTPAYLOAD> * in, size_t latency, size_t value_every_n_cycles) {
      if(latency == 0)
        addZeroCycleDep(in);
      //Search for an existing wire with the given parameters.  If there is none, push a new one on the back.
      for(typename wirevec::iterator it = wires.begin(), end = wires.end(); it != end; ++it) {
        if((it->getLatency() == latency) && (it->getValueEveryNCycles() == value_every_n_cycles)) {
          it->registerInput(in);
          return;
        }
      }
      wire_t w(latency, value_every_n_cycles);
      wires.push_back(w);
      wires.back().registerInput(in);
      //In either case, register the Output with the Input (TODO: Do we need this for anything?)
      in->addDriver(this);
      return;
    }

    void clock() {
      for(typename wirevec::iterator it = wires.begin(), end = wires.end(); it != end; ++it) {
        it->clock(getcycle());
      }
    }

    void setNext(OUTPUTPAYLOAD p) {
      for(typename wirevec::iterator it = wires.begin(), end = wires.end(); it != end; ++it) {
        it->setNext(p);
      }
    }

  private:
    wirevec wires;
};

OUTPUT_TEMPLATE_PARAM
void Output<OUTPUTPAYLOAD>::wire_t::clock(cycle_t c) {
  if(latency == 0) return;
  if(c % value_every_n_cycles == 0) {
    if(nextvalid) {
      q[inidx].p = next;
      q[inidx].cycle_written = c;
      for(typename sinkvec::const_iterator it = sinks.begin(), end = sinks.end(); it != end; ++it) {
        (*it)->putValue(q[outidx].p, q[outidx].cycle_written);
        //TODO: Check to see if the input used our last pushed value or not, warn or error out if necessary.
      }
      nextvalid = false;
    }
    outidx = inidx;
    inidx = (inidx+1) % qsize;
  }
}
OUTPUT_TEMPLATE_PARAM
void Output<OUTPUTPAYLOAD>::wire_t::registerInput(Input<OUTPUTPAYLOAD> *in) {
  sinks.push_back(in);
}

#endif //#ifndef __OUTPUT_HH__
