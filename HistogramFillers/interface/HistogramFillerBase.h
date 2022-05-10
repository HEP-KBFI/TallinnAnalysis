#ifndef TallinnAnalysis_HistogramFillers_HistogramFillerBase_h
#define TallinnAnalysis_HistogramFillers_HistogramFillerBase_h

#include "CommonTools/Utils/interface/TFileDirectory.h"           // TFileDirectory
#include "FWCore/ParameterSet/interface/ParameterSet.h"           // edm::ParameterSet

#include "TallinnAnalysis/HistogramFiller/interface/BranchVars.h" // BranchVarBase, BranchVar*

#include <string>                                                 // std::string
#include <vector>                                                 // std::vector

// forward declarations
class TTree;

class HistogramFillerBase
{
 public:
  HistogramFillerBase(const edm::ParameterSet & cfg);
  virtual ~HistogramFillerBase();

  /**
   * @brief Call branchVar->setSetBranchAddress for all branchVars
   */
  virtual
  void
  setSetBranchAddresses(TTree * tree);

  /**
   * @brief Book histogram(s)
   */
  virtual
  void
  bookHistograms(TFileDirectory & dir);

  /**
   * @brief Fill histogram(s)
   */
  virtual
  void
  fillHistograms(double evtWeight);

 protected:
  edm::ParameterSet cfg_;

  std::vector<BranchVarBase *> branchVars_;
};

#include "FWCore/PluginManager/interface/PluginFactory.h"  // edmplugin::PluginFactory

typedef edmplugin::PluginFactory<HistogramFillerBase*(const edm::ParameterSet&)> HistogramFillerPluginFactory;

#endif // TallinnAnalysis_HistogramFillers_HistogramFillerBase_h

