#include "TallinnAnalysis/HistogramTools/plugins/HistogramFiller1D.h"

#include "TallinnAnalysis/HistogramTools/interface/createHistogram.h"   // createHistogram1D()
#include "TallinnAnalysis/HistogramTools/interface/fillWithOverFlow.h"  // fillWithOverFlow1D()
#include "TallinnNtupleProducer/CommonTools/interface/cmsException.h"   // cmsException()

#include "TString.h"                                                    // Form()
#include "TTree.h"                                                      // TTree
#include "TTreeFormula.h"                                               // TTreeFormula
#include "TH1.h"                                                        // TH1

HistogramFiller1D::HistogramFiller1D(const edm::ParameterSet & cfg)
  : HistogramFillerBase(cfg)
  , observable_(nullptr)
{
  edm::ParameterSet cfg_observable = cfg.getParameterSet("xAxis").getParameterSet("observable");
  std::string pluginType = cfg_observable.getParameter<std::string>("pluginType");
  observable_ = std::shared_ptr<VarExtractorBase>(VarExtractorPluginFactory::get()->create(pluginType, cfg_observable).release());
  extractors_.push_back(observable_);
}

HistogramFiller1D::~HistogramFiller1D()
{
  for ( categoryEntry * category : categories_ )
  {
    delete category->treeFormula_;
  }
}

void
HistogramFiller1D::bookHistograms(TFileDirectory & dir)
{
  if ( cfg_.exists("categories") )
  {
    edm::VParameterSet cfg_categories = cfg_.getParameterSetVector("categories");
    for ( const edm::ParameterSet & cfg_category : cfg_categories )
    {
      categoryEntry * category = new categoryEntry();
      category->histogramDir_ = Form("%s/%s", cfg_.getParameter<std::string>("histogramDir").data(), cfg_category.getParameter<std::string>("histogramDir").data());
      category->selection_ = cfg_category.getParameter<std::string>("selection");
      category->treeFormula_ = nullptr;
      edm::ParameterSet cfg_histogram = cfg_;
      cfg_histogram.addParameter<std::string>("histogramDir", category->histogramDir_);
      category->histogram_ = createHistogram1D(dir, cfg_histogram);
      categories_.push_back(category);
    }
  }
  else
  {
    categoryEntry * category = new categoryEntry();
    category->histogramDir_ = cfg_.getParameter<std::string>("histogramDir");
    category->selection_ = "";
    category->treeFormula_ = nullptr;
    category->histogram_ = createHistogram1D(dir, cfg_);
    categories_.push_back(category);
  }
}

void
HistogramFiller1D::setBranchAddresses(TTree * tree)
{
  HistogramFillerBase::setBranchAddresses(tree);
  for ( categoryEntry * category : categories_ )
  {
    if ( category->selection_ != "" )
    {
      std::string treeFormulaName = Form("treeFormula%i", gNumTreeFormulas);
      category->treeFormula_ = new TTreeFormula(treeFormulaName.data(), category->selection_.data(), tree);
      ++gNumTreeFormulas;
    }
  }
}

void
HistogramFiller1D::fillHistograms(double evtWeight)
{
  double x = observable_->get_value();
  for ( categoryEntry * category : categories_ )
  {
    if ( category->treeFormula_ == nullptr || category->treeFormula_->EvalInstance() )
    {
      fillWithOverFlow1D(category->histogram_, x, evtWeight);
    }
  }
}

DEFINE_EDM_PLUGIN(HistogramFillerPluginFactory, HistogramFiller1D, "HistogramFiller1D");
