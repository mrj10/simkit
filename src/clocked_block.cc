//#include "clocked_block.hh"
//#include "clockable.hh"
//If a block is purely combinational, all outputs are calculated in 0 cycles based on
//all inputs, so we should add outputs as 0-cycle deps of the inputs.
//Call this once all Inputs and Outputs have been added to the block.
//If a block has some internal registers, then the block setup code should call addZeroCycleDep()
//itself as necessary, instead of using this method.
//void ClockedBlock::setIsPureCombinational() {
//  for(std::vector<Clockable *>::iterator itin = inputs.begin(), endin = inputs.end(); itin != endin; ++itin)
//    for(std::vector<Clockable *>::iterator itout = outputs.begin(), endout = outputs.end(); itout != endout; ++itout)
//      itin->addZeroCycleDep(*itout);
//}

#include "clocked_block.hh"
#include "input.hh"

void ClockedBlock::clock() {
  update();
  for(std::vector<Clockable *>::iterator it = inputs.begin(), end = inputs.end(); it != end; ++it) {
    InputBase *ib = static_cast<InputBase *>(*it);
    ib->resetDriveCount();
  }
}
