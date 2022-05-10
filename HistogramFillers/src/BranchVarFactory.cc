#include "TallinnAnalysis/HistogramFiller/interface/BranchVarFactory.h"

#include "TallinnAnalysis/HistogramFiller/interface/BranchVars.h"     // BranchVarBase, BranchVar*

#include "TallinnNtupleProducer/CommonTools/interface/cmsException.h" // cmsException()

BranchVarFactory::BranchVarFactory()
{}

BranchVarFactory::~BranchVarFactory()
{}

BranchVarBase *
BranchVarFactory::create(const std::string & branchName_and_Type)
{
  size_t pos_separator = branchName_and_Type.rfind("/";
  if ( pos_separator == 0 || pos_separator == std::string::npos )
    throw cmsException(__func__, __LINE__) 
      << "Invalid branch name = '" << branchName_and_Type << "' !!";
  std::string branchName = std::string(branchName_and_Type, 0, pos_separator);
  std::string branchType = std::string(branchName_and_Type, pos_separator + 1);
  assert(branchName != "" && branchType != "");
  if      ( branchType == "O" ) return BranchVarBool_t(branchName);
  else if ( branchType == "D" ) return BranchVarDouble_t(branchName);
  else if ( branchType == "F" ) return BranchVarFloat_t(branchName);
  else if ( branchType == "I" ) return BranchVarInt_t(branchName);
  else if ( branchType == "i" ) return BranchVarUInt_t(branchName);
  else if ( branchType == "L" ) return BranchVarLong64_t(branchName);
  else if ( branchType == "l" ) return BranchVarULong64_t(branchName);
  else throw cmsException(__func__, __LINE__) 
      << "Invalid branch type = '" << branchType << "' !!";
  return nullptr;
}

std::vector<BranchVarBase *>
BranchVarFactory::create(const std::vector<std::string> & branchNames_and_Types)
{
  std::vector<BranchVarBase *> branchVars;
  for ( const std::string & branchName_and_Type : branchNames_and_Types )
  { 
    BranchVarBase * branchVar = create(branchName);
    branchVars.push_back(branchVar);
  }
  return branchVars;
}
