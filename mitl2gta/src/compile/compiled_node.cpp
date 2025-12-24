/*
 * This file is a part of MITL2GTA project.
 *
 * See AUTHORS and LICENSE for copyright details.
 *
 */

#include "mitl2gta/compile/compiled_node.hpp"

namespace mitl2gta {

namespace compilation {

// compiled_node_t
compiled_node_t::compiled_node_t(
    mitl2gta::compilation::compiled_node_id_allocator_t &allocator)
    : _id(allocator.allocate_id()) {}

// unary_node_t
unary_node_t::unary_node_t(mitl2gta::compilation::node_id_t child)
    : _child(child) {}

// binary_node_t
binary_node_t::binary_node_t(mitl2gta::compilation::node_id_t lchild,
                             mitl2gta::compilation::node_id_t rchild)
    : _lchild(lchild), _rchild(rchild) {}

// timed_node_t
timed_node_t::timed_node_t(mitl2gta::nonempty_interval_t const interval)
    : _interval(interval){};

// upper_bounded_timed_node_t
upper_bounded_timed_node_t::upper_bounded_timed_node_t(
    mitl2gta::interval_bound_t const upper_bound)
    : mitl2gta::compilation::timed_node_t(mitl2gta::nonempty_interval_t(
          mitl2gta::ZERO_BOUND_CLOSED, upper_bound)) {}

// lower_bounded_timed_node_t
lower_bounded_timed_node_t::lower_bounded_timed_node_t(
    mitl2gta::interval_bound_t const lower_bound)
    : mitl2gta::compilation::timed_node_t(
          mitl2gta::nonempty_interval_t(lower_bound, mitl2gta::INF_BOUND)) {}

} // namespace compilation

} // namespace mitl2gta
