/*
 * This file is a part of MITL2GTA project.
 *
 * See AUTHORS and LICENSE for copyright details.
 *
 */

#include <stdexcept>

#include "mitl2gta/interval/interval.hpp"

namespace mitl2gta {

namespace {}

// nonempty_interval_t
nonempty_interval_t::nonempty_interval_t(
    mitl2gta::interval_bound_t const &lower_bound,
    mitl2gta::interval_bound_t const &upper_bound)
    : _lower_bound(lower_bound), _upper_bound(upper_bound) {
  if (not mitl2gta::nonempty_interval_bounds(lower_bound, upper_bound)) {
    throw std::invalid_argument(
        "Bounds are not valid for a non-empty interval");
  }
}

std::ostream &nonempty_interval_t::output(std::ostream &os) const {
  switch (_lower_bound.bound_type) {
  case mitl2gta::bound_type_t::OPEN_BOUND:
    os << "(";
    break;

  case mitl2gta::bound_type_t::CLOSED_BOUND:
    os << "[";
    break;

  default:
    throw std::runtime_error("Incomplete switch statement");
  }
  output_boundval(os, _lower_bound.val);
  os << ",";
  output_boundval(os, _upper_bound.val);

  switch (_upper_bound.bound_type) {
  case mitl2gta::bound_type_t::OPEN_BOUND:
    os << ")";
    break;

  case mitl2gta::bound_type_t::CLOSED_BOUND:
    os << "]";
    break;

  default:
    throw std::runtime_error("Incomplete switch statement");
  }
  return os;
}

enum mitl2gta::interval_type_t
interval_type(mitl2gta::nonempty_interval_t const &interval) {
  if (interval.lower_bound() == mitl2gta::ZERO_BOUND_CLOSED &&
      interval.upper_bound() == mitl2gta::INF_BOUND) {
    return mitl2gta::interval_type_t::UNBOUNDED;
  }

  else if (interval.lower_bound() == mitl2gta::ZERO_BOUND_CLOSED) {
    return mitl2gta::interval_type_t::UPPER_BOUNDED;
  }

  else if (interval.upper_bound() == mitl2gta::INF_BOUND) {
    return mitl2gta::interval_type_t::LOWER_BOUNDED;
  }

  else {
    return mitl2gta::interval_type_t::LOWER_UPPER_BOUNDED;
  }
}

} // namespace mitl2gta
