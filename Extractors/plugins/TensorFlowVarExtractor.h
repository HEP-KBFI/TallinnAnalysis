#ifndef TallinnAnalysis_Extractors_TensorFlowVarExtractor_h
#define TallinnAnalysis_Extractors_TensorFlowVarExtractor_h

#include "FWCore/ParameterSet/interface/ParameterSet.h"               // edm::ParameterSet

#include "TallinnAnalysis/Extractors/interface/MVAVarExtractorBase.h" // MVAVarExtractorBase
#include "TallinnAnalysis/MLTools/interface/TensorFlowInterface.h"    // TMVAInterface

#include <map>                                                        // std::map

// forward declarations
class TTree;

class TensorFlowVarExtractor : public MVAVarExtractorBase
{
 public:
  TensorFlowVarExtractor(const edm::ParameterSet & cfg);
  ~TensorFlowVarExtractor();

  bool is_multiclass();

 private:
  double
  get_valueImp(const std::map<std::string, double> & mvaInputs) const;

  std::map<std::string, double>
  get_value_multiclassImp(const std::map<std::string, double> & mvaInputs) const;

  TensorFlowInterface * mvaInterface_;

  std::vector<std::string> classes_;
};

#endif // TallinnAnalysis_Extractors_TensorFlowVarExtractor_h

