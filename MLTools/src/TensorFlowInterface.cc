#include "TallinnAnalysis/MLTools/interface/TensorFlowInterface.h"

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wshadow"
#pragma GCC diagnostic ignored "-Wswitch-enum"
#include "PhysicsTools/TensorFlow/interface/TensorFlow.h"
#pragma GCC diagnostic pop

#include "TallinnNtupleProducer/CommonTools/interface/cmsException.h" // cmsException()
#include "TallinnNtupleProducer/CommonTools/interface/get_fullpath.h" // get_fullpath()

#include "TallinnAnalysis/MLTools/interface/MVAInputVarTransformer.h" // MVAInputVarTransformer

#include <boost/algorithm/string.hpp>                                 // boost::contains()

TensorFlowInterface::TensorFlowInterface(const std::string & mvaFileName_odd,
                                         const std::vector<std::string> & mvaInputVariables,
                                         const std::vector<std::string> & classes,
                                         const std::string & mvaFileName_even,
                                         const std::string & fitFunctionFileName)
  : classes_(classes)
  , is_multiclass_(classes.size() > 0)
  , mvaFileName_odd_(mvaFileName_odd)
  , graphDef_odd_(nullptr)
  , session_odd_(nullptr)
  , n_input_layer_odd_(0)
  , n_output_layer_odd_(0)
  , mvaFileName_even_(mvaFileName_even)
  , graphDef_even_(nullptr)
  , session_even_(nullptr)
  , n_input_layer_even_(0)
  , n_output_layer_even_(0)
  , mvaInputVariables_(mvaInputVariables)
  , fitFunctionFileName_(fitFunctionFileName)
  , mvaInputTransformation_(nullptr)
  , isDEBUG_(false)
{
  if(! fitFunctionFileName_.empty())
  {
    // initialize the new map and extracts the TF1s
    mvaInputTransformation_ = new MVAInputVarTransformer(mvaInputVariables_, get_fullpath(fitFunctionFileName_));
  }

  // load the model
  tensorflow::SessionOptions options;
  tensorflow::setThreading(options, 1, "no_threads");

  graphDef_odd_ = tensorflow::loadGraphDef(get_fullpath(mvaFileName_odd_));
  session_odd_ = tensorflow::createSession(graphDef_odd_);

  if(! mvaFileName_even_.empty())
  {
    std::cout << "Loaded Odd Evt. No. file: " << mvaFileName_odd_ << '\n';
    graphDef_even_ = tensorflow::loadGraphDef(get_fullpath(mvaFileName_even_));
    session_even_ = tensorflow::createSession(graphDef_even_);
    std::cout << "Loaded Even Evt. No. file: " << mvaFileName_even_ << '\n';
  }
  else
  {
    std::cout << "Loaded file: " << mvaFileName_odd_ << '\n';
  }

  // getting elements to evaluate -- the number of the input/output layer deppends of how the model was exported
  int shape_variables_odd = 0;
  for(int idx_node = 0; idx_node < graphDef_odd_->node_size(); idx_node++)
  {
    input_layer_name_odd_  = graphDef_odd_->node(idx_node).name();
    const bool is_input = boost::contains(input_layer_name_odd_, "_input");
    if(is_input)
    {
      n_input_layer_odd_ = idx_node;
      if(isDEBUG_)
      {
        std::cout << "read input layer "<< input_layer_name_odd_ << " " << n_input_layer_odd_ << '\n';
      }

      const auto & shape_odd = graphDef_odd_->node(idx_node).attr().at("shape").shape();
      if(isDEBUG_)
      {
        std::cout << "read input layer shape  " << shape_odd.dim_size() << '\n';
      }

      shape_variables_odd = static_cast<int>(shape_odd.dim(1).size());
      if(isDEBUG_)
      {
        std::cout << "read input layer shape length "<< shape_variables_odd << '\n';
      }
      break;
    }
  }

  if(! mvaFileName_even_.empty())
  {
    int shape_variables_even = 0;
    for(int idx_node = 0; idx_node < graphDef_even_->node_size(); idx_node++)
    {
      input_layer_name_even_  = graphDef_even_->node(idx_node).name();
      const bool is_input = boost::contains(input_layer_name_even_, "_input");
      if(is_input)
      {
        n_input_layer_even_ = idx_node;
        if(isDEBUG_)
        {
          std::cout << "read input layer "<< input_layer_name_even_ << " " << n_input_layer_even_ << '\n';
        }

        const auto & shape_even = graphDef_even_->node(idx_node).attr().at("shape").shape();
        if(isDEBUG_)
        {
          std::cout << "read input layer shape  " << shape_even.dim_size() << '\n';
        }

        shape_variables_even = static_cast<int>(shape_even.dim(1).size());
        if(isDEBUG_)
        {
          std::cout << "read input layer shape length "<< shape_variables_even << '\n';
        }
        break;
      }
    }
    if(shape_variables_even != static_cast<int>(mvaInputVariables_.size()))
    {
      throw cmsException(this)
        << "number of variables declared ("<< mvaInputVariables_.size() << ") does not match the expected inputs for "
           "the given version (" << shape_variables_even << ')'
      ;
    }
  }

  if(! mvaFileName_even_.empty() && shape_variables_odd != static_cast<int>(mvaInputVariables_.size()))
  {
    throw cmsException(this)
      << "number of variables declared ("<< mvaInputVariables_.size() << ") does not match the expected inputs for "
         "the given version (" << shape_variables_odd << ')'
    ;
  }

  //  int shape_classes = 0;
  std::string last_layer =  (classes.size() > 0) ? "/Softmax" : "/Sigmoid";
  for(int idx_node = 0; idx_node < graphDef_odd_->node_size(); idx_node++)
  {
    output_layer_name_odd_ = graphDef_odd_->node(idx_node).name();

    const bool is_output = boost::contains(output_layer_name_odd_, last_layer);
    if(is_output)
    {
      n_output_layer_odd_ = idx_node;
      if(isDEBUG_)
      {
        std::cout << "read output layer "<< output_layer_name_odd_ << " " << idx_node << '\n';
      }
      break;
    }
  }

  if(! mvaFileName_even_.empty())
  {
    for(int idx_node = 0; idx_node < graphDef_even_->node_size(); idx_node++)
    {
      output_layer_name_even_ = graphDef_even_->node(idx_node).name();
      const bool is_output = boost::contains(output_layer_name_even_, last_layer);
      if(is_output)
      {
        n_output_layer_even_ = idx_node;
        if(isDEBUG_)
        {
          std::cout << "read output layer "<< output_layer_name_even_ << " " << idx_node << '\n';
        }
        break;
      }
    }
  }
}

