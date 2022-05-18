#include "TallinnAnalysis/Extractors/interface/BranchVars.h"

std::map<std::string, int> BranchVarBase::numInstances_;
std::map<std::string, const BranchVarBase *> BranchVarBase::instances_;

BranchVarBase::BranchVarBase(const std::string & branchName)
  : branchName_(branchName)
{
  registerBranchName();
}

BranchVarBase::~BranchVarBase()
{
  --numInstances_[branchName_];
  assert(numInstances_[branchName_] >= 0);
  if ( numInstances_[branchName_] == 0 )
  {
    instances_[branchName_] = nullptr;
  }
}

const std::string &
BranchVarBase::getBranchName() const
{
  return branchName_;
}

void
BranchVarBase::registerBranchName() const
{
  if ( numInstances_[branchName_] == 0 )
  {
    instances_[branchName_] = this;
  }
  ++numInstances_[branchName_];
}
