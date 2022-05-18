#include "TallinnAnalysis/HistogramTools/plugins/HistogramFiller2D.h"

#include "TallinnAnalysis/HistogramTools/interface/createHistogram.h"   // createHistogram1D()
#include "TallinnAnalysis/HistogramTools/interface/fillWithOverFlow.h"  // fillWithOverFlow1D()
#include "TallinnNtupleProducer/CommonTools/interface/cmsException.h"   // cmsException()

#include "TString.h"                                                    // Form()
#include "TTree.h"                                                      // TTree
#include "TTreeFormula.h"                                               // TTreeFormula
#include "TH2.h"                                                        // TH2

HistogramFiller2D::HistogramFiller2D(const edm::ParameterSet & cfg)
  : HistogramFillerBase(cfg)
  , observableX_(nullptr)
  , observableY_(nullptr)
{
  edm::ParameterSet cfg_observableX = cfg.getParameterSet("xAxis").getParameterSet("observable");
  std::string pluginTypeX = cfg_observableX.getParameter<std::string>("pluginType");
  observableX_ = std::shared_ptr<VarExtractorBase>(VarExtractorPluginFactory::get()->create(pluginTypeX, cfg_observableX).release());
  extractors_.push_back(observableX_);

  edm::ParameterSet cfg_observableY = cfg.getParameterSet("yAxis").getParameterSet("observable");
  std::string pluginTypeY = cfg_observableY.getParameter<std::string>("pluginType");
  observableY_ = std::shared_ptr<VarExtractorBase>(VarExtractorPluginFactory::get()->create(pluginTypeY, cfg_observableY).release());
  extractors_.push_back(observableY_);
}

HistogramFiller2D::~HistogramFiller2D()
{
  for ( categoryEntry * category : categories_ )
  {
    delete category->treeFormula_;
  }
}

void
HistogramFiller2D::bookHistograms(TFileDirectory & dir)
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
      category->histogram_ = createHistogram2D(dir, cfg_histogram);
      categories_.push_back(category);
    }
  }
  else
  {
    categoryEntry * category = new categoryEntry();
    category->histogramDir_ = cfg_.getParameter<std::string>("histogramDir");
    category->selection_ = "";
    category->treeFormula_ = nullptr;
    category->histogram_ = createHistogram2D(dir, cfg_);
    categories_.push_back(category);
  }
}

void
HistogramFiller2D::setBranchAddresses(TTree * tree)
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
HistogramFiller2D::fillHistograms(double evtWeight)
{
  double x = observableX_->get_value();
  double y = observableY_->get_value();
  for ( categoryEntry * category : categories_ )
  {
    if ( category->treeFormula_ == nullptr || category->treeFormula_->EvalInstance() )
    {
      fillWithOverFlow2D(category->histogram_, x, y, evtWeight);
    }
  }
}

DEFINE_EDM_PLUGIN(HistogramFillerPluginFactory, HistogramFiller2D, "HistogramFiller2D");
