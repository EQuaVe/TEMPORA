/*
 * This file is a part of MITL2GTA project.
 *
 * See AUTHORS and LICENSE for copyright details.
 *
 */

#pragma once

#include <variant>
#include <vector>

#include "mitl2gta/clock/clock_registry.hpp"
#include "mitl2gta/compile/id_allocation.hpp"
#include "mitl2gta/memory/memory_handler.hpp"
#include "mitl2gta/utils/extended_int.hpp"

namespace mitl2gta {

namespace transducer {

struct clock_abs_val_in_interval_t {
  mitl2gta::clock::clock_id_t clk_id;
  mitl2gta::nonempty_interval_t interval;
};

struct clock_abs_val_less_than_interval_t {
  mitl2gta::clock::clock_id_t clk_id;
  mitl2gta::nonempty_interval_t interval;
};

struct clock_abs_val_greater_than_interval_t {
  mitl2gta::clock::clock_id_t clk_id;
  mitl2gta::nonempty_interval_t interval;
};

struct clock_abs_val_geq_lower_bound_t {
  mitl2gta::clock::clock_id_t clk_id;
  mitl2gta::nonempty_interval_t interval;
};

struct clock_abs_val_leq_upper_bound_t {
  mitl2gta::clock::clock_id_t clk_id;
  mitl2gta::nonempty_interval_t interval;
};

struct clock_abs_val_not_in_interval_t {
  mitl2gta::clock::clock_id_t clk_id;
  mitl2gta::nonempty_interval_t interval;
};

struct clock_val_equal_to_t {
  mitl2gta::clock::clock_id_t clk_id;
  mitl2gta::extended_integer_t value;
};

struct clock_val_greater_than {
  mitl2gta::clock::clock_id_t clk_id;
  mitl2gta::extended_integer_t value;
};

struct clock_val_less_than_t {
  mitl2gta::clock::clock_id_t clk_id;
  mitl2gta::extended_integer_t value;
};

struct clock_val_less_equals_t {
  mitl2gta::clock::clock_id_t clk_id;
  mitl2gta::extended_integer_t value;
};

struct release_reset_clock_t {
  mitl2gta::clock::clock_id_t clk_id;
};

struct shift_clocks_backward_t {
  std::vector<mitl2gta::clock::clock_id_t> clocks;
};

using gta_program_t =
    std::variant<mitl2gta::transducer::clock_abs_val_in_interval_t,
                 mitl2gta::transducer::clock_abs_val_less_than_interval_t,
                 mitl2gta::transducer::clock_abs_val_greater_than_interval_t,
                 mitl2gta::transducer::clock_abs_val_geq_lower_bound_t,
                 mitl2gta::transducer::clock_abs_val_leq_upper_bound_t,
                 mitl2gta::transducer::clock_abs_val_not_in_interval_t,
                 mitl2gta::transducer::clock_val_equal_to_t,
                 mitl2gta::transducer::clock_val_greater_than,
                 mitl2gta::transducer::clock_val_less_equals_t,
                 mitl2gta::transducer::clock_val_less_than_t,
                 mitl2gta::transducer::release_reset_clock_t,
                 mitl2gta::transducer::shift_clocks_backward_t>;

enum node_value_t {
  TRUE,
  FALSE,
  UNCERTAIN,
};

struct on_node_values_t {
  std::vector<std::pair<mitl2gta::compilation::node_id_t,
                        enum mitl2gta::transducer::node_value_t>>
      id_to_value;
};

struct on_epsilon_t {};

using transition_on_t = std::variant<mitl2gta::transducer::on_node_values_t,
                                     mitl2gta::transducer::on_epsilon_t>;

struct set_node_value_t {
  mitl2gta::compilation::node_id_t node_id;
  enum mitl2gta::transducer::node_value_t value;
};

struct set_memory_value_t {
  mitl2gta::memory::memory_id_t mem_id;
  int value;
};

using transition_action_t =
    std::variant<mitl2gta::transducer::set_node_value_t,
                 mitl2gta::transducer::set_memory_value_t>;

struct provided_memory_value_t {
  mitl2gta::memory::memory_id_t mem_id;
  int value;
};

using transition_guard_t =
    std::variant<mitl2gta::transducer::provided_memory_value_t>;

} // namespace transducer

} // namespace mitl2gta
