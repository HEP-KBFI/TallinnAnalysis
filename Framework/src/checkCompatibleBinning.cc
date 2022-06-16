#include "TallinnAnalysis/Framework/interface/checkCompatibleBinning.h"

#include "TallinnNtupleProducer/CommonTools/interface/cmsException.h" // cmsException

#include <TAxis.h> // TAxis

void
checkCompatibleBinning(const TH1 * histogram1,
                       const TH1 * histogram2)
{
  if(histogram1 && histogram2)
  {
    int dimension1 = histogram1->GetDimension();
    int dimension2 = histogram2->GetDimension();
    if(dimension1 != dimension2)
    {
      throw cmsException(__func__, __LINE__)
        << "Histograms " << histogram1->GetName() << " and " << histogram2->GetName()
        << " have incompatible dimension (dimension: histogram1 = "
        << dimension1 << ", histogram2 = " << dimension2 << ')';
    }
    if(histogram1->GetNbinsX() != histogram2->GetNbinsX())
    {
      throw cmsException(__func__, __LINE__)
        << "Histograms " << histogram1->GetName() << " and " << histogram2->GetName()
        << " have incompatible x-axis binning !!\n (NbinsX: histogram1 = "
        << histogram1->GetNbinsX() << ", histogram2 = " << histogram2->GetNbinsX() << ')';
    }
    const TAxis * const xAxis1 = histogram1->GetXaxis();
    const TAxis * const xAxis2 = histogram2->GetXaxis();
    const int numBinsX = xAxis1->GetNbins();
    for(int idxBinX = 1; idxBinX <= numBinsX; ++idxBinX)
    {
      const double binWidthX = 0.5 * (xAxis1->GetBinWidth(idxBinX) + xAxis2->GetBinWidth(idxBinX));
      const double dBinLowEdgeX = xAxis1->GetBinLowEdge(idxBinX) - xAxis2->GetBinLowEdge(idxBinX);
      const double dBinUpEdgeX  = xAxis1->GetBinUpEdge(idxBinX)  - xAxis2->GetBinUpEdge(idxBinX);
      if(! (dBinLowEdgeX < 1.e-3 * binWidthX && dBinUpEdgeX < 1.e-3 * binWidthX))
      {
        throw cmsException(__func__, __LINE__)
          << "Histograms " << histogram1->GetName() << " and " << histogram2->GetName()
          << " have incompatible x-axis binning !!\n (bin #" << idxBinX << ": histogram1 = "
          << xAxis1->GetBinLowEdge(idxBinX) << ".." << xAxis1->GetBinUpEdge(idxBinX) << ","
          " histogram2 = " << xAxis2->GetBinLowEdge(idxBinX) << ".." << xAxis2->GetBinUpEdge(idxBinX) << ')';
      }
    }
    if(dimension1 >= 2 && dimension2 >= 2)
    {
      if(histogram1->GetNbinsY() != histogram2->GetNbinsY())
      {
        throw cmsException(__func__, __LINE__)
          << "Histograms " << histogram1->GetName() << " and " << histogram2->GetName()
          << " have incompatible y-axis binning !!\n (NbinsY: histogram1 = "
          << histogram1->GetNbinsY() << ", histogram2 = " << histogram2->GetNbinsY() << ')';
      }
      const TAxis * const yAxis1 = histogram1->GetYaxis();
      const TAxis * const yAxis2 = histogram2->GetYaxis();
      const int numBinsY = yAxis1->GetNbins();
      for(int idxBinY = 1; idxBinY <= numBinsY; ++idxBinY)
      {
        const double binWidthY = 0.5 * (yAxis1->GetBinWidth(idxBinY) + yAxis2->GetBinWidth(idxBinY));
        const double dBinLowEdgeY = yAxis1->GetBinLowEdge(idxBinY) - yAxis2->GetBinLowEdge(idxBinY);
        const double dBinUpEdgeY  = yAxis1->GetBinUpEdge(idxBinY)  - yAxis2->GetBinUpEdge(idxBinY);
        if(! (dBinLowEdgeY < 1.e-3 * binWidthY && dBinUpEdgeY < 1.e-3 * binWidthY))
        {
          throw cmsException(__func__, __LINE__)
            << "Histograms " << histogram1->GetName() << " and " << histogram2->GetName()
            << " have incompatible y-axis binning !!\n (bin #" << idxBinY << ": histogram1 = "
            << yAxis1->GetBinLowEdge(idxBinY) << ".." << yAxis1->GetBinUpEdge(idxBinY) << ","
            " histogram2 = " << yAxis2->GetBinLowEdge(idxBinY) << ".." << yAxis2->GetBinUpEdge(idxBinY) << ')';
        }
      }
    }
  }
}
