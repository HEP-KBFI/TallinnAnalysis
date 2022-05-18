#include "TallinnAnalysis/Extractors/plugins/BranchVarExtractor.h"

#include "TallinnAnalysis/Extractors/interface/BranchVarFactory.h" // BranchVarFactory
#include "TallinnAnalysis/Extractors/interface/BranchVars.h"       // BranchVarBase

#include "TTree.h"                                                 // TTree

BranchVarExtractor::BranchVarExtractor(const edm::ParameterSet & cfg)
  : VarExtractorBase(cfg)
  , branchVar_(nullptr)
{
  branchName_ = cfg.getParameter<std::string>("branchName");
  BranchVarFactory branchVarFactory;
  branchVar_ = branchVarFactory.create(branchName_);
  branchVars_.push_back(branchVar_);
}

BranchVarExtractor::~BranchVarExtractor()
{}

double
BranchVarExtractor::get_value() const
{
  return branchVar_->operator()();
}

DEFINE_EDM_PLUGIN(VarExtractorPluginFactory, BranchVarExtractor, "BranchVarExtractor");
