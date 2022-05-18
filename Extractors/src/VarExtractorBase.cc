#include "TallinnAnalysis/Extractors/interface/VarExtractorBase.h"

#include <TTree.h> // TTree

VarExtractorBase::VarExtractorBase(const edm::ParameterSet & cfg)
{}

VarExtractorBase::~VarExtractorBase()
{}

void
VarExtractorBase::setBranchAddresses(TTree * tree)
{
  for ( std::shared_ptr<BranchVarBase> & branchVar : branchVars_ )
  {
    branchVar->setBranchAddress(tree);
  }
}

EDM_REGISTER_PLUGINFACTORY(VarExtractorPluginFactory, "VarExtractorPluginFactory");
