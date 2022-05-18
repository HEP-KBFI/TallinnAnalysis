#include "TallinnAnalysis/Extractors/interface/BranchVarFactory.h"

#include "TallinnAnalysis/Extractors/interface/BranchVars.h"          // BranchVarBase, BranchVar*

#include "TallinnNtupleProducer/CommonTools/interface/cmsException.h" // cmsException()

BranchVarFactory::BranchVarFactory()
{}

BranchVarFactory::~BranchVarFactory()
{}

std::shared_ptr<BranchVarBase>
BranchVarFactory::create(const std::string & branchName_and_Type)
{
  size_t pos_separator = branchName_and_Type.rfind("/");
  if ( pos_separator == 0 || pos_separator == std::string::npos )
    throw cmsException(__func__, __LINE__) 
      << "Invalid branch name = '" << branchName_and_Type << "' !!";
  std::string branchName = std::string(branchName_and_Type, 0, pos_separator);
  std::string branchType = std::string(branchName_and_Type, pos_separator + 1);
  assert(branchName != "" && branchType != "");
  if      ( branchType == "O" ) return std::shared_ptr<BranchVarBase>(new BranchVarBool_t(branchName));
  else if ( branchType == "D" ) return std::shared_ptr<BranchVarBase>(new BranchVarDouble_t(branchName));
  else if ( branchType == "F" ) return std::shared_ptr<BranchVarBase>(new BranchVarFloat_t(branchName));
  else if ( branchType == "I" ) return std::shared_ptr<BranchVarBase>(new BranchVarInt_t(branchName));
  else if ( branchType == "i" ) return std::shared_ptr<BranchVarBase>(new BranchVarUInt_t(branchName));
  else if ( branchType == "L" ) return std::shared_ptr<BranchVarBase>(new BranchVarLong64_t(branchName));
  else if ( branchType == "l" ) return std::shared_ptr<BranchVarBase>(new BranchVarULong64_t(branchName));
  else throw cmsException(__func__, __LINE__) 
      << "Invalid branch type = '" << branchType << "' !!";
  return std::shared_ptr<BranchVarBase>();
}

std::vector<std::shared_ptr<BranchVarBase>>
BranchVarFactory::create(const std::vector<std::string> & branchNames_and_Types)
{
  std::vector<std::shared_ptr<BranchVarBase>> branchVars;
  for ( const std::string & branchName_and_Type : branchNames_and_Types )
  { 
    std::shared_ptr<BranchVarBase> branchVar = create(branchName_and_Type);
    branchVars.push_back(branchVar);
  }
  return branchVars;
}
