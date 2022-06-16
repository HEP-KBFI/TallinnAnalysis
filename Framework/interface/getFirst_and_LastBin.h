#ifndef TallinnAnalysis_Framework_getFirst_and_LastBin_h
#define TallinnAnalysis_Framework_getFirst_and_LastBin_h

#include <TAxis.h> // TAxis

#include <utility> // std::pair

std::pair<int, int>
getFirst_and_LastBin(const TAxis * axis, bool includeUnderflowBin, bool includeOverflowBin);

int
getFirstBin(const TAxis * axis, bool includeUnderflowBin);

int
getLastBin(const TAxis * axis, bool includeOverflowBin);

#endif // TallinnAnalysis_Framework_getFirst_and_LastBin_h
