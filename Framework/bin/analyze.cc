
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

#include "TallinnNtupleProducer/CommonTools/interface/cmsException.h"                           // cmsException
#include "TallinnNtupleProducer/CommonTools/interface/format_vT.h"                              // format_vdouble(), format_vstring()
#include "TallinnNtupleProducer/Selectors/interface/RunLumiEventSelector.h"                     // RunLumiEventSelector

#include "TallinnAnalysis/Extractors/interface/VarExtractorBase.h"                              // VarExtractorBase, VarExtractorPluginFactory
#include "TallinnAnalysis/HistogramTools/interface/histogramAuxFunctions.h"                     // fillWithOverFlow()

#include <TBenchmark.h>                                                                         // TBenchmark
#include <TError.h>                                                                             // gErrorAbortLevel, kError
#include <TString.h>                                                                            // Form()
#include <TTree.h>                                                                              // TTree

#include <assert.h>                                                                             // assert()
#include <cmath>                                                                                // std::sqrt()
#include <cstdlib>                                                                              // EXIT_SUCCESS, EXIT_FAILURE
#include <fstream>                                                                              // std::ofstream
#include <iostream>                                                                             // std::cerr, std::fixed
#include <iomanip>                                                                              // std::setprecision(), std::setw()
#include <map>                                                                                  // std::map
#include <string>                                                                               // std::string
#include <vector>                                                                               // std::vector

typedef std::vector<double> vdouble;
typedef std::vector<std::string> vstring;

