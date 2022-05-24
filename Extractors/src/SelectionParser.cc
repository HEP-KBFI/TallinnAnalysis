#include "TallinnAnalysis/Extractors/interface/SelectionParser.h"

#include "TallinnNtupleProducer/CommonTools/interface/cmsException.h" // cmsException()

#include <boost/spirit/include/qi.hpp>
#include <boost/phoenix.hpp>

namespace qi = boost::spirit::qi;
namespace phx = boost::phoenix;

#include <iostream> // std::cout

struct MyGrammar : qi::grammar<std::string::const_iterator, std::vector<std::string>(), qi::space_type>
{
  MyGrammar() : MyGrammar::base_type(disjunction_)
  {
    disjunction_ = conjunction_ >> *(logical_or_ >> conjunction_);
    logical_or_ = qi::string("||") | qi::string("|");
    conjunction_ = conjunct_ >> *(logical_and_ >> conjunct_);
    logical_and_ = qi::string("&&") | qi::string("&");
    conjunct_ = condition1_ | *(not_) >> (condition2_ | group_);
    condition1_ = variable_ >> *(mult_operator_ >> variable_) >> *(cmp_operator_ >> value_);
    variable_ = +(qi::char_("a-zA-Z0-9_"));
    mult_operator_ = qi::string("*");
    cmp_operator_ = qi::string(">=") | qi::string("==") | qi::string("<=") | qi::string(">") | qi::string("=") | qi::string("<");
    value_ = +(qi::char_("0-9.e+-"));
    not_ = qi::string("!");
    condition2_ = variable_;
    group_ = left_bracket_ >> disjunction_ >> right_bracket_;
    left_bracket_ = qi::string("(");
    right_bracket_ = qi::string(")");
  }

  qi::rule<std::string::const_iterator, std::vector<std::string>(), qi::space_type> disjunction_;
  qi::rule<std::string::const_iterator, std::string(), qi::space_type> logical_or_;
  qi::rule<std::string::const_iterator, std::vector<std::string>(), qi::space_type> conjunction_;
  qi::rule<std::string::const_iterator, std::string(), qi::space_type> logical_and_;
  qi::rule<std::string::const_iterator, std::vector<std::string>(), qi::space_type> conjunct_;
  qi::rule<std::string::const_iterator, std::vector<std::string>(), qi::space_type> condition1_;
  qi::rule<std::string::const_iterator, std::string(), qi::space_type> variable_;
  qi::rule<std::string::const_iterator, std::string(), qi::space_type> mult_operator_;
  qi::rule<std::string::const_iterator, std::string(), qi::space_type> cmp_operator_;
  qi::rule<std::string::const_iterator, std::string(), qi::space_type> value_;
  qi::rule<std::string::const_iterator, std::string(), qi::space_type> not_;
  qi::rule<std::string::const_iterator, std::vector<std::string>(), qi::space_type> condition2_;
  qi::rule<std::string::const_iterator, std::vector<std::string>(), qi::space_type> group_;
  qi::rule<std::string::const_iterator, std::string(), qi::space_type> left_bracket_;
  qi::rule<std::string::const_iterator, std::string(), qi::space_type> right_bracket_;
};

SelectionParser::SelectionParser()
{
  grammar_ = new MyGrammar();
}

SelectionParser::~SelectionParser()
{
  delete grammar_;
}

template <typename Parser, typename Skipper, typename ... Args>
void PhraseParseOrDie(const std::string& input, const Parser& p, const Skipper& s, Args&& ... args)
{
  std::string::const_iterator begin = input.begin(), end = input.end();
  boost::spirit::qi::phrase_parse(begin, end, p, s, std::forward<Args>(args) ...);
  if ( begin != end ) 
  {
    throw cmsException(__func__, __LINE__) 
      << "Failed to parse expression '" << std::string(begin, end) << "' !!";
  }
}

std::vector<std::string>
SelectionParser::parse(const std::string & selection) const
{
  std::vector<std::string> elements;
  PhraseParseOrDie(selection, *grammar_, qi::space, elements);
  //std::cout << "result of parsing expression '" << selection << "':" << std::endl;
  //for ( size_t idx = 0; idx < elements.size(); ++idx )
  //{
  //  std::cout << "#" << idx << ": " << elements.at(idx) << std::endl;
  //}
  return elements;
}

