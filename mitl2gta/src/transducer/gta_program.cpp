/*
 * This file is a part of MITL2GTA project.
 *
 * See AUTHORS and LICENSE for copyright details.
 *
 */

#include <cassert>

#include "mitl2gta/transducer/edge_instructions.hpp"
#include "mitl2gta/transducer/gta_program.hpp"
#include "mitl2gta/utils/string_construction.hpp"

namespace mitl2gta {

namespace transducer {

namespace {

std::string const GUARD_DELIM = " && ";
std::string const ACTION_DELIM = ", ";

enum gta_program_type_t {
  GUARD,
  ACTION,
};

gta_program_type_t type(mitl2gta::transducer::gta_program_t const &prog) {
  return std::visit(
      [](auto &&arg) -> gta_program_type_t {
        using T = std::decay_t<decltype(arg)>;
        if constexpr (std::is_same_v<
                          T,
                          mitl2gta::transducer::clock_abs_val_in_interval_t>) {
          return gta_program_type_t::GUARD;
        }

        else if constexpr (std::is_same_v<
                               T, mitl2gta::transducer::
                                      clock_abs_val_not_in_interval_t>) {
          return gta_program_type_t::GUARD;
        }

        else if constexpr (std::is_same_v<
                               T, mitl2gta::transducer::clock_val_equal_to_t>) {
          return gta_program_type_t::GUARD;
        }

        else if constexpr (std::is_same_v<
                               T, mitl2gta::transducer::
                                      clock_abs_val_less_than_interval_t>) {
          return gta_program_type_t::GUARD;
        }

        else if constexpr (std::is_same_v<
                               T, mitl2gta::transducer::
                                      clock_abs_val_geq_lower_bound_t>) {
          return gta_program_type_t::GUARD;
        }

        else if constexpr (std::is_same_v<
                               T, mitl2gta::transducer::
                                      clock_abs_val_leq_upper_bound_t>) {
          return gta_program_type_t::GUARD;
        }

        else if constexpr (std::is_same_v<
                               T, mitl2gta::transducer::
                                      clock_abs_val_greater_than_interval_t>) {
          return gta_program_type_t::GUARD;
        }

        else if constexpr (std::is_same_v<
                               T,
                               mitl2gta::transducer::clock_val_greater_than>) {
          return gta_program_type_t::GUARD;
        }

        else if constexpr (std::is_same_v<
                               T,
                               mitl2gta::transducer::clock_val_less_equals_t>) {
          return gta_program_type_t::GUARD;
        }

        else if constexpr (std::is_same_v<
                               T,
                               mitl2gta::transducer::clock_val_less_than_t>) {
          return gta_program_type_t::GUARD;
        }

        else if constexpr (std::is_same_v<
                               T,
                               mitl2gta::transducer::release_reset_clock_t>) {
          return gta_program_type_t::ACTION;
        }

        else if constexpr (std::is_same_v<
                               T,
                               mitl2gta::transducer::shift_clocks_backward_t>) {
          return gta_program_type_t::ACTION;
        }
      },
      prog);
}

std::string clk_op_val(mitl2gta::clock::clock_id_t const id,
                       std::string const &op,
                       mitl2gta::extended_integer_t const val) {

  return mitl2gta::clock::clock_name(id) + op +
         mitl2gta::extended_int_string(val);
}

std::string clk_leq_val(mitl2gta::clock::clock_id_t const id,
                        mitl2gta::extended_integer_t const val) {
  return clk_op_val(id, "<=", val);
}

std::string clk_lt_val(mitl2gta::clock::clock_id_t const id,
                       mitl2gta::extended_integer_t const val) {
  return clk_op_val(id, "<", val);
}

std::string clk_geq_val(mitl2gta::clock::clock_id_t const id,
                        mitl2gta::extended_integer_t const val) {
  return clk_op_val(id, ">=", val);
}

std::string clk_gt_val(mitl2gta::clock::clock_id_t const id,
                       mitl2gta::extended_integer_t const val) {
  return clk_op_val(id, ">", val);
}

std::string clk_eq_val(mitl2gta::clock::clock_id_t const id,
                       mitl2gta::extended_integer_t const val) {
  return clk_op_val(id, "==", val);
}

std::string abs_clk_leq_val(mitl2gta::clock::clock_id_t const id,
                            mitl2gta::gta::gta_clock_type_t const type,
                            mitl2gta::extended_integer_t const val) {
  switch (type) {
  case mitl2gta::gta::gta_clock_type_t::HISTORY_ZERO:
  case mitl2gta::gta::gta_clock_type_t::HISTORY_INF: {
    return clk_leq_val(id, val);
  }

  case mitl2gta::gta::gta_clock_type_t::PROPHECY: {
    return clk_geq_val(id, mitl2gta::negate(val));
  }

  default:
    throw std::runtime_error("Incomplete switch statement");
  }
}

std::string abs_clk_lt_val(mitl2gta::clock::clock_id_t const id,
                           mitl2gta::gta::gta_clock_type_t const type,
                           mitl2gta::extended_integer_t const val) {
  switch (type) {
  case mitl2gta::gta::gta_clock_type_t::HISTORY_ZERO:
  case mitl2gta::gta::gta_clock_type_t::HISTORY_INF: {
    return clk_lt_val(id, val);
  }

  case mitl2gta::gta::gta_clock_type_t::PROPHECY: {
    return clk_gt_val(id, mitl2gta::negate(val));
  }

  default:
    throw std::runtime_error("Incomplete switch statement");
  }
}

std::string abs_clk_geq_val(mitl2gta::clock::clock_id_t const id,
                            mitl2gta::gta::gta_clock_type_t const type,
                            mitl2gta::extended_integer_t const val) {
  switch (type) {
  case mitl2gta::gta::gta_clock_type_t::HISTORY_ZERO:
  case mitl2gta::gta::gta_clock_type_t::HISTORY_INF: {
    return clk_geq_val(id, val);
  }

  case mitl2gta::gta::gta_clock_type_t::PROPHECY: {
    return clk_leq_val(id, mitl2gta::negate(val));
  }

  default:
    throw std::runtime_error("Incomplete switch statement");
  }
}

std::string abs_clk_gt_val(mitl2gta::clock::clock_id_t const id,
                           mitl2gta::gta::gta_clock_type_t const type,
                           mitl2gta::extended_integer_t const val) {
  switch (type) {
  case mitl2gta::gta::gta_clock_type_t::HISTORY_ZERO:
  case mitl2gta::gta::gta_clock_type_t::HISTORY_INF: {
    return clk_gt_val(id, val);
  }

  case mitl2gta::gta::gta_clock_type_t::PROPHECY: {
    return clk_lt_val(id, mitl2gta::negate(val));
  }

  default:
    throw std::runtime_error("Incomplete switch statement");
  }
}

mitl2gta::extended_integer_t
boundval_to_extended_int(mitl2gta::boundval_t const v) {
  if (v == mitl2gta::INF_VAL) {
    return mitl2gta::EXTENDED_INF;
  }
  return static_cast<mitl2gta::extended_integer_t>(v);
}

std::string
abs_clk_val_geq_lower_bound(mitl2gta::clock::clock_id_t const id,
                            mitl2gta::gta::gta_clock_type_t const type,
                            mitl2gta::nonempty_interval_t const &interval) {
  mitl2gta::extended_integer_t const lower_v =
      boundval_to_extended_int(interval.lower_bound().val);

  switch (interval.lower_bound().bound_type) {
  case mitl2gta::bound_type_t::CLOSED_BOUND: {
    return abs_clk_geq_val(id, type, lower_v);
  }
  case mitl2gta::bound_type_t::OPEN_BOUND: {
    return abs_clk_gt_val(id, type, lower_v);
  }
  }
  throw std::runtime_error("Incomplete switch statement");
}

std::string
abs_clk_val_leq_upper_bound(mitl2gta::clock::clock_id_t const id,
                            mitl2gta::gta::gta_clock_type_t const type,
                            mitl2gta::nonempty_interval_t const &interval) {
  mitl2gta::extended_integer_t const upper_v =
      boundval_to_extended_int(interval.upper_bound().val);

  switch (interval.upper_bound().bound_type) {
  case mitl2gta::bound_type_t::CLOSED_BOUND: {
    return abs_clk_leq_val(id, type, upper_v);
  }

  case mitl2gta::bound_type_t::OPEN_BOUND: {
    return abs_clk_lt_val(id, type, upper_v);
  }
  }
  throw std::runtime_error("Incomplete switch statement");
}

std::string
abs_clk_val_in_interval(mitl2gta::clock::clock_id_t const id,
                        mitl2gta::gta::gta_clock_type_t const type,
                        mitl2gta::nonempty_interval_t const &interval) {

  return abs_clk_val_geq_lower_bound(id, type, interval) + GUARD_DELIM +
         abs_clk_val_leq_upper_bound(id, type, interval);
}

std::string
abs_clk_val_lt_interval(mitl2gta::clock::clock_id_t const id,
                        mitl2gta::gta::gta_clock_type_t const type,
                        mitl2gta::nonempty_interval_t const &interval) {
  mitl2gta::extended_integer_t const lower_v =
      boundval_to_extended_int(interval.lower_bound().val);

  switch (interval.lower_bound().bound_type) {
  case mitl2gta::bound_type_t::CLOSED_BOUND: {
    return abs_clk_lt_val(id, type, lower_v);
  }
  case mitl2gta::bound_type_t::OPEN_BOUND: {
    return abs_clk_leq_val(id, type, lower_v);
  }

  default:
    throw std::runtime_error("Incomplete switch statement");
  }
}

std::string
abs_clk_val_gt_interval(mitl2gta::clock::clock_id_t const id,
                        mitl2gta::gta::gta_clock_type_t const type,
                        mitl2gta::nonempty_interval_t const &interval) {
  mitl2gta::extended_integer_t const upper_v =
      boundval_to_extended_int(interval.upper_bound().val);

  switch (interval.upper_bound().bound_type) {
  case mitl2gta::bound_type_t::CLOSED_BOUND: {
    return abs_clk_gt_val(id, type, upper_v);
  }
  case mitl2gta::bound_type_t::OPEN_BOUND: {
    return abs_clk_geq_val(id, type, upper_v);
  }

  default:
    throw std::runtime_error("Incomplete switch statement");
  }
}

std::vector<std::string>
abs_clk_not_in_interval(mitl2gta::clock::clock_id_t const id,
                        mitl2gta::gta::gta_clock_type_t const type,
                        mitl2gta::nonempty_interval_t const &interval) {
  std::vector<std::string> res;
  if (interval.lower_bound() != mitl2gta::ZERO_BOUND_CLOSED) {
    res.emplace_back(abs_clk_val_lt_interval(id, type, interval));
  }
  if (interval.upper_bound() != mitl2gta::INF_BOUND) {
    res.emplace_back(abs_clk_val_gt_interval(id, type, interval));
  }
  return res;
}

std::string reset_release_clk(mitl2gta::clock::clock_id_t const id) {
  return "[" + mitl2gta::clock::clock_name(id) + "]";
}

std::string
shift_clks_backward(std::vector<mitl2gta::clock::clock_id_t> const &ids) {
  if (ids.empty()) {
    throw std::invalid_argument("Cannot shift empty list of clocks");
  }

  std::vector<std::string> substrings;
  for (std::size_t i = 0; i < ids.size() - 1; i++) {
    substrings.push_back(mitl2gta::clock::clock_name(ids[i]) + "=" +
                         mitl2gta::clock::clock_name(ids[i + 1]));
  }

  return mitl2gta::join(substrings.begin(), substrings.end(), ACTION_DELIM);
}

std::vector<std::string>
gta_program_disjunction(mitl2gta::transducer::gta_program_t const &prog,
                        mitl2gta::clock::clock_registry_t const &reg) {
  return std::visit(
      [&reg](auto &&arg) -> std::vector<std::string> {
        using T = std::decay_t<decltype(arg)>;
        if constexpr (std::is_same_v<
                          T,
                          mitl2gta::transducer::clock_abs_val_in_interval_t>) {

          return {abs_clk_val_in_interval(arg.clk_id, reg.type(arg.clk_id),
                                          arg.interval)};
        }

        else if constexpr (std::is_same_v<
                               T, mitl2gta::transducer::
                                      clock_abs_val_not_in_interval_t>) {
          return abs_clk_not_in_interval(arg.clk_id, reg.type(arg.clk_id),
                                         arg.interval);
        }

        else if constexpr (std::is_same_v<
                               T, mitl2gta::transducer::clock_val_equal_to_t>) {
          return {clk_eq_val(arg.clk_id, arg.value)};
        }

        else if constexpr (std::is_same_v<
                               T,
                               mitl2gta::transducer::clock_val_greater_than>) {
          return {clk_gt_val(arg.clk_id, arg.value)};
        }

        else if constexpr (std::is_same_v<
                               T, mitl2gta::transducer::
                                      clock_abs_val_less_than_interval_t>) {
          if (arg.interval.lower_bound() == mitl2gta::ZERO_BOUND_CLOSED) {
            return {};
          }
          return {abs_clk_val_lt_interval(arg.clk_id, reg.type(arg.clk_id),
                                          arg.interval)};
        }

        else if constexpr (std::is_same_v<
                               T, mitl2gta::transducer::
                                      clock_abs_val_greater_than_interval_t>) {
          if (arg.interval.upper_bound() == mitl2gta::INF_BOUND) {
            return {};
          }
          return {abs_clk_val_gt_interval(arg.clk_id, reg.type(arg.clk_id),
                                          arg.interval)};
        }

        else if constexpr (std::is_same_v<
                               T, mitl2gta::transducer::
                                      clock_abs_val_geq_lower_bound_t>) {
          return {abs_clk_val_geq_lower_bound(arg.clk_id, reg.type(arg.clk_id),
                                              arg.interval)};
        }

        else if constexpr (std::is_same_v<
                               T, mitl2gta::transducer::
                                      clock_abs_val_leq_upper_bound_t>) {
          return {abs_clk_val_leq_upper_bound(arg.clk_id, reg.type(arg.clk_id),
                                              arg.interval)};
        }

        else if constexpr (std::is_same_v<
                               T,
                               mitl2gta::transducer::clock_val_less_equals_t>) {
          return {clk_leq_val(arg.clk_id, arg.value)};
        }

        else if constexpr (std::is_same_v<
                               T,
                               mitl2gta::transducer::clock_val_less_than_t>) {
          return {clk_lt_val(arg.clk_id, arg.value)};
        }

        else if constexpr (std::is_same_v<
                               T,
                               mitl2gta::transducer::release_reset_clock_t>) {
          return {reset_release_clk(arg.clk_id)};
        }

        else if constexpr (std::is_same_v<
                               T,
                               mitl2gta::transducer::shift_clocks_backward_t>) {
          return {shift_clks_backward(arg.clocks)};
        }
      },
      prog);
}

template <typename T>
std::vector<std::vector<T>>
add_elems_to_lists(std::vector<std::vector<T>> const &lists,
                   std::vector<T> const &elems) {
  std::vector<std::vector<T>> new_lists;

  if (lists.empty()) {
    for (auto const &b : elems) {
      new_lists.emplace_back(std::vector<T>({b}));
    }
    return new_lists;
  }

  for (auto const &a : lists) {
    for (auto const &b : elems) {
      auto new_list = a;
      new_list.push_back(b);
      new_lists.emplace_back(std::move(new_list));
    }
  }

  return new_lists;
}

} // namespace

std::vector<std::string> gta_programs_disjunction(
    std::vector<mitl2gta::transducer::gta_program_t> const &prog,
    mitl2gta::clock::clock_registry_t const &reg) {
  if (prog.empty()) {
    return std::vector<std::string>({""});
  }

  gta_program_type_t curr_type = GUARD;

  std::vector<std::vector<std::string>> disjunction_gta_prog_substrings;
  std::vector<std::vector<std::string>> disjunction_guards_substrings;
  std::vector<std::vector<std::string>> disjunction_actions_substrings;

  for (int i = 0; i < prog.size(); i++) {
    mitl2gta::transducer::gta_program_t const &s = prog[i];
    gta_program_type_t const s_type = type(s);

    std::vector<std::string> disjunction_gta_progs =
        gta_program_disjunction(s, reg);

    if (disjunction_gta_progs.empty()) {
      return std::vector<std::string>();
    }

    switch (s_type) {
    case GUARD: {
      disjunction_guards_substrings = add_elems_to_lists(
          disjunction_guards_substrings, disjunction_gta_progs);
      break;
    }
    case ACTION: {
      disjunction_actions_substrings = add_elems_to_lists(
          disjunction_actions_substrings, disjunction_gta_progs);
      break;
    }
    }

    if (s_type != curr_type) {
      switch (curr_type) {
      case GUARD: {
        std::vector<std::string> guards;
        for (auto const &g : disjunction_guards_substrings) {
          guards.emplace_back(mitl2gta::join(g.begin(), g.end(), GUARD_DELIM));
        }
        if (guards.empty()) {
          guards.emplace_back("");
        }
        disjunction_gta_prog_substrings =
            add_elems_to_lists(disjunction_gta_prog_substrings, guards);
        disjunction_guards_substrings.clear();
        break;
      }
      case ACTION: {
        std::vector<std::string> actions;
        for (auto const &a : disjunction_actions_substrings) {
          actions.emplace_back(
              mitl2gta::join(a.begin(), a.end(), ACTION_DELIM));
        }
        if (actions.empty()) {
          actions.emplace_back("");
        }
        disjunction_gta_prog_substrings =
            add_elems_to_lists(disjunction_gta_prog_substrings, actions);
        disjunction_actions_substrings.clear();
        break;
      }
      }
    }
    curr_type = s_type;
  }

  assert(disjunction_guards_substrings.empty() ||
         disjunction_actions_substrings.empty());

  if (!disjunction_guards_substrings.empty()) {
    std::vector<std::string> guards;
    for (auto const &g : disjunction_guards_substrings) {
      guards.emplace_back(mitl2gta::join(g.begin(), g.end(), GUARD_DELIM));
    }
    if (guards.empty()) {
      guards.push_back("");
    }
    disjunction_gta_prog_substrings =
        add_elems_to_lists(disjunction_gta_prog_substrings, guards);
    disjunction_guards_substrings.clear();
  }

  if (!disjunction_actions_substrings.empty()) {
    std::vector<std::string> actions;
    for (auto const &a : disjunction_actions_substrings) {
      actions.emplace_back(mitl2gta::join(a.begin(), a.end(), ACTION_DELIM));
    }
    if (actions.empty()) {
      actions.push_back("");
    }
    disjunction_gta_prog_substrings =
        add_elems_to_lists(disjunction_gta_prog_substrings, actions);
    disjunction_actions_substrings.clear();
  }

  std::vector<std::string> res;

  for (auto const &v : disjunction_gta_prog_substrings) {
    res.push_back(mitl2gta::join(v.begin(), v.end(), ";") + ";");
  }

  return res;
}

} // namespace transducer

} // namespace mitl2gta
