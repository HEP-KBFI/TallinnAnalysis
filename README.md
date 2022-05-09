# TallinnAnalysis
set of software packages for analyzing "plain" Tallinn Ntuples

# To checkout the code:
source /cvmfs/cms.cern.ch/cmsset_default.sh

cmsrel CMSSW_12_3_1

cd CMSSW_12_3_1

cmsenv

git clone https://github.com/HEP-KBFI/TallinnAnalysis $CMSSW_BASE/src/TallinnAnalysis
