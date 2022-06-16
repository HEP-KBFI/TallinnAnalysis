#ifndef TallinnAnalysis_Framework_getSubdirectories_h
#define TallinnAnalysis_Framework_getSubdirectories_h

#include <TDirectory.h> // TDirectory

#include <vector>       // std::vector

std::vector<const TDirectory *>
getSubdirectories(const TDirectory *);

#endif // TallinnAnalysis_Framework_getSubdirectories_h