TensorFlowInterface::~TensorFlowInterface()
{
  tensorflow::closeSession(session_odd_);
  delete session_odd_;
  delete graphDef_odd_;
  tensorflow::closeSession(session_even_);
  delete session_even_;
  delete graphDef_even_;
}

bool
TensorFlowInterface::is_multiclass() const
{
  return is_multiclass_;
}

double
TensorFlowInterface::get_mvaOutput(const std::map<std::string, double> & mvaInputs, ULong64_t event_number) const
{
  if ( is_multiclass_ )
    throw cmsException(this, __func__, __LINE__)
      << "The function 'get_mvaOutput' is reserved for the non-multiclass case !!";
  evaluate(mvaInputs, event_number);
  return mvaOutput_;
}

std::map<std::string, double>
TensorFlowInterface::get_mvaOutput_multiclass(const std::map<std::string, double> & mvaInputs, ULong64_t event_number) const
{
  if ( !is_multiclass_ )
    throw cmsException(this, __func__, __LINE__)
      << "The function 'get_mvaOutput_multiclass' is reserved for the multiclass case !!";
  evaluate(mvaInputs, event_number);
  return mvaOutput_multiclass_;
}

const std::vector<std::string> &
TensorFlowInterface::get_mvaInputVariables() const
{
  return mvaInputVariables_;
}

