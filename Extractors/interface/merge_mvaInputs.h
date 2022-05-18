#ifndef TallinnAnalysis_Extractors_merge_mvaInputs_h
#define TallinnAnalysis_Extractors_merge_mvaInputs_h

#include <map>    // std::map
#include <string> // std::string

void
merge_mvaInputs(std::map<std::string, double>& mvaInputs, const std::map<std::string, double>& to_add, bool overwrite = false);

#endif // TallinnAnalysis_Extractors_merge_mvaInputs_h
