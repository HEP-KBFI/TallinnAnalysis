#ifndef TallinnAnalysis_HistogramTools_HistogramFillerBase_h
#define TallinnAnalysis_HistogramTools_HistogramFillerBase_h

#include "CommonTools/Utils/interface/TFileDirectory.h"            // TFileDirectory
#include "FWCore/ParameterSet/interface/ParameterSet.h"            // edm::ParameterSet

#include "TallinnAnalysis/Extractors/interface/BranchVars.h"       // BranchVarBase, BranchVar*
#include "TallinnAnalysis/Extractors/interface/VarExtractorBase.h" // VarExtractorBase

#include <memory>                                                  // std::shared_ptr
#include <string>                                                  // std::string
#include <vector>                                                  // std::vector

// forward declarations
class TH1;
class TH2;
class TTree;

class HistogramFillerBase
{
 public:
  HistogramFillerBase(const edm::ParameterSet & cfg);
  virtual ~HistogramFillerBase();

  /**
   * @brief Book histogram(s)
   */
  virtual
  void
  bookHistograms(TFileDirectory & dir) = 0;

  /**
   * @brief Call branchVar->setSetBranchAddress for all branchVars
   */
  virtual
  void
  setBranchAddresses(TTree * tree);

  /**
   * @brief Fill histogram(s)
   */
  virtual
  void
  fillHistograms(double evtWeight) = 0;

  /**
   * @brief Write all histograms to output file
   */
  static void
  writeHistograms();

 protected:
  TH1 *
  bookHistogram1D(TFileDirectory & dir, const edm::ParameterSet & cfg);

  TH2 *
  bookHistogram2D(TFileDirectory & dir, const edm::ParameterSet & cfg);

  edm::ParameterSet cfg_;

  TTree * tree_;

  std::vector<std::shared_ptr<VarExtractorBase>> extractors_;
  std::vector<std::shared_ptr<BranchVarBase>> branchVars_;

  // global "registry" of all histograms (1D and 2D) booked by analysis job 
  static std::map<TDirectory *, std::vector<TH1 *>> gHistograms_;

  // counter of TTreeFormula instantiated by all classes derived from HistogramFillerBase class,
  // used to assign each TTreeFormula a different name
  static int gNumTreeFormulas;
};

#include "FWCore/PluginManager/interface/PluginFactory.h"  // edmplugin::PluginFactory

typedef edmplugin::PluginFactory<HistogramFillerBase*(const edm::ParameterSet&)> HistogramFillerPluginFactory;

#endif // TallinnAnalysis_HistogramTools_HistogramFillerBase_h

