#ifndef TallinnAnalysis_Framework_createSubdirectory_h
#define TallinnAnalysis_Framework_createSubdirectory_h

#include <CommonTools/Utils/interface/TFileDirectory.h> // TFileDirectory

#include <TDirectory.h>                                 // TDirectory

#include <string>                                       // std::string

TDirectory *
createSubdirectory(TDirectory * dir,
                   const std::string & subdirName,
                   int verbosity = 0);

TDirectory *
createSubdirectory_recursively(TFileDirectory & dir,
                               const std::string & fullSubdirName,
                               int verbosity = 0);

#endif // TallinnAnalysis_Framework_createSubdirectory_h

