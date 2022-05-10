#include "TallinnAnalysis/HistogramFillers/interface/HistogramFillerBase.h"

#include <TTree.h> // TTree

HistogramFillerBase::HistogramFillerBase(const edm::ParameterSet & cfg)
  : cfg_(cfg)
{}

HistogramFillerBase::~HistogramFillerBase()
{
  for ( BranchVarBase * branchVar : branchVars_ )
  {
    delete branchVar;
  }
}

void
HistogramFillerBase::setSetBranchAddresses(TTree * tree)
{
  for ( BranchVarBase * branchVar : branchVars_ )
  {
    branchVar->setSetBranchAddress(TTree * tree);
  }
}

EDM_REGISTER_PLUGINFACTORY(HistogramFillerPluginFactory, "HistogramFillerPluginFactory");
