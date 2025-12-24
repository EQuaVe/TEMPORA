/*
 * This file is a part of the TChecker project.
 *
 * See files AUTHORS and LICENSE for copyright details.
 *
 */

%{
#include <cstdlib>
#include <memory>
#include <sstream>

#include "tchecker/expression/expression.hh"
#include "tchecker/parsing/parsing.hh"
#include "tchecker/statement/statement.hh"
#include "tchecker/utils/log.hh"

#include "gta_program.tab.hh"


// Tell Flex the lexer's prototype ...
#define YY_DECL \
tchecker::parsing::gta_program::parser_t::symbol_type gpyylex \
(std::string const & gta_program_context, \
 std::shared_ptr<tchecker::gta::gta_program_t> & gta_program)

// Work around an incompatibility in flex (at least versions
// 2.5.31 through 2.5.33): it generates code that does
// not conform to C89.  See Debian bug 333231
// <http://bugs.debian.org/cgi-bin/bugreport.cgi?bug=333231>.
#undef yywrap
#define yywrap() 1

// The location of the current token.
static tchecker::parsing::gta_program::location loc;
	
void gp_reset_locations()
{
	loc.initialize(nullptr, 1, 1);
}

using namespace tchecker::parsing;
%}

%option noyywrap nounput batch noinput
/*%option debug*/

id         [[:alpha:]_$][[:alnum:]_]*
integer    (\-)?[0-9]+
blankspace [ \t\r]
newline    [\n]

%{
  // Code run each time a pattern is matched.
	// move token's length wide
  #define YY_USER_ACTION  loc.columns(static_cast<int>(gpyyleng));
%}

%%

%{
	// Code run each time gpyylex is called.
	loc.step();
%}

"-"            { return gta_program::parser_t::make_TOK_MINUS(loc); }
"="            { return gta_program::parser_t::make_TOK_ASSIGN(loc); }
"("            { return gta_program::parser_t::make_TOK_LPAR(loc); }
")"            { return gta_program::parser_t::make_TOK_RPAR(loc); }
"["            { return gta_program::parser_t::make_TOK_LBRACKET(loc); }
"]"            { return gta_program::parser_t::make_TOK_RBRACKET(loc); }
";"            { return gta_program::parser_t::make_TOK_SEMICOLON(loc); }
","            { return gta_program::parser_t::make_TOK_COMMA(loc); }
"&&"           { return gta_program::parser_t::make_TOK_LAND(loc); }
"=="           { return gta_program::parser_t::make_TOK_EQ(loc); }
"<="           { return gta_program::parser_t::make_TOK_LE(loc); }
">="           { return gta_program::parser_t::make_TOK_GE(loc); }
"<"            { return gta_program::parser_t::make_TOK_LT(loc); }
">"            { return gta_program::parser_t::make_TOK_GT(loc); }
"inf"          { return gta_program::parser_t::make_TOK_INF(loc); }
{integer}      { return gta_program::parser_t::make_TOK_INTEGER(gpyytext,loc); }
{id}           { return gta_program::parser_t::make_TOK_ID(gpyytext, loc); }
{newline}+     { loc.lines(static_cast<int>(gpyyleng)); loc.step(); }
{blankspace}+  { loc.step(); }
<<EOF>>        { return gta_program::parser_t::make_TOK_EOF(loc); }

<*>.|{newline} { std::stringstream msg;
                 msg << loc << " Invalid character: " << gpyytext;
                 throw std::runtime_error(msg.str()); }

%%


namespace tchecker {
	
	namespace parsing {
    std::shared_ptr<tchecker::gta::gta_program_t> parse_gta_program(std::string const & gta_prog_context,
                                                                    std::string const & gta_prog_str)
		{
			std::size_t old_error_count = tchecker::log_error_count();
      std::shared_ptr<tchecker::gta::gta_program_t> gta_program;     
			// Scan from string
			YY_BUFFER_STATE previous_buffer = YY_CURRENT_BUFFER;
			YY_BUFFER_STATE current_buffer = gpyy_scan_string(gta_prog_str.c_str());
			
			// Initialise
			gp_reset_locations();
			BEGIN INITIAL;
			
			// Parse
			try {
				tchecker::parsing::gta_program::parser_t parser(gta_prog_context, gta_program);
				parser.parse();
				gpyy_delete_buffer(current_buffer);
				gpyy_switch_to_buffer(previous_buffer);
			}
			catch (...) {
        gta_program = nullptr;
				gpyy_delete_buffer(current_buffer);
				gpyy_switch_to_buffer(previous_buffer);
				throw;
			}
      
			if (tchecker::log_error_count() > old_error_count) {
        gta_program = nullptr;
        return gta_program;
      }

      return gta_program;
		}
	}
	
}

