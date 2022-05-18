#ifndef TallinnAnalysis_HistogramTools_BDTHistogramFiller_HH_multilepton_h
#define TallinnAnalysis_HistogramTools_BDTHistogramFiller_HH_multilepton_h

#include "CommonTools/Utils/interface/TFileDirectory.h"                   // TFileDirectory
#include "FWCore/ParameterSet/interface/ParameterSet.h"                   // edm::ParameterSet

#include "TallinnAnalysis/Extractors/interface/VarExtractorBase.h"        // VarExtractorBase, VarExtractorPluginFactory
#include "TallinnAnalysis/HistogramTools/interface/HistogramFillerBase.h" // HistogramFillerBase

#include <map>                                                            // std::map
#include <memory>                                                         // std::shared_ptr
#include <string>                                                         // std::string

// forward declarations
class HHWeightInterfaceCouplings;
class TMVAInterface;
class TH1;
class TTree;
class TTreeFormula;

class BDTHistogramFiller_HH_multilepton : public HistogramFillerBase
{
 public:
  BDTHistogramFiller_HH_multilepton(const edm::ParameterSet & cfg);
  ~BDTHistogramFiller_HH_multilepton();

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
  std::vector<std::string> mvaInputVariables_;
  std::map<std::string, std::shared_ptr<BranchVarBase>> branchVarMap_; // key = (branch)name of mvaInputVariable
  std::map<std::string, double> mvaInputs_;                            // key = (branch)name of mvaInputVariable
  TMVAInterface * mvaInterface_;

  enum Mode { kNonresonant, kResonant };
  Mode mode_;

  std::vector<std::string> parameters_; // coupling scenarios for non-resonant, mass-points for resonant

  std::map<std::string, TH1 *> histograms_; // key = parameter

  std::shared_ptr<BranchVarBase> branchVar_event_;

  // data-members specific to non-resonant case
  std::map<std::string, std::shared_ptr<BranchVarBase>> hhReweightMap_; // key = parameter
  HHWeightInterfaceCouplings * hhCouplings_;

  // data-members specific to resonant case
  std::map<std::string, double> massPoints_;
};

#endif // TallinnAnalysis_HistogramTools_HistogramFiller1D_h
