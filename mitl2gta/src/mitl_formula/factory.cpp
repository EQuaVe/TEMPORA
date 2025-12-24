/*
 * This file is a part of MITL2GTA project.
 *
 * See AUTHORS and LICENSE for copyright details.
 *
 */
#include <memory>

#include "mitl2gta/mitl_formula/factory.hpp"
#include "mitl2gta/mitl_formula/parsed_formula.hpp"
#include "mitl2gta/mitl_formula/tree_transformations.hpp"
#include "mitl2gta/parsing/declarations.hpp"

namespace mitl2gta {

namespace mitl {

std::shared_ptr<mitl2gta::mitl::formula_t const>
construct_formula_graph(std::string const &formula,
                        mitl2gta::mitl::parsing_mode_t const parsing_mode) {
  std::shared_ptr<mitl2gta::mitl::formula_t const> const parsed_formula =
      mitl2gta::mitl::parse_formula(formula);

  if (parsed_formula.get() == nullptr) {
    return nullptr;
  }

  std::shared_ptr<mitl2gta::mitl::formula_t const> simplified_formula =
      mitl2gta::mitl::remove_unbounded_intervals(parsed_formula);

  if (parsing_mode ==
      mitl2gta::mitl::parsing_mode_t::SUBFORMULA_SHARING_ENABLED) {
    simplified_formula = mitl2gta::mitl::share_subformula(simplified_formula);
  }

  return simplified_formula;
}

} // namespace mitl

} // namespace mitl2gta
