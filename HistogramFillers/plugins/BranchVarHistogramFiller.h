#ifndef TallinnAnalysis_HistogramFillers_BranchVarHistogramFiller_h
#define TallinnAnalysis_HistogramFillers_BranchVarHistogramFiller_h

#include "CommonTools/Utils/interface/TFileDirectory.h"                     // TFileDirectory
#include "FWCore/ParameterSet/interface/ParameterSet.h"                     // edm::ParameterSet

#include "TallinnAnalysis/HistogramFillers/interface/HistogramFillerBase.h" // HistogramFillerBase

// forward declarations
class TTree;

class BranchVarHistogramFiller : public HistogramFillerBase
{
 public:
  BranchVarHistogramFiller(const edm::ParameterSet & cfg);
  ~BranchVarHistogramFiller();

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
  std::vector<std::string> branchNames_;

  TH1* histogram_;

  int dimension_;
};

#endif // TallinnAnalysis_HistogramFillers_BranchVarHistogramFiller_h
