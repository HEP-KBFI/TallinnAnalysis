#include "TallinnAnalysis/Extractors/interface/get_max_mvaOutput_multiclass.h"

std::pair<std::string, double>
get_max_mvaOutput_multiclass(std::map<std::string, double>& mvaOutputs)
{
  std::string selected_class;
  double max_mvaOutput = -1.e+3;
  for ( std::map<std::string, double>::const_iterator it = mvaOutputs.begin(); it != mvaOutputs.end(); ++it )
  {
    if ( it->second > max_mvaOutput )
    {
      selected_class = it->first;
      max_mvaOutput = it->second;
    }
  }
  return std::pair<std::string, double>(selected_class, max_mvaOutput);
}
