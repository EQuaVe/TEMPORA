/*
 * This file is a part of MITL2GTA project.
 *
 * See AUTHORS and LICENSE for copyright details.
 *
 */

#include "io.hpp"
#include "parsing.hpp"
#include <argp.h>
#include <iostream>
#include <memory>
#include <optional>
#include <string>

#include "mitl2gta/compile/compiled_node.hpp"
#include "mitl2gta/compile/compiler.hpp"
#include "mitl2gta/compile/compiler_initsat.hpp"
#include "mitl2gta/gta_system/static_analysis.hpp"
#include "mitl2gta/mitl_formula/factory.hpp"
#include "mitl2gta/utils/string_construction.hpp"

int translate(parsed_config_t const &config) {
  io_handler_t const io_handler(config.formula_input(), config.gta_output());

  std::string formula = io_handler.read_formula();

  std::shared_ptr<mitl2gta::mitl::formula_t const> root_ptr =
      mitl2gta::mitl::construct_formula_graph(formula, config.parsing_mode());

  if (root_ptr.get() == nullptr) {
    std::cerr << "Error: Formula parsing unsuccessful" << std::endl;
    return -1;
  }

  std::cout << "Parsed formula: " << *root_ptr << std::endl;

  mitl2gta::compilation::translation_specs_t specs{
      config.nodes_for(), config.system_for(), config.mapping_mode()};
  mitl2gta::gta::system_t system;

  switch (config.translation_mode()) {
  case ALL_TRANSDUCERS: {
    mitl2gta::compilation::compiler_t compiler(root_ptr,
                                               config.verifying_mode());
    system = compiler.translate_to_gta_system(
        config.sharing_mode(), config.p_until_q_sharer_states(), specs);
    break;
  }

  case INIT_SAT_AUTOMATA: {
    mitl2gta::compilation::compiler_initsat_t compiler(root_ptr,
                                                       config.verifying_mode());
    system = compiler.translate_to_gta_system(
        config.sharing_mode(), config.p_until_q_sharer_states(), specs);
    break;
  }
  }

  std::string const system_repr = mitl2gta::gta::declaration(system);

  std::set<std::string> labels = mitl2gta::gta::labels(system);
  std::string query = mitl2gta::join(labels.cbegin(), labels.cend(), ",");

  std::cout << "Labels: " << query << std::endl;

  io_handler.write_system(system_repr);
  return 0;
}

int main(int argc, char *argv[]) {

  std::optional<parsed_config_t> parsed_config_or_none = parse_args(argc, argv);

  if (!parsed_config_or_none.has_value()) {
    std::cerr << "Error parsing arguments" << std::endl;
    return -1;
  }

  try {
    return translate(*parsed_config_or_none);
  } catch (std::exception &e) {
    std::cerr << "Error:" << e.what() << std::endl;
    return -1;
  }
}
