#include "TallinnAnalysis/CommonTools/interface/DumpToJSONFile.h"

#include <iomanip> // std::quoted()
#include <ostream> // std::endl;
#include <sstream> // std::ostringstream

DumpToJSONFile::DumpToJSONFile(const std::string & jsonFileName)
  : jsonFileName_(jsonFileName)
  , isFirstEvent_(true)
{
  jsonFile_ = new std::ofstream(jsonFileName);
  (*jsonFile_) << "{ ";
}

DumpToJSONFile::~DumpToJSONFile()
{
  (*jsonFile_) << " }" << std::endl;
  delete jsonFile_;
}

void
DumpToJSONFile::write(UInt_t run, UInt_t lumi, ULong64_t event,
                      const std::map<std::string, double> & variables) const
{
  std::ostringstream run_lumi_event;
  if ( !isFirstEvent_ )
  {
    (*jsonFile_) << ", ";
  }
  isFirstEvent_ = false;
  run_lumi_event << run << ":" << lumi << ":" << event;
  (*jsonFile_) << std::quoted(run_lumi_event.str()) << " : { ";
  bool isFirstVariable = true;
  for ( std::map<std::string, double>::const_iterator variable = variables.begin(); variable != variables.end(); ++variable )
  {
    if ( !isFirstVariable )
    {
      (*jsonFile_) << ", ";
    }
    isFirstVariable = false;
    (*jsonFile_) << std::quoted(variable->first) << " : " << variable->second;
  }
  (*jsonFile_) << " }";
}