namespace
{
  void copy_mvaInputs(const std::map<std::string, double> & mvaInputs, 
                      const std::vector<std::string> & mvaInputVariables, 
                      tensorflow::Tensor & inputTensor,
                      bool isDEBUG)
  {
    // the order of input variables should be the same as during the training
    size_t numInputs = mvaInputVariables.size();
    if ( isDEBUG )
    {
      std::cout << "mvaInputs:" << std::endl;
      for ( size_t idxInput = 0; idxInput < numInputs; ++idxInput )
      {
        std::cout << mvaInputVariables[idxInput] << " = " << mvaInputs.at(mvaInputVariables.at(idxInput)) << std::endl;
      }
    }
    for ( size_t idxInput = 0; idxInput < numInputs; ++idxInput )
    {
      if ( mvaInputs.count(mvaInputVariables[idxInput]) )
      {
        inputTensor.matrix<float>()(0, idxInput) = static_cast<float>(mvaInputs.at(mvaInputVariables.at(idxInput)));
      }
      else
      {
        throw cmsException(__func__, __LINE__)
          << "Missing value for MVA input variable = '" << mvaInputVariables[idxInput] << "' !!";
      }
    }
  }
}

void
TensorFlowInterface::evaluate(const std::map<std::string, double> & mvaInputs, ULong64_t event_number) const
{
  const int nofInputs = mvaInputVariables_.size();

  tensorflow::Tensor inputTensor(tensorflow::DT_FLOAT, { 1, nofInputs});

  if(fitFunctionFileName_ != "")
  {
    // transform MVA input variables (to remove e.g. dependency on gen_mHH)
    std::map<std::string, double> mvaInputs_transformed = mvaInputTransformation_->TransformMVAInputVars(mvaInputs);
    copy_mvaInputs(mvaInputs_transformed, mvaInputVariables_, inputTensor, isDEBUG_);
  }
  else
  {
    copy_mvaInputs(mvaInputs, mvaInputVariables_, inputTensor, isDEBUG_);
  }

  tensorflow::Session * session = nullptr;
  tensorflow::GraphDef * graphDef = nullptr;
  int n_input_layer = 0;
  int n_output_layer = 0;
  if ( session_odd_ && graphDef_odd_ && event_number % 2 )
  {
    session = session_odd_;
    graphDef = graphDef_odd_;
    n_input_layer = n_input_layer_odd_;
    n_output_layer = n_output_layer_odd_;
  }
  else
  {
    session = session_even_;
    graphDef = graphDef_even_;
    n_input_layer = n_input_layer_even_;
    n_output_layer = n_output_layer_even_;
  }  

  if(isDEBUG_)
  {
    const int node_count = graphDef->node_size();
    for (int idx_node = 0; idx_node < node_count; ++idx_node)
    {
      const auto node = graphDef->node(idx_node);
      std::cout << "Names : " << node.name() << '\n';
    }
  }

  std::vector<tensorflow::Tensor> outputTensor;
  {
    std::cout
          << "start run " << event_number << " " << graphDef->node(n_input_layer).name()
          << ' '          << graphDef->node(n_output_layer).name()
          << '\n';
  }
  tensorflow::run(
    session,
    { { graphDef->node(n_input_layer).name(), inputTensor } }, 
    { graphDef->node(n_output_layer).name() },
    &outputTensor
  );

  // store the output
  if ( is_multiclass_ )
  {
    size_t numClasses = classes_.size();
    for ( size_t idxClass = 0; idxClass < numClasses; ++idxClass )
    {
      mvaOutput_multiclass_[classes_.at(idxClass)] = outputTensor[0].matrix<float>()(0, idxClass);
    }
    if ( isDEBUG_ )
    {
      std::cout << "mvaOutputs:" << std::endl;
      for ( size_t idxClass = 0; idxClass < numClasses; ++idxClass ) 
      {
        std::cout << classes_.at(idxClass) << " = " <<  mvaOutput_multiclass_[classes_.at(idxClass)] << std::endl;
      }
    }
  }
  else
  {
    mvaOutput_ = outputTensor[0].matrix<float>()(0, 0);
    if ( isDEBUG_ )
    {
      std::cout << "mvaOutput = " << mvaOutput_ << std::endl;
    }
  }
}
