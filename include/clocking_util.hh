#ifndef __CLOCKING_UTIL_HH__
#define __CLOCKING_UTIL_HH__

#include <stdint.h>

typedef uint64_t cycle_t;
typedef uint32_t addr_t;

enum MultiDriverCombType_t {
  MDCT_ONE, //This input can be registered with multiple outputs, but only one may drive it in a given cycle (emulates tristate bus)
  MDCT_AND, //If multiple outputs drive this input in a cycle, they are ANDed together.
  MDCT_OR,  //If multiple outputs drive this input in a cycle, they are ORed together.
  MDCT_LAST, //If multiple outputs drive this input in a cycle, the last one that happens to execute will win (DANGEROUS)
  MDCT_DISALLOWED //No multiple drivers allowed
};

#endif //#ifndef __CLOCKING_UTIL_HH__
