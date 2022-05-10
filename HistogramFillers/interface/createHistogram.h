#ifndef TallinnAnalysis_HistogramTools_createHistogram_h
#define TallinnAnalysis_HistogramTools_createHistogram_h

#include "CommonTools/Utils/interface/TFileDirectory.h" // TFileDirectory

#include "FWCore/ParameterSet/interface/ParameterSet.h" // edm::ParameterSet

// forward declarations
class TH1;
class TH2;

TH1 *
createHistogram1D(TFileDirectory & dir, const edm::ParameterSet & cfg);

TH2 *
createHistogram2D(TFileDirectory & const edm::ParameterSet & cfg);

#endif // TallinnAnalysis_HistogramTools_createHistogram_h
