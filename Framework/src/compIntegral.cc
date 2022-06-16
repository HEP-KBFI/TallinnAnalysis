#include "TallinnAnalysis/Framework/interface/compIntegral.h"

#include "TallinnAnalysis/Framework/interface/getFirst_and_LastBin.h" // getFirstBin(), getLastBin()

#include <TAxis.h>                                                    // TAxis

#include <assert.h>                                                   // assert()
#include <cmath>                                                      // std::sqrt()

double
compIntegral(const TH1 * histogram,
             bool includeUnderflowBin,
             bool includeOverflowBin)
{
  const TAxis * xAxis = histogram->GetXaxis();
  int firstBinX = getFirstBin(xAxis, includeUnderflowBin);
  int lastBinX = getLastBin(xAxis, includeOverflowBin);

  int firstBinY = 1;
  int lastBinY = 1;
  bool isHistogram2d = ( histogram->GetDimension() == 2 ) ? true : false;
  if ( isHistogram2d ) 
  {
    const TAxis * yAxis = histogram->GetYaxis();
    firstBinY = getFirstBin(yAxis, includeUnderflowBin);
    lastBinY = getLastBin(yAxis, includeOverflowBin);
  }

  double sumBinContent = 0.;
  for ( int idxBinX = firstBinX; idxBinX <= lastBinX; ++idxBinX)
  {
    for ( int idxBinY = firstBinY; idxBinY <= lastBinY; ++idxBinY)
    {
      if ( isHistogram2d )
      {
        sumBinContent += histogram->GetBinContent(idxBinX, idxBinY);
      }
      else
      {
        assert(idxBinY == firstBinY);
        sumBinContent += histogram->GetBinContent(idxBinX);
      }
    }
  }
  return sumBinContent;
}

namespace
{
  double 
  square(double x)
  {
    return x*x;
  }
}

double
compIntegralErr(const TH1 * histogram,
                bool includeUnderflowBin,
                bool includeOverflowBin)
{
  int firstBinX = getFirstBin(histogram->GetXaxis(), includeUnderflowBin);
  int lastBinX = getLastBin(histogram->GetXaxis(), includeOverflowBin);

  int firstBinY = 1;
  int lastBinY = 1;
  bool isHistogram2d = ( histogram->GetDimension() == 2 ) ? true : false;
  if ( isHistogram2d ) 
  {
    firstBinY = getFirstBin(histogram->GetYaxis(), includeUnderflowBin);
    lastBinY = getLastBin(histogram->GetYaxis(), includeOverflowBin);
  }

  double sumBinErr2 = 0.;
  for ( int idxBinX = firstBinX; idxBinX <= lastBinX; ++idxBinX)
  {
    for ( int idxBinY = firstBinY; idxBinY <= lastBinY; ++idxBinY)
    {
      if (isHistogram2d)
      {
        sumBinErr2 += square(histogram->GetBinError(idxBinX, idxBinY));
      }
      else
      {
        assert(idxBinY == firstBinY);
        sumBinErr2 += square(histogram->GetBinError(idxBinX));
      }
    }
  }
  return std::sqrt(sumBinErr2);
}
