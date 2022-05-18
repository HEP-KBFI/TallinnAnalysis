#ifndef TallinnAnalysis_Extractors_MVAVarExtractorBase_h
#define TallinnAnalysis_Extractors_MVAVarExtractorBase_h

#include "FWCore/ParameterSet/interface/ParameterSet.h"            // edm::ParameterSet

#include "TallinnAnalysis/Extractors/interface/BranchVars.h"       // BranchVarULong64_t
#include "TallinnAnalysis/Extractors/interface/VarExtractorBase.h" // VarExtractorBase

#include <Rtypes.h>                                                // ULong64_t

#include <map>                                                     // std::map

// forward declarations
class TTree;

class MVAVarExtractorBase : public VarExtractorBase
{
 public:
  MVAVarExtractorBase(const edm::ParameterSet & cfg);
  virtual ~MVAVarExtractorBase();

  virtual
  bool is_multiclass() = 0;

  /**
   * @brief Return MVA output, to be filled into histogram.
   *        The functions of name "get_value" are for MVAs trained to distinguish between signal and background,
   *        while the functions of name "get_value_multiclass" are for MVAs trained 
   *        to additionally distinguish between different types of signal and/or between different types of background.
   *        The functions without arguments need to be used in case of non-parametrized MVA trainings,
   *        while the functions with arguments  need to be used in case of parametrized MVA trainings.
   *        Examples for the latter are { 'SM', 'BM1',...,'BM12' } in case of non-resonant HH production or 'gen_mHH' in case of resonant HH production.
   *        In case of non-resonant HH production, the coupling scenarios { 'SM', 'BM1',...,'BM12' } 
   *        need to be passed as one-hot-encoded inputs, i.e. the names of the coupling scenarios are the keys in the std::map
   *        and in each std::map, the value is 1 for exactly one key and 0 for all other keys,
   *        and different coupling scenarios are passed as separate entries in the std::vector or std::maps.
   *        In case of resonant HH production, the mass 'gen_mHH' of the resonance is the only key in the std::map,
   *        and different mass values are passed as separate entries in the std::vector or std::maps.
   */
  virtual
  double
  get_value() const;

  virtual
  double
  get_value(const std::map<std::string, double> & param) const;

  virtual
  std::vector<double>
  get_value(const std::vector<std::map<std::string, double>> & params) const;

  virtual
  std::map<std::string, double>
  get_value_multiclass() const;

  virtual
  std::map<std::string, double>
  get_value_multiclass(const std::map<std::string, double> & param) const;

  virtual
  std::vector<std::map<std::string, double>>
  get_value_multiclass(const std::vector<std::map<std::string, double>> & params) const;

 protected:
  void
  read_mvaInputs() const;

  virtual
  double
  get_valueImp(const std::map<std::string, double> & mvaInputs) const = 0;

  virtual
  std::map<std::string, double>
  get_value_multiclassImp(const std::map<std::string, double> & mvaInputs) const = 0;

  std::vector<std::string> branchNames_mvaInputVariables_;
  std::vector<std::shared_ptr<BranchVarBase>> branchVars_mvaInputVariables_;

  std::string branchName_event_;
  std::shared_ptr<BranchVarBase> branchVar_event_;
  mutable ULong64_t event_number_;

  mutable std::map<std::string, double> mvaInputs_; // key = branchName

  std::string mvaFileName_odd_;
  std::string mvaFileName_even_;

  std::string fitFunctionFileName_;
};

#endif // TallinnAnalysis_Extractors_MVAVarExtractorBase_h
