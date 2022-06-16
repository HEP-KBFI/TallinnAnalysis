#include "TallinnAnalysis/Framework/interface/getFirst_and_LastBin.h"

bool
checkIfLabeledAxis(const TAxis * axis)
{
  bool isLabelled = false;
  const int numBins = axis->GetNbins();
  for(int idxBin = 1; idxBin <= numBins; ++idxBin)
  {
    const std::string binLabel = axis->GetBinLabel(idxBin);
    if(!binLabel.empty())
    {
      isLabelled = true;
      break;
    }
  }
  return isLabelled;
}

std::pair<int, int>
getFirst_and_LastBin(const TAxis * axis, bool includeUnderflowBin, bool includeOverflowBin)
{
  int numBins = axis->GetNbins();
  bool isLabeledAxis = checkIfLabeledAxis(axis);
  int firstBin = ( includeUnderflowBin && !isLabeledAxis ) ? 1 : 0;
  int lastBin  = ( includeOverflowBin  && !isLabeledAxis ) ? numBins + 1 : numBins;
  return std::pair<int, int>(firstBin, lastBin);
}

int
getFirstBin(const TAxis * axis, bool includeUnderflowBin)
{
  return getFirst_and_LastBin(axis, includeUnderflowBin, false).first;
}

int
getLastBin(const TAxis * axis, bool includeOverflowBin)
{
  return getFirst_and_LastBin(axis, false, includeOverflowBin).second;
}
