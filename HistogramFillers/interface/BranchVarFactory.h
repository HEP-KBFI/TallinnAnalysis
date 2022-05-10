#ifndef TallinnAnalysis_HistogramFiller_BranchVarFactory_h
#define TallinnAnalysis_HistogramFiller_BranchVarFactory_h

#include <string> // std::string
#include <vector> // std::vector

// forward declarations
class BranchVarBase;

class BranchVarFactory
{
 public:
  BranchVarFactory();
  ~BranchVarFactory();

  BranchVarBase *
  create(const std::string & branchName_and_Type);

  std::vector<BranchVarBase *>
  create(const std::vector<std::string> & branchNames_and_Types);
};

#endif // TallinnAnalysis_HistogramFiller_BranchVarFactory_h
