#ifndef TallinnAnalysis_HistogramTools_HistogramFiller1D_h
#define TallinnAnalysis_HistogramTools_HistogramFiller1D_h

#include "CommonTools/Utils/interface/TFileDirectory.h"                   // TFileDirectory
#include "FWCore/ParameterSet/interface/ParameterSet.h"                   // edm::ParameterSet

#include "TallinnAnalysis/Extractors/interface/VarExtractorBase.h"        // VarExtractorBase, VarExtractorPluginFactory
#include "TallinnAnalysis/HistogramTools/interface/HistogramFillerBase.h" // HistogramFillerBase

#include <memory>                                                         // std::shared_ptr

// forward declarations
class TH1;
class TTree;
class TTreeFormula;

class HistogramFiller1D : public HistogramFillerBase
{
 public:
  HistogramFiller1D(const edm::ParameterSet & cfg);
  ~HistogramFiller1D();

  /**
   * @brief Book histogram
   */
  void
  bookHistograms(TFileDirectory & dir);

  /**
   * @brief Call VarExtractorBase->setBranchAddresses and TTreeFormula constructor
   */
  virtual
  void
  setBranchAddresses(TTree * tree);

  /**
   * @brief Fill histogram
   */
  void
  fillHistograms(double evtWeight);

 private:
  struct categoryEntry
  {
    std::string histogramDir_;
    std::string selection_;
    TTreeFormula * treeFormula_;
    TH1 * histogram_;
  };
  std::vector<categoryEntry *> categories_;

  std::shared_ptr<VarExtractorBase> observable_;
};

#endif // TallinnAnalysis_HistogramTools_HistogramFiller1D_h
