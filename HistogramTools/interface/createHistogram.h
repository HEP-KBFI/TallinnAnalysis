#ifndef TallinnAnalysis_HistogramTools_createHistogram_h
#define TallinnAnalysis_HistogramTools_createHistogram_h

#include "CommonTools/Utils/interface/TFileDirectory.h" // TFileDirectory

#include "FWCore/ParameterSet/interface/ParameterSet.h" // edm::ParameterSet

#include <utility>                                      // std::pair	

// forward declarations
class TDirectory;
class TH1;
class TH2;

std::pair<TDirectory*, TH1 *>
createHistogram1D(TFileDirectory & dir, const edm::ParameterSet & cfg);

std::pair<TDirectory*, TH2 *>
createHistogram2D(TFileDirectory & dir, const edm::ParameterSet & cfg);

#endif // TallinnAnalysis_HistogramTools_createHistogram_h
