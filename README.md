# TallinnAnalysis

A set of software packages for analyzing "plain" Tallinn Ntuples

# To checkout the code:

```bash
source /cvmfs/cms.cern.ch/cmsset_default.sh

cmsrel CMSSW_12_3_1
cd $_
cmsenv

git clone git+ssh://git@github.com/HEP-KBFI/TallinnAnalysis.git $CMSSW_BASE/src/TallinnAnalysis

cd $CMSSW_BASE/src/
scram b -j 8
```

The package `TallinnAnalysis` depends on the package `TallinnNtupleProducer`
(the code will not compile unless the package TallinnNtupleProducer is checked-out too).
Please follow the installation instructions given in https://github.com/HEP-KBFI/TallinnNtupleProducer/blob/main/README.md 
to install the `TallinnNtupleProducer` package.
