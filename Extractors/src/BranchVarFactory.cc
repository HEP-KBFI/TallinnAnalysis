#include "TallinnAnalysis/Extractors/interface/BranchVarFactory.h"

#include "TallinnAnalysis/Extractors/interface/BranchVars.h"          // BranchVarBase, BranchVar*
#include "TallinnAnalysis/Extractors/interface/SelectionParser.h"     // SelectionParser

#include "TallinnNtupleProducer/CommonTools/interface/cmsException.h" // cmsException()
#include "TallinnNtupleProducer/CommonTools/interface/contains.h"     // contains()

#include <TBranch.h>   // TBranch
#include <TLeaf.h>     // TLeaf
#include <TObjArray.h> // TObjArray
#include <TString.h>   // Form()
#include <TTree.h>     // TTree

#include <iostream>    // std::cout

std::map<std::string, std::string> BranchVarFactory::branchMap_;
std::string BranchVarFactory::central_or_shift_;

BranchVarFactory::BranchVarFactory()
{}

BranchVarFactory::~BranchVarFactory()
{}

void
BranchVarFactory::initialize(TTree * tree)
{
  branchMap_.clear();
  assert(tree);
  TObjArray * branches = tree->GetListOfBranches();
  int numBranches = branches->GetEntries();
  for ( int idxBranch = 0; idxBranch < numBranches; ++idxBranch )
  {
    TBranch * branch = dynamic_cast<TBranch *>(branches->At(idxBranch));
    assert(branch);
    std::string branchName_and_Type = branch->GetTitle();
    std::string branchName, branchType;
    bool branchName_and_Type_isInitialized = false;
    size_t pos_separator = branchName_and_Type.rfind("/");
    if ( pos_separator != 0 && pos_separator != std::string::npos )
    {
      branchName = std::string(branchName_and_Type, 0, pos_separator);
      branchType = std::string(branchName_and_Type, pos_separator + 1);
      branchName_and_Type_isInitialized = true;
    }
    else
    {
      TObjArray * leaves = branch->GetListOfLeaves();
      int numLeaves = leaves->GetEntries();
      if ( numLeaves == 1 )
      {
        TLeaf * leaf = dynamic_cast<TLeaf * >(leaves->At(0));
        assert(leaf);
        branchName = branch->GetName();
        branchType = leaf->GetTypeName();
        branchName_and_Type_isInitialized = true;
      }
    }
    if ( !branchName_and_Type_isInitialized )
      throw cmsException(__func__, __LINE__) 
        << "Failed to infer branch type for branch '" << branch->GetName() << "' !!";
    if ( branchMap_.find(branchName) != branchMap_.end() )
      throw cmsException(__func__, __LINE__) 
        << "Branch '" << branchName << "' found in TTree more than once !!";
    branchMap_[branchName] = branchType;
  }
}

void
BranchVarFactory::set_central_or_shift(const std::string & central_or_shift)
{
  central_or_shift_ = central_or_shift;
}

std::string
BranchVarFactory::get_selection(const std::string & selection)
{
  if ( branchMap_.size() == 0 )
    throw cmsException(__func__, __LINE__) 
      << "Branch map not initialized !!";
  if ( central_or_shift_ == "" )
    throw cmsException(__func__, __LINE__) 
      << "Systematic uncertainty not initialized !!";
  SelectionParser parser;
  std::vector<std::string> elements = parser.parse(selection);
  std::vector<std::string> elements_with_whitespace = { "&&", "&", "||", "|", ">=", "==", "<=", ">", "=", "<" };
  std::string selection_modified;
  for ( const std::string element : elements )
  {
    if ( contains(elements_with_whitespace, element) )
    {
      selection_modified += Form(" %s ", element.data());
    }
    else
    {
      std::map<std::string, std::string>::const_iterator branch = branchMap_.find(element);
      std::map<std::string, std::string>::const_iterator branch_modified = branchMap_.find(Form("%s_%s", element.data(), central_or_shift_.data()));
      if ( branch != branchMap_.end() && branch_modified != branchMap_.end() )
      {
        std::cout << "Replacing branch '" << branch->first << "' by '" << branch_modified->first << "'" << std::endl;
        selection_modified += branch_modified->first;
      }
      else
      {
        selection_modified += element;
      }
    }
  }
  return selection_modified;
}

std::shared_ptr<BranchVarBase>
BranchVarFactory::create(const std::string & branchName)
{
  if ( branchMap_.size() == 0 )
    throw cmsException(__func__, __LINE__) 
      << "Branch map not initialized !!";
  if ( central_or_shift_ == "" )
    throw cmsException(__func__, __LINE__) 
      << "Systematic uncertainty not initialized !!";
  std::map<std::string, std::string>::const_iterator branch = branchMap_.find(branchName);
  if ( branch == branchMap_.end() )
    throw cmsException(__func__, __LINE__) 
      << "Branch '" << branchName << "' not found in TTree !!";
  std::string branchName_modified = Form("%s_%s", branchName.data(), central_or_shift_.data());
  std::map<std::string, std::string>::const_iterator branch_modified = branchMap_.find(branchName_modified);
  std::map<std::string, std::string>::const_iterator branchName_and_Type;
  if ( branch_modified != branchMap_.end() )
  {
    std::cout << "Replacing branch '" << branch->first << "' by '" << branch_modified->first << "'" << std::endl;
    branchName_and_Type = branch_modified;
  }
  else
  {
    branchName_and_Type = branch;
  }
  std::string branchNameToUse = branchName_and_Type->first;
  std::string branchType = branchName_and_Type->second;
  assert(branchNameToUse != "" && branchType != "");
  if      ( branchType == "O" ) return std::shared_ptr<BranchVarBase>(new BranchVarBool_t(branchNameToUse));
  else if ( branchType == "D" ) return std::shared_ptr<BranchVarBase>(new BranchVarDouble_t(branchNameToUse));
  else if ( branchType == "F" ) return std::shared_ptr<BranchVarBase>(new BranchVarFloat_t(branchNameToUse));
  else if ( branchType == "I" ) return std::shared_ptr<BranchVarBase>(new BranchVarInt_t(branchNameToUse));
  else if ( branchType == "i" ) return std::shared_ptr<BranchVarBase>(new BranchVarUInt_t(branchNameToUse));
  else if ( branchType == "L" ) return std::shared_ptr<BranchVarBase>(new BranchVarLong64_t(branchNameToUse));
  else if ( branchType == "l" ) return std::shared_ptr<BranchVarBase>(new BranchVarULong64_t(branchNameToUse));
  else throw cmsException(__func__, __LINE__) 
      << "Invalid branch type = '" << branchType << "' !!";
  return std::shared_ptr<BranchVarBase>();
}

std::vector<std::shared_ptr<BranchVarBase>>
BranchVarFactory::create(const std::vector<std::string> & branchNames)
{
  std::vector<std::shared_ptr<BranchVarBase>> branchVars;
  for ( const std::string & branchName : branchNames )
  { 
    std::shared_ptr<BranchVarBase> branchVar = create(branchName);
    branchVars.push_back(branchVar);
  }
  return branchVars;
}
