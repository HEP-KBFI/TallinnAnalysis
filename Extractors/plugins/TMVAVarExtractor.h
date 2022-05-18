#ifndef TallinnAnalysis_Extractors_TMVAVarExtractor_h
#define TallinnAnalysis_Extractors_TMVAVarExtractor_h

#include "FWCore/ParameterSet/interface/ParameterSet.h"               // edm::ParameterSet

#include "TallinnAnalysis/Extractors/interface/MVAVarExtractorBase.h" // MVAVarExtractorBase
#include "TallinnAnalysis/MLTools/interface/TMVAInterface.h"          // TMVAInterface

#include <map>                                                        // std::map

// forward declarations
class TTree;

class TMVAVarExtractor : public MVAVarExtractorBase
{
 public:
  TMVAVarExtractor(const edm::ParameterSet & cfg);
  ~TMVAVarExtractor();

  bool is_multiclass();

 private:
  double
  get_valueImp(const std::map<std::string, double> & mvaInputs) const;

  std::map<std::string, double>
  get_value_multiclassImp(const std::map<std::string, double> & mvaInputs) const;

  TMVAInterface * mvaInterface_;

  std::vector<std::string> classes_;
};

#endif // TallinnAnalysis_Extractors_TMVAVarExtractor_h
