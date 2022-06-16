#ifndef TallinnAnalysis_Framework_getHistogram_h
#define TallinnAnalysis_Framework_getHistogram_h

#include <TDirectory.h> // TDirectory
#include <TH1.h>        // TH1

#include <string>       // std::string

TH1 *
getHistogram(const TDirectory * dir,
             const std::string & process,
             const std::string & central_or_shift,
             bool enableException,
             bool verbose = false);

#endif // TallinnAnalysis_Framework_getHistogram_h
