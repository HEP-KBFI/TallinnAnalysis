#include "TallinnAnalysis/HistogramTools/interface/HistogramFillerBase.h"

#include "TallinnAnalysis/HistogramTools/interface/createHistogram.h" // createHistogram1D(), createHistogram2D()

#include <TH1.h>   // TH1
#include <TH2.h>   // TH2
#include <TTree.h> // TTree

std::map<TDirectory *, std::vector<TH1 *>> HistogramFillerBase::gHistograms_;

int HistogramFillerBase::gNumTreeFormulas = 0;

HistogramFillerBase::HistogramFillerBase(const edm::ParameterSet & cfg)
  : cfg_(cfg)
{}

HistogramFillerBase::~HistogramFillerBase()
{}

void
HistogramFillerBase::setBranchAddresses(TTree * tree)
{
  for ( std::shared_ptr<VarExtractorBase> & extractor : extractors_ )
  {
    extractor->setBranchAddresses(tree);
  }
  for ( std::shared_ptr<BranchVarBase> & branchVar : branchVars_ )
  {
    branchVar->setBranchAddress(tree);
  }
}

TH1 *
HistogramFillerBase::bookHistogram1D(TFileDirectory & dir, const edm::ParameterSet & cfg)
{
  std::pair<TDirectory*, TH1 *> subDir_and_histogram = createHistogram1D(dir, cfg);
  TDirectory* subDir = subDir_and_histogram.first;
  TH1 * histogram = subDir_and_histogram.second;
  gHistograms_[subDir].push_back(histogram);
  return histogram;
}

TH2 *
HistogramFillerBase::bookHistogram2D(TFileDirectory & dir, const edm::ParameterSet & cfg)
{
  std::pair<TDirectory*, TH2 *> subDir_and_histogram = createHistogram2D(dir, cfg);
  TDirectory* subDir = subDir_and_histogram.first;
  TH2 * histogram = subDir_and_histogram.second;
  gHistograms_[subDir].push_back(histogram);
  return histogram;
}

void
HistogramFillerBase::writeHistograms()
{
  for ( std::map<TDirectory *, std::vector<TH1 *>>::iterator dir = gHistograms_.begin(); dir != gHistograms_.end(); ++dir )
  {
    assert(dir->first);
    dir->first->cd();
    for ( std::vector<TH1 *>::iterator histogram = dir->second.begin(); histogram != dir->second.end(); ++histogram ) 
    {
      assert(*histogram);
      (*histogram)->Write();
    }
  }
}

EDM_REGISTER_PLUGINFACTORY(HistogramFillerPluginFactory, "HistogramFillerPluginFactory");
