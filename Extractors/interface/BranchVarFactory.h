#ifndef TallinnAnalysis_Extractors_BranchVarFactory_h
#define TallinnAnalysis_Extractors_BranchVarFactory_h

#include <map>    // std::map
#include <memory> // std::shared_ptr
#include <string> // std::string
#include <vector> // std::vector

// forward declarations
class BranchVarBase;
class TTree;

class BranchVarFactory
{
 public:
  BranchVarFactory();
  ~BranchVarFactory();

  static
  void
  initialize(TTree * tree);

  static
  void
  set_central_or_shift(const std::string & central_or_shift);

  static
  std::string
  get_selection(const std::string & selection);

  static
  std::shared_ptr<BranchVarBase>
  create(const std::string & branchName);

  static
  std::vector<std::shared_ptr<BranchVarBase>>
  create(const std::vector<std::string> & branchNames);

 private:
  static std::map<std::string, std::string> branchMap_; // key = branchName, value = branchType
  static std::string central_or_shift_;
};

#endif // TallinnAnalysis_Extractors_BranchVarFactory_h
