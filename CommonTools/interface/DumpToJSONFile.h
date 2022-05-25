#ifndef TallinnAnalysis_CommonTools_DumpToJSONFile_h
#define TallinnAnalysis_CommonTools_DumpToJSONFile_h

#include <Rtypes.h> // UInt_t, ULong64_t

#include <fstream>  // std::ofstream
#include <map>      // std::map
#include <string>   // std::string

class DumpToJSONFile
{
 public:
  DumpToJSONFile(const std::string & jsonFileName);
  ~DumpToJSONFile();

  void
  write(UInt_t run, UInt_t lumi, ULong64_t event,
        const std::map<std::string, double> & variables) const;

 private:
  std::string jsonFileName_;
  std::ofstream * jsonFile_;
  mutable bool isFirstEvent_;
};

#endif // TallinnAnalysis_CommonTools_DumpToJSONFile_h
