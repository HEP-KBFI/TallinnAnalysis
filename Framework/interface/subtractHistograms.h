#ifndef TallinnAnalysis_Framework_subtractHistograms_h
#define TallinnAnalysis_Framework_subtractHistograms_h

#include <TH1.h>  // TH1

#include <string> // std::string
#include <vector> // std::vector

TH1 *
subtractHistograms(const std::string & newHistogramName,
                   const TH1 * histogramMinuend,
                   const TH1 * histogramSubtrahend,
                   int verbosity = 0);

TH1 *
subtractHistograms(const std::string & newHistogramName,
                   const TH1 * histogram,
                   const std::vector<const TH1 *> & histogramsToSubtract,
                   int verbosity = 0);

#endif // TallinnAnalysis_Framework_subtractHistograms_h

