/** \executable addBackgroundFromData
 *
 * Compute shape templates and normalization for "fakes" and "flips" backgrounds,
 * which are obtained from control regions in data.
 *
 * \author Christian Veelken, Tallinn
 *
 */

#include "DataFormats/FWLite/interface/InputSource.h"                            // fwlite::InputSource
#include "DataFormats/FWLite/interface/OutputFiles.h"                            // fwlite::OutputFiles
#include "FWCore/ParameterSet/interface/ParameterSet.h"                          // edm::ParameterSet
#include "FWCore/ParameterSetReader/interface/ParameterSetReader.h"              // edm::readPSetsFrom()
#include "PhysicsTools/FWLite/interface/TFileService.h"                          // fwlite::TFileService

#include "TallinnAnalysis/Framework/interface/compIntegral.h"                    // compIntegral(), compIntegralErr()
#include "TallinnAnalysis/Framework/interface/createSubdirectory.h"              // createSubdirectory_recursively()
#include "TallinnAnalysis/Framework/interface/getHistogram.h"                    // getHistogram()
#include "TallinnAnalysis/Framework/interface/getSubdirectories.h"               // getSubdirectories()
#include "TallinnAnalysis/Framework/interface/makeBinContentsPositive.h"         // makeBinContentsPositive()
#include "TallinnAnalysis/Framework/interface/subtractHistograms.h"              // subtractHistograms()
#include "TallinnNtupleProducer/CommonTools/interface/cmsException.h"            // cmsException
#include "TallinnNtupleProducer/CommonTools/interface/contains.h"                // contains()

#include <TBenchmark.h>                                                          // TBenchmark
#include <TDirectory.h>                                                          // TDirectory
#include <TError.h>                                                              // gErrorAbortLevel, kError
#include <TKey.h>                                                                // TKey
#include <TList.h>                                                               // TList
#include <TObject.h>                                                             // TObject
#include <TString.h>                                                             // Form()
#include <TTree.h>                                                               // TTree

#include <assert.h>                                                              // assert()
#include <cstdlib>                                                               // EXIT_SUCCESS, EXIT_FAILURE
#include <iostream>                                                              // std::cout
#include <set>                                                                   // std::set
#include <string>                                                                // std::string
#include <vector>                                                                // std::vector

typedef std::vector<std::string> vstring;

namespace
{
  void processSubdirectory_recursively(TFileDirectory& fs, 
                                       const TDirectory* dir, const std::string& dirName, 
                                       const std::string& processData, const std::string& processFakes_or_Flips, const vstring& processesToSubtract,
                                       const vstring& central_or_shifts,
                                       bool makeBinContentsPositive_dir,
                                       int depth_recursion, int max_depth_recursion,
                                       bool isDEBUG = false)
  {
    if ( isDEBUG )
    {
      std::cout << "<processSubdirectory_recursively>: Processing directory = '" << dirName << "'" << std::endl;
    }
      
    // check if directory given as function argument contains histograms for all input processes
    vstring processes_input;
    processes_input.push_back(processData);
    processes_input.insert(processes_input.end(), processesToSubtract.begin(), processesToSubtract.end());
    bool allProcessesExist = true;
    for ( const std::string& process_input : processes_input )
    {
      TH1* histogram = getHistogram(dir, process_input, "central", false);
      if ( !histogram ) 
      {
        allProcessesExist = false;
      }
    }
    if ( allProcessesExist ) 
    {
      // add histograms
      for ( const std::string& central_or_shift : central_or_shifts )
      {
        int verbosity = ( central_or_shift == "" || central_or_shift == "central" ) ? 1 : 0;
        TH1* histogramData = getHistogram(dir, processData, central_or_shift, false);
	if ( !histogramData ) 
        {
	  histogramData = getHistogram(dir, processData, "central", true);
	}
	if ( verbosity )
        {
          double integral = compIntegral(histogramData, false, false);
          double integralErr = compIntegralErr(histogramData, false, false);
	  std::cout << " integral(data_obs) = " << integral << " +/- " << integralErr << std::endl;
	}

	std::vector<const TH1*> histogramsToSubtract;
	for ( const std::string& processToSubtract : processesToSubtract )
        {
	  const TH1* histogramToSubtract = getHistogram(dir, processToSubtract, central_or_shift, false);
	  if ( !histogramToSubtract ) 
          {
            histogramToSubtract = getHistogram(dir, processToSubtract, "central", true);
          }
	  if ( verbosity )
          {
            double integral = compIntegral(histogramToSubtract, false, false);
            double integralErr = compIntegralErr(histogramToSubtract, false, false);
            std::cout << " integral(" << processToSubtract << ") = " << integral << " +/- " << integralErr << std::endl;
	  }
          histogramsToSubtract.push_back(histogramToSubtract);
        }
	    
        std::string dirName_output = dirName;
        if ( isDEBUG )
        {
          std::cout << "Creating directory = '" << dirName_output << "'" << std::endl;
        }
        TDirectory* dir_output = createSubdirectory_recursively(fs, dirName_output, verbosity);
        dir_output->cd();

        std::string histogramNameFakes_or_Flips;
        if( !central_or_shift.empty() && central_or_shift != "central" )
        {
          histogramNameFakes_or_Flips = Form("%s_%s", processFakes_or_Flips.data(), central_or_shift.data());
        }
        else
        {
          histogramNameFakes_or_Flips = processFakes_or_Flips;
        }
        TH1* histogramFakes_or_Flips = subtractHistograms(histogramNameFakes_or_Flips, histogramData, histogramsToSubtract, verbosity);
        if ( verbosity )
        {
          double integral = compIntegral(histogramFakes_or_Flips, false, false);
          double integralErr = compIntegralErr(histogramFakes_or_Flips, false, false);
	  std::cout << " integral(Fakes) = " << integral << " +/- " << integralErr << std::endl;
        }

        if( makeBinContentsPositive_dir )
        {
          makeBinContentsPositive(histogramFakes_or_Flips, false, verbosity); 
        }
      }
    }

    // recursively process all subdirectories
    std::vector<const TDirectory*> subdirs = getSubdirectories(dir);
    bool stopRecursion = ( max_depth_recursion != -1 && depth_recursion >= max_depth_recursion ) ? true : false;
    if ( !stopRecursion )
    {
      for ( std::vector<const TDirectory*>::iterator subdir = subdirs.begin();
            subdir != subdirs.end(); ++subdir ) {
        bool makeBinContentsPositive_subdir = makeBinContentsPositive_dir;
        if ( std::string((*subdir)->GetName()).find("mvaInputVarCorrelation") != std::string::npos ) 
        {
          makeBinContentsPositive_subdir = false;
        }
        processSubdirectory_recursively(
          fs, *subdir, dirName + "/" + (*subdir)->GetName(), 
          processData, processFakes_or_Flips, processesToSubtract, 
          central_or_shifts, 
          makeBinContentsPositive_subdir,
          depth_recursion + 1, max_depth_recursion,
          isDEBUG
        );
      }
    }
    else
    {
      if ( isDEBUG )
      {
        std::cout << "aborting recursion, because maximum-recursion-depth = " << max_depth_recursion << " has been reached." << std::endl;
      }
    }
    for ( const TDirectory* subdir : subdirs )
    {
      delete subdir;
      subdir = nullptr;
    }
  }
}

