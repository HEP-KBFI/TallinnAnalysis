#include "TallinnAnalysis/HistogramTools/interface/createHistogram.h"

#include "TallinnNtupleProducer/CommonTools/interface/cmsException.h" // cmsException()
#include "TallinnNtupleProducer/CommonTools/interface/format_vT.h"    // format_vdouble()

#include <TArrayD.h>                                                  // TArrayD
#include <TDirectory.h>                                               // TDirectory
#include <TH1.h>                                                      // TH1, TH1D
#include <TH2.h>                                                      // TH2, TH2D
#include <TObjArray.h>                                                // TObjArray
#include <TObjString.h>                                               // TObjString
#include <TString.h>                                                  // Form(), TString

#include <assert.h>                                                   // assert()
#include <iostream>                                                   // std::cout
#include <vector>                                                     // std::vector

typedef std::vector<double> vdouble;

namespace
{
  std::pair<std::string, std::string>
  getHistogramName_and_Title(const edm::ParameterSet & cfg)
  {
    std::string process = cfg.getParameter<std::string>("process");
    std::string central_or_shift = cfg.getParameter<std::string>("central_or_shift");
    std::string histogramTitle;
    if ( central_or_shift != "" && central_or_shift != "central" ) histogramTitle = Form("%s_%s", process.data(), central_or_shift.data());
    else                                                           histogramTitle = process;
    std::string histogramName = histogramTitle;
    return std::pair<std::string, std::string>(histogramName, histogramTitle);
  }

