
/** analyse
 *
 * Read "plain" Ntuple and fill histograms for control plots and datacards.
 *
 * \authors Christian Veelken, Tallinn
 *
 */

#include "DataFormats/FWLite/interface/InputSource.h"                                           // fwlite::InputSource
#include "DataFormats/FWLite/interface/OutputFiles.h"                                           // fwlite::OutputFiles
#include "FWCore/ParameterSet/interface/ParameterSet.h"                                         // edm::ParameterSet
#include "FWCore/ParameterSetReader/interface/ParameterSetReader.h"                             // edm::readPSetsFrom()
#include "FWCore/PluginManager/interface/PluginManager.h"                                       // edmplugin::PluginManager::configure()
#include "FWCore/PluginManager/interface/standard.h"                                            // edmplugin::standard::config()
#include "PhysicsTools/FWLite/interface/TFileService.h"                                         // fwlite::TFileService

#include "TallinnAnalysis/Extractors/interface/BranchVars.h"                                    // BranchVarUInt_t, BranchVarULong64_t
#include "TallinnAnalysis/HistogramTools/interface/HistogramFillerBase.h"                       // HistogramFillerBase, HistogramFillerPluginFactory
#include "TallinnNtupleProducer/CommonTools/interface/cmsException.h"                           // cmsException
#include "TallinnNtupleProducer/CommonTools/interface/format_vT.h"                              // format_vdouble(), format_vstring()
#include "TallinnNtupleProducer/Selectors/interface/RunLumiEventSelector.h"                     // RunLumiEventSelector

#include <TBenchmark.h>                                                                         // TBenchmark
#include <TError.h>                                                                             // gErrorAbortLevel, kError
#include <TString.h>                                                                            // Form()
#include <TTree.h>                                                                              // TTree

#include <assert.h>                                                                             // assert()
#include <cstdlib>                                                                              // EXIT_SUCCESS, EXIT_FAILURE
#include <fstream>                                                                              // std::ofstream
#include <iostream>                                                                             // std::cout
#include <map>                                                                                  // std::map
#include <string>                                                                               // std::string
#include <vector>                                                                               // std::vector

typedef std::vector<double> vdouble;
typedef std::vector<std::string> vstring;

