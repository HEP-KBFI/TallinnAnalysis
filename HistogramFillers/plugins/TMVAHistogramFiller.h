#ifndef TallinnAnalysis_HistogramFillers_TMVAHistogramFiller_h
#define TallinnAnalysis_HistogramFillers_TMVAHistogramFiller_h

#include "CommonTools/Utils/interface/TFileDirectory.h"                     // TFileDirectory
#include "FWCore/ParameterSet/interface/ParameterSet.h"                     // edm::ParameterSet

#include "TallinnAnalysis/HistogramFillers/interface/BranchVars.h"          // BranchVarULong64_t
#include "TallinnAnalysis/HistogramFillers/interface/HistogramFillerBase.h" // HistogramFillerBase
#include "TallinnAnalysis/MLTools/interface/TMVAInterface.h"                // TMVAInterface

#include <map>                                                              // std::map

// forward declarations
class TTree;

class TMVAHistogramFiller : public HistogramFillerBase
{
 public:
  TMVAHistogramFiller(const edm::ParameterSet & cfg);
  ~TMVAHistogramFiller();

  /**
   * @brief Book histogram
   */
  void
  bookHistograms(TFileDirectory & dir);

  /**
   * @brief Fill histogram
   */
  void
  fillHistograms(double evtWeight);

 private:
  std::vector<std::string> branchNames_mvaInputVariables_;
  std::vector<BranchVarBase *> branchVars_mvaInputVariables_;

  std::map<std::string, double> mvaInputs_; // key = branchName

  std::string mvaFileName_odd_;
  std::string mvaFileName_even_;
  TMVAInterface * mvaInterface_;
  bool enableBDTTransform_;
  std::string fitFunctionFileName_;
  std::string branchName_fitFunctionVariable_;
  BranchVarBase * branchVar_fitFunctionVariable_;

  std::string branchName_event_;
  BranchVarULong64_t * branchVar_event_;

  TH1* histogram_;

  int dimension_;
};

#endif // TallinnAnalysis_HistogramFillers_BranchVarHistogramFiller_h
