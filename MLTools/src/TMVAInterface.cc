#include "TallinnAnalysis/MLTools/interface/TMVAInterface.h"

#include "TallinnNtupleProducer/CommonTools/interface/cmsException.h" // cmsException()
#include "TallinnNtupleProducer/CommonTools/interface/get_fullpath.h" // get_fullpath()

#include "TallinnAnalysis/MLTools/interface/MVAInputVarTransformer.h" // MVAInputVarTransformer

#include <TMVA/Tools.h>                                               // TMVA::Tools::Instance()
#include <TMVA/Reader.h>                                              // TMVA::Reader

TMVAInterface::TMVAInterface(const std::string & mvaFileName_odd,
                             const std::vector<std::string> & mvaInputVariables,
                             const std::vector<std::string> & classes,
                             const std::string & mvaFileName_even,
                             const std::string & fitFunctionFileName)
  : classes_(classes)
  , is_multiclass_(classes.size() > 0)
  , mvaFileName_odd_("")
  , mva_odd_(nullptr)
  , mvaFileName_even_("")
  , mva_even_(nullptr)
  , mvaInputVariables_(mvaInputVariables)
  , fitFunctionFileName_("")
  , mvaInputTransformation_(nullptr)
  , isDEBUG_(false)
{
  mvaFileName_odd_ = get_fullpath(mvaFileName_odd);

  TMVA::Tools::Instance();
  mva_odd_ = new TMVA::Reader("!V:!Silent");

  for(const std::string & mvaInputVariable: mvaInputVariables)
  {
    mvaInputVariableMap_[mvaInputVariable] = -1.;
    mva_odd_->AddVariable(mvaInputVariable, &mvaInputVariableMap_[mvaInputVariable]);
  }

  mva_odd_->BookMVA("BDTG", mvaFileName_odd_);

  if(! mvaFileName_even.empty())
  {
    mvaFileName_even_ = get_fullpath(mvaFileName_even);

    TMVA::Tools::Instance();
    mva_even_ = new TMVA::Reader("!V:!Silent");

    for(const std::string & mvaInputVariable: mvaInputVariables)
    {
      mva_even_->AddVariable(mvaInputVariable, &mvaInputVariableMap_[mvaInputVariable]);
    }

    mva_even_->BookMVA("BDTG", mvaFileName_even_);
  }
  
  if(! fitFunctionFileName.empty())
  {
    fitFunctionFileName_ = get_fullpath(fitFunctionFileName);

    // initialize the new map and extracts the TF1s
    mvaInputTransformation_ = new MVAInputVarTransformer(mvaInputVariables_, fitFunctionFileName_);
  }
}

TMVAInterface::~TMVAInterface()
{
  delete mva_odd_;
  delete mva_even_;
  delete mvaInputTransformation_;
}

void
TMVAInterface::transform_mvaOutput(bool flag)
{
  transform_mvaOutput_ = flag;
}

bool
TMVAInterface::is_multiclass() const
{
  return is_multiclass_;
}

double
TMVAInterface::get_mvaOutput(const std::map<std::string, double> & mvaInputs, ULong64_t event_number) const
{
  if ( is_multiclass_ )
    throw cmsException(this, __func__, __LINE__)
      << "The function 'get_mvaOutput' is reserved for the non-multiclass case !!";
  evaluate(mvaInputs, event_number);
  return mvaOutput_;
}

std::map<std::string, double>
TMVAInterface::get_mvaOutput_multiclass(const std::map<std::string, double> & mvaInputs, ULong64_t event_number) const
{
  if ( !is_multiclass_ )
    throw cmsException(this, __func__, __LINE__)
      << "The function 'get_mvaOutput_multiclass' is reserved for the multiclass case !!";
  evaluate(mvaInputs, event_number);
  return mvaOutput_multiclass_;
}

const std::vector<std::string> &
TMVAInterface::get_mvaInputVariables() const
{
  return mvaInputVariables_;
}

namespace
{
  void copy_mvaInputs(const std::map<std::string, double> & mvaInputs,
                      std::map<std::string, Float_t> & mvaInputVariableMap)
  {
    for ( std::map<std::string, Float_t>::iterator mvaInputVariable = mvaInputVariableMap.begin(); 
          mvaInputVariable != mvaInputVariableMap.end(); ++mvaInputVariable ) {
      if ( mvaInputs.count(mvaInputVariable->first) )
      {
        mvaInputVariable->second = mvaInputs.at(mvaInputVariable->first);
      }
      else
      {
        throw cmsException(__func__, __LINE__)
          << "Missing value for MVA input variable = '" << mvaInputVariable->first << "' !!";
      }
    }
  }
}

void
TMVAInterface::evaluate(const std::map<std::string, double> & mvaInputs, ULong64_t event_number) const
{
  if ( fitFunctionFileName_ != "" )
  {
    // transform MVA input variables (to remove e.g. dependency on gen_mHH)
    std::map<std::string, double> mvaInputs_transformed = mvaInputTransformation_->TransformMVAInputVars(mvaInputs);
    copy_mvaInputs(mvaInputs_transformed, mvaInputVariableMap_);
  }
  else
  {
    copy_mvaInputs(mvaInputs, mvaInputVariableMap_);
  }

  const TMVA::Reader * mva = nullptr;
  if ( mva_odd_ && event_number % 2 ) mva = mva_odd_;
  else                                mva = mva_even_;


  if ( is_multiclass_ )
  {
    // Casting mva from "const TMVA::Reader*" to "TMVA::Reader*" (since EvaluateMulticlass() doesn't accept const input)
    std::vector<float> mvaOutput = (const_cast<TMVA::Reader*>(mva))->EvaluateMulticlass("BDTG");
    if ( mvaOutput.size() != classes_.size() )
      throw cmsException(this, __func__, __LINE__)
        << "Size of MVA output vector does not match number of classes !!";
    size_t numClasses = classes_.size();
    for ( size_t idxClass = 0; idxClass < numClasses; ++idxClass )
    {
      mvaOutput_multiclass_[classes_.at(idxClass)] = mvaOutput.at(idxClass);
    }
  }
  else
  {
    // Casting mva from "const TMVA::Reader*" to "TMVA::Reader*" (since EvaluateMVA() doesn't accept const input)
    double mvaOutput = (const_cast<TMVA::Reader*>(mva))->EvaluateMVA("BDTG");
    if ( transform_mvaOutput_ )
    {
      mvaOutput_ = 1. / (1. + std::sqrt((1. - mvaOutput) / (1. + mvaOutput)));
    }
  }
}
