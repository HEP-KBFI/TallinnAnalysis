#ifndef TallinnAnalysis_Extractors_get_max_mvaOutput_multiclass_h
#define TallinnAnalysis_Extractors_get_max_mvaOutput_multiclass_h

#include <map>     // std::map
#include <string>  // std::string
#include <utility> // std::pair

std::pair<std::string, double>
get_max_mvaOutput_multiclass(std::map<std::string, double>& mvaOutputs);

#endif // TallinnAnalysis_Extractors_get_max_mvaOutput_multiclass_h
