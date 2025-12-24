/*
 * This file is a part of the TChecker project.
 *
 * See files AUTHORS and LICENSE for copyright details.
 *
 */

#include "tchecker/gta/system.hh"
#include "tchecker/basictypes.hh"
#include "tchecker/expression/static_analysis.hh"
#include "tchecker/gta/static_analysis.hh"
#include "tchecker/gta_program/gta_program.hh"
#include "tchecker/gta_program/typechecking.hh"
#include "tchecker/gta_program/typed_gta_program.hh"
#include "tchecker/parsing/parsing.hh"
#include "tchecker/statement/static_analysis.hh"
#include "tchecker/system/attribute.hh"
#include "tchecker/system/clock.hh"
#include "tchecker/utils/log.hh"
#include "tchecker/variables/variables.hh"

namespace tchecker {

namespace gta {

system_t::system_t(tchecker::parsing::system_declaration_t const & sysdecl) : tchecker::ta::system_t(sysdecl)
{
  compute_from_ta_system();
}

system_t::system_t(tchecker::system::system_t const & system) : tchecker::ta::system_t(system) { compute_from_ta_system(); }

system_t::system_t(tchecker::syncprod::system_t const & system) : tchecker::ta::system_t(system) { compute_from_ta_system(); }

system_t::system_t(tchecker::ta::system_t const & system) : tchecker::ta::system_t(system) { compute_from_ta_system(); }

system_t::system_t(tchecker::gta::system_t const & system) : tchecker::ta::system_t(system.as_ta_system())
{
  compute_from_ta_system();
}

tchecker::gta::system_t & system_t::operator=(tchecker::gta::system_t const & system)
{
  if (this != &system) {
    tchecker::ta::system_t::operator=(system);
    compute_from_ta_system();
  }
  return *this;
}

tchecker::system::attribute_keys_map_t const & system_t::known_attributes()
{
  static tchecker::system::attribute_keys_map_t const known_attr{[&]() {
    tchecker::system::attribute_keys_map_t attr(tchecker::ta::system_t::known_attributes());
    attr[tchecker::system::ATTR_CLOCK].insert("type");
    attr[tchecker::system::ATTR_EDGE].insert("gta_program");
    return attr;
  }()};
  return known_attr;
}

bool system_t::is_prophecy_clock(tchecker::clock_id_t const clk_id) const { return (_prophecy_clocks[clk_id] == 1); }

bool system_t::is_edge_unsatisfiable(tchecker::edge_id_t const id) const
{
  assert(is_edge(id));
  return (_unsatisfiable_edges[id]);
}

tchecker::gta::typed_gta_program_t const & system_t::gta_program(tchecker::edge_id_t const id) const
{
  assert(is_edge(id));
  assert(not is_edge_unsatisfiable(id));
  return *((_gta_progs[id])._typed_gta_prog);
}

tchecker::gta::compiled_gta_program_t const & system_t::compiled_gta_program(tchecker::edge_id_t const id) const
{
  assert(is_edge(id));
  assert(not is_edge_unsatisfiable(id));
  return ((_gta_progs[id])._compiled_gta_prog);
}

void system_t::compute_from_ta_system()
{
  _gta_progs.clear();
  _prophecy_clocks.clear();
  _history_zero_clocks.clear();
  _history_inf_clocks.clear();
  _unsatisfiable_edges.clear();

  size_t const clocks_count = this->clocks_count(tchecker::VK_FLATTENED);
  size_t const edges_count = this->edges_count();

  _prophecy_clocks.resize(clocks_count);
  _history_zero_clocks.resize(clocks_count);
  _history_inf_clocks.resize(clocks_count);
  _gta_progs.resize(edges_count);
  _unsatisfiable_edges.resize(edges_count);

  for (tchecker::clock_id_t const clk_id : this->clocks_identifiers(tchecker::VK_FLATTENED)) {
    tchecker::system::attributes_t const & type_attribute = this->clock_attributes(clk_id);
    set_clock_type(clk_id, type_attribute.range("type"));
  }

  // Consistency check for the three bitsets
  assert((_prophecy_clocks | _history_zero_clocks | _history_inf_clocks).all()); // A clock is either of the three
  // Any two bitsets are disjoint
  assert((_prophecy_clocks & _history_inf_clocks).none());
  assert((_prophecy_clocks & _history_zero_clocks).none());
  assert((_history_zero_clocks & _history_inf_clocks).none());

  for (tchecker::edge_id_t const id : this->edges_identifiers()) {
    auto const & attributes = tchecker::ta::system_t::edge(id)->attributes();
    set_gta_program(id, attributes.range("gta_program"));
  }

  // Validations
  tchecker::clock_variables_t const & clk_variables = this->clock_variables();
  for (tchecker::clock_id_t const clk_id : clk_variables.identifiers(tchecker::VK_DECLARED)) {
    tchecker::size_info_t::size_t const size = clk_variables.info(clk_id).size();
    if (size > 1) {
      throw std::invalid_argument("Array of clocks not supported in GTA model");
    }
  }

  for (tchecker::edge_id_t const id : edges_identifiers()) {
    tchecker::has_clock_constraints_t const & edge_has_clk_constraints = tchecker::has_clock_constraints(guard(id));
    if (edge_has_clk_constraints.simple || edge_has_clk_constraints.diagonal) {
      std::cerr << tchecker::log_warning
                << "Clock constraints in provided attribute will not be checked. "
                   "Please add all the clock constraints in gta_program attribute"
                << std::endl;
    }

    tchecker::has_clock_resets_t const & edge_has_clk_updates = tchecker::has_clock_resets(statement(id));
    if (edge_has_clk_updates.clock || edge_has_clk_updates.sum || edge_has_clk_updates.constant) {
      std::cerr << tchecker::log_warning
                << "Clock updates in do attribute will not be executed. "
                   "Please add all the clock updates in gta_program attribute"
                << std::endl;
    }
  }

  for (std::size_t const id : locations_identifiers()) {
    tchecker::has_clock_constraints_t const & loc_invariant_has_clk_constriants =
        tchecker::has_clock_constraints(invariant(id));
    if (loc_invariant_has_clk_constriants.simple || loc_invariant_has_clk_constriants.diagonal) {
      std::cerr << tchecker::log_warning
                << "Clock constraints in invariant attribute will not be checked. "
                   "Please add all the clock constraints in gta_program attribute"
                << std::endl;
    }
  }
}

void system_t::set_clock_type(tchecker::clock_id_t const & id,
                              tchecker::range_t<tchecker::system::attributes_t::const_iterator_t> const & clk_type)
{
  // Initialize with default value
  _history_zero_clocks[id] = 1;
  _prophecy_clocks[id] = 0;
  _history_inf_clocks[id] = 0;

  for (tchecker::system::attr_t const & type_attr : clk_type) {
    std::string const & type_val = type_attr.value();
    if (type_val == "prophecy") {
      _prophecy_clocks[id] = 1;
      _history_zero_clocks[id] = 0;
      _history_inf_clocks[id] = 0;
    }

    else if (type_val == "history_zero") {
      _history_zero_clocks[id] = 1;
      _prophecy_clocks[id] = 0;
      _history_inf_clocks[id] = 0;
    }

    else if (type_val == "history_inf") {
      _history_inf_clocks[id] = 1;
      _prophecy_clocks[id] = 0;
      _history_zero_clocks[id] = 0;
    }

    else {
      std::cerr << tchecker::log_warning << "Unknown type " << type_val << " for clock " << this->clock_name(id) << std::endl;
    }
  }
}

void system_t::set_gta_program(tchecker::edge_id_t id,
                               tchecker::range_t<tchecker::system::attributes_t::const_iterator_t> const & gta_programs)
{
  assert(is_edge(id));

  if (gta_programs.begin() == gta_programs.end()) {
    _gta_progs[id]._typed_gta_prog = std::make_shared<tchecker::gta::typed_gta_program_t>();
    return;
  }

  for (tchecker::system::attr_t const & gta_program : gta_programs) {
    std::shared_ptr<tchecker::gta::gta_program_t> gta_prog_ast =
        tchecker::parsing::parse_gta_program(gta_program.parsing_position().value_position(), gta_program.value());

    if (gta_prog_ast.get() == nullptr) {
      throw std::invalid_argument(std::string("Syntax error at line ") + gta_program.parsing_position().value_position());
    }

    std::shared_ptr<tchecker::gta::typed_gta_program_t> typed_gta_prog =
        tchecker::gta::typechecking(gta_prog_ast, clock_variables(), _prophecy_clocks);

    if (typed_gta_prog.get() == nullptr) {
      throw std::invalid_argument(std::string("Type error at line ") + gta_program.parsing_position().value_position());
    }

    std::shared_ptr<tchecker::gta::typed_gta_program_t> simplified_gta_prog =
        tchecker::gta::simplify_gta_prog(typed_gta_prog, _prophecy_clocks);

    if (simplified_gta_prog.get() == nullptr) {
      std::cerr << tchecker::log_warning << "unsatisfiable GTA program at line "
                << gta_program.parsing_position().value_position() << std::endl;
      _unsatisfiable_edges[id] = 1;
      return;
    }

    _gta_progs[id] = edge_gta_program_t{simplified_gta_prog, tchecker::gta::compile_gta_program(*simplified_gta_prog)};
  }
}

} // end of namespace gta

} // end of namespace tchecker
