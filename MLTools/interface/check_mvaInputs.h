#ifndef TallinnAnalysis_MLTools_check_mvaInputs_h
#define TallinnAnalysis_MLTools_check_mvaInputs_h

#include "tthAnalysis/HiggsToTauTau/interface/EventInfo.h"

void
check_mvaInputs(std::map<std::string, double> & mvaInputs,
                UInt_t run = 0,
                UInt_t lumi = 0,
                ULong64_t event = 0);

#endif // TallinnAnalysis_MLTools_check_mvaInputs_h
