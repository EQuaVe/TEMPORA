
/*
 * This file is a part of MITL2GTA project.
 *
 * See AUTHORS and LICENSE for copyright details.
 *
 */

#include <stdexcept>
#include <string>

#include "mitl2gta/interval/bound.hpp"

namespace mitl2gta {

std::ostream &output_boundval(std::ostream &os,
                              mitl2gta::boundval_t const val) {
  if (val == mitl2gta::INF_VAL) {
    return os << "Inf";
  }

  return os << val;
}

bool operator==(mitl2gta::interval_bound_t const &bound1,
                mitl2gta::interval_bound_t const &bound2) {
  return (bound1.val == bound2.val && bound1.bound_type == bound2.bound_type);
}

mitl2gta::interval_bound_t finite_bound(boundval_t const val,
                                        enum bound_type_t const bound_type) {
  if (val >= mitl2gta::INF_VAL) {
    throw std::invalid_argument("Bound value not finite: " +
                                std::to_string(val));
  }

  return mitl2gta::interval_bound_t{val, bound_type};
}

bool nonempty_interval_bounds(mitl2gta::interval_bound_t const &lower_bound,
                              mitl2gta::interval_bound_t const &upper_bound) {
  if (lower_bound.val != upper_bound.val) {
    return (lower_bound.val < upper_bound.val);
  }

  // (a, a), [a, a) and (a, a] are empty intervals
  bool const is_nonempty_singleton =
      (lower_bound.bound_type == mitl2gta::bound_type_t::CLOSED_BOUND &&
       upper_bound.bound_type == mitl2gta::bound_type_t::CLOSED_BOUND);

  return is_nonempty_singleton;
}

} // namespace mitl2gta
