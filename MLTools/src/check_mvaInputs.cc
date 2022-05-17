#include "TallinnAnalysis/MLTools/interface/check_mvaInputs.h"

#include <TMath.h>  // IsNaN()

#include <iostream> // std::cout

void
check_mvaInputs(std::map<std::string, double> & mvaInputs,
                UInt_t run,
                UInt_t lumi,
                ULong64_t event)
{
  int index = 1;
  for(const auto & mvaInput: mvaInputs)
  {
    if(TMath::IsNaN(mvaInput.second))
    {
      std::cout << "Warning";
      if(! (run == 0 && lumi == 0 && event == 0))
      {
        std::cout << " in run = " << run << ", lumi = " << lumi << ", event = " << event;
      }
      std::cout << ":\n mvaInput #" << index << " ('" << mvaInput.first << "') = "
                << mvaInput.second << " --> setting mvaInput value to zero !!\n";
      mvaInputs[mvaInput.first] = 0.;
      ++index;
    }
  }
}
