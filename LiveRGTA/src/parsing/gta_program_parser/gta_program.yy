/*
 * This file is a part of the TChecker project.
 *
 * See files AUTHORS and LICENSE for copyright details.
 *
 */

%skeleton "lalr1.cc" // c++
%require "3.0.4"

%defines
%define parser_class_name {parser_t}
%define api.namespace {tchecker::parsing::gta_program}
%define api.prefix {gpyy}
%define api.token.constructor
%define api.value.type variant
%define parse.assert true
%define parse.trace true
%define parse.error verbose


%code requires {
  #include <exception>
  #include <iostream>
  #include <limits>
  #include <memory>
  #include <string>

  #include "tchecker/gta_program/guard.hh"
  #include "tchecker/gta_program/action.hh"
  #include "tchecker/gta_program/gta_program.hh"
  #include "tchecker/parsing/parsing.hh"
  #include "tchecker/utils/log.hh"
  #include "tchecker/basictypes.hh"
}


%param { std::string const & gta_program_context }
%param { std::shared_ptr<tchecker::gta::gta_program_t> & gta_program }
%locations


%code {
  // Declare the lexer for the parser's sake.
  tchecker::parsing::gta_program::parser_t::symbol_type gpyylex
  (std::string const & gta_program_context, 
   std::shared_ptr<tchecker::gta::gta_program_t> &);
  
  
  // Global variables
  static unsigned int old_error_count;
  
  // Fake lterm used is case of syntax error to allow parsing of the
  // entire gta program
  class fake_guard_lterm_t final : public tchecker::gta::guard_lterm_t {
  public:
    virtual ~fake_guard_lterm_t() = default;
    virtual std::ostream & output(std::ostream & os) const override { return os; }
    virtual fake_guard_lterm_t * clone() const override { return new fake_guard_lterm_t(); }
    virtual void visit(tchecker::gta::guard_visitor_t & v) const override {}
  };

  auto fake_guard_lterm = std::make_shared<fake_guard_lterm_t>();
  
  // Fake rterm used is case of syntax error to allow parsing of the
  // entire gta program
  class fake_guard_rterm_t final : public tchecker::gta::guard_rterm_t {
  public:
    virtual ~fake_guard_rterm_t() = default;
    virtual std::ostream & output(std::ostream & os) const override { return os; }
    virtual fake_guard_rterm_t * clone() const override { return new fake_guard_rterm_t(); }
    virtual void visit(tchecker::gta::guard_visitor_t & v) const override {}
  };

  auto fake_guard_rterm = std::make_shared<fake_guard_rterm_t>();

  // Fake guard used is case of syntax error to allow parsing of the
  // entire gta program
  class fake_guard_t final : public tchecker::gta::guard_t {
  public:
    fake_guard_t() : tchecker::gta::guard_t(fake_guard_lterm, tchecker::gta::GUARD_EQ, fake_guard_rterm)
    {}
    virtual ~fake_guard_t() = default;
    virtual std::ostream & output(std::ostream & os) const override { return os; }
    virtual fake_guard_t * clone() const override { return new fake_guard_t(); }
    virtual void visit(tchecker::gta::guard_visitor_t & v) const override {}
  };

  auto fake_guard = std::make_shared<fake_guard_t>();

  // Fake action used is case of syntax error to allow parsing of the
  // entire gta program
  class fake_action_t final : public tchecker::gta::action_t {
  public:
    fake_action_t() : tchecker::gta::action_t()
    {}
    virtual ~fake_action_t() = default;
    virtual std::ostream & output(std::ostream & os) const override { return os; }
    virtual fake_action_t* clone() const override { return new fake_action_t(); }
    virtual void visit(tchecker::gta::action_visitor_t & v) const override {}
  };

  auto fake_action = std::make_shared<fake_action_t>();
}


%initial-action {
  old_error_count = tchecker::log_error_count();
};


