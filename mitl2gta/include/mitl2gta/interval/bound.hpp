/*
 * This file is a part of MITL2GTA project.
 *
 * See AUTHORS and LICENSE for copyright details.
 *
 */

#pragma once
#include <limits>
#include <ostream>

namespace mitl2gta {

using boundval_t = int;

mitl2gta::boundval_t const INF_VAL = std::numeric_limits<boundval_t>::max();

std::ostream &output_boundval(std::ostream &os, mitl2gta::boundval_t const val);

enum bound_type_t {
  OPEN_BOUND,
  CLOSED_BOUND,
};

struct interval_bound_t {
  boundval_t val;
  bound_type_t bound_type;
};

bool operator==(mitl2gta::interval_bound_t const &bound1,
                mitl2gta::interval_bound_t const &bound2);

inline bool operator!=(mitl2gta::interval_bound_t const &bound1,
                       mitl2gta::interval_bound_t const &bound2) {
  return !(bound1 == bound2);
}

mitl2gta::interval_bound_t const INF_BOUND{INF_VAL,
                                           mitl2gta::bound_type_t::OPEN_BOUND};

mitl2gta::interval_bound_t const ZERO_BOUND_CLOSED{
    0, mitl2gta::bound_type_t::CLOSED_BOUND};

mitl2gta::interval_bound_t finite_bound(boundval_t const val,
                                        enum bound_type_t const bound_type);

bool nonempty_interval_bounds(mitl2gta::interval_bound_t const &lower_bound,
                              mitl2gta::interval_bound_t const &upper_bound);
} // namespace mitl2gta
