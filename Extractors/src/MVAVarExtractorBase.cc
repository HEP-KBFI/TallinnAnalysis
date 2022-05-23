#include "TallinnAnalysis/Extractors/interface/MVAVarExtractorBase.h"

#include "TallinnAnalysis/Extractors/interface/BranchVarFactory.h"    // BranchVarFactory
#include "TallinnAnalysis/Extractors/interface/BranchVars.h"          // BranchVarBase, BranchVar*
#include "TallinnAnalysis/Extractors/interface/merge_mvaInputs.h"     // merge_mvaInputs()

#include "TallinnNtupleProducer/CommonTools/interface/cmsException.h" // cmsException()

#include "TTree.h"                                                    // TTree

typedef std::vector<std::string> vstring;

MVAVarExtractorBase::MVAVarExtractorBase(const edm::ParameterSet & cfg)
  : VarExtractorBase(cfg)
  , branchName_event_("event")
  , branchVar_event_(nullptr)
{
  branchNames_mvaInputVariables_ = cfg.getParameter<vstring>("mvaInputVariables");
  branchVars_mvaInputVariables_ = branchVarFactory::create(branchNames_mvaInputVariables_);
  branchVars_.insert(branchVars_.end(), branchVars_mvaInputVariables_.begin(), branchVars_mvaInputVariables_.end());

  branchVar_event_ = std::shared_ptr<BranchVarBase>(new BranchVarULong64_t(branchName_event_));
  branchVars_.push_back(branchVar_event_);

  mvaFileName_even_ = cfg.getParameter<std::string>("mvaFileName_even");
  mvaFileName_odd_ = cfg.getParameter<std::string>("mvaFileName_odd");  

  if ( cfg.exists("fitFunctionFileName") )
  {
    fitFunctionFileName_ = cfg.getParameter<std::string>("fitFunctionFileName");
  }
}

MVAVarExtractorBase::~MVAVarExtractorBase()
{}

double
MVAVarExtractorBase::get_value() const
{
  read_mvaInputs();

  double mvaOutput = get_valueImp(mvaInputs_);
  return mvaOutput;
}

double
MVAVarExtractorBase::get_value(const std::map<std::string, double> & param) const
{
  read_mvaInputs();

  std::map<std::string, double> mvaInputs_parametrized = mvaInputs_;
  merge_mvaInputs(mvaInputs_parametrized, param);

  double mvaOutput = get_valueImp(mvaInputs_parametrized);
  return mvaOutput;
}

std::vector<double>
MVAVarExtractorBase::get_value(const std::vector<std::map<std::string, double>> & params) const
{
  read_mvaInputs();

  std::vector<double> mvaOutputs;

  std::map<std::string, double> mvaInputs_parametrized = mvaInputs_;
  for ( std::vector<std::map<std::string, double>>::const_iterator param = params.begin(); param != params.end(); ++param )
  {
    bool overwrite = ( param == params.begin() ) ? false : true;
    merge_mvaInputs(mvaInputs_parametrized, *param, overwrite);
    double mvaOutput = get_valueImp(mvaInputs_parametrized);
    mvaOutputs.push_back(mvaOutput);
  }

  return mvaOutputs;
}

std::map<std::string, double>
MVAVarExtractorBase::get_value_multiclass() const
{
  read_mvaInputs();

  std::map<std::string, double> mvaOutput = get_value_multiclassImp(mvaInputs_);
  return mvaOutput;
}

std::map<std::string, double>
MVAVarExtractorBase::get_value_multiclass(const std::map<std::string, double> & param) const
{
  read_mvaInputs();

  std::map<std::string, double> mvaInputs_parametrized = mvaInputs_;
  merge_mvaInputs(mvaInputs_parametrized, param);

  std::map<std::string, double> mvaOutput = get_value_multiclassImp(mvaInputs_parametrized);
  return mvaOutput;
}

std::vector<std::map<std::string, double>>
MVAVarExtractorBase::get_value_multiclass(const std::vector<std::map<std::string, double>> & params) const
{
  read_mvaInputs();

  std::vector<std::map<std::string, double>> mvaOutputs;

  std::map<std::string, double> mvaInputs_parametrized = mvaInputs_;
  for ( std::vector<std::map<std::string, double>>::const_iterator param = params.begin(); param != params.end(); ++param )
  {
    bool overwrite = ( param == params.begin() ) ? false : true;
    merge_mvaInputs(mvaInputs_parametrized, *param, overwrite);
    std::map<std::string, double> mvaOutput = get_value_multiclassImp(mvaInputs_parametrized);
    mvaOutputs.push_back(mvaOutput);
  }

  return mvaOutputs;
}

void
MVAVarExtractorBase::read_mvaInputs() const
{
  for ( const std::shared_ptr<BranchVarBase> & branchVar : branchVars_mvaInputVariables_ )
  {
    mvaInputs_[branchVar->getBranchName()] = branchVar->operator()();
  }

  event_number_ = (dynamic_cast<BranchVarULong64_t *>(branchVar_event_.get()))->getValue();
}
