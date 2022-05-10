#ifndef TallinnAnalysis_HistogramFillers_BranchVars_h
#define TallinnAnalysis_HistogramFillers_BranchVars_h

#include <Rtypes.h> // Bool_t, Double_t, Float_t, Int_t, UInt_t, Long64_t, ULong64_t
#include <TTree.h>  // TTree

#include <map>      // std::map
#include <string>   // std::string

class BranchVarBase
{
 public:
  BranchVarBase(const std::string & branchName);
  virtual ~BranchVarBase();

  virtual 
  void
  setBranchAddress(TTree * tree) = 0;
  
  const std::string &
  getBranchName() const;

  virtual
  double
  operator()() const = 0;

 protected:
  void
  registerBranchName() const;

  std::string branchName_;

  // CV: make sure that only one BranchVar instance exists for a given branchName,
  //     as ROOT cannot handle multiple TTree::SetBranchAddress calls for the same branch.
  static std::map<std::string, int> numInstances_;          // key = branchName
  static std::map<std::string, BranchVarBase *> instances_; // key = branchName
};

template <typename T>
class BranchVar : BranchVarBase
{
 public:
  BranchVar(const std::string & branchName)
    : BranchVarBase(branchName)
    , value_(0)
  {}
  ~BranchVar()
  {}

  void
  setSetBranchAddress(TTree * tree)
  {
    if ( instances_[branchName_] == this )
    {
      tree->SetBranchAddress(branchName_.data(), &value_);
    }
  }

  T
  getValue() const
  {
    const BranchVar<T> * gInstance = instances_[branchName_];
    assert(gInstance);
    return gInstance->value_;
  }

  double
  operator()() const
  {
    return static_cast<double>(this->getValue());
  }

 private:
  T value_;
};

typedef BranchVar<Bool_t>    BranchVarBool_t;
typedef BranchVar<Double_t>  BranchVarDouble_t;
typedef BranchVar<Float_t>   BranchVarFloat_t;
typedef BranchVar<Int_t>     BranchVarInt_t;
typedef BranchVar<UInt_t>    BranchVarUInt_t;
typedef BranchVar<Long64_t>  BranchVarLong64_t;
typedef BranchVar<ULong64_t> BranchVarULong64_t;

#endif // TallinnAnalysis_HistogramFillers_BranchVars_h