%token                TOK_MINUS             "-"
%token                TOK_ASSIGN            "="
%token                TOK_LPAR              "("
%token                TOK_RPAR              ")"
%token                TOK_LBRACKET          "["
%token                TOK_RBRACKET          "]"
%token                TOK_SEMICOLON         ";"
%token                TOK_COMMA             ","
%token                TOK_LAND              "&&"
%token                TOK_EQ                "=="
%token                TOK_LE                "<="
%token                TOK_GE                ">="
%token                TOK_LT                "<"
%token                TOK_GT                ">"
%token                TOK_INF               "inf"
%token <std::string>  TOK_ID                "identifier"
%token <std::string>  TOK_INTEGER           "integer"
%token                TOK_EOF          0    "end of file"


%nonassoc             "="
%left                 "&&"
%nonassoc             "=="  "<="  ">="  "<"  ">"
%left                 "!"

%type <enum tchecker::gta::guard_operator_t>    guard_operator 
%type <std::shared_ptr<tchecker::gta::guard_lterm_t const>> guard_lterm
%type <std::shared_ptr<tchecker::gta::guard_rterm_t const>> guard_rterm
%type <std::shared_ptr<tchecker::gta::guard_t const>> atomic_guard
%type <tchecker::gta::guards_conjunction_ptr_t> guard
%type <tchecker::gta::guards_conjunction_ptr_t> non_empty_guard
%type <tchecker::gta::guards_conjunction_ptr_t> non_atomic_guard
%type <std::shared_ptr<tchecker::gta::action_t>> atomic_action
%type <tchecker::gta::actions_ptr_t> action_list
%type <tchecker::gta::actions_ptr_t> action
%type <std::shared_ptr<tchecker::gta::gta_program_t>> list_ending_at_guard
%type <std::shared_ptr<tchecker::gta::gta_program_t>> list_ending_at_action

%printer { yyoutput << $$; }                <*>;
%printer { $$->output(yyoutput); }          guard_lterm
                                            guard_rterm
                                            atomic_guard
                                            atomic_action
                                            list_ending_at_guard
                                            list_ending_at_action;
%printer { yyoutput << (*$$); }             guard
                                            non_empty_guard
                                            non_atomic_guard
                                            action_list
                                            action;
%start   gta_program

%%

gta_program:
  list_ending_at_guard 
  {
    gta_program = $1;
  }
  | list_ending_at_action 
  {
    gta_program = $1;
  }
  | 
  {
    gta_program = std::make_shared<tchecker::gta::gta_program_t>();
  }

prog_delim : 
  ";"
;

list_ending_at_guard :
  guard prog_delim 
  {
    $$ = std::make_shared<tchecker::gta::gta_program_t>();
    $$->append_guards($1);
  }
  | list_ending_at_action guard prog_delim 
  {
    $$ = $1;
    $$->append_guards($2);
  }
;

list_ending_at_action :
  list_ending_at_guard action prog_delim 
  {
    $$ = $1;
    $$->append_actions($2);
  }
;

guard:
  non_empty_guard
  {
    $$ = $1;
  }
  | 
  {
    $$ = std::make_shared<tchecker::gta::guards_conjunction_t>();
  }
;
non_empty_guard :
  atomic_guard 
  {
    $$ = std::make_shared<tchecker::gta::guards_conjunction_t>();
    $$->push_back($1);
  }
  | non_atomic_guard 
  {
    $$ = $1;
  };

non_atomic_guard :
  "(" non_atomic_guard ")" 
  {
    $$ = $2;
  }
  | atomic_guard "&&" non_empty_guard 
  {
    assert($3.get() != nullptr);
    $3->push_back($1);
    $$ = $3;
  }
;

guard_lterm : 
  "(" guard_lterm ")" 
  { $$ = $2; }
  | TOK_ID 
  {
    try {
      const auto var = std::make_shared<tchecker::gta::guard_variable_t>($1);
      $$ = std::make_shared<tchecker::gta::guard_unary_lterm_t>(var);
    }
    catch (std::exception const & e){
      std::cerr << tchecker::log_error << @$ << " " << e.what() << std::endl;
      $$ = fake_guard_lterm;
    }
  }
  | TOK_ID "-" TOK_ID 
  {
    try {
      const auto var_l = std::make_shared<tchecker::gta::guard_variable_t>($1);
      const auto var_r = std::make_shared<tchecker::gta::guard_variable_t>($3);
      $$ = std::make_shared<tchecker::gta::guard_binary_lterm_t>(var_l, var_r);
    }
    catch (std::exception const & e){
      std::cerr << tchecker::log_error << @$ << " " << e.what() << std::endl;
      $$ = fake_guard_lterm;
    }
  }
