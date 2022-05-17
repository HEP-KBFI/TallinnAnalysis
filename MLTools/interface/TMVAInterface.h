#ifndef TallinnAnalysis_MLTools_TMVAInterface_h
#define TallinnAnalysis_MLTools_TMVAInterface_h 

#include <Rtypes.h> // Float_t, ULong64_t

#include <map>      // std::map
#include <string>   // std::string
#include <vector>   // std::vector

// forward declarations
namespace TMVA
{
  class Reader;
}
class MVAInputVarTransformer;

class TMVAInterface
{
 public:
  TMVAInterface(const std::string & mvaFileName_odd,
                const std::vector<std::string> & mvaInputVariables,
                const std::vector<std::string> & classes,
                const std::string & mvaFileName_even = "",
                const std::string & fitFunctionFileName = "");
  ~TMVAInterface();

  // CV: call transform_mvaOutput(true) if using XML files converted from xgboost trainings,
  //     call transform_mvaOutput(false) if using XML files trained with TMVA
  void transform_mvaOutput(bool flag);

  bool is_multiclass() const;

  /**
   * @brief Calculates MVA output. Use the function get_mvaOutput for the non-multiclass ("regular") case 
   *        and the function get_mvaOutput_multiclass for the multiclass case.
   * @params mvaInputs: Values of MVA input variables (stored in std::map with key = MVA input variable name)
   *         event_number: Event number for the current event (used to switch between BDTs/ANNs for odd vs even events, to avoid train vs test bias)
   * @return In non-multiclass case: ANN output, in multiclass case: std::map with key = class name and value = ANN output for that class
   */
  double
  get_mvaOutput(const std::map<std::string, double> & mvaInputs,
                ULong64_t event_number = 0) const;

  std::map<std::string, double>
  get_mvaOutput_multiclass(const std::map<std::string, double> & mvaInputs,
                           ULong64_t event_number = 0) const;

  /**
   * @brief Return list of MVA input variables
   */
  const std::vector<std::string> &
  get_mvaInputVariables() const;

 private:
  /**
   * @brief Calculate MVA output. This function handles the multiclass as well as the non-multiclass ("regular") case.
   * @params mvaInputs: Values of ANN input variables (stored in std::map with key = ANN input variable name)
   *         event_number: Event number for the current event (used to switch between ANNs for odd vs even events, to avoid train vs test bias)
   */
  void evaluate(const std::map<std::string, double> & mvaInputs, 
                ULong64_t event_number) const;

  const std::vector<std::string> classes_;
  bool is_multiclass_;

  std::string mvaFileName_odd_;
  TMVA::Reader * mva_odd_;

  std::string mvaFileName_even_;
  TMVA::Reader * mva_even_;

  bool transform_mvaOutput_;

  std::vector<std::string> mvaInputVariables_;
  mutable std::map<std::string, Float_t> mvaInputVariableMap_; // key = MVA input variable name

  std::string fitFunctionFileName_;
  MVAInputVarTransformer * mvaInputTransformation_;

  mutable std::map<std::string, double> mvaOutput_multiclass_;
  mutable double mvaOutput_;

  bool isDEBUG_;
};

#endif // TallinnAnalysis_MLTools_TMVAInterface_h
