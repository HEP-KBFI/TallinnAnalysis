#include "TallinnAnalysis/Framework/interface/getHistogram.h"

#include "TallinnNtupleProducer/CommonTools/interface/cmsException.h" // cmsException

#include <iostream>                                                   // std::cout

TH1 *
getHistogram(const TDirectory * dir,
             const std::string & process,
             const std::string & central_or_shift,
             bool enableException,
             bool verbose)
{
  std::string histogramName_full;
  if(! central_or_shift.empty() && central_or_shift != "central")
  {
    histogramName_full = Form("%s_%s", process.data(), central_or_shift.data());
  }
  else
  {
    histogramName_full = process.data();
  }

  TH1 * histogram = dynamic_cast<TH1 *>((const_cast<TDirectory *>(dir))->Get(histogramName_full.data()));
  if(histogram)
  {
    if(verbose)
    {
      std::cout << "--> returning histogram = " << histogram << ": name = '" << histogram->GetName() << "'\n";
    }
  }
  else if(enableException)
  {
    dir->ls();
    throw cmsException(__func__, __LINE__)
      << "Failed to find histogram = '" << histogramName_full << "' in directory = " << dir << ": "
         "name = '" << dir->GetName() << "' !!\n";
  }
  return histogram;
}
