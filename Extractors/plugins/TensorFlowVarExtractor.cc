#include "TallinnAnalysis/Extractors/plugins/TensorFlowVarExtractor.h"

#include "TallinnAnalysis/Extractors/interface/merge_mvaInputs.h"     // merge_mvaInputs()

#include "TallinnNtupleProducer/CommonTools/interface/cmsException.h" // cmsException()

#include "TTree.h"                                                    // TTree

typedef std::vector<std::string> vstring;

TensorFlowVarExtractor::TensorFlowVarExtractor(const edm::ParameterSet & cfg)
  : MVAVarExtractorBase(cfg)
  , mvaInterface_(nullptr)
{
  if ( cfg.exists("classes") )
  {
    classes_ = cfg.getParameter<vstring>("classes");
  }
  mvaInterface_ = new TensorFlowInterface(mvaFileName_odd_, branchNames_mvaInputVariables_, classes_, mvaFileName_even_, fitFunctionFileName_);
}

TensorFlowVarExtractor::~TensorFlowVarExtractor()
{}

bool
TensorFlowVarExtractor::is_multiclass()
{
  return mvaInterface_->is_multiclass();
}

double
TensorFlowVarExtractor::get_valueImp(const std::map<std::string, double> & mvaInputs) const
{
  return mvaInterface_->get_mvaOutput(mvaInputs, event_number_);
}

std::map<std::string, double>
TensorFlowVarExtractor::get_value_multiclassImp(const std::map<std::string, double> & mvaInputs) const
{
  return mvaInterface_->get_mvaOutput_multiclass(mvaInputs, event_number_);
}

DEFINE_EDM_PLUGIN(VarExtractorPluginFactory, TensorFlowVarExtractor, "TensorFlowVarExtractor");
