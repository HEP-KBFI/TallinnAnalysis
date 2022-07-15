#include "TallinnAnalysis/HistogramTools/plugins/BDTHistogramFiller_HH_multilepton.h"

#include "TallinnAnalysis/Extractors/interface/BranchVarFactory.h"                     // BranchVarFactory
#include "TallinnAnalysis/Extractors/interface/BranchVars.h"                           // BranchVarUInt_t, BranchVarULong64_t
#include "TallinnAnalysis/HistogramTools/interface/fillWithOverFlow.h"                 // fillWithOverFlow1D()
#include "TallinnAnalysis/MLTools/interface/TMVAInterface.h"
#include "TallinnNtupleProducer/CommonTools/interface/cmsException.h"                  // cmsException()
#include "TallinnNtupleProducer/CommonTools/interface/contains.h"                      // contains()
#include "TallinnNtupleProducer/EvtWeightTools/interface/HHCoupling.h"                 // HHCoupling
#include "TallinnNtupleProducer/EvtWeightTools/interface/HHWeightInterfaceCouplings.h" // HHWeightInterfaceCouplings

#include "TString.h"                                                                   // Form()
#include "TTree.h"                                                                     // TTree
#include "TTreeFormula.h"                                                              // TTreeFormula
#include "TH1.h"                                                                       // TH1

typedef std::vector<double> vdouble;
typedef std::vector<std::string> vstring;

BDTHistogramFiller_HH_multilepton::BDTHistogramFiller_HH_multilepton(const edm::ParameterSet & cfg)
  : HistogramFillerBase(cfg)
  , mvaInterface_(nullptr)
  , hhCouplings_(nullptr)
  , jsonFile_(nullptr)
{
  mvaInputVariables_ = cfg.getParameter<vstring>("mvaInputVariables");
  mvaInputVariables_wParameters_ = mvaInputVariables_;

  std::string mode_string = cfg.getParameter<std::string>("mode");
  if      ( mode_string == "nonresonant" ) mode_ = Mode::kNonresonant;
  else if ( mode_string == "resonant"    ) mode_ = Mode::kResonant;
  else throw cmsException(this, __func__, __LINE__) 
         << "Invalid Configuration parameter 'mode' = " << mode_string << " !!";
  if ( mode_ == Mode::kNonresonant )
  {
    edm::ParameterSet cfg_hhCoupling = cfg.getParameterSet("hhCoupling");
    hhCouplings_ = new HHWeightInterfaceCouplings(cfg_hhCoupling);
    parameters_ = cfg.getParameter<vstring>("nonRes_BMs");
    for ( const std::string & bmName : parameters_ )
    {
      std::string branchName = Form("hhReweight_%s", bmName.data());
      std::shared_ptr<BranchVarBase> branchVar(new BranchVarFloat_t(branchName));
      hhReweightMap_[bmName] = branchVar;
      branchVars_.push_back(branchVar);
      const HHCoupling & hhCoupling = hhCouplings_->getCoupling(bmName);
      const std::string & bmName_training = hhCoupling.training();
      if ( !contains(mvaInputVariables_wParameters_, bmName_training) )
      {
        mvaInputVariables_wParameters_.push_back(bmName_training);
        mvaInputs_[bmName_training] = 0.;
      }
    }
  }
  else if ( mode_ == Mode::kResonant )
  {
    vdouble massPoints = cfg.getParameter<vdouble>("gen_mHH");
    for ( auto massPoint : massPoints )
    {
      std::string parameter = Form("%1.0f", massPoint);
      massPoints_[parameter] = massPoint;
      parameters_.push_back(parameter);
    }
    mvaInputVariables_wParameters_.push_back("gen_mHH");
    mvaInputs_["gen_mHH"] = 0.;
  } else assert(0);

  std::string mvaFileName_odd = cfg.getParameter<std::string>("mvaFileName_odd");
  std::string mvaFileName_even = cfg.getParameter<std::string>("mvaFileName_even");
  std::string fitFunctionFileName = cfg.getParameter<std::string>("fitFunctionFileName");
  mvaInterface_ = new TMVAInterface(mvaFileName_odd, mvaInputVariables_wParameters_, {}, mvaFileName_even, fitFunctionFileName);
  mvaInterface_->transform_mvaOutput(cfg.getParameter<bool>("transform_mvaOutput"));

  branchVar_event_ = std::shared_ptr<BranchVarBase>(new BranchVarULong64_t("event"));
  branchVars_.push_back(branchVar_event_);

  isDEBUG_ = cfg.getParameter<bool>("isDEBUG");
  if ( isDEBUG_ )
  {
    branchVar_run_ = std::shared_ptr<BranchVarBase>(new BranchVarUInt_t("run"));
    branchVars_.push_back(branchVar_run_);
    branchVar_lumi_ = std::shared_ptr<BranchVarBase>(new BranchVarUInt_t("lumi"));
    branchVars_.push_back(branchVar_lumi_);
    std::string jsonFileName = cfg.getParameter<std::string>("jsonFileName");
    jsonFile_ = new DumpToJSONFile(jsonFileName);
  }
}

