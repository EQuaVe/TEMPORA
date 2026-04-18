/*
 * This file is a part of MITL2GTA project.
 *
 * See AUTHORS and LICENSE for copyright details.
 *
 */

#include "mitl2gta/compile/until.hpp"
#include "mitl2gta/compile/truth_value_predictor.hpp"
#include "mitl2gta/interval/bound.hpp"
#include "mitl2gta/sharer_automaton/sharer_types.hpp"
#include "mitl2gta/transducer/edge_instructions.hpp"

namespace mitl2gta {

namespace compilation {

namespace {
struct general_until_prog_t {
  std::vector<mitl2gta::transducer::gta_program_t> program;
  int next_state_index;
  mitl2gta::transducer::node_value_t output;
};
} // namespace

using mitl2gta::transducer::clock_abs_val_geq_lower_bound_t;
using mitl2gta::transducer::clock_abs_val_leq_upper_bound_t;
using mitl2gta::transducer::clock_abs_val_greater_than_interval_t;
using mitl2gta::transducer::clock_abs_val_in_interval_t;
using mitl2gta::transducer::clock_abs_val_less_than_interval_t;
using mitl2gta::transducer::clock_abs_val_not_in_interval_t;
using mitl2gta::transducer::clock_val_equal_to_t;
using mitl2gta::transducer::clock_val_greater_than;
using mitl2gta::transducer::node_value_t;
using mitl2gta::transducer::on_node_values_t;
using mitl2gta::transducer::provided_memory_value_t;
using mitl2gta::transducer::release_reset_clock_t;
using mitl2gta::transducer::set_node_value_t;
using mitl2gta::transducer::shift_clocks_backward_t;
using mitl2gta::transducer::clock_val_less_than_clock_t;
using mitl2gta::transducer::clock_val_equal_to_clock_t;

using mitl2gta::transducer::provided_memory_value_t;
// compiled_untimed_until_t
compiled_untimed_until_t::compiled_untimed_until_t(
    mitl2gta::compilation::node_id_t lchild,
    mitl2gta::compilation::node_id_t rchild,
    mitl2gta::compilation::compiled_node_id_allocator_t &allocator)
    : mitl2gta::compilation::compiled_node_t(allocator),
      mitl2gta::compilation::binary_node_t(lchild, rchild) {}

mitl2gta::compilation::truth_value_predictor_t
compiled_untimed_until_t::generate_truth_value_predictor(
    mitl2gta::transducer::transducer_construction_tools_t &tools,
    mitl2gta::compilation::translation_specs_t const &specs) const {
  mitl2gta::transducer::state_t s1(tools.state_id_allocator);
  s1.make_initial();

  mitl2gta::sharer::p_until_q_sharer_t const p_until_q =
      tools.sharer_registry.request_p_until_q_sharer(
          id(), lchild(), rchild(),
          mitl2gta::sharer::p_until_q_sharer_type_t::UNTIMED,
          tools.clk_registry, tools.mem_handler);

  mitl2gta::transducer::edge_t const e1(
      s1.id(), s1.id(), on_node_values_t{},
      {provided_memory_value_t{p_until_q.p_until_q_truth_value,
                               mitl2gta::sharer::SHARER_TRUE_VAL}},
      {set_node_value_t{_id, node_value_t::TRUE}}, {});

  mitl2gta::transducer::edge_t const e2(
      s1.id(), s1.id(), on_node_values_t{},
      {provided_memory_value_t{p_until_q.p_until_q_truth_value,
                               mitl2gta::sharer::SHARER_FALSE_VAL}},
      {set_node_value_t{_id, node_value_t::FALSE}}, {});

  mitl2gta::transducer::transducer_t t{{s1}, {e1, e2}};

  return mitl2gta::compilation::predicted_by_transducer_t{t};
}

std::set<mitl2gta::compilation::node_id_t>
compiled_untimed_until_t::direct_dependencies() const {
  return {_lchild, _rchild};
};

// compiled_timed_until_t
compiled_timed_until_t::compiled_timed_until_t(
    mitl2gta::nonempty_interval_t const interval,
    mitl2gta::compilation::node_id_t lchild,
    mitl2gta::compilation::node_id_t rchild,
    mitl2gta::compilation::compiled_node_id_allocator_t &allocator)
    : mitl2gta::compilation::compiled_node_t(allocator),
      mitl2gta::compilation::timed_node_t(interval),
      mitl2gta::compilation::binary_node_t(lchild, rchild) {
  if (interval.lower_bound() == interval.upper_bound()) {
    throw std::invalid_argument("Until does not support singleton intervals");
  }
}

mitl2gta::compilation::truth_value_predictor_t
compiled_timed_until_t::generate_truth_value_predictor(
    mitl2gta::transducer::transducer_construction_tools_t &tools,
    mitl2gta::compilation::translation_specs_t const &specs) const {
  mitl2gta::boundval_t const lower_bound = interval().lower_bound().val;
  mitl2gta::boundval_t const upper_bound = interval().upper_bound().val;

  if (upper_bound == mitl2gta::INF_VAL) {
    throw std::runtime_error("Upper bound should be finite");
  }

  if (upper_bound <= lower_bound) {
    throw std::runtime_error("Invalid interval");
  }

  mitl2gta::sharer::p_until_q_sharer_t const p_until_q_sharer =
      tools.sharer_registry.request_p_until_q_sharer(
          id(), lchild(), rchild(),
          mitl2gta::sharer::p_until_q_sharer_type_t::
              TIMED_FIRST_AND_LAST_WITNESS,
          tools.clk_registry, tools.mem_handler);

  mitl2gta::clock::clock_id_t const x =
      p_until_q_sharer.first_witness_predicting_clk.value();
  mitl2gta::clock::clock_id_t const y =
      p_until_q_sharer.last_witness_predicting_clk.value();

  // k = 2 + floor(b/(c-b)), where b is the lower bound of the interval and c is the upper bound of the interval. 
  int const num_clks = mitl2gta::both_bounds_open(interval())
      ? 1 + (lower_bound) / (upper_bound - lower_bound) // Both bounds are not open
      : 2 + (lower_bound) / (upper_bound - lower_bound); // Both bounds are open
  
  std::vector<mitl2gta::clock::clock_id_t> x_clks;
  std::vector<mitl2gta::clock::clock_id_t> y_clks;
  for (int i = 0; i < num_clks; i++) {
    x_clks.emplace_back(tools.clk_registry.register_clock(
        mitl2gta::gta::gta_clock_type_t::PROPHECY));

    y_clks.emplace_back(tools.clk_registry.register_clock(
        mitl2gta::gta::gta_clock_type_t::PROPHECY));
  }

  //Vector of states for (k,1)
  std::vector<mitl2gta::transducer::state_t> locations_1;
  //Vector of states for (k,2)
  std::vector<mitl2gta::transducer::state_t> locations_2;

  for (int i = 0; i <= num_clks; i++) {
    locations_1.emplace_back(
        mitl2gta::transducer::state_t(tools.state_id_allocator));
    locations_2.emplace_back(
        mitl2gta::transducer::state_t(tools.state_id_allocator));
  }

  locations_1.at(0).make_initial();

  if (specs.system_for == mitl2gta::transducer::system_for_t::INFINITE_TRACE) {
    locations_1.at(0).make_final();
  }

  if (specs.system_for == mitl2gta::transducer::system_for_t::FINITE_TRACE) {
    locations_1.at(0).make_final();
  }

  std::vector<mitl2gta::transducer::edge_t> edges;

  // Defining transitions from state (0,1)

  //Line 2 of Algorithm 2
  //Since 0 is not in the interval, if the current position satisfies (pUq), it should also satisfy p && X(pUq).
  //If not, the output should be False

  // !p -> False
  edges.emplace_back(mitl2gta::transducer::edge_t(
      locations_1.at(0).id(), locations_1.at(0).id(),
      on_node_values_t{{{lchild(), node_value_t::FALSE}}}, {},
      {set_node_value_t{id(), node_value_t::FALSE}}, {}));

  //Line 2 of Algorithm 2
  // p && !X(pUq) -> False
  edges.emplace_back(mitl2gta::transducer::edge_t(
      locations_1.at(0).id(), locations_1.at(0).id(),
      on_node_values_t{{{lchild(), node_value_t::TRUE}}},
      {provided_memory_value_t{p_until_q_sharer.next_p_until_q_truth_value,
                               mitl2gta::sharer::SHARER_FALSE_VAL}},
      {set_node_value_t{id(), node_value_t::FALSE}}, {}));

  //From all the other transitions in the automaton, p is true and X(pUq) is true 

  //Line 3 of Algorithm 2 - case true
  // p  &&  X(pUq) && |x| is not too close, i.e., (!(|x| < I)) 
  // Check |x| in I -> True
  // First witness is not too close, and is within the interval
  edges.emplace_back(mitl2gta::transducer::edge_t(
      locations_1.at(0).id(), locations_1.at(0).id(),
      on_node_values_t{{{lchild(), node_value_t::TRUE}}},
      {provided_memory_value_t{p_until_q_sharer.next_p_until_q_truth_value,
                               mitl2gta::sharer::SHARER_TRUE_VAL}},
      {set_node_value_t{id(), node_value_t::TRUE}},
      {
//        clock_abs_val_geq_lower_bound_t{x, interval()},
        clock_abs_val_in_interval_t{x, interval()}
    }));

  //Line 3 of Algorithm 2 - case false
  // p  &&  X(pUq) && |x| is not too close, i.e., (!(|x| < I)) 
  // Check |x| > I -> False
  // First witness is not too close, but too far i.e., outside the interval
  edges.emplace_back(mitl2gta::transducer::edge_t(
      locations_1.at(0).id(), locations_1.at(0).id(),
      on_node_values_t{{{lchild(), node_value_t::TRUE}}},
      {provided_memory_value_t{p_until_q_sharer.next_p_until_q_truth_value,
                               mitl2gta::sharer::SHARER_TRUE_VAL}},
      {set_node_value_t{id(), node_value_t::FALSE}},
      {
        clock_abs_val_geq_lower_bound_t{x, interval()},
        clock_abs_val_greater_than_interval_t{x, interval()}
    }));
    

  //Line 4 of Algorithm 2 - case true
  // p  &&  X(pUq) && |x| < I && !(|y| > I)
  // First witness is too close
  // Last witness is not too far away, and is in the interval
  // |y| in I -> True
  // This subsumes the check that the last witness is not too away
  edges.emplace_back(mitl2gta::transducer::edge_t(
      locations_1.at(0).id(), locations_1.at(0).id(),
      on_node_values_t{{{lchild(), node_value_t::TRUE}}},
      {provided_memory_value_t{p_until_q_sharer.next_p_until_q_truth_value,
                               mitl2gta::sharer::SHARER_TRUE_VAL}},
      {set_node_value_t{id(), node_value_t::TRUE}},
      {
          clock_abs_val_less_than_interval_t{x, interval()},
//          clock_abs_val_leq_upper_bound_t{y, interval()},
          clock_abs_val_in_interval_t{y, interval()}
        }));

  //Line 4 of Algorithm 2 - case false
  // p  &&  X(pUq) && |x| < I && !(|y| > I)
  // First witness is too close, 
  // Last witness is not too far away, but is too close, i.e., before the interval
  // |y| < I -> False
  edges.emplace_back(mitl2gta::transducer::edge_t(
      locations_1.at(0).id(), locations_1.at(0).id(),
      on_node_values_t{{{lchild(), node_value_t::TRUE}}},
      {provided_memory_value_t{p_until_q_sharer.next_p_until_q_truth_value,
                               mitl2gta::sharer::SHARER_TRUE_VAL}},
      {set_node_value_t{id(), node_value_t::FALSE}},
      {
          clock_abs_val_less_than_interval_t{x, interval()},
          clock_abs_val_leq_upper_bound_t{y, interval()},
          clock_abs_val_less_than_interval_t{y, interval()}
        }));

  //Difficult point 
  //p  &&  X(pUq), and
  // Earliest witness is too close, before the interval
  // Latest witness is too far away, after the interval
  // There could be a witness in the interval

  // We start a new pair of clocks 
  // x1 stores the time till the last witness before the end of the interval
  // y1 stores the time till the first witness after the end of the interval


  // Lines 5-8 of Algorithm 2 - case true
  // The following transition checks all the preconditions && 
  // |x_1| in I, but |y_1| > I 
  // Output True
  edges.emplace_back(mitl2gta::transducer::edge_t(
      locations_1.at(0).id(), locations_1.at(1).id(),
      on_node_values_t{{{lchild(), node_value_t::TRUE}}},
      {provided_memory_value_t{p_until_q_sharer.next_p_until_q_truth_value,
                               mitl2gta::sharer::SHARER_TRUE_VAL}},
      {set_node_value_t{id(), node_value_t::TRUE}},
      {
          clock_abs_val_less_than_interval_t{x, interval()},
          clock_abs_val_greater_than_interval_t{y, interval()},
          release_reset_clock_t{x_clks.at(0)},
          release_reset_clock_t{y_clks.at(0)},
        //   clock_val_less_than_clock_t{y, y_clks.at(0)},
        //   clock_val_less_than_clock_t{y_clks.at(0), x_clks.at(0)},
        //   clock_val_less_than_clock_t{x_clks.at(0), x},
          clock_abs_val_in_interval_t{x_clks.at(0), interval()},
          clock_abs_val_greater_than_interval_t{y_clks.at(0), interval()},
          clock_val_greater_than{y_clks.at(0), mitl2gta::EXTENDED_MINUS_INF},
      }));

  // Lines 5-8 of Algorithm 2 - case false
  // The following transition checks all the preconditions && 
  // |x_1| < I and |y_1| > I 
  // Output False
  edges.emplace_back(mitl2gta::transducer::edge_t(
      locations_1.at(0).id(), locations_1.at(1).id(),
      on_node_values_t{{{lchild(), node_value_t::TRUE}}},
      {provided_memory_value_t{p_until_q_sharer.next_p_until_q_truth_value,
                               mitl2gta::sharer::SHARER_TRUE_VAL}},
      {set_node_value_t{id(), node_value_t::FALSE}},
      {
          clock_abs_val_less_than_interval_t{x, interval()},
          clock_abs_val_greater_than_interval_t{y, interval()},
          release_reset_clock_t{x_clks.at(0)},
          release_reset_clock_t{y_clks.at(0)},
        //   clock_val_less_than_clock_t{y, y_clks.at(0)},
        //   clock_val_less_than_clock_t{y_clks.at(0), x_clks.at(0)},
        //   clock_val_less_than_clock_t{x_clks.at(0), x},
          clock_abs_val_less_than_interval_t{x_clks.at(0), interval()},
          clock_abs_val_greater_than_interval_t{y_clks.at(0), interval()},
          clock_val_greater_than{y_clks.at(0), mitl2gta::EXTENDED_MINUS_INF},
      }));

        // Question: ---- To check - should we also do 
        // clock_abs_val_greater_than_interval_t{x_clks.at(0), interval()},
  
  
  // Lines 9-11 of Algorithm 2
  // From the state (1,2) 
  // Takes care of the special case when y_1 = y, and we are seeing the last witness 
  // In this case, the value of y that we are reading is incorrect - it is the value after it is released and invalidated, while we should be reading the pre-value. In any case,
  // since this is a witness, it satisfies q
  // since this is the last witness, it also satisfies !(p && X(p U q))
  // Output False


  // Case 1: q ∧ ¬p
  edges.emplace_back(mitl2gta::transducer::edge_t(
      locations_2.at(1).id(), locations_1.at(0).id(),
      on_node_values_t{
          {{lchild(), node_value_t::FALSE}, {rchild(), node_value_t::TRUE}}},
      {}, {set_node_value_t{id(), node_value_t::FALSE}},
      {clock_val_equal_to_t{y_clks.at(0), 0},
       release_reset_clock_t{y_clks.at(0)},
       clock_val_equal_to_t{y_clks.at(0), mitl2gta::EXTENDED_MINUS_INF}}));

  // Case 2: q && p && !X(p U q)  
  edges.emplace_back(mitl2gta::transducer::edge_t(
      locations_2.at(1).id(), locations_1.at(0).id(),
      on_node_values_t{
          {{lchild(), node_value_t::TRUE}, {rchild(), node_value_t::TRUE}}},
      {provided_memory_value_t{p_until_q_sharer.next_p_until_q_truth_value,
                               mitl2gta::sharer::SHARER_FALSE_VAL}},
      {set_node_value_t{id(), node_value_t::FALSE}},
      {clock_val_equal_to_t{y_clks.at(0), 0},
       release_reset_clock_t{y_clks.at(0)},
       clock_val_equal_to_t{y_clks.at(0), mitl2gta::EXTENDED_MINUS_INF}}));


  // So far, we have defined all the transitions from states (0,1) and some special transitions from (1,2). We will now define the transitions from states (k,1) and (k,2) for k > 1. These transitions correspond to lines 14-35 of Algorithm 2.       


  for (int k = 1; k < locations_1.size(); k++) {

  // Next, we generate all the gta_programs that will be used on the transitions of this automaton in an array called all_progs    

    std::vector<general_until_prog_t> all_progs;

  // Lines 13-14 of Algorithm 2 - case true
  // The earliest witness is not too close, i.e., (!(|x| < I)) 
  // Check |x| in I -> True
  // First witness is not too close, and is within the interval
    std::vector<mitl2gta::transducer::gta_program_t> first_if1({
//        clock_abs_val_geq_lower_bound_t{x, interval()},
        clock_abs_val_in_interval_t{x, interval()}
    });

    all_progs.emplace_back(
        general_until_prog_t{first_if1, k, node_value_t::TRUE});

  // Lines 13-14 of Algorithm 2 - case false
  // First witness is not too close, but too far i.e., outside the interval
  // Check |x| > I -> False
    std::vector<mitl2gta::transducer::gta_program_t> first_if2({
        clock_abs_val_geq_lower_bound_t{x, interval()},
        clock_abs_val_greater_than_interval_t{x, interval()}
    });

    all_progs.emplace_back(
        general_until_prog_t{first_if2, k, node_value_t::FALSE});


  // Lines 15-16 of Algorithm 2 - case true
  // First witness is too close
  // Last witness is not too far away, and is in the interval
  // |x| < I && !(|y| > I)
  // |y| in I -> True
    std::vector<mitl2gta::transducer::gta_program_t> first_else1({
          clock_abs_val_less_than_interval_t{x, interval()},
//          clock_abs_val_leq_upper_bound_t{y, interval()},
          clock_abs_val_in_interval_t{y, interval()}
    });

    all_progs.emplace_back(
        general_until_prog_t{first_else1, k, node_value_t::TRUE});

  // Lines 15-16 of Algorithm 2 - case false
  // |x| < I && !(|y| > I)
  // First witness is too close, 
  // Last witness is not too far away, but is too close, i.e., before the interval
  // |y| < I -> False
    std::vector<mitl2gta::transducer::gta_program_t> first_else2({
          clock_abs_val_less_than_interval_t{x, interval()},
          clock_abs_val_leq_upper_bound_t{y, interval()},
          clock_abs_val_less_than_interval_t{y, interval()}
    });

    all_progs.emplace_back(
        general_until_prog_t{first_else2, k, node_value_t::FALSE});


// Lines 17-18 of Algorithm 2 - case true - first active witness
  // First witness is too close
  // Last witness is too far away
  // Latest last active witness is not too close
  // Latest first active witness is in the interval
  // |x| < I && (|y| > I) && !(|y_k| < I) &&
  // (|x_k| in I) -> True
    std::vector<mitl2gta::transducer::gta_program_t> second_else1({
          clock_abs_val_less_than_interval_t{x, interval()},
          clock_abs_val_leq_upper_bound_t{y, interval()},
          clock_abs_val_less_than_interval_t{y, interval()},
          clock_abs_val_geq_lower_bound_t{y_clks.at(k - 1), interval()},
          clock_abs_val_in_interval_t{x_clks.at(k - 1), interval()},
    });

    all_progs.emplace_back(
        general_until_prog_t{second_else1, k, node_value_t::TRUE});

  // Lines 17-18 of Algorithm 2 - case true - last active witness
  // First witness is too close
  // Latest  last witness is too far away
  // Latest last active witness is not too close
  // Latest first active witness is too close 
  // Latest last active witness is in the interval
  // |x| < I && (|y| > I) && !(|y_k| < I) && (|x_k| < I) -> True
  // |y_k| in I -> True
    std::vector<mitl2gta::transducer::gta_program_t> second_else2({
          clock_abs_val_less_than_interval_t{x, interval()},
          clock_abs_val_leq_upper_bound_t{y, interval()},
          clock_abs_val_less_than_interval_t{y, interval()},
          clock_abs_val_geq_lower_bound_t{y_clks.at(k - 1), interval()},
          clock_abs_val_less_than_interval_t{x_clks.at(k - 1), interval()},
          clock_abs_val_in_interval_t{y_clks.at(k - 1), interval()},
    });

    all_progs.emplace_back(
        general_until_prog_t{second_else2, k, node_value_t::TRUE});

  // Lines 17-18 of Algorithm 2 - case false 
  // First witness is too close
  // Latest  last witness is too far away
  // Latest last active witness is not too close
  // Latest first active witness is too close 
  // Latest last active witness is also too far away, i.e., after the interval
  // |x| < I && (|y| > I) && !(|y_k| < I) && (|x_k| < I) -> True
  // |y_k| > I -> False
    std::vector<mitl2gta::transducer::gta_program_t> second_else3({
          clock_abs_val_less_than_interval_t{x, interval()},
          clock_abs_val_leq_upper_bound_t{y, interval()},
          clock_abs_val_less_than_interval_t{y, interval()},
          clock_abs_val_geq_lower_bound_t{y_clks.at(k - 1), interval()},
          clock_abs_val_less_than_interval_t{x_clks.at(k - 1), interval()},
          clock_abs_val_greater_than_interval_t{y_clks.at(k - 1), interval()},
    });

    all_progs.emplace_back(
        general_until_prog_t{second_else3, k, node_value_t::FALSE});




  // Lines 20-22 of Algorithm 2
  // First witness is too close, is before the interval, i.e., (x < I),
  // Last witness is too far away, is after the interval i.e., (y > I),
  // Latest Last active witness is too close  i.e., (y_k < I),

  // Special Difficult point      
  // Activate new pair of clocks x_{k+1}, y_{k+1}
  // Check  x_{k+1} in I, but y_{k+1} > I
  // Output TRUE
    if (k != locations_1.size() - 1) {
      std::vector<mitl2gta::transducer::gta_program_t> third_else1({
          clock_abs_val_less_than_interval_t{x, interval()},
          clock_abs_val_greater_than_interval_t{y, interval()},
          clock_abs_val_less_than_interval_t{y_clks.at(k - 1), interval()},
          release_reset_clock_t{x_clks.at(k)},
          release_reset_clock_t{y_clks.at(k)},
          clock_abs_val_greater_than_interval_t{y_clks.at(k), interval()},
        //   clock_val_less_than_clock_t{y, y_clks.at(k)},
        //   clock_val_less_than_clock_t{y_clks.at(k), x_clks.at(k)},
        //   clock_val_less_than_clock_t{x_clks.at(k), x_clks.at(k - 1)},
          clock_abs_val_in_interval_t{x_clks.at(k), interval()},
      });
      all_progs.emplace_back(
          general_until_prog_t{third_else1, k + 1, node_value_t::TRUE});

  // Activate new pair of clocks x_{k+1}, y_{k+1}
  // Check  x_{k+1} < I and y_{k+1} > I
  // Output FALSE
      std::vector<mitl2gta::transducer::gta_program_t> third_else2({
          clock_abs_val_less_than_interval_t{x, interval()},
          clock_abs_val_greater_than_interval_t{y, interval()},
          clock_abs_val_less_than_interval_t{y_clks.at(k - 1), interval()},
          release_reset_clock_t{x_clks.at(k)},
          release_reset_clock_t{y_clks.at(k)},
          clock_abs_val_greater_than_interval_t{y_clks.at(k), interval()},
        //   clock_val_less_than_clock_t{y, y_clks.at(k)},
        //   clock_val_less_than_clock_t{y_clks.at(k), x_clks.at(k)},
        //   clock_val_less_than_clock_t{x_clks.at(k), x_clks.at(k - 1)},
          clock_abs_val_less_than_interval_t{x_clks.at(k), interval()},
      });
      all_progs.emplace_back(
          general_until_prog_t{third_else2, k + 1, node_value_t::FALSE});
    }

    // Next, we add the transitions from state (k,1) and (k,2) for each of the gta_programs generated above. These transitions correspond to lines 14-35 of Algorithm 2. We will first add the transitions from (k,1) in the first for loop below, and then we will add the transitions from (k,2) in the second for loop below.

    // Since all these transitions should satisfy p and X(p U q), we add the corresponding checks to each of these transitions. 

    // For-loop to add the transitions from (k,1) for each gta_program generated above.
    for (auto const &prog : all_progs) {
      std::vector<mitl2gta::transducer::gta_program_t> gta_prog = prog.program;
      int kprime = prog.next_state_index;
      node_value_t val = prog.output;

      // The program defined above provides three attributes for the transitions
      // 1. program - The gta program associated with the transition  
      // 2. kprime - The value of k after the execution of lines 10-25. This value implies the target state - whether it should go to k itself or k+1       
      // 3. val - The associated output of the transition   


      // We split the transition 34 from (k,1) to (k',1) into two cases, depending on whether q is true or false.

      // (k,1) -!q -> (k',1)
      // Line 24 of Algorithm 2 case !q
      edges.emplace_back(mitl2gta::transducer::edge_t(
          locations_1.at(k).id(), locations_1.at(kprime).id(),
          {on_node_values_t{{{lchild(), node_value_t::TRUE}, 
                             {rchild(), node_value_t::FALSE}}}},
          {provided_memory_value_t{p_until_q_sharer.next_p_until_q_truth_value,
                                   mitl2gta::sharer::SHARER_TRUE_VAL}},
          {set_node_value_t{id(), val}}, gta_prog));

      // (k,1) -q-> (k',1)
      // But not the last q at the timestamp
      // Line 34 of Algorithm 2
      edges.emplace_back(mitl2gta::transducer::edge_t(
          locations_1.at(k).id(), locations_1.at(kprime).id(),
          {on_node_values_t{{{lchild(), node_value_t::TRUE}, 
                             {rchild(), node_value_t::TRUE}}}},
          {provided_memory_value_t{p_until_q_sharer.next_p_until_q_truth_value,
                                   mitl2gta::sharer::SHARER_TRUE_VAL}},
          {set_node_value_t{id(), val}}, gta_prog));
      // We already dealt with the case when !q is true in line 24, and we are now in the case when q is true. 

      // (k,1) -q-> (k',2)
      // Last q at the timestamp, so we need to check x_1 == 0 to make sure this is the last witness at the timestamp, and then we release and invalidate the clock x_1
      // Line 35 of Algorithm 2
      gta_prog.emplace_back(clock_val_equal_to_t{x_clks.at(0), 0});
      gta_prog.emplace_back(release_reset_clock_t{x_clks.at(0)});
      gta_prog.emplace_back(
          clock_val_equal_to_t{x_clks.at(0), mitl2gta::EXTENDED_MINUS_INF});

      edges.emplace_back(mitl2gta::transducer::edge_t(
          locations_1.at(k).id(), locations_2.at(kprime).id(),
          {on_node_values_t{{{lchild(), node_value_t::TRUE},
                             {rchild(), node_value_t::TRUE}}}},
          {provided_memory_value_t{p_until_q_sharer.next_p_until_q_truth_value,
                                   mitl2gta::sharer::SHARER_TRUE_VAL}},
          {set_node_value_t{id(), val}}, gta_prog));
    }

    // For-loop to add the transitions from (k,2) for each gta_program generated above.
    for (auto const &prog : all_progs) {
      std::vector<mitl2gta::transducer::gta_program_t> gta_prog = prog.program;
      int const kprime = prog.next_state_index;
      node_value_t val = prog.output;

      // The program defined above provides three attributes for the transitions
      // 1. program - The gta program associated with the transition  
      // 2. kprime - The value of k after the execution of lines 10-25. This value implies the target state - whether it should go to k itself or k-1
      // 3. val - The associated output of the transition   

        //Lines 12-22 of Algorithm2  
        // (k,2) -!q-> (k',2)    
        //Lines 25 of Algorithm2  
        edges.emplace_back(mitl2gta::transducer::edge_t(
          locations_2.at(k).id(), locations_2.at(kprime).id(),
          {on_node_values_t{{{lchild(), node_value_t::TRUE},
                             {rchild(), node_value_t::FALSE}}}},
          {provided_memory_value_t{p_until_q_sharer.next_p_until_q_truth_value,
                                   mitl2gta::sharer::SHARER_TRUE_VAL}},
          {set_node_value_t{id(), val}}, gta_prog));

        //Lines 28 of Algorithm2  
      gta_prog.emplace_back(clock_val_equal_to_t{y_clks.at(0), 0});
      gta_prog.emplace_back(release_reset_clock_t{y_clks.at(0)});
      gta_prog.emplace_back(
          clock_val_equal_to_t{y_clks.at(0), mitl2gta::EXTENDED_MINUS_INF});

        //Lines 29 of Algorithm2  
      if (kprime == 1) {
        edges.emplace_back(mitl2gta::transducer::edge_t(
            locations_2.at(k).id(), locations_1.at(0).id(),
            {on_node_values_t{{{lchild(), node_value_t::TRUE},
                               {rchild(), node_value_t::TRUE}}}},
            {provided_memory_value_t{
                p_until_q_sharer.next_p_until_q_truth_value,
                mitl2gta::sharer::SHARER_TRUE_VAL}},
            {set_node_value_t{id(), val}}, gta_prog));
            //q has to be true as we just checked y_1 ==0     
            //Then, since this q is not the last witness (WE ALREADY CHECKED THIS ABOVE in lines 10-11), this also means p is true and X(p U q) is true  --- the last part is the routine check of the invariant
      }
      else {

        std::vector<mitl2gta::clock::clock_id_t> shiftx_clks;
        std::vector<mitl2gta::clock::clock_id_t> shifty_clks;

        for (std::size_t i = 0; i < kprime; i++) {
          shiftx_clks.emplace_back(x_clks.at(i));
          shifty_clks.emplace_back(y_clks.at(i));
        }

      // Line 30 of Algorithm 2
      // Adding clock shift operation to gta_prog
        gta_prog.emplace_back(shift_clocks_backward_t{shiftx_clks});
        gta_prog.emplace_back(shift_clocks_backward_t{shifty_clks});

      // (k,2) - -> (k'-1,1)
      // k' is the new k after lines 14-31
      // Line 34 of Algorithm 2
        edges.emplace_back(mitl2gta::transducer::edge_t(
            locations_2.at(k).id(), locations_1.at(kprime - 1).id(),
            {on_node_values_t{{{lchild(), node_value_t::TRUE},
                               {rchild(), node_value_t::TRUE}}}},
            {provided_memory_value_t{
                p_until_q_sharer.next_p_until_q_truth_value,
                mitl2gta::sharer::SHARER_TRUE_VAL}},
            {set_node_value_t{id(), val}}, gta_prog));
      // We already dealt with the case when !q is true in line 26, and we are now in the case when q is true. 
      // If q is true, we cannot stay in (k,2) because that would mean that the latest witness is still active, which contradicts the fact that we just checked y_1 == 0. So, we have to move to (k'-1,1) because after the clock shift, the new x_1 (which is the old x_2) should be 0, which means that the earliest witness is active. 
        gta_prog.emplace_back(clock_val_equal_to_t{x_clks.at(0), 0});
        gta_prog.emplace_back(release_reset_clock_t{x_clks.at(0)});
        gta_prog.emplace_back(
            clock_val_equal_to_t{x_clks.at(0), mitl2gta::EXTENDED_MINUS_INF});

      // (k,2) - -> (k'-1,2)
      // k' is the new k after lines 14-31
      // Line 35 of Algorithm 2
        edges.emplace_back(mitl2gta::transducer::edge_t(
            locations_2.at(k).id(), locations_2.at(kprime - 1).id(),
            {on_node_values_t{{{lchild(), node_value_t::TRUE},
                               {rchild(), node_value_t::TRUE}}}},
            {provided_memory_value_t{
                p_until_q_sharer.next_p_until_q_truth_value,
                mitl2gta::sharer::SHARER_TRUE_VAL}},
            {set_node_value_t{id(), val}}, gta_prog));
      }
    }
  }

  std::vector<mitl2gta::transducer::state_t> states;

  for (auto const &s : locations_1) {
    states.emplace_back(s);
  }

  for (int i = 1; i < locations_2.size(); i++) {
    states.emplace_back(locations_2.at(i));
  }

  mitl2gta::transducer::transducer_t t{std::move(states), std::move(edges)};

  return mitl2gta::compilation::predicted_by_transducer_t{t};
}

std::set<mitl2gta::compilation::node_id_t>
compiled_timed_until_t::direct_dependencies() const {
  return {_lchild, _rchild};
};

// compiled_upper_bounded_until_t
compiled_upper_bounded_until_t::compiled_upper_bounded_until_t(
    mitl2gta::interval_bound_t const upper_bound,
    mitl2gta::compilation::node_id_t lchild,
    mitl2gta::compilation::node_id_t rchild,
    mitl2gta::compilation::compiled_node_id_allocator_t &allocator)
    : mitl2gta::compilation::compiled_node_t(allocator),
      mitl2gta::compilation::upper_bounded_timed_node_t(upper_bound),
      mitl2gta::compilation::binary_node_t(lchild, rchild) {}

std::set<mitl2gta::compilation::node_id_t>
compiled_upper_bounded_until_t::direct_dependencies() const {
  return {_lchild, _rchild};
};

mitl2gta::compilation::truth_value_predictor_t
compiled_upper_bounded_until_t::generate_truth_value_predictor(
    mitl2gta::transducer::transducer_construction_tools_t &tools,
    mitl2gta::compilation::translation_specs_t const &specs) const {
  mitl2gta::transducer::state_t s1(tools.state_id_allocator);
  s1.make_initial();

  mitl2gta::sharer::p_until_q_sharer_t const p_until_q =
      tools.sharer_registry.request_p_until_q_sharer(
          id(), lchild(), rchild(),
          mitl2gta::sharer::p_until_q_sharer_type_t::TIMED_FIRST_WITNESS,
          tools.clk_registry, tools.mem_handler);

  mitl2gta::clock::clock_id_t const first_witness_clk =
      p_until_q.first_witness_predicting_clk.value();

  mitl2gta::transducer::edge_t const e1(
      s1.id(), s1.id(), on_node_values_t{{{rchild(), node_value_t::TRUE}}}, {},
      {set_node_value_t{_id, node_value_t::TRUE}}, {});

  mitl2gta::transducer::edge_t const e2(
      s1.id(), s1.id(), on_node_values_t{{{rchild(), node_value_t::FALSE}}},
      {provided_memory_value_t{p_until_q.p_until_q_truth_value,
                               mitl2gta::sharer::SHARER_FALSE_VAL}},
      {set_node_value_t{_id, node_value_t::FALSE}}, {});

  mitl2gta::transducer::edge_t const e3(
      s1.id(), s1.id(), on_node_values_t{{{rchild(), node_value_t::FALSE}}},
      {provided_memory_value_t{p_until_q.p_until_q_truth_value,
                               mitl2gta::sharer::SHARER_TRUE_VAL}},
      {set_node_value_t{_id, node_value_t::TRUE}},
      {clock_abs_val_in_interval_t{first_witness_clk, interval()}});

  mitl2gta::transducer::edge_t const e4(
      s1.id(), s1.id(), on_node_values_t{{{rchild(), node_value_t::FALSE}}},
      {provided_memory_value_t{p_until_q.p_until_q_truth_value,
                               mitl2gta::sharer::SHARER_TRUE_VAL}},
      {set_node_value_t{_id, node_value_t::FALSE}},
      {clock_abs_val_not_in_interval_t{first_witness_clk, interval()}});

  mitl2gta::transducer::transducer_t t{{s1}, {e1, e2, e3, e4}};

  return mitl2gta::compilation::predicted_by_transducer_t{t};
}

// compiled_lower_bounded_until_t
compiled_lower_bounded_until_t::compiled_lower_bounded_until_t(
    mitl2gta::interval_bound_t const lower_bound,
    mitl2gta::compilation::node_id_t lchild,
    mitl2gta::compilation::node_id_t rchild,
    mitl2gta::compilation::compiled_node_id_allocator_t &allocator)
    : mitl2gta::compilation::compiled_node_t(allocator),
      mitl2gta::compilation::lower_bounded_timed_node_t(lower_bound),
      mitl2gta::compilation::binary_node_t(lchild, rchild) {}

std::set<mitl2gta::compilation::node_id_t>
compiled_lower_bounded_until_t::direct_dependencies() const {
  return {_lchild, _rchild};
};

mitl2gta::compilation::truth_value_predictor_t
compiled_lower_bounded_until_t::generate_truth_value_predictor(
    mitl2gta::transducer::transducer_construction_tools_t &tools,
    mitl2gta::compilation::translation_specs_t const &specs) const {
  mitl2gta::transducer::state_t s1(tools.state_id_allocator);
  s1.make_initial();

  mitl2gta::sharer::p_until_q_sharer_t const p_until_q =
      tools.sharer_registry.request_p_until_q_sharer(
          id(), lchild(), rchild(),
          mitl2gta::sharer::p_until_q_sharer_type_t::
              TIMED_FIRST_AND_LAST_WITNESS,
          tools.clk_registry, tools.mem_handler);

  mitl2gta::clock::clock_id_t const last_witness_clk =
      p_until_q.last_witness_predicting_clk.value();

  mitl2gta::transducer::edge_t const e1(
      s1.id(), s1.id(), on_node_values_t{{{lchild(), node_value_t::FALSE}}}, {},
      {set_node_value_t{_id, node_value_t::FALSE}}, {});

  mitl2gta::transducer::edge_t const e2(
      s1.id(), s1.id(), on_node_values_t{{{lchild(), node_value_t::TRUE}}},
      {provided_memory_value_t{p_until_q.next_p_until_q_truth_value,
                               mitl2gta::sharer::SHARER_FALSE_VAL}},
      {set_node_value_t{_id, node_value_t::FALSE}}, {});

  mitl2gta::transducer::edge_t const e3(
      s1.id(), s1.id(), on_node_values_t{{{lchild(), node_value_t::TRUE}}},
      {provided_memory_value_t{p_until_q.next_p_until_q_truth_value,
                               mitl2gta::sharer::SHARER_TRUE_VAL}},
      {set_node_value_t{_id, node_value_t::FALSE}},
      {clock_abs_val_not_in_interval_t{last_witness_clk, interval()}});

  mitl2gta::transducer::edge_t const e4(
      s1.id(), s1.id(), on_node_values_t{{{lchild(), node_value_t::TRUE}}},
      {provided_memory_value_t{p_until_q.next_p_until_q_truth_value,
                               mitl2gta::sharer::SHARER_TRUE_VAL}},
      {set_node_value_t{_id, node_value_t::TRUE}},
      {clock_abs_val_in_interval_t{last_witness_clk, interval()}});

  mitl2gta::transducer::transducer_t t{{s1}, {e1, e2, e3, e4}};

  return mitl2gta::compilation::predicted_by_transducer_t{t};
}
} // namespace compilation

} // namespace mitl2gta
