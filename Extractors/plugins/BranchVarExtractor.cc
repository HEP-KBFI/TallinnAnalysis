#include "TallinnAnalysis/Extractors/plugins/BranchVarExtractor.h"

#include "TallinnAnalysis/Extractors/interface/BranchVarFactory.h" // BranchVarFactory
#include "TallinnAnalysis/Extractors/interface/BranchVars.h"       // BranchVarBase

#include "TTree.h"                                                 // TTree

BranchVarExtractor::BranchVarExtractor(const edm::ParameterSet & cfg)
  : VarExtractorBase(cfg)
  , branchVar_(nullptr)
{
  branchName_ = cfg.getParameter<std::string>("branchName");
}

BranchVarExtractor::~BranchVarExtractor()
{}

void
BranchVarExtractor::setBranchAddresses(TTree * tree)
{
  branchVar_ = BranchVarFactory::create(branchName_);
  branchVars_.clear();
  branchVars_.push_back(branchVar_);
  VarExtractorBase::setBranchAddresses(tree);
}

double
BranchVarExtractor::get_value() const
{
  return branchVar_->operator()();
}

DEFINE_EDM_PLUGIN(VarExtractorPluginFactory, BranchVarExtractor, "BranchVarExtractor");