BDTHistogramFiller_HH_multilepton::~BDTHistogramFiller_HH_multilepton()
{
  delete mvaInterface_;
  delete hhCouplings_;
  delete jsonFile_;
}

void
BDTHistogramFiller_HH_multilepton::bookHistograms(TFileDirectory & dir)
{
  std::string histogramDir = cfg_.getParameter<std::string>("histogramDir");
  
  for ( const std::string & parameter : parameters_ )
  {
    edm::ParameterSet cfg_histogram = cfg_;
    cfg_histogram.addParameter<std::string>("histogramDir", Form("%s/%s", histogramDir.data(), parameter.data()));
    histograms_[parameter] = bookHistogram1D(dir, cfg_histogram);
  }
}

void
BDTHistogramFiller_HH_multilepton::setBranchAddresses(TTree * tree)
{
  branchVars_.clear();
  const vstring & branchNames = mvaInputVariables_;
  for ( const std::string & branchName : branchNames )
  {
    std::shared_ptr<BranchVarBase> branchVar = BranchVarFactory::create(branchName);
    branchVarMap_[branchName] = branchVar;
    mvaInputs_[branchName] = 0.;
    branchVars_.push_back(branchVar);
  }
  if ( isDEBUG_ )
  {
    branchVars_.push_back(branchVar_run_);
    branchVars_.push_back(branchVar_lumi_);
  }
  branchVars_.push_back(branchVar_event_);
  for(auto & kv: hhReweightMap_)
  {
    kv.second->setBranchAddress(tree);
  }
  HistogramFillerBase::setBranchAddresses(tree);
}

void
BDTHistogramFiller_HH_multilepton::fillHistograms(double evtWeight)
{
  for ( const std::string & mvaInputVariable : mvaInputVariables_ )
  {
    std::map<std::string, std::shared_ptr<BranchVarBase>>::const_iterator branchVar_mvaInput = branchVarMap_.find(mvaInputVariable);
    assert(branchVar_mvaInput != branchVarMap_.end());
    double mvaInput = branchVar_mvaInput->second->operator()();
    mvaInputs_[mvaInputVariable] = mvaInput;
  }
  for ( const std::string & parameter : parameters_ )
  {
    TH1 * histogram = histograms_[parameter];
    assert(histogram);
    ULong64_t event_number = (dynamic_cast<BranchVarULong64_t *>(branchVar_event_.get()))->getValue();
    if ( mode_ == Mode::kNonresonant )
    {
      const HHCoupling & hhCoupling = hhCouplings_->getCoupling(parameter);
      const std::string & bmName_training = hhCoupling.training();
      mvaInputs_[bmName_training] = 1.;
      double mvaOutput = mvaInterface_->get_mvaOutput(mvaInputs_, event_number);
      if ( isDEBUG_ && parameter == "SM" )
      {
        UInt_t run_number = (dynamic_cast<BranchVarUInt_t *>(branchVar_run_.get()))->getValue();
        UInt_t lumi_number = (dynamic_cast<BranchVarUInt_t *>(branchVar_lumi_.get()))->getValue();
        std::map<std::string, double> variables = mvaInputs_;
        variables["mvaOutput"] = mvaOutput;
        jsonFile_->write(run_number, lumi_number, event_number, variables);
      }
      mvaInputs_[bmName_training] = 0.;
      std::map<std::string, std::shared_ptr<BranchVarBase>>::const_iterator branchVar_hhReweight = hhReweightMap_.find(parameter);
      assert(branchVar_hhReweight != hhReweightMap_.end());
      double hhReweight = branchVar_hhReweight->second->operator()();
      fillWithOverFlow1D(histogram, mvaOutput, evtWeight*hhReweight);      
    }
    else if ( mode_ == Mode::kResonant )
    {
      std::map<std::string, double>::const_iterator massPoint = massPoints_.find(parameter);
      assert(massPoint != massPoints_.end());
      mvaInputs_["gen_mHH"] = massPoint->second;
      double mvaOutput = mvaInterface_->get_mvaOutput(mvaInputs_, event_number);
      if ( isDEBUG_ && massPoint->second == 500. )
      {
        UInt_t run_number = (dynamic_cast<BranchVarUInt_t *>(branchVar_run_.get()))->getValue();
        UInt_t lumi_number = (dynamic_cast<BranchVarUInt_t *>(branchVar_lumi_.get()))->getValue();
        std::map<std::string, double> variables = mvaInputs_;
        variables["mvaOutput"] = mvaOutput;
        jsonFile_->write(run_number, lumi_number, event_number, variables);
      }
      mvaInputs_["gen_mHH"] = 0.;
      fillWithOverFlow1D(histogram, mvaOutput, evtWeight);      
    }
    else assert(0);
  }
}

DEFINE_EDM_PLUGIN(HistogramFillerPluginFactory, BDTHistogramFiller_HH_multilepton, "BDTHistogramFiller_HH_multilepton");
