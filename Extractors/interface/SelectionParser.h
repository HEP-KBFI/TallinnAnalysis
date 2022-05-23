#ifndef TallinnAnalysis_Extractors_SelectionParser_h
#define TallinnAnalysis_Extractors_SelectionParser_h

#include <string> // std::string
#include <vector> // std::vector

// forward declarations
class MyGrammar;

class SelectionParser
{
 public:
  SelectionParser();
  ~SelectionParser();

  std::vector<std::string>
  parse(const std::string & selection) const;

 private:
  MyGrammar * grammar_;
};

#endif // TallinnAnalysis_Extractors_SelectionParser_h
