#ifndef TallinnAnalysis_Framework_addHistograms_h
#define TallinnAnalysis_Framework_addHistograms_h

#include <TH1.h>  // TH1

#include <string> // std::string
#include <vector> // std::vector

TH1*
addHistograms(const std::string & newHistogramName,
              const TH1* histogram1,
              const TH1* histogram2,
              int verbosity = 0);
TH1*
addHistograms(const std::string & newHistogramName,
              const std::vector<const TH1 *> & histogramsToAdd,
              int verbosity = 0);

#endif // TallinnAnalysis_Framework_addHistograms_h