int main(int argc, char* argv[]) 
{
//--- throw an exception in case ROOT encounters an error
  gErrorAbortLevel = kError;

//--- parse command-line arguments
  if ( argc < 2 ) {
    std::cout << "Usage: " << argv[0] << " [parameters.py]" << std::endl;
    return EXIT_FAILURE;
  }

  std::cout << "<addBackgroundFromData>:" << std::endl;

//--- keep track of time it takes the macro to execute
  TBenchmark clock;
  clock.Start("addBackgroundFromData");

//--- read python configuration parameters
  if ( !edm::readPSetsFrom(argv[1])->existsAs<edm::ParameterSet>("process") ) 
    throw cms::Exception("addBackgroundFromData") 
      << "No ParameterSet 'process' found in configuration file = " << argv[1] << " !!\n";
  
  edm::ParameterSet cfg = edm::readPSetsFrom(argv[1])->getParameter<edm::ParameterSet>("process");

  edm::ParameterSet cfg_addBackgroundFromData = cfg.getParameter<edm::ParameterSet>("addBackgroundFromData");
  
  std::string processData = cfg_addBackgroundFromData.getParameter<std::string>("processData");
  std::string processFakes_or_Flips = cfg_addBackgroundFromData.getParameter<std::string>("processFakes_or_Flips");
  vstring processesToSubtract = cfg_addBackgroundFromData.getParameter<vstring>("processesToSubtract");

  const bool makeBinContentsPositive = ( cfg_addBackgroundFromData.exists("makeBinContentsPositive") ) ? 
    cfg_addBackgroundFromData.getParameter<bool>("makeBinContentsPositive") : false;

  vstring central_or_shifts = cfg_addBackgroundFromData.getParameter<vstring>("central_or_shifts");

  int max_depth_recursion = cfg_addBackgroundFromData.exists("max_depth_recursion") ? cfg_addBackgroundFromData.getParameter<int>("max_depth_recursion") : -1;

  bool isDEBUG = cfg_addBackgroundFromData.exists("isDEBUG") ? cfg_addBackgroundFromData.getParameter<bool>("isDEBUG") : false;

  fwlite::InputSource inputFiles(cfg); 
  if ( !(inputFiles.files().size() == 1) )
    throw cms::Exception("addBackgroundFromData") 
      << "Exactly one input file expected !!\n";
  TFile* inputFile = new TFile(inputFiles.files().front().data());

  fwlite::OutputFiles outputFile(cfg);
  fwlite::TFileService fs = fwlite::TFileService(outputFile.file().data());
  
  processSubdirectory_recursively(
    fs, 
    fs.getBareDirectory(), "",
    processData, processFakes_or_Flips, processesToSubtract,
    central_or_shifts,
    makeBinContentsPositive,
    1, max_depth_recursion,
    isDEBUG
  );

  delete inputFile;

  clock.Show("addBackgroundFromData");

  return EXIT_SUCCESS;
}
