#ifndef TallinnAnalysis_MLTools_TensorFlowInterface_h
#define TallinnAnalysis_MLTools_TensorFlowInterface_h

#include <Rtypes.h> // Float_t, ULong64_t

#include <map>    // std::map
#include <string> // std::string
#include <vector> // std::vector

// forward declarations
namespace tensorflow
{
  class Reader;
  class GraphDef;
  class Session;
  class Status;
}
class MVAInputVarTransformer;

class TensorFlowInterface
{
public:
  TensorFlowInterface(const std::string & mvaFileName_odd,
                      const std::vector<std::string> & mvaInputVariables,
                      const std::vector<std::string> & classes,
                      const std::string & mvaFileName_even = "",
                      const std::string & fitFunctionFileName = "");
  ~TensorFlowInterface();

  bool is_multiclass() const;

  /**
   * @brief Calculates ANN output. Use the function get_mvaOutput for the non-multiclass ("regular") case 
   *        and the function get_mvaOutput_multiclass for the multiclass case.
   * @params mvaInputs: Values of ANN input variables (stored in std::map with key = ANN input variable name)
   *         event_number: Event number for the current event (used to switch between ANNs for odd vs even events, to avoid train vs test bias)
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
  tensorflow::GraphDef * graphDef_odd_;
  tensorflow::Session * session_odd_;
  std::string input_layer_name_odd_;
  std::string output_layer_name_odd_;
  int n_input_layer_odd_;
  int n_output_layer_odd_;

  std::string mvaFileName_even_;
  tensorflow::GraphDef * graphDef_even_;
  tensorflow::Session * session_even_;
  std::string input_layer_name_even_;
  std::string output_layer_name_even_;
  int n_input_layer_even_;
  int n_output_layer_even_;

  std::vector<std::string> mvaInputVariables_;

  std::string fitFunctionFileName_;
  MVAInputVarTransformer * mvaInputTransformation_;

  mutable std::map<std::string, double> mvaOutput_multiclass_;
  mutable double mvaOutput_;

  bool isDEBUG_;
};

#endif // TallinnAnalysis_MLTools_TensorFlowInterface_h
