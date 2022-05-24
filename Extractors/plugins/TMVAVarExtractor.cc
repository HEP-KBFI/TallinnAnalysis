#include "TallinnAnalysis/Extractors/plugins/TMVAVarExtractor.h"

#include "TallinnAnalysis/Extractors/interface/merge_mvaInputs.h"     // merge_mvaInputs()

#include "TallinnNtupleProducer/CommonTools/interface/cmsException.h" // cmsException()

#include "TTree.h"                                                    // TTree

typedef std::vector<std::string> vstring;

TMVAVarExtractor::TMVAVarExtractor(const edm::ParameterSet & cfg)
  : MVAVarExtractorBase(cfg)
  , mvaInterface_(nullptr)
{
  if ( cfg.exists("classes") )
  {
    classes_ = cfg.getParameter<vstring>("classes");
  }
  mvaInterface_ = new TMVAInterface(mvaFileName_odd_, mvaInputVariables_, classes_, mvaFileName_even_, fitFunctionFileName_);
  mvaInterface_->transform_mvaOutput(cfg.getParameter<bool>("transform_mvaOutput"));
}

TMVAVarExtractor::~TMVAVarExtractor()
{}

bool
TMVAVarExtractor::is_multiclass()
{
  return mvaInterface_->is_multiclass();
}

double
TMVAVarExtractor::get_valueImp(const std::map<std::string, double> & mvaInputs) const
{
  return mvaInterface_->get_mvaOutput(mvaInputs, event_number_);
}

std::map<std::string, double>
TMVAVarExtractor::get_value_multiclassImp(const std::map<std::string, double> & mvaInputs) const
{
  return mvaInterface_->get_mvaOutput_multiclass(mvaInputs, event_number_);
}

DEFINE_EDM_PLUGIN(VarExtractorPluginFactory, TMVAVarExtractor, "TMVAVarExtractor");
