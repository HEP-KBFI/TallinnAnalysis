#include "TallinnAnalysis/HistogramFillers/interface/BranchVarHistogramFiller.h"

#include "TallinnAnalysis/HistogramFiller/interface/BranchVarFactory.h" // BranchVarFactory
#include "TallinnAnalysis/HistogramFiller/interface/BranchVars.h"       // BranchVarBase, BranchVar*
#include "TallinnAnalysis/HistogramTools/interface/createHistogram.h"   // createHistogram1D(), createHistogram2D()
#include "TallinnAnalysis/HistogramTools/interface/fillWithOverFlow.h"  // fillWithOverFlow1D(), fillWithOverFlow2D()

#include "TallinnNtupleProducer/CommonTools/interface/cmsException.h"   // cmsException()
#include "TallinnNtupleProducer/CommonTools/interface/format_vT.h"      // format_vstring()

#include "TTree.h"                                                      // TTree

typedef std::vector<std::string> vstring;

BranchVarHistogramFiller::BranchVarHistogramFiller(const edm::ParameterSet & cfg)
  : HistogramFillerBase(cfg)
  , histogram_(nullptr)
{
  branchNames_ = cfg.getParameter<vstring>("branches");
  if ( !(branchNames_.size() == 1 || branchNames_.size() == 2) )
    throw cmsException(this, __func__, __LINE__)
      << "Invalid Configuration parameter 'branches' = " << format_vstring(branches_) << " !!";
  BranchVarFactory branchVarFactory;
  branchVars_ = branchVarFactory.create(branchNames_);
  dimension_ = branchVars_.size();
}

BranchVarHistogramFiller::~BranchVarHistogramFiller()
{}

void
BranchVarHistogramFiller::bookHistograms(TFileDirectory & dir)
{
  if      ( dimension_ == 1 ) histogram_ = createHistogram1D(dir, cfg);
  else if ( dimension_ == 2 ) histogram_ = createHistogram2D(dir, cfg);
  else assert(0);
}

void
BranchVarHistogramFiller::fillHistograms(double evtWeight)
{
  if ( dimension_ == 1 )
  {
    BranchVarBase * branchVarX = branchVars_.at(0);
    assert(branchVarX);
    double x = branchVarX->operator()();
    fillWithOverFlow1D(histogram_, x, evtWeight);
  }
  else if ( dimension_ == 2 )
  {
    BranchVarBase * branchVarX = branchVars_.at(0);
    assert(branchVarX);
    double x = branchVarX->operator()();
    BranchVarBase * branchVarY = branchVars_.at(1);
    assert(branchVarY);
    double y = branchVarY->operator()();
    fillWithOverFlow2D(histogram_, x, y, evtWeight);
  } else assert(0);
}

DEFINE_EDM_PLUGIN(HistogramFillerPluginFactory, BranchVarHistogramFiller, "BranchVarHistogramFiller");
