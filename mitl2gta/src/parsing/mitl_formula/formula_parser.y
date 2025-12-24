%skeleton "lalr1.cc" // -*- C++ -*-
%require "3.8.1"
%header

%define api.token.raw

%define api.token.constructor
%define api.value.type variant
%define parse.assert

%define api.namespace {mitl2gta::parsing::mitl}
%define api.parser.class {formula_parser_t}

%define api.prefix {mfyy}

%code requires {
  #include <iostream>
  # include <string>

  # include "mitl2gta/interval/bound.hpp"
  # include "mitl2gta/interval/interval.hpp"
  # include "mitl2gta/mitl_formula/parsed_formula.hpp"
}

%param { std::shared_ptr<mitl2gta::mitl::formula_t> &formula_root }

%locations

%code{
  mitl2gta::parsing::mitl::formula_parser_t::symbol_type mfyylex
   (std::shared_ptr<mitl2gta::mitl::formula_t> &);
}

%define parse.trace
%define parse.error detailed
%define parse.lac full

%define api.symbol.prefix {TOK_}

%token
AND  					  "&&"
OR					    "||"
NOT					    "!"
LPARAN		      "("
RPARAN		      ")"
SQLPARAN	      "["
SQRPARAN	      "]"
COMMA				    ","
TRUE					  "True"
FALSE					  "False"
NEXT				    "X"
YESTERDAY       "Y"
GLOBALLY			  "G"
FINALLY				  "F"
UNTIL   			  "U"
SINCE           "S"
INF					    "Inf"
;

%right    "||"
%right    "&&"
%right    "X"
%right    "Y"
%right    "G"
%right    "F"
%right    "U"
%right    "S"
%nonassoc "!"

%token <std::string> IDENTIFIER "identifier"
%token <int> NUMBER "number"

%nterm <std::shared_ptr<mitl2gta::mitl::formula_t>> formula

%nterm <std::shared_ptr<mitl2gta::nonempty_interval_t>> interval
%nterm <mitl2gta::interval_bound_t> lower_bound
%nterm <mitl2gta::interval_bound_t> upper_bound

%%
%start program;

program: formula YYEOF { formula_root = $1; };

formula: 
  "identifier"                  { $$ = std::make_shared<mitl2gta::mitl::atomic_proposition_t>($1); }
| "True"                        { $$ = std::make_shared<mitl2gta::mitl::atomic_constant_t>(mitl2gta::mitl::constant_value_t::BOOL_TRUE); }
| "False"                       { $$ = std::make_shared<mitl2gta::mitl::atomic_constant_t>(mitl2gta::mitl::constant_value_t::BOOL_FALSE); }
| "(" formula ")"               { $$ = $2; }
| "!" formula                   { $$ = std::make_shared<mitl2gta::mitl::negation_t>($2); }
| "X" formula                   { $$ = std::make_shared<mitl2gta::mitl::untimed_next_t>($2); }
| "X" interval formula          { $$ = std::make_shared<mitl2gta::mitl::timed_next_t>(*$2, $3); }
| "Y" formula                   { $$ = std::make_shared<mitl2gta::mitl::untimed_yesterday_t>($2); }
| "Y" interval formula          { $$ = std::make_shared<mitl2gta::mitl::timed_yesterday_t>(*$2, $3); }
| "G" formula                   { $$ = std::make_shared<mitl2gta::mitl::untimed_globally_t>($2); }
| "G" interval formula          { $$ = std::make_shared<mitl2gta::mitl::timed_globally_t>(*$2, $3); }
| "F" formula                   { $$ = std::make_shared<mitl2gta::mitl::untimed_finally_t>($2); }
| "F" interval formula          { $$ = std::make_shared<mitl2gta::mitl::timed_finally_t>(*$2, $3); }
| formula "U" formula           { $$ = std::make_shared<mitl2gta::mitl::untimed_until_t>($1, $3); }
| formula "U" interval formula  { $$ = std::make_shared<mitl2gta::mitl::timed_until_t>(*$3, $1, $4); }
| formula "S" formula           { $$ = std::make_shared<mitl2gta::mitl::untimed_since_t>($1, $3); }
| formula "S" interval formula  { $$ = std::make_shared<mitl2gta::mitl::timed_since_t>(*$3, $1, $4); }
| formula "||" formula          { $$ = std::make_shared<mitl2gta::mitl::or_t>($1, $3); }
| formula "&&" formula          { $$ = std::make_shared<mitl2gta::mitl::and_t>($1, $3); }
;

lower_bound: 
  "[" "number" { $$ = mitl2gta::finite_bound($2, mitl2gta::bound_type_t::CLOSED_BOUND); }
| "(" "number" { $$ = mitl2gta::finite_bound($2, mitl2gta::bound_type_t::OPEN_BOUND); }
;

upper_bound: 
  "number" "]"  { $$ = mitl2gta::finite_bound($1, mitl2gta::bound_type_t::CLOSED_BOUND); }
| "number" ")"  { $$ = mitl2gta::finite_bound($1, mitl2gta::bound_type_t::OPEN_BOUND); }
| "Inf" ")"     { $$ = mitl2gta::INF_BOUND;  }
;

interval:
  lower_bound "," upper_bound {$$ = std::make_shared<mitl2gta::nonempty_interval_t>($1, $3); }
;

%%

void
mitl2gta::parsing::mitl::formula_parser_t::error (location_type const &l, std::string const &m)
{
  std::cerr << l << ": " << m << std::endl;
}
