#include "TallinnAnalysis/Framework/interface/addHistograms.h"

#include "TallinnAnalysis/Framework/interface/checkCompatibleBinning.h" // checkCompatibleBinning()
#include "TallinnAnalysis/Framework/interface/getFirst_and_LastBin.h"   // getFirstBin(), getLastBin()
#include "TallinnNtupleProducer/CommonTools/interface/cmsException.h"   // cmsException

#include <assert.h>                                                     // assert()
#include <cmath>                                                        // std::sqrt()
#include <iostream>                                                     // std::cout

TH1 *
addHistograms(const std::string & newHistogramName,
              const TH1 * histogram1,
              const TH1 * histogram2,
              int verbosity)
{
  const std::vector<const TH1 *> histogramsToAdd = { histogram1, histogram2 };
  return addHistograms(newHistogramName, histogramsToAdd, verbosity);
}

TH1 *
addHistograms(const std::string & newHistogramName,
              const std::vector<const TH1 *> & histogramsToAdd,
              int verbosity) 
{
  if(verbosity)
  {
    std::cout << "<addHistograms>:\n"
      " newHistogramName = " << newHistogramName << '\n';
  }
  if(histogramsToAdd.empty())
  {
    throw cmsException(__func__, __LINE__) << "No histograms given to add";
  }

  const TH1 * histogramRef = histogramsToAdd.front();
  if(verbosity)
  {
    std::cout << "histogramRef = " << histogramRef->GetName() << "\n"
      "#histogramsToAdd = " << histogramsToAdd.size() << '\n';
  }

  for(const TH1 * const histogramToAdd: histogramsToAdd)
  {
    if(histogramToAdd == histogramRef)
    {
      continue;
    }
    if(verbosity)
    {
      std::cout << "histogramToAdd = " << histogramToAdd->GetName() << ", "
        "integral = " << histogramToAdd->Integral() << '\n';
    }
    checkCompatibleBinning(histogramToAdd, histogramRef);
  }
  
  TH1 * newHistogram = static_cast<TH1 *>(histogramRef->Clone(newHistogramName.data()));
  newHistogram->Reset();
  if(! newHistogram->GetSumw2N())
  {
    newHistogram->Sumw2();
  }
  
  const TAxis * xAxis = newHistogram->GetXaxis();
  int firstBinX = getFirstBin(xAxis, true);
  int lastBinX = getLastBin(xAxis, true);

  const TAxis * yAxis = nullptr;
  int firstBinY = 1;
  int lastBinY = 1;
  bool isHistogram2d = ( newHistogram->GetDimension() == 2 ) ? true : false;
  if ( isHistogram2d ) 
  {
    yAxis = newHistogram->GetYaxis();
    firstBinY = getFirstBin(yAxis, true);
    lastBinY = getLastBin(yAxis, true);
  }

  for(int idxBinX = firstBinX; idxBinX <= lastBinX; ++idxBinX)
  {
    for(int idxBinY = firstBinY; idxBinY <= lastBinY; ++idxBinY)
    {
      double sumBinContent = 0.;
      double sumBinError2 = 0.;
      for(const TH1 * histogramToAdd: histogramsToAdd)
      {
        double binContent = 0.;
        double binError = 0.;
        if(isHistogram2d)
        {
          binContent = histogramToAdd->GetBinContent(idxBinX, idxBinY);
          binError = histogramToAdd->GetBinError(idxBinX, idxBinY);
        }
        else
        {
          assert(idxBinY == firstBinY);
          binContent = histogramToAdd->GetBinContent(idxBinX);
          binError = histogramToAdd->GetBinError(idxBinX);
        }
        if(verbosity)
        {
          std::cout << "histogramToAdd = " << histogramToAdd->GetName() << ", "
            "binContent = " << binContent << ", "
            "binError = " << binError << '\n';
        }
        sumBinContent += binContent;
        sumBinError2 += binError*binError;
      }
      if(verbosity)
      {
        std::cout << "binX #" << idxBinX << " (x = " << xAxis->GetBinCenter(idxBinX) << ")";
        if(isHistogram2d)
        {
          std::cout << ", binY #" << idxBinY << " (y = " << yAxis->GetBinCenter(idxBinY) << ")";
        }
        std::cout << ": "
          "sumBinContent = " << sumBinContent << ", "
          "sumBinError2 = " << sumBinError2 << '\n';
      }
      if (!(sumBinError2 >= 0.))
      {
        std::cout << "Infinite: binX #" << idxBinX << " (x = " << xAxis->GetBinCenter(idxBinX) << ")";
        if(isHistogram2d)
        {
          std::cout << ", binY #" << idxBinY << " (y = " << yAxis->GetBinCenter(idxBinY) << ")";
        }
        std::cout << ": "
          "sumBinContent = " << sumBinContent << ", "
          "sumBinError2 = " << sumBinError2 << '\n';
        sumBinError2 = 0.;
        sumBinContent = 0.;
      }
      if(isHistogram2d)
      {
        newHistogram->SetBinContent(idxBinX, idxBinY, sumBinContent);
        newHistogram->SetBinError(idxBinX, idxBinY, std::sqrt(sumBinError2));
      }
      else
      {
        newHistogram->SetBinContent(idxBinX, sumBinContent);
        newHistogram->SetBinError(idxBinX, std::sqrt(sumBinError2));
      }
    }
  }
  return newHistogram;
}
