#include "TallinnAnalysis/Extractors/interface/merge_mvaInputs.h"

#include "TallinnNtupleProducer/CommonTools/interface/cmsException.h" // cmsException()

void
merge_mvaInputs(std::map<std::string, double>& mvaInputs, const std::map<std::string, double>& to_add, bool overwrite)
{
  for ( std::map<std::string, double>::const_iterator it = to_add.begin(); it != to_add.end(); ++it )
  {
    if ( !overwrite && mvaInputs.find(it->first) != mvaInputs.end() )
      throw cmsException(__func__, __LINE__) << "Key '" << it->first << "' already exists in mvaInputs !!";
    mvaInputs[it->first] = it->second;
  }
}