int main(int argc, char* argv[])
{
//--- throw an exception in case ROOT encounters an error
  gErrorAbortLevel = kError;

//--- stop ROOT from keeping track of all histograms
  TH1::AddDirectory(false);

//--- parse command-line arguments
  if ( argc < 2 ) {
    std::cout << "Usage: " << argv[0] << " [parameters.py]" << std::endl;
    return EXIT_FAILURE;
  }

  std::cout << "<analyze>:" << std::endl;

//--- keep track of time it takes the macro to execute
  TBenchmark clock;
  clock.Start("analyze");

//--- read python configuration parameters
  std::cout << "Reading config file " << argv[1] << std::endl;
  if ( !edm::readPSetsFrom(argv[1])->existsAs<edm::ParameterSet>("process") )
    throw cmsException("analyze", __LINE__) << "No ParameterSet 'process' found in config file !!";

  edm::ParameterSet cfg = edm::readPSetsFrom(argv[1])->getParameterSet("process");

  edm::ParameterSet cfg_analyze = cfg.getParameterSet("analyze");

  std::string treeName = cfg_analyze.getParameter<std::string>("treeName");

  std::string process = cfg_analyze.getParameter<std::string>("process");
  std::string central_or_shift = cfg_analyze.getParameter<std::string>("central_or_shift");
  std::cout << "Processing process = '" << process << "', central_or_shift = '" << central_or_shift << "'" << std::endl;

  std::string selection = cfg_analyze.getParameter<std::string>("selection");
  std::cout << "Applying selection = '" << selection << "'" << std::endl;
  
  vstring branchNames_evtWeights = cfg_analyze.getParameter<vstring>("evtWeights");
  std::cout << "Applying event weights = '" << format_vstring(branchNames_evtWeights) << "'" << std::endl;

  bool isDEBUG = cfg_analyze.getParameter<bool>("isDEBUG");

  std::string selEventsFileName_input = cfg_analyze.getParameter<std::string>("selEventsFileName_input");
  std::cout << "selEventsFileName_input = " << selEventsFileName_input << std::endl;
  RunLumiEventSelector* run_lumi_eventSelector = 0;
  if ( selEventsFileName_input != "" ) {
    edm::ParameterSet cfg_run_lumi_eventSelector;
    cfg_run_lumi_eventSelector.addParameter<std::string>("inputFileName", selEventsFileName_input);
    cfg_run_lumi_eventSelector.addParameter<std::string>("separator", ":");
    run_lumi_eventSelector = new RunLumiEventSelector(cfg_run_lumi_eventSelector);
  }

  std::string selEventsFileName_output = cfg_analyze.getParameter<std::string>("selEventsFileName_output");

  fwlite::InputSource inputFiles(cfg);
  int maxEvents = inputFiles.maxEvents();
  std::cout << " maxEvents = " << maxEvents << std::endl;
  unsigned reportEvery = inputFiles.reportAfter();

  fwlite::OutputFiles outputFile(cfg);
  fwlite::TFileService fs = fwlite::TFileService(outputFile.file().data());

  // CV: create plugins that book & fill histograms
  if ( !edmplugin::PluginManager::isAvailable() )
  {  
    edmplugin::PluginManager::configure(edmplugin::standard::config());
  }
  edm::VParameterSet cfg_histograms = cfg_analyze.getParameterSetVector("histogramPlugins");
  std::vector<std::unique_ptr<HistogramFillerBase>> histograms;
  for ( auto cfg_histogram : cfg_histograms )
  {
    std::string pluginType = cfg_histogram.getParameter<std::string>("pluginType");
    cfg_histogram.addParameter<std::string>("process", process);
    cfg_histogram.addParameter<std::string>("central_or_shift", central_or_shift);
    std::unique_ptr<HistogramFillerBase> histogram = HistogramFillerPluginFactory::get()->create(pluginType, cfg_histogram);
    histogram->bookHistograms(fs);
    histograms.push_back(std::move(histogram));
  }

  std::vector<std::string> inputFileNames = inputFiles.files();
  size_t numInputFiles = inputFileNames.size();
  std::cout << "Loaded " << numInputFiles << " file(s).\n";

  TDirectory* dir = fs.getBareDirectory();
  dir->cd();

  // create output file containing run:lumi:event numbers of events passing final event selection criteria  
  std::ostream* selEventsFile = ( selEventsFileName_output != "" ) ? new std::ofstream(selEventsFileName_output.data(), std::ios::out) : 0;
  std::cout << "selEventsFileName_output = " << selEventsFileName_output << std::endl;

  int analyzedEntries = 0;
  int processedEntries_allInputFiles = 0;
  int processedInputFiles = 0;
  int selectedEntries = 0;
  double selectedEntries_weighted = 0.;  
  for ( size_t idxInputFile = 0; idxInputFile < numInputFiles; ++idxInputFile )
  {
    const std::string & inputFileName = inputFileNames.at(idxInputFile);
    std::cout << "Opening #" << idxInputFile << " file " << inputFileName << '\n';
    TFile* inputFile = TFile::Open(inputFileName.data());
    if ( !inputFile )
      throw cmsException("analyze", __LINE__) 
        << "The file '" << inputFileName << "' failed to open";
   
    TTree* inputTree = dynamic_cast<TTree *>(inputFile->Get(treeName.data()));
    if ( !inputTree )
      throw cmsException("analyze", __LINE__) 
        << "The file '" << inputFileName << "' does not have a TTree named "
        << treeName;
    std::cout << "The file " << inputFileName << " has " << inputTree->GetEntries() << " entries\n";

    const std::string histogramName_analyzedEntries = "analyzedEntries";
    TH1* histogram_analyzedEntries = dynamic_cast<TH1 *>(inputFile->Get(histogramName_analyzedEntries.data()));
    if ( !histogram_analyzedEntries )
      throw cmsException("analyze", __LINE__) 
        << "The file '" << inputFileName << "' does not have a TH1 named "
        << histogramName_analyzedEntries;
    analyzedEntries += histogram_analyzedEntries->GetEntries();
    
    TTree* inputTree_selected = nullptr;
    if ( selection != "" )
    {
      inputTree_selected = inputTree->CopyTree(selection.data());
    }
    else
    {
      inputTree_selected = inputTree;
    }

    BranchVarUInt_t* branchVar_run = new BranchVarUInt_t("run");
    branchVar_run->setBranchAddress(inputTree_selected);
    BranchVarUInt_t* branchVar_lumi = new BranchVarUInt_t("lumi");
    branchVar_lumi->setBranchAddress(inputTree_selected);
    BranchVarULong64_t* branchVar_event = new BranchVarULong64_t("event");
    branchVar_event->setBranchAddress(inputTree_selected);

    for ( auto & histogram : histograms )
    {
      histogram->setBranchAddresses(inputTree_selected);
    }

    std::vector<BranchVarBase*> branchVars_evtWeight;
    for ( const std::string & branchName_evtWeight : branchNames_evtWeights )
    {
      BranchVarBase* branchVar = new BranchVarFloat_t(branchName_evtWeight);
      branchVar->setBranchAddress(inputTree_selected);
      branchVars_evtWeight.push_back(branchVar);
    }

    int numEntries = inputTree_selected->GetEntries();
    for ( int idxEntry = 0; idxEntry < numEntries && (maxEvents == -1 || processedEntries_allInputFiles < maxEvents); ++idxEntry )
    {
      inputTree_selected->GetEntry(idxEntry);

      UInt_t run = branchVar_run->getValue();
      UInt_t lumi = branchVar_lumi->getValue();
      ULong64_t event = branchVar_event->getValue();

      ++processedEntries_allInputFiles;
      if ( idxEntry == 0 ) ++processedInputFiles;
      if ( processedEntries_allInputFiles > 0 && (processedEntries_allInputFiles % reportEvery) == 0 )
      {
        std::cout << "processing Entry " << processedEntries_allInputFiles
                  << " or " << idxEntry << " entry in #"
                  << idxInputFile
                  << " (run = " << run << ", ls = " << lumi << ", event = " << event
                  << ") file\n";
      }

      if ( run_lumi_eventSelector )
      { 
        if ( !(*run_lumi_eventSelector)(run, lumi, event) )
        {
          continue;
        }
        std::cout << "processing Entry " << processedEntries_allInputFiles << ": run = " << run << ", ls = " << lumi << ", event = " << event << '\n';
        std::cout << "input File = " << inputFileName << '\n';
      }

      if ( isDEBUG )
      {
        std::cout << "event #" << processedEntries_allInputFiles << " run = " << run << ", ls = " << lumi << ", event = " << event << '\n';
      }

      double evtWeight = 1.;
      for ( auto & branchVar : branchVars_evtWeight )
      {
        evtWeight *= (*branchVar)();
      }

      for ( auto & histogram : histograms )
      {
        histogram->fillHistograms(evtWeight);
      }

//-----------------------------------------------------------------------
// CV: only for debugging !!
std::cout << "selected event = " << run << ":" << lumi << ":" << event << std::endl;
std::cout << "evtWeight = " << evtWeight << std::endl;
//-----------------------------------------------------------------------

      if ( selEventsFile )
      {
        (*selEventsFile) << run << ":" << lumi << ":" << event << '\n';
      }

      ++selectedEntries;
      selectedEntries_weighted += evtWeight;
    }

    delete branchVar_run;
    delete branchVar_lumi;
    delete branchVar_event;
    for ( auto & branchVar : branchVars_evtWeight )
    {
      delete branchVar;
    }
    if ( inputTree_selected != inputTree )
    {
      delete inputTree_selected;
    }
    delete inputTree;
    delete histogram_analyzedEntries;
    delete inputFile;
  }

  std::cout << "max num. Entries = " << processedEntries_allInputFiles
            << " (limited by " << maxEvents << ") processed in "
            << processedInputFiles << " file(s) (out of "
            << numInputFiles << ")\n"
            << " analyzed = " << analyzedEntries << '\n'
            << " selected = " << selectedEntries << " (weighted = " << selectedEntries_weighted << ")" << std::endl;

//--- memory clean-up
  delete run_lumi_eventSelector;

  for ( auto & histogram : histograms )
  {
    histogram.reset(nullptr);
  }

  clock.Show("analyze");

  return EXIT_SUCCESS;
}
