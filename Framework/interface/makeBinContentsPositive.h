#ifndef TallinnAnalysis_Framework_makeBinContentsPositive_h
#define TallinnAnalysis_Framework_makeBinContentsPositive_h

#include <TH1.h> // TH1

void
makeBinContentsPositive(TH1 * histogram, 
                        bool isData,
                        int verbosity);

#endif // TallinnAnalysis_Framework_makeBinContentsPositive_h

