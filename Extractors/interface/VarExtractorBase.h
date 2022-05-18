#ifndef TallinnAnalysis_Extractors_VarExtractorBase_h
#define TallinnAnalysis_Extractors_VarExtractorBase_h

#include "CommonTools/Utils/interface/TFileDirectory.h"      // TFileDirectory
#include "FWCore/ParameterSet/interface/ParameterSet.h"      // edm::ParameterSet

#include "TallinnAnalysis/Extractors/interface/BranchVars.h" // BranchVarBase, BranchVar*

#include <memory>                                            // std::shared_ptr
#include <string>                                            // std::string
#include <vector>                                            // std::vector

// forward declarations
class TTree;

class VarExtractorBase
{
 public:
  VarExtractorBase(const edm::ParameterSet & cfg);
  virtual ~VarExtractorBase();

  /**
   * @brief Call branchVar->setBranchAddress for all branchVars
   */
  virtual
  void
  setBranchAddresses(TTree * tree);

  /**
   * @brief Return value to be filled into histogram
   */
  virtual
  double
  get_value() const = 0;

 protected:
  std::vector<std::shared_ptr<BranchVarBase>> branchVars_;
};

#include "FWCore/PluginManager/interface/PluginFactory.h"  // edmplugin::PluginFactory

typedef edmplugin::PluginFactory<VarExtractorBase*(const edm::ParameterSet&)> VarExtractorPluginFactory;

#endif // TallinnAnalysis_Extractors_VarExtractorBase_h

