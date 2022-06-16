#include "TallinnAnalysis/Framework/interface/makeBinContentsPositive.h"

#include "TallinnAnalysis/Framework/interface/compIntegral.h"         // compIntegral()
#include "TallinnAnalysis/Framework/interface/getFirst_and_LastBin.h" // getFirstBin(), getLastBin()

#include <assert.h>                                                   // assert()
#include <cmath>                                                      // std::sqrt()
#include <iostream>                                                   // std::cout

namespace
{
  double
  square(double x)
  {
    return x*x;
  }
}

void
makeBinContentsPositive(TH1 * histogram, 
                        bool isData,
                        int verbosity)
{
  if ( verbosity )
  {
    std::cout << "<makeBinContentsPositive>:\n"
                 " integral(" << histogram->GetName()  << ") = "
                              << histogram->Integral() << '\n';
  }

  double integral_original = compIntegral(histogram, false, false);
  if ( integral_original < 0. )
  {
    integral_original = 0.;
  }
  if ( verbosity )
  {
    std::cout << " integral_original = " << integral_original << '\n';
  }

  const TAxis * xAxis = histogram->GetXaxis();
  const int firstBinX = getFirstBin(xAxis, true);
  const int lastBinX = getLastBin(xAxis, true);

  const TAxis * yAxis = nullptr;
  int firstBinY = 1;
  int lastBinY = 1;
  bool isHistogram2d = ( histogram->GetDimension() == 2 ) ? true : false;
  if ( isHistogram2d ) 
  {
    yAxis = histogram->GetYaxis();
    firstBinY = getFirstBin(yAxis, true);
    lastBinY = getLastBin(yAxis, true);
  }

  for ( int idxBinX = firstBinX; idxBinX <= lastBinX; ++idxBinX )
  {
    for ( int idxBinY = firstBinY; idxBinY <= lastBinY; ++idxBinY )
    {
      double binContent_original = 0.;
      double binError2_original = 0.;
      if ( isHistogram2d )
      {
        binContent_original = histogram->GetBinContent(idxBinX, idxBinY);
        binError2_original = square(histogram->GetBinError(idxBinX, idxBinY));
      }
      else
      {
        assert(idxBinY == firstBinY);
        binContent_original = histogram->GetBinContent(idxBinX);
        binError2_original = square(histogram->GetBinError(idxBinX));
      }
      if ( binContent_original < 0. )
      {
        const double binContent_modified = 0.;
        const double binError2_modified = binError2_original + square(binContent_original - binContent_modified);
        assert(binError2_modified >= 0.);
        if ( verbosity )
        {
          std::cout << "binX #" << idxBinX << " (x =  " << xAxis->GetBinCenter(idxBinX) << ")";
          if ( isHistogram2d )
          {
            std::cout << ", binY #" << idxBinY << " (y =  " << yAxis->GetBinCenter(idxBinY) << ")";
          }
          std::cout << ": " 
                       "binContent = " << binContent_original << " +/- " << std::sqrt(binError2_original) << " --> setting it to "
                       "binContent = " << binContent_modified << " +/- " << std::sqrt(binError2_modified) << '\n';
        }
        if ( isHistogram2d )
        {
          histogram->SetBinContent(idxBinX, idxBinY, binContent_modified);
          histogram->SetBinError(idxBinX, idxBinY, std::sqrt(binError2_modified));
        }
        else
        {          
          histogram->SetBinContent(idxBinX, binContent_modified);
          histogram->SetBinError(idxBinX, std::sqrt(binError2_modified));
        }
      }
    }
  }

  double integral_modified = compIntegral(histogram, false, false);
  if ( integral_modified < 0. )
  {
    integral_modified = 0.;
  }
  if (verbosity )
  {
    std::cout << " integral_modified = " << integral_modified << '\n';
  }

  if ( integral_original > 0. && integral_modified > 0. )
  {
    const double sf = integral_original / integral_modified;
    if(verbosity)
    {
      std::cout << "--> scaling bin-contents by factor = " << sf << ", while keeping the bin-errors the same" << '\n';
    }
    for(int idxBinX = firstBinX; idxBinX <= lastBinX; ++idxBinX)
    {
      for(int idxBinY = firstBinY; idxBinY <= lastBinY; ++idxBinY)
      {
        if ( isHistogram2d )
        {
          const double binContent = histogram->GetBinContent(idxBinX, idxBinY);
          histogram->SetBinContent(idxBinX, idxBinY, sf*binContent);
        }
        else
        {
          const double binContent = histogram->GetBinContent(idxBinX);
          histogram->SetBinContent(idxBinX, sf*binContent);
        }
      }
    }
  }
  else if ( !isData )
  {
    for ( int idxBinX = firstBinX; idxBinX <= lastBinX; ++idxBinX )
    {
      for ( int idxBinY = firstBinY; idxBinY <= lastBinY; ++idxBinY )
      {
        if ( isHistogram2d )
        {
          histogram->SetBinContent(idxBinX, idxBinY, 1.e-3/((lastBinX - firstBinX + 1)*(lastBinY - firstBinY + 1)));
        }
        else
        {
          histogram->SetBinContent(idxBinX, 1.e-3/(lastBinX - firstBinX + 1));
        }
      }
    }
  }

  if ( verbosity )
  {
    double integral = compIntegral(histogram, false, false);
    double integralErr = compIntegralErr(histogram, false, false);
    std::cout << " integral(" << histogram->GetName() << ") = " << integral << " +/- " << integralErr << '\n';
  }
}