namespace
{
  std::vector<double>
  getBinning(const edm::ParameterSet & cfg_binning, const std::string & attr_xMin, const std::string & attr_xMax)
  {
    std::vector<double> binning;
    if ( cfg_analyze.exists("binning" ) )
    {
      binning = cfg_analyze.getParameter("binning");
      int numBins = binning.size() - 1;
      if ( !(numBins >= 1) ) 
        throw cmsException("analyze", __LINE__) << "Invalid Configuration parameter 'binning' = " << format_vdouble(binning) << " !!";
    }
    else
    {
      unsigned int numBins = cfg_analyze.getParameter<unsigned int>("numBins");
      if ( !(numBins >= 1) ) 
        throw cmsException("analyze", __LINE__) << "Invalid Configuration parameter 'numBins' = " << numBins << " !!";
      double xMin = cfg_analyze.getParameter<double>(attr_xMin);
      double xMax = cfg_analyze.getParameter<double>(attr_xMax);
      if ( !(xMax > xMin) )
        throw cmsException("analyze", __LINE__) << "Invalid Configuration parameters '" << attr_xMin << "' = " << xMin << "," 
                                                << " '" << attr_xMax << "' = " << xMax << " !!";
      }
      double binWidth = (xMax - xMin)/numBins;
      for ( int idxBin = 0; idxBin < numBins; ++idxBin )
      {
        binning.push_back(xMin + idxBin*binWidth);
      }
      binning.push_back(xMin + numBins*binWidth);
    }
    return binning;
  }

  TArrayD
  convert_to_TArrayD(const std::vector<double> & binning)
  {
    TArrayD retVal;
    int numBins = binning.size() - 1;
    assert(numBins >= 1);
    retVal.Set(numBins + 1);
    for ( int idxBin = 0; idxBin <= numBins; ++idxBin )
    {
      retVal[idxBin] = binning[idxBin];
    }
    return retVal;
  }
}

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

  edm::ParameterSet cfg = edm::readPSetsFrom(argv[1])->getParameter<edm::ParameterSet>("process");

  edm::ParameterSet cfg_analyze = cfg.getParameter<edm::ParameterSet>("analyze");

  std::string treeName = cfg_analyze.getParameter<std::string>("treeName");

  std::string process = cfg_analyze.getParameter<std::string>("process");
  std::string central_or_shift = cfg_analyze.getParameter<std::string>("central_or_shift");
  std::cout << "Processing process = '" << process << "', central_or_shift = '" << central_or_shift << "'" << std::endl;

  std::string selection = cfg_analyze.getParameter<std::string>("selection");
  std::cout << "Applying selection = '" << selection << "'" << std::endl;
  
  vstring branchNames_evtWeights = cfg_analyze.getParameter<vstring>("evtWeights");
  std::cout << "Applying event weights = '" << format_vstring(branchNames_evtWeights) << "'" << std::endl;

  bool isDEBUG = cfg_analyze.getParameter<bool>("isDEBUG");

  std::string selEventsFileName = cfg_analyze.getParameter<std::string>("selEventsFileName");
  std::cout << "selEventsFileName = " << selEventsFileName << std::endl;
  RunLumiEventSelector* run_lumi_eventSelector = 0;
  if ( selEventsFileName != "" ) {
    edm::ParameterSet cfg_run_lumi_eventSelector;
    cfg_run_lumi_eventSelector.addParameter<std::string>("inputFileName", selEventsFileName);
    cfg_run_lumi_eventSelector.addParameter<std::string>("separator", ":");
    run_lumi_eventSelector = new RunLumiEventSelector(cfg_run_lumi_eventSelector);
  }

  fwlite::InputSource inputFiles(cfg);
  int maxEvents = inputFiles.maxEvents();
  std::cout << " maxEvents = " << maxEvents << std::endl;
  unsigned reportEvery = inputFiles.reportAfter();

  fwlite::OutputFiles outputFile(cfg);
  fwlite::TFileService fs = fwlite::TFileService(outputFile.file().data());

  std::string histogramDir = cfg_analyze.getParameter<std::string>("histogramDir");
  std::string histogramType_string = cfg_analyze.getParameter<std::string>("histogramType");
  enum { kTH1, kTH2 };
  int histogramType = -1;
  if      ( histogramType_string == "TH1" ) histogramType = kTH1;
  else if ( histogramType_string == "TH2" ) histogramType = kTH2;
  else throw cmsException("analyze", __LINE__) << "Invalid Configuration parameter 'histogramType' = " << histogramType << " !!";
  std::string histogramTitle;
  if ( central_or_shift != "" && central_or_shift != "central" ) histogramTitle = Form("%s_%s", process.data(), central_or_shift.data());
  else                                                           histogramTitle = process;
  std::string histogramName;
  if ( histogramDir != "" ) histogramName = Form("%s/%s", histogramDir.data(), histogramTitle.data());
  else                      histogramName = histogramTitle;
  TH1* histogram = nullptr;
  if ( histogramType == kTH1 )
  {
    std::cout << "Filling histogram = '" << histogramName << "' of type TH1D, stored in directory = '" << histogramDir << "'" << std::endl;
    std::vector<double> binning = getBinning(cfg_analyze.getParameterSet("xAxis"), "xMin", "xMax");
    TArrayD binning_TArrayD = convert_to_TArrayD(binning);
    int numBins = binning_TArrayD.GetSize() - 1;
    histogram = fs.make<TH1D>(histogramName.data(), histogramTitle.data(), numBins, binning_TArrayD.GetArray());
    std::cout << " binning = " << format_vdouble(binning) << std::endl;
  } 
  else if ( histogramType == kTH2 )
  {
    std::cout << "Filling histogram = '" << histogramName << "' of type TH2D, stored in directory = '" << histogramDir << "'" << std::endl;
    std::vector<double> binningX = getBinning(cfg_analyze.getParameterSet("xAxis"), "xMin", "xMax");
    TArrayD binningX_TArrayD = convert_to_TArrayD(binningX);
    int numBinsX = binningX_TArrayD.GetSize() - 1;
    std::vector<double> binningY = getBinning(cfg_analyze.getParameterSet("yAxis"), "yMin", "yMax");
    TArrayD binningY_TArrayD = convert_to_TArrayD(binningY);
    int numBinsY = binningY_TArrayD.GetSize() - 1;
    histogram = fs.make<TH2D>(histogramName.data(), histogramTitle.data(), numBinsX, binningX_TArrayD.GetArray(), numBinsY, binningY_TArrayD.GetArray());
    std::cout << " binning: x-axis = " << format_vdouble(binningX) << ", y-axis = " << format_vdouble(binningY) << std::endl;
  }
  else assert(0);

  std::vector<std::string> inputFileNames = inputFiles.files();
  size_t numInputFiles = inputFileNames.size();
  std::cout << "Loaded " << numInputFiles << " file(s).\n";
 
  // CV: create plugin that extracts value to be filled into histogram from the event
  if ( !edmplugin::PluginManager::isAvailable() )
  {  
    edmplugin::PluginManager::configure(edmplugin::standard::config());
  }
  edm::VParameterSet cfg_varExtractors = cfg_analyze.getParameterSetVector("varExtractorPlugins");
  std::vector<std::unique_ptr<VarExtractorBase>> varExtractors;
  for ( auto cfg_varExtractor : cfg_varExtractors )
  {
    std::string pluginType = cfg_varExtractor.getParameter<std::string>("pluginType");
    std::unique_ptr<VarExtractorBase> varExtractor = VarExtractorPluginFactory::get()->create(pluginType, cfg_varExtractor);
    varExtractors.push_back(std::move(varExtractor));
  }
  if ( (histogramType == kTH1 && varExtractors.size() != 1) || (histogramType == kTH2 && varExtractors.size() != 2) )
    throw cmsException("analyze", __LINE__) << "Number of extractor plugins does not match histogram type !!";

  TDirectory* dir = fs.getBareDirectory();
  dir->cd();

  int analyzedEntries = 0;
  int processedEntries_allInputFiles = 0;
  int processedInputFiles = 0;
  int selectedEntries = 0;
  double selectedEntries_weighted = 0.;  
  for ( size_t idxInputFile = 0; idxInputFile < numInputFiles; ++idxInputFile )
  {
    const std::string & inputFileName = inputFileNames.at(idxInputFile);
    std::cout << "Opening #" << idxInputFile << " file " << inputFileName << '\n';
    TFile* inputFile = TFile::Open(inputFileName);
    if ( !inputFile )
      throw cmsException("analyze", __LINE__) 
        << "The file '" << inputFileName << "' failed to open";
   
    TTree* inputTree = dynamic_cast<TTree *>(inputFile->Get(treeName));
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
 
    UInt_t run, lumi;
    ULong64_t event;
    inputTree_selected->SetBranchAddress("run", &run);
    inputTree_selected->SetBranchAddress("lumi", &lumi);
    inputTree_selected->SetBranchAddress("event", &event);

    for ( auto & varExtractor : varExtractors )
    {
      varExtractor.setBranchAddresses(inputTree_selected);
    }

    std::map<std::string, Float_t> evtWeights;
    for ( const std::string & branchName_evtWeight : branchNames_evtWeights )
    {
      evtWeights[branchName] = 1.;
      inputTree_selected->SetBranchAddress(branchName_evtWeight.data(), &evtWeights[branchName]);
    }

    int numEntries = inputTree_selected->GetEntries();
    for ( int idxEntry = 0; idxEntry < numEntries && (maxEvents == -1 || idxEntry_allInputFiles < maxEvents); ++idxEntry )
    {
      ++processedEntries_allInputFiles;
      if ( idxEntry == 0 ) ++processedInputFiles;
      if ( processedEntries_allInputFiles > 0 && (processedEntries_allInputFiles % reportEvery) == 0 )
      {
        std::cout << "processing Entry " << processedEntries_allInputFiles
                  << " or " << idxEntry << " entry in #"
                  << idxInputFile
                  << " (run = " << run() << ", ls = " << lumi << ", event = " << event
                  << ") file\n";
      }

      if ( run_lumi_eventSelector )
      { 
        if ( !(*run_lumi_eventSelector)(run, lumi, event) )
        {
          continue;
        }
        std::cout << "processing Entry " << processedEntries_allInputFiles << ": run = " << run << ", ls = " << lumi << ", event = " << event << '\n';
        if ( inputTree_selected->isOpen() )
        {
          std::cout << "input File = " << inputFileName << '\n';
        }
      }

      if ( isDEBUG )
      {
        std::cout << "event #" << processedEntries_allInputFiles << " run = " << run << ", ls = " << lumi << ", event = " << event << '\n';
      }

      double evtWeight = 1.;
      for ( const std::string & branchName_evtWeight : branchNames_evtWeights )
      {
        evtWeight *= evtWeights[branchName];
      }

      if ( histogramType == kTH1 )
      {
        assert(varExtractors.size() == 1);
        double varValue = varExtractors.at(0)->read();
        fillWithOverFlow(histogram, varValue, evtWeight);
      }
      else if ( histogramType == kTH2 )
      {
        double varValueX = varExtractors.at(0)->read();
        double varValueY = varExtractors.at(1)->read();
        fillWithOverFlow2d(histogram, varValueX, varValueY, evtWeight);
      } else assert(0);

      ++selectedEntries;
      selectedEntries_weighted += evtWeight;
      histogram_selectedEntries->Fill(0.);
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

  for ( auto & varExtractor : varExtractors )
  {
    varExtractor.reset(nullptr);
  }

  clock.Show("analyze");

  return EXIT_SUCCESS;
}
