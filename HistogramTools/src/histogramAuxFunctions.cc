#include "TallinnAnalysis/HistogramTools/interface/histogramAuxFunctions.h"

#include <TH1.h> // TH1
#include <TH2.h> // TH2

namespace
{
  template <typename T,
            typename = typename std::enable_if<std::is_arithmetic<T>::value>>
  T constexpr
  square(T x)
  {
    return x * x;
  }

  template <typename T,
            typename... Args,
            typename = typename std::enable_if<std::is_arithmetic<T>::value>>
  T constexpr
  square(T x,
         Args... rest)
  {
    return square(x) + square(rest...);
  }

  template<typename... Args>
  double
  quad(Args... args)
  {
    return std::sqrt(square(args...));
  }
}

void
fill(TH1 * histogram,
     double x,
     double evtWeight,
     double evtWeightErr)
{
  if(!histogram) return;
  const TAxis * const xAxis = histogram->GetXaxis();
  const int bin     = xAxis->FindBin(x);
  const int numBins = xAxis->GetNbins();
  if(! (bin >= 1 && bin <= numBins))
  {
    return;
  }

  const double binContent = histogram->GetBinContent(bin);
  const double binError   = histogram->GetBinError(bin);
  histogram->SetBinContent(bin, binContent + evtWeight);
  histogram->SetBinError(bin, quad(binError, evtWeight, evtWeightErr));
}

void
fillWithOverFlow(TH1 * histogram,
                 double x,
                 double evtWeight,
                 double evtWeightErr)
{
  if(!histogram) return;
  const TAxis * const xAxis = histogram->GetXaxis();
  const int bin = constrainValue(xAxis->FindBin(x), 1, xAxis->GetNbins());
  const double binContent = histogram->GetBinContent(bin);
  const double binError   = histogram->GetBinError(bin);
  histogram->SetBinContent(bin, binContent + evtWeight);
  histogram->SetBinError(bin, quad(binError, evtWeight, evtWeightErr));
}

void
fill2d(TH2 * histogram,
       double x,
       double y,
       double evtWeight,
       double evtWeightErr)
{
  if(!histogram) return;
  const TAxis * const xAxis = histogram->GetXaxis();
  const int binX     = xAxis->FindBin(x);
  const int numBinsX = xAxis->GetNbins();
  if(! (binX >= 1 && binX <= numBinsX))
  {
    return;
  }

  const TAxis * const yAxis = histogram->GetYaxis();
  const int binY     = yAxis->FindBin(y);
  const int numBinsY = yAxis->GetNbins();
  if(! (binY >= 1 && binY <= numBinsY))
  {
    return;
  }

  const double binContent = histogram->GetBinContent(binX, binY);
  const double binError   = histogram->GetBinError(binX, binY);
  histogram->SetBinContent(binX, binY, binContent + evtWeight);
  histogram->SetBinError(binX, binY, quad(binError, evtWeight, evtWeightErr));
}

void
fillWithOverFlow2d(TH2 * histogram,
                   double x,
                   double y,
                   double evtWeight,
                   double evtWeightErr)
{
  if(!histogram) return;
  const TAxis * const xAxis = histogram->GetXaxis();
  const int binX = constrainValue(xAxis->FindBin(x), 1, xAxis->GetNbins());

  const TAxis * const yAxis = histogram->GetYaxis();
  const int binY = constrainValue(yAxis->FindBin(y), 1, yAxis->GetNbins());

  const double binContent = histogram->GetBinContent(binX, binY);
  const double binError   = histogram->GetBinError(binX, binY);
  histogram->SetBinContent(binX, binY, binContent + evtWeight);
  histogram->SetBinError(binX, binY, quad(binError, evtWeight, evtWeightErr));
}