  std::vector<double>
  getBinning(const edm::ParameterSet & cfg_binning, const std::string & attr_xMin, const std::string & attr_xMax)
  {
    std::vector<double> binning;
    if ( cfg_binning.exists("binning" ) )
    {
      binning = cfg_binning.getParameter<vdouble>("binning");
      int numBins = binning.size() - 1;
      if ( !(numBins >= 1) ) 
        throw cmsException(__func__, __LINE__) << "Invalid Configuration parameter 'binning' = " << format_vdouble(binning) << " !!";
    }
    else
    {
      unsigned int numBins = cfg_binning.getParameter<unsigned int>("numBins");
      if ( !(numBins >= 1) ) 
        throw cmsException(__func__, __LINE__) << "Invalid Configuration parameter 'numBins' = " << numBins << " !!";
      double xMin = cfg_binning.getParameter<double>(attr_xMin);
      double xMax = cfg_binning.getParameter<double>(attr_xMax);
      if ( !(xMax > xMin) )
        throw cmsException(__func__, __LINE__) << "Invalid Configuration parameters '" << attr_xMin << "' = " << xMin << "," 
                                                << " '" << attr_xMax << "' = " << xMax << " !!";
      double binWidth = (xMax - xMin)/numBins;
      for ( unsigned int idxBin = 0; idxBin < numBins; ++idxBin )
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

  TDirectory *
  createSubdirectory(TDirectory * dir,
                     const std::string & subdirName,
                     bool verbose = false)
  {
    if(verbose)
    {
      std::cout << "<createSubdirectory>:\n"
                   " dir = " << dir << ": name = '" << dir->GetName() << "'\n"
                   " subdirName = '" << subdirName << "'\n";
    }

    dir->cd();
    if(! dir->Get(subdirName.data()))
    {
      if(verbose)
      {
        std::cout << "--> creating subdir = '" << subdirName << "'\n";
      }
      dir->mkdir(subdirName.data());
    }
    else
    {
      if(verbose)
      {
        std::cout << "--> subdir = '" << subdirName << "' already exists --> skipping\n";
      }
    }
    TDirectory * subdir = dynamic_cast<TDirectory *>(dir->Get(subdirName.data()));
    assert(subdir);
    return subdir;
  }

  TDirectory *
  createSubdirectory_recursively(TFileDirectory & dir,
                               const std::string & fullSubdirName,
                               bool verbose = false)
  {
    if(verbose)
    {
      std::cout << "<createSubdirectory_recursively>:\n"
                   " dir = " << &dir << ": name = '" << dir.getBareDirectory()->GetName() << "'\n"
                   " fullSubdirName = '" << fullSubdirName << "'\n";
    }

    TString fullSubdirName_tstring = fullSubdirName.data();
    TObjArray * subdirNames = fullSubdirName_tstring.Tokenize("/");
    int numSubdirectories = subdirNames->GetEntries();

    TDirectory * parent = dir.getBareDirectory();
    for ( int idxSubdir = 0; idxSubdir < numSubdirectories; ++idxSubdir )
    {
      const TObjString * subdirName = dynamic_cast<TObjString *>(subdirNames->At(idxSubdir));
      assert(subdirName);

      TDirectory * subdir = createSubdirectory(parent, subdirName->GetString().Data(), verbose);
      parent = subdir;
    }
    delete subdirNames;
    return parent;
  }
}

std::pair<TDirectory*, TH1 *>
createHistogram1D(TFileDirectory & dir, const edm::ParameterSet & cfg)
{
  std::pair<std::string, std::string> histogramName_and_Title = getHistogramName_and_Title(cfg);
  const std::string & histogramName = histogramName_and_Title.first;
  const std::string & histogramTitle = histogramName_and_Title.second;
  std::string histogramDir = cfg.getParameter<std::string>("histogramDir");
  std::cout << "Filling histogram = '" << histogramTitle << "' of type TH1D, stored in directory = '" << histogramDir << "'" << std::endl;
  std::vector<double> binning = getBinning(cfg.getParameterSet("xAxis"), "xMin", "xMax");
  TArrayD binning_TArrayD = convert_to_TArrayD(binning);
  int numBins = binning_TArrayD.GetSize() - 1;
  TDirectory* const subdir = createSubdirectory_recursively(dir, histogramDir);
  subdir->cd();
  TH1* histogram = new TH1D(histogramName.data(), histogramTitle.data(), numBins, binning_TArrayD.GetArray());
  //std::cout << " binning = " << format_vdouble(binning) << std::endl;
  return std::pair<TDirectory*, TH1 *>(subdir, histogram);
}

std::pair<TDirectory*, TH2 *>
createHistogram2D(TFileDirectory & dir, const edm::ParameterSet & cfg)
{
  std::pair<std::string, std::string> histogramName_and_Title = getHistogramName_and_Title(cfg);
  const std::string & histogramName = histogramName_and_Title.first;
  const std::string & histogramTitle = histogramName_and_Title.second;
  std::string histogramDir = cfg.getParameter<std::string>("histogramDir");
  std::cout << "Filling histogram = '" << histogramTitle << "' of type TH2D, stored in directory = '" << histogramDir << "'" << std::endl;
  std::vector<double> binningX = getBinning(cfg.getParameterSet("xAxis"), "xMin", "xMax");
  TArrayD binningX_TArrayD = convert_to_TArrayD(binningX);
  int numBinsX = binningX_TArrayD.GetSize() - 1;
  std::vector<double> binningY = getBinning(cfg.getParameterSet("yAxis"), "yMin", "yMax");
  TArrayD binningY_TArrayD = convert_to_TArrayD(binningY);
  int numBinsY = binningY_TArrayD.GetSize() - 1;
  TDirectory* const subdir = createSubdirectory_recursively(dir, histogramDir);
  subdir->cd();
  TH2* histogram = new TH2D(histogramName.data(), histogramTitle.data(), numBinsX, binningX_TArrayD.GetArray(), numBinsY, binningY_TArrayD.GetArray());
  //std::cout << " binning: x-axis = " << format_vdouble(binningX) << ", y-axis = " << format_vdouble(binningY) << std::endl;
  return std::pair<TDirectory*, TH2 *>(subdir, histogram);
}
