
#include <boost/spirit/include/qi.hpp>
#include <boost/phoenix.hpp>

namespace qi = boost::spirit::qi;
namespace phx = boost::phoenix;

#include <iomanip>  // std::quoted
#include <iostream> // std::cout
#include <string>   // std::string
#include <vector>   // std::vector

struct my_grammar : qi::grammar<std::string::const_iterator, std::vector<std::string>(), qi::space_type>
{
  my_grammar() : my_grammar::base_type(disjunction_)
  {
    disjunction_ = conjunction_ >> *(logical_or_ >> conjunction_);
    conjunction_ = conjunct_ >> *(logical_and_ >> conjunct_);
    further_conjunction_ = logical_or_ >> conjunction_;
    logical_or_ = qi::string("||") | qi::string("|");
    conjunct_ = condition1_ | *(not_) >> (condition2_ | group_);
    further_conjunct_ = logical_and_ >> conjunct_;
    logical_and_ = qi::string("&&") | qi::string("&");
    condition1_ = variable_ >> *(mult_operator_ >> variable_) >> *(cmp_operator_ >> value_);
    mult_operator_ = qi::string("*");
    not_ = qi::string("!");
    condition2_ = variable_;
    variable_ = +(qi::char_("a-zA-Z0-9_"));
    cmp_operator_ = qi::string(">=") | qi::string("==") | qi::string("<=") | qi::string(">") | qi::string("=") | qi::string("<");
    value_ = +(qi::char_("0-9.e+-"));
    left_bracket_ = qi::string("(");
    right_bracket_ = qi::string(")");
    group_ = left_bracket_ >> disjunction_ >> right_bracket_;
  }

  qi::rule<std::string::const_iterator, std::vector<std::string>(), qi::space_type> disjunction_;
  qi::rule<std::string::const_iterator, std::vector<std::string>(), qi::space_type> conjunction_;
  qi::rule<std::string::const_iterator, std::vector<std::string>(), qi::space_type> further_conjunction_;
  qi::rule<std::string::const_iterator, std::string(), qi::space_type> logical_or_;
  qi::rule<std::string::const_iterator, std::vector<std::string>(), qi::space_type> conjunct_;
  qi::rule<std::string::const_iterator, std::vector<std::string>(), qi::space_type> further_conjunct_;
  qi::rule<std::string::const_iterator, std::string(), qi::space_type> logical_and_;
  qi::rule<std::string::const_iterator, std::vector<std::string>(), qi::space_type> condition1_;
  qi::rule<std::string::const_iterator, std::string(), qi::space_type> mult_operator_;
  qi::rule<std::string::const_iterator, std::string(), qi::space_type> not_;
  qi::rule<std::string::const_iterator, std::vector<std::string>(), qi::space_type> condition2_;
  qi::rule<std::string::const_iterator, std::string(), qi::space_type> variable_;
  qi::rule<std::string::const_iterator, std::string(), qi::space_type> cmp_operator_;
  qi::rule<std::string::const_iterator, std::string(), qi::space_type> value_;
  qi::rule<std::string::const_iterator, std::string(), qi::space_type> left_bracket_;
  qi::rule<std::string::const_iterator, std::string(), qi::space_type> right_bracket_;
  qi::rule<std::string::const_iterator, std::vector<std::string>(), qi::space_type> group_;
};

template <typename Parser, typename Skipper, typename ... Args>
void PhraseParseOrDie(const std::string& input, const Parser& p, const Skipper& s, Args&& ... args)
{
  std::string::const_iterator begin = input.begin(), end = input.end();
  boost::spirit::qi::phrase_parse(begin, end, p, s, std::forward<Args>(args) ...);
  if ( begin != end ) 
  {
    std::cout << "Failed to parse expression '" << std::string(begin, end) << "' !!" << std::endl;
    throw std::runtime_error("Parse error");
  }
}

int main(int argc, char* argv[])
{
  std::string test_string = "nlep == 2 && ntau == 1 && passesTrigger && lep1_pt > 25. && lep1_isTight && lep1_tightCharge >= 2 && !(lep1_isFake || lep1_isFlip) && lep2_pt > 15. && lep2_isTight && lep2_tightCharge >= 2 && !(lep2_isFake || lep2_isFlip) && lep1_charge*lep2_charge > 0 && tau1_pt > 20. && tau1_isTight && !tau1_isFake && (njetAK4 >= 2 || njetAK8Wjj >= 1) && njetAK4bL <= 1 && njetAK4bM == 0 && (lep1_pdgId == 13 || lep2_pdgId == 13 || met_LD > 30.) && passesLowMassLeptonPairVeto && passesZbosonVeto && passesHtoZZto4lVeto && passesMEtFilters && ntightlep == 2 && ntighttau == 1";

  std::vector<std::string> result;

  PhraseParseOrDie(test_string, my_grammar(), qi::space, result);

  std::cout << "result of parsing expression '" << test_string << "':" << std::endl;
  for ( size_t idx = 0; idx < result.size(); ++idx )
  {
    std::cout << "#" << idx << ": " << result.at(idx) << std::endl;
  }
}
