#ifndef TallinnAnalysis_Extractors_BranchVarExtractor_h
#define TallinnAnalysis_Extractors_BranchVarExtractor_h

#include "FWCore/ParameterSet/interface/ParameterSet.h"            // edm::ParameterSet

#include "TallinnAnalysis/Extractors/interface/VarExtractorBase.h" // VarExtractorBase

// forward declarations
class BranchVarBase;
class TTree;

class BranchVarExtractor : public VarExtractorBase
{
 public:
  BranchVarExtractor(const edm::ParameterSet & cfg);
  ~BranchVarExtractor();

  /**
   * @brief Return value to be filled into histogram
   */
  double
  get_value() const;

 private:
  std::string branchName_;
  std::shared_ptr<BranchVarBase> branchVar_;
};

#endif // TallinnAnalysis_Extractors_BranchVarExtractor_h
