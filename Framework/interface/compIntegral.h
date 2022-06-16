#ifndef TallinnAnalysis_Framework_compIntegral_h
#define TallinnAnalysis_Framework_compIntegral_h

#include <TH1.h> // TH1

double
compIntegral(const TH1 * histogram,
             bool includeUnderflowBin,
             bool includeOverflowBin);

double
compIntegralErr(const TH1 * histogram,
                bool includeUnderflowBin,
                bool includeOverflowBin);

#endif // TallinnAnalysis_Framework_compIntegral_h
