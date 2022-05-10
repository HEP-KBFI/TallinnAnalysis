#include "TallinnAnalysis/HistogramFillers/interface/TMVAHistogramFiller.h"

#include "TallinnAnalysis/HistogramFiller/interface/BranchVarFactory.h" // BranchVarFactory
#include "TallinnAnalysis/HistogramFiller/interface/BranchVars.h"       // BranchVarBase, BranchVar*
#include "TallinnAnalysis/HistogramTools/interface/createHistogram.h"   // createHistogram1D()
#include "TallinnAnalysis/HistogramTools/interface/fillWithOverFlow.h"  // fillWithOverFlow1D()

#include "TallinnNtupleProducer/CommonTools/interface/cmsException.h"   // cmsException()

#include "TTree.h"                                                      // TTree

typedef std::vector<std::string> vstring;

TMVAHistogramFiller::TMVAHistogramFiller(const edm::ParameterSet & cfg)
  : HistogramFillerBase(cfg)
  , mvaInterface_even_(nullptr)
  , mvaInterface_odd_(nullptr)
  , branchVar_fitFunctionVariable_;
  , branchName_event_("event")
  , branchVar_event_(nullptr)
  , histogram_(nullptr)
{
  branchNames_mvaInputVariables_ = cfg.getParameter<vstring>("mvaInputVariables");
  mvaFileName_even_ = cfg.getParameter<std::string>("mvaFileName_even");
  mvaFileName_odd_ = cfg.getParameter<std::string>("mvaFileName_odd");  
  if ( cfg.exists("fitFunctionFileName" )
  {
    fitFunctionFileName_ = cfg.getParameter<std::string>("fitFunctionFileName");
    branchName_fitFunctionVariable_ = cfg.getParameter<std::string>("fitFunctionVariable");
  }
  BranchVarFactory branchVarFactory;
  branchVars_mvaInputVariables_ = branchVarFactory.create(branchNames_mvaInputVariables_);
  branchVars_.insert(branchVars_.end(), branchVars_mvaInputVariables_.begin(), branchVars_mvaInputVariables_.end());
  if ( fitFunctionFileName_ != "" && branchName_fitFunctionVariable_ != "" )
  {
    branchVar_fitFunctionVariable_ = branchVarFactory.create(branchName_fitFunctionVariable_);
    branchVars_.push_back(branchVar_fitFunctionVariable_);
    mvaInterface_ = new TMVAInterface(mvaFileName_odd_, mvaFileName_even_, mvaInputVariables_, fitFunctionFileName_);
  }
  else
  {
    mvaInterface_ = new TMVAInterface(mvaFileName_odd_, mvaFileName_even_, mvaInputVariables_);
  }
  enableBDTTransform_ = cfg.getParameter<bool>("enableBDTTransform");
  if ( enableBDTTransform_ ) mvaInterface_->enableBDTTransform();
  else                       mvaInterface_->disableBDTTransform();
  branch_event_ = new BranchVarULong64_t(branchName_event_);
  branchVars_.push_back(branch_event_);
}

TMVAHistogramFiller::~TMVAHistogramFiller()
{}

void
TMVAHistogramFiller::bookHistograms(TFileDirectory & dir)
{
  histogram_ = createHistogram1D(dir, cfg);
}

void
TMVAHistogramFiller::fillHistograms(double evtWeight)
{
  for ( BranchVarBase * branchVar : branchVars_mvaInputVariables_ )
  {
    mvaInputs_[branchVar->getBranchName()] = branchVar->operator()();
  }

  ULong64_t event_number = branchVar_event_->getValue();

  double mvaOutput = mvaInterface_->operator(mvaInputs_, event_number);

  if ( branchVar_fitFunctionVariable_ )
  {
    // TO-DO: check if adding the fit function variable to the mvaInputs map is indeed the correct thing to do !!
    mvaInputs_[branchVar_fitFunctionVariable_->getBranchName()] = branchVar_fitFunctionVariable_->operator()();
  }
 
  // TO-DO: add HH reweights and add "SM", "BM1",..., "BM12" as one-hot encoded input variables

  https://github.com/HEP-KBFI/hh-multilepton/blob/master/bin/analyze_hh_2lss_leq1tau.cc#L2843-L2845

  BDTOutput_Map_spin2 = CreateResonantBDTOutputMap(gen_mHH, BDT_spin2, BDTInputs_spin2, eventInfo.event, "_spin2");
  BDTOutput_Map_spin0 = CreateResonantBDTOutputMap(gen_mHH, BDT_spin0, BDTInputs_spin0, eventInfo.event, "_spin0");
  BDTOutput_Map_nonRes = CreateNonResonantBDTOutputMap(nonRes_BMs, BDT_nonRes, BDTInputs_nonRes, eventInfo.event, hhWeight_couplings);

  fillWithOverFlow1D(histogram_, mvaOutput, evtWeight);
}

DEFINE_EDM_PLUGIN(HistogramFillerPluginFactory, TMVAHistogramFiller, "TMVAHistogramFiller");
