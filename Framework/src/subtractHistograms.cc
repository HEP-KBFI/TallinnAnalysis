#include "TallinnAnalysis/Framework/interface/subtractHistograms.h"

#include "TallinnAnalysis/Framework/interface/addHistograms.h"          // addHistograms()
#include "TallinnAnalysis/Framework/interface/checkCompatibleBinning.h" // checkCompatibleBinning()
#include "TallinnAnalysis/Framework/interface/getFirst_and_LastBin.h"   // getFirstBin(), getLastBin()
#include "TallinnNtupleProducer/CommonTools/interface/cmsException.h"   // cmsException

#include <assert.h>                                                     // assert()
#include <cmath>                                                        // std::sqrt()
#include <iostream>                                                     // std::cout

TH1 *
subtractHistograms(const std::string & newHistogramName,
                   const TH1 * histogramMinuend,
                   const TH1 * histogramSubtrahend,
                   int verbosity)
{
  if(verbosity)
  {
    std::cout << "<subtractHistograms>:\n"
      " newHistogramName = " << newHistogramName << '\n';
  }
  checkCompatibleBinning(histogramMinuend, histogramSubtrahend);

  TH1 * newHistogram = static_cast<TH1 *>(histogramMinuend->Clone(newHistogramName.data()));
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
      double binContentMinuend = 0.;
      double binErrorMinuend = 0.;
      double binContentSubtrahend = 0.;
      double binErrorSubtrahend = 0.;
      if(isHistogram2d)
      {
        binContentMinuend = histogramMinuend->GetBinContent(idxBinX, idxBinY);
        binErrorMinuend = histogramMinuend->GetBinError(idxBinX, idxBinY);
        binContentSubtrahend = histogramSubtrahend->GetBinContent(idxBinX, idxBinY);
        binErrorSubtrahend = histogramSubtrahend->GetBinError(idxBinX, idxBinY);
      }
      else
      {
        assert(idxBinY == firstBinY);
        binContentMinuend = histogramMinuend->GetBinContent(idxBinX);
        binErrorMinuend = histogramMinuend->GetBinError(idxBinX);
        binContentSubtrahend = histogramSubtrahend->GetBinContent(idxBinX);
        binErrorSubtrahend = histogramSubtrahend->GetBinError(idxBinX);
      }
      if(verbosity)
      {
        std::cout << "binX #" << idxBinX << " (x = " << xAxis->GetBinCenter(idxBinX) << ")";
        if(isHistogram2d)
        {
          std::cout << ", binY #" << idxBinY << " (y = " << yAxis->GetBinCenter(idxBinY) << ")";
        }
        std::cout << ": "
          "minuend = " << binContentMinuend << " +/- " << binErrorMinuend << ", "
          "subtrahend = " << binContentSubtrahend << " +/- " << binErrorSubtrahend << '\n';
      }
      double newBinContent = binContentMinuend - binContentSubtrahend;
      double newBinError2  = (binErrorMinuend*binErrorMinuend) + (binErrorSubtrahend*binErrorSubtrahend);
      if (!(newBinError2 >= 0.))
      {
        std::cout << "Infinite: binX #" << idxBinX << " (x = " << xAxis->GetBinCenter(idxBinX) << ")";
        if(isHistogram2d)
        {
          std::cout << ", binY #" << idxBinY << " (y = " << yAxis->GetBinCenter(idxBinY) << ")";
        }
        std::cout << ": "
          "newBinContent = " << newBinContent << ", "
          "newBinError2 = " << newBinError2 << '\n';
        newBinError2 = 0.;
        newBinContent = 0.;
      }
      if(isHistogram2d)
      {
        newHistogram->SetBinContent(idxBinX, idxBinY, newBinContent);
        newHistogram->SetBinError(idxBinX, idxBinY, std::sqrt(newBinError2));
      }
      else
      {
        newHistogram->SetBinContent(idxBinX, newBinContent);
        newHistogram->SetBinError(idxBinX, std::sqrt(newBinError2));
      }
    }
  }
  return newHistogram;
}

TH1 *
subtractHistograms(const std::string & newHistogramName,
                   const TH1* histogram,
                   const std::vector<const TH1 *> & histogramsToSubtract,
                   int verbosity)
{
  if(verbosity)
  {
    std::cout << "<subtractHistograms>:\n"
      " newHistogramName = " << newHistogramName << "\n"
      " histogram = " << histogram->GetName() << "\n"
      " #histogramsToSubtract = " << histogramsToSubtract.size() << '\n';
  }
  if(! histogram)
  {
    throw cmsException(__func__, __LINE__) << "No histograms given to subtract !!";
  }
  TH1 * newHistogram = nullptr;
  if(histogramsToSubtract.empty())
  {
    newHistogram = static_cast<TH1 *>(histogram->Clone(newHistogramName.data()));
  }
  else
  {
    const TH1 * histogramSubtrahend = addHistograms("subtractHistogramsTMP", histogramsToSubtract);
    newHistogram = subtractHistograms(newHistogramName, histogram, histogramSubtrahend, verbosity);
    delete histogramSubtrahend;
  }
  return newHistogram;
}


