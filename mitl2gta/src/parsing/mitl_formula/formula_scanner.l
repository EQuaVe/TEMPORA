%{ /* -*- C++ -*- */
#include <exception>
#include <limits>
#include <memory>
# include <string>

# include "mitl2gta/parsing/declarations.hpp"
# include "formula_parser.hpp"

#define YY_DECL \
  mitl2gta::parsing::mitl::formula_parser_t::symbol_type mfyylex \
  (std::shared_ptr<mitl2gta::mitl::formula_t> &)

%}

%option noyywrap nounput noinput batch 
%option prefix="mfyy"

%{
  mitl2gta::parsing::mitl::formula_parser_t::symbol_type
  make_NUMBER (std::string const &s, mitl2gta::parsing::mitl::formula_parser_t::location_type const &loc);
%}

id    [a-z][a-zA-Z_0-9]*
num   [0-9]+
blank [ \t\r]

%{
  // Code run each time a pattern is matched.
  # define YY_USER_ACTION  loc.columns (mfyyleng);
%}

%%
%{
  static mitl2gta::parsing::mitl::location loc;
  loc.step ();

  using namespace mitl2gta::parsing::mitl;
%}

{blank}+   loc.step();
\n+        loc.lines(mfyyleng); loc.step();
\/\/[^\n]* {};

"&&"    return formula_parser_t::make_AND(loc);  		 
"||"    return formula_parser_t::make_OR(loc);					    
"!"     return formula_parser_t::make_NOT(loc);					    
"("     return formula_parser_t::make_LPARAN(loc);		      
")"     return formula_parser_t::make_RPARAN(loc);		      
"["     return formula_parser_t::make_SQLPARAN(loc);	      
"]"     return formula_parser_t::make_SQRPARAN(loc);	      
","     return formula_parser_t::make_COMMA(loc);				    
"True"  return formula_parser_t::make_TRUE(loc);					  
"False" return formula_parser_t::make_FALSE(loc);					  
"X"     return formula_parser_t::make_NEXT(loc);				    
"Y"     return formula_parser_t::make_YESTERDAY(loc);       
"G"     return formula_parser_t::make_GLOBALLY(loc);			  
"F"     return formula_parser_t::make_FINALLY(loc);				  
"U"     return formula_parser_t::make_UNTIL(loc);   			  
"S"     return formula_parser_t::make_SINCE(loc);           
"Inf"   return formula_parser_t::make_INF(loc);					    
{num}   return make_NUMBER (mfyytext, loc);
{id}    return formula_parser_t::make_IDENTIFIER (mfyytext, loc);
.       throw formula_parser_t::syntax_error(loc, "invalid character: " + std::string(mfyytext));

<<EOF>> return mitl2gta::parsing::mitl::formula_parser_t::make_MFYYEOF (loc);
%%

mitl2gta::parsing::mitl::formula_parser_t::symbol_type
make_NUMBER (std::string const &s, mitl2gta::parsing::mitl::formula_parser_t::location_type const &loc)
{
  errno = 0;
  long n = strtol (s.c_str(), NULL, 10);

  if (! (std::numeric_limits<int>::min() <= n && n <= std::numeric_limits<int>::max() && errno != ERANGE)){
    throw mitl2gta::parsing::mitl::formula_parser_t::syntax_error (loc, "integer is out of range: " + s);
  }

  return mitl2gta::parsing::mitl::formula_parser_t::make_NUMBER((int) n, loc);
}

namespace mitl2gta {

namespace mitl {

std::shared_ptr<mitl2gta::mitl::formula_t const>
parse_formula(std::string const &formula) {
  std::shared_ptr<mitl2gta::mitl::formula_t> formula_root;     

  YY_BUFFER_STATE buffer = mfyy_scan_string(formula.c_str());

  try {
    mitl2gta::parsing::mitl::formula_parser_t parser(formula_root);
    parser.parse();
  } catch (std::exception const & e) {
    std::cerr << e.what() << std::endl;
    formula_root = nullptr;
  }

  mfyy_delete_buffer(buffer);

  return formula_root;
}

} // namespace mitl

} // namespace mitl2gta
