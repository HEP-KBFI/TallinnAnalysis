#include "TallinnAnalysis/HistogramTools/interface/HistogramFillerBase.h"

#include <TTree.h> // TTree

int HistogramFillerBase::gNumTreeFormulas = 0;

HistogramFillerBase::HistogramFillerBase(const edm::ParameterSet & cfg)
  : cfg_(cfg)
{}

HistogramFillerBase::~HistogramFillerBase()
{}

void
HistogramFillerBase::setBranchAddresses(TTree * tree)
{
  for ( std::shared_ptr<BranchVarBase> & branchVar : branchVars_ )
  {
    branchVar->setBranchAddress(tree);
  }
}

EDM_REGISTER_PLUGINFACTORY(HistogramFillerPluginFactory, "HistogramFillerPluginFactory");
