/*
 * This file is a part of MITL2GTA project.
 *
 * See AUTHORS and LICENSE for copyright details.
 *
 */

#pragma once

#include <map>
#include <variant>

#include "mitl2gta/boolean_expression/expression.hpp"
#include "mitl2gta/mitl_formula/parsed_formula.hpp"
#include "mitl2gta/transducer/transducer.hpp"

namespace mitl2gta {

namespace compilation {

struct predicted_by_transducer_t {
  mitl2gta::transducer::transducer_t transducer;
};

struct atomic_constant_t {
  mitl2gta::mitl::constant_value_t val;
};

struct atomic_proposition_t {
  mitl2gta::compilation::node_id_t node;
};

struct boolean_negation_t {
  mitl2gta::compilation::node_id_t node;
};

struct boolean_or_t {
  mitl2gta::compilation::node_id_t l_node;
  mitl2gta::compilation::node_id_t r_node;
};

struct boolean_and_t {
  mitl2gta::compilation::node_id_t l_node;
  mitl2gta::compilation::node_id_t r_node;
};

using truth_value_predictor_t =
    std::variant<mitl2gta::compilation::predicted_by_transducer_t,
                 mitl2gta::compilation::atomic_constant_t,
                 mitl2gta::compilation::atomic_proposition_t,
                 mitl2gta::compilation::boolean_negation_t,
                 mitl2gta::compilation::boolean_or_t, boolean_and_t>;

using node_to_truth_value_predictor_t =
    std::map<mitl2gta::compilation::node_id_t,
             mitl2gta::compilation::truth_value_predictor_t>;

mitl2gta::boolean::expression_t expression_for_predictor(
    mitl2gta::compilation::node_id_t const expression_for,
    mitl2gta::compilation::node_to_truth_value_predictor_t const &mapping,
    std::map<mitl2gta::compilation::node_id_t,
             mitl2gta::memory::memory_id_t> const &placeholder_memory);

} // namespace compilation

} // namespace mitl2gta
