/*
 * This file is a part of MITL2GTA project.
 *
 * See AUTHORS and LICENSE for copyright details.
 *
 */

#pragma once
#include <ostream>

#include "mitl2gta/interval/bound.hpp"

namespace mitl2gta {

enum interval_type_t {
  UNBOUNDED,
  UPPER_BOUNDED,
  LOWER_BOUNDED,
  LOWER_UPPER_BOUNDED,
};

class nonempty_interval_t {
public:
  nonempty_interval_t(mitl2gta::interval_bound_t const &lower_bound,
                      mitl2gta::interval_bound_t const &upper_bound);

  nonempty_interval_t(nonempty_interval_t const &) = default;
  nonempty_interval_t(nonempty_interval_t &&) = default;
  nonempty_interval_t &operator=(nonempty_interval_t const &) = default;
  nonempty_interval_t &operator=(nonempty_interval_t &&) = default;

  ~nonempty_interval_t() = default;

  inline mitl2gta::interval_bound_t lower_bound() const { return _lower_bound; }

  inline mitl2gta::interval_bound_t upper_bound() const { return _upper_bound; }

  std::ostream &output(std::ostream &os) const;

protected:
  mitl2gta::interval_bound_t _lower_bound;
  mitl2gta::interval_bound_t _upper_bound;
};

// Returns true if both bounds of the interval are open
inline bool both_bounds_open(const nonempty_interval_t& interval) {
  return interval.lower_bound().bound_type == OPEN_BOUND &&
         interval.upper_bound().bound_type == OPEN_BOUND;
}

inline std::ostream &operator<<(std::ostream &os,
                                mitl2gta::nonempty_interval_t const &interval) {
  return interval.output(os);
}

enum mitl2gta::interval_type_t
interval_type(mitl2gta::nonempty_interval_t const &interval);

} // namespace mitl2gta
