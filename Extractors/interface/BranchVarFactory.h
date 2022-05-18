#ifndef TallinnAnalysis_Extractors_BranchVarFactory_h
#define TallinnAnalysis_Extractors_BranchVarFactory_h

#include <memory> // std::shared_ptr
#include <string> // std::string
#include <vector> // std::vector

// forward declarations
class BranchVarBase;

class BranchVarFactory
{
 public:
  BranchVarFactory();
  ~BranchVarFactory();

  std::shared_ptr<BranchVarBase>
  create(const std::string & branchName_and_Type);

  std::vector<std::shared_ptr<BranchVarBase>>
  create(const std::vector<std::string> & branchNames_and_Types);
};

#endif // TallinnAnalysis_Extractors_BranchVarFactory_h