;

guard_operator :
  "==" { $$ = tchecker::gta::GUARD_EQ; }
  | "<=" { $$ = tchecker::gta::GUARD_LE; }
  | "<" { $$ = tchecker::gta::GUARD_LT; }
  | ">=" { $$ = tchecker::gta::GUARD_GE; }
  | ">" { $$ = tchecker::gta::GUARD_GT; }
;

guard_rterm : 
  TOK_INTEGER 
  {
    try {
      long long l = std::stoll($1);
      if ( (l < std::numeric_limits<tchecker::integer_t>::min()) 
        || (l > std::numeric_limits<tchecker::integer_t>::max()) ) {
       throw std::out_of_range("Integer token out of range"); 
      }
      auto val = std::make_shared<tchecker::gta::guard_integer_t>(static_cast<tchecker::integer_t>(l));
      $$ = std::make_shared<tchecker::gta::guard_integer_rterm_t>(val);
    }  
    catch (std::exception const & e) {
      std::cerr << tchecker::log_error << @$ << " " << e.what() << std::endl;
      $$ = fake_guard_rterm;
    }
  }
  | "inf" 
  {
    $$ = std::make_shared<tchecker::gta::guard_constant_rterm_t>(tchecker::gta::RTERM_INF);
  }
  | "-" "inf" 
  {
    $$ = std::make_shared<tchecker::gta::guard_constant_rterm_t>(tchecker::gta::RTERM_MINUS_INF);
  }
;

atomic_guard : 
  "(" atomic_guard ")" 
  {
    $$ = $2;
  }
  | guard_lterm guard_operator guard_rterm 
  {
    try {
      $$ = std::make_shared<tchecker::gta::guard_t>($1, $2, $3); 
    }
    catch (std::exception const & e) {
      std::cerr << tchecker::log_error << e.what() << std::endl; 
      $$ = fake_guard;
    }
  }
;

action_delim : 
  ","
;

action :
  action_list 
  {
    $$ = $1;
  }
  | 
  {
    $$ = std::make_shared<tchecker::gta::actions_t>();
  }
;

action_list :
  atomic_action 
  {
    $$ = std::make_shared<tchecker::gta::actions_t>();
    $$->push_back($1);
  }
  | action_list action_delim atomic_action 
  {
    $$ = $1;
    $$->push_back($3);
  }
;

rename_operator : 
  "="
;

atomic_action : 
  "[" TOK_ID "]"
  {
    try {
      auto clk_var = std::make_shared<tchecker::gta::action_variable_t>($2);
      $$ = std::make_shared<tchecker::gta::release_reset_action_t>(clk_var);
    }
    catch (std::exception const & e){
      std::cerr << tchecker::log_error << @$ << " " << e.what() << std::endl;
      $$ = fake_action;
    }
  }
  | TOK_ID rename_operator TOK_ID {
    try {
      auto lhs_clk_var = std::make_shared<tchecker::gta::action_variable_t>($1);
      auto rhs_clk_var = std::make_shared<tchecker::gta::action_variable_t>($3);
      $$ = std::make_shared<tchecker::gta::rename_action_t>(lhs_clk_var, rhs_clk_var);
    }
    catch (std::exception const & e){
      std::cerr << tchecker::log_error << @$ << " " << e.what() << std::endl;
      $$ = fake_action;
    }
  }
;

%%


void tchecker::parsing::gta_program::parser_t::error(location_type const & l, std::string const & msg)
{
  if (gta_program_context.empty())
    std::cerr << tchecker::log_error << l << " " << msg << std::endl;
  else
    std::cerr << tchecker::log_error << gta_program_context << " " << msg << " (at " << l << ")" << std::endl;
}
