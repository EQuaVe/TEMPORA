/*
 * This file is a part of MITL2GTA project.
 *
 * See AUTHORS and LICENSE for copyright details.
 *
 */

#include <memory>

#include "mitl2gta/compile/atomic_constant.hpp"
#include "mitl2gta/compile/atomic_proposition.hpp"
#include "mitl2gta/compile/booleans.hpp"
#include "mitl2gta/compile/compiled_node_registry.hpp"
#include "mitl2gta/compile/finally.hpp"
#include "mitl2gta/compile/next.hpp"
#include "mitl2gta/compile/since.hpp"
#include "mitl2gta/compile/until.hpp"
#include "mitl2gta/compile/yesterday.hpp"

namespace mitl2gta {

namespace compilation {

compiled_node_registry_t::compiled_node_registry_t() {}

template <class T, typename... Args>
mitl2gta::compilation::node_id_t
compiled_node_registry_t::create_and_add_node(Args... args) {
  auto node = std::make_shared<T>(args..., _id_allocator);

  mitl2gta::compilation::node_id_t const id = node->id();
  _id_to_node_map.insert({id, node});

  return id;
}

mitl2gta::compilation::node_id_t
compiled_node_registry_t::request_atomic_proposition() {
  return create_and_add_node<
      mitl2gta::compilation::compiled_atomic_proposition_t>();
}

mitl2gta::compilation::node_id_t
compiled_node_registry_t::request_atomic_constant(
    enum mitl2gta::mitl::constant_value_t const val) {
  return create_and_add_node<mitl2gta::compilation::compiled_atomic_constant_t>(
      val);
}

mitl2gta::compilation::node_id_t compiled_node_registry_t::request_negation(
    mitl2gta::compilation::node_id_t const child) {
  return create_and_add_node<mitl2gta::compilation::compiled_negation_t>(child);
}

mitl2gta::compilation::node_id_t compiled_node_registry_t::request_or(
    mitl2gta::compilation::node_id_t const lchild,
    mitl2gta::compilation::node_id_t const rchild) {
  return create_and_add_node<mitl2gta::compilation::compiled_or_t>(lchild,
                                                                   rchild);
}

mitl2gta::compilation::node_id_t compiled_node_registry_t::request_and(
    mitl2gta::compilation::node_id_t const lchild,
    mitl2gta::compilation::node_id_t const rchild) {
  return create_and_add_node<mitl2gta::compilation::compiled_and_t>(lchild,
                                                                    rchild);
}

mitl2gta::compilation::node_id_t compiled_node_registry_t::request_untimed_next(
    mitl2gta::compilation::node_id_t const child) {
  return create_and_add_node<mitl2gta::compilation::compiled_untimed_next_t>(
      child);
}
mitl2gta::compilation::node_id_t compiled_node_registry_t::request_timed_next(
    mitl2gta::nonempty_interval_t const interval,
    mitl2gta::compilation::node_id_t const child) {
  return create_and_add_node<mitl2gta::compilation::compiled_timed_next_t>(
      interval, child);
}

mitl2gta::compilation::node_id_t
compiled_node_registry_t::request_untimed_until(
    mitl2gta::compilation::node_id_t const lchild,
    mitl2gta::compilation::node_id_t const rchild) {
  return create_and_add_node<mitl2gta::compilation::compiled_untimed_until_t>(
      lchild, rchild);
}

mitl2gta::compilation::node_id_t compiled_node_registry_t::request_timed_until(
    mitl2gta::nonempty_interval_t const interval,
    mitl2gta::compilation::node_id_t const lchild,
    mitl2gta::compilation::node_id_t const rchild) {
  switch (mitl2gta::interval_type(interval)) {
  case mitl2gta::interval_type_t::UNBOUNDED: {
    return request_untimed_until(lchild, rchild);
  }
  case mitl2gta::interval_type_t::LOWER_BOUNDED: {
    return create_and_add_node<
        mitl2gta::compilation::compiled_lower_bounded_until_t>(
        interval.lower_bound(), lchild, rchild);
  }

  case mitl2gta::interval_type_t::UPPER_BOUNDED: {
    return create_and_add_node<
        mitl2gta::compilation::compiled_upper_bounded_until_t>(
        interval.upper_bound(), lchild, rchild);
  }

  case mitl2gta::interval_type_t::LOWER_UPPER_BOUNDED: {
    return create_and_add_node<mitl2gta::compilation::compiled_timed_until_t>(
        interval, lchild, rchild);
  }

  default: {
    throw std::runtime_error("Incomplete switch statement");
  }
  }
}

// F a <==> True U a
mitl2gta::compilation::node_id_t
compiled_node_registry_t::request_untimed_finally(
    mitl2gta::compilation::node_id_t const child) {
  mitl2gta::compilation::node_id_t const true_node =
      request_atomic_constant(mitl2gta::mitl::constant_value_t::BOOL_TRUE);

  return request_untimed_until(true_node, child);
}

mitl2gta::compilation::node_id_t
compiled_node_registry_t::request_timed_finally(
    mitl2gta::nonempty_interval_t const interval,
    mitl2gta::compilation::node_id_t const child) {
  switch (mitl2gta::interval_type(interval)) {

  case mitl2gta::interval_type_t::UNBOUNDED: {
    return request_untimed_finally(child);
  }

  case mitl2gta::interval_type_t::UPPER_BOUNDED: {
    return create_and_add_node<
        mitl2gta::compilation::compiled_upper_bounded_finally_t>(
        interval.upper_bound(), child);
  }

  case mitl2gta::interval_type_t::LOWER_BOUNDED:
  case mitl2gta::interval_type_t::LOWER_UPPER_BOUNDED: {
    mitl2gta::compilation::node_id_t const true_node =
        request_atomic_constant(mitl2gta::mitl::constant_value_t::BOOL_TRUE);

    return request_timed_until(interval, true_node, child);
  }

  default: {
    throw std::runtime_error("Incomplete switch statement");
  }
  }
}

// G a <==> !(F(!a))
mitl2gta::compilation::node_id_t
compiled_node_registry_t::request_untimed_globally(
    mitl2gta::compilation::node_id_t const child) {
  mitl2gta::compilation::node_id_t const negation_child =
      request_negation(child);
  mitl2gta::compilation::node_id_t const finally_negation_child =
      request_untimed_finally(negation_child);
  return request_negation(finally_negation_child);
}

mitl2gta::compilation::node_id_t
compiled_node_registry_t::request_timed_globally(
    mitl2gta::nonempty_interval_t const interval,
    mitl2gta::compilation::node_id_t const child) {
  mitl2gta::compilation::node_id_t const negation_child =
      request_negation(child);
  mitl2gta::compilation::node_id_t const finally_negation_child =
      request_timed_finally(interval, negation_child);
  return request_negation(finally_negation_child);
}

mitl2gta::compilation::node_id_t
compiled_node_registry_t::request_untimed_yesterday(
    mitl2gta::compilation::node_id_t const child) {
  return create_and_add_node<
      mitl2gta::compilation::compiled_untimed_yesterday_t>(child);
}

mitl2gta::compilation::node_id_t
compiled_node_registry_t::request_timed_yesterday(
    mitl2gta::nonempty_interval_t const interval,
    mitl2gta::compilation::node_id_t const child) {
  return create_and_add_node<mitl2gta::compilation::compiled_timed_yesterday_t>(
      interval, child);
}

mitl2gta::compilation::node_id_t
compiled_node_registry_t::request_untimed_since(
    mitl2gta::compilation::node_id_t const lchild,
    mitl2gta::compilation::node_id_t const rchild) {
  return create_and_add_node<mitl2gta::compilation::compiled_untimed_since_t>(
      lchild, rchild);
}

mitl2gta::compilation::node_id_t compiled_node_registry_t::request_timed_since(
    mitl2gta::nonempty_interval_t const interval,
    mitl2gta::compilation::node_id_t const lchild,
    mitl2gta::compilation::node_id_t const rchild) {
  switch (mitl2gta::interval_type(interval)) {
  case mitl2gta::interval_type_t::UNBOUNDED: {
    return request_untimed_since(lchild, rchild);
  }
  case mitl2gta::interval_type_t::LOWER_BOUNDED: {
    return create_and_add_node<
        mitl2gta::compilation::compiled_lower_bounded_since_t>(
        interval.lower_bound(), lchild, rchild);
  }

  case mitl2gta::interval_type_t::UPPER_BOUNDED: {
    return create_and_add_node<
        mitl2gta::compilation::compiled_upper_bounded_since_t>(
        interval.upper_bound(), lchild, rchild);
  }

  case mitl2gta::interval_type_t::LOWER_UPPER_BOUNDED: {
    return create_and_add_node<mitl2gta::compilation::compiled_timed_since_t>(
        interval, lchild, rchild);
  }

  default: {
    throw std::runtime_error("Incomplete switch statement");
  }
  }
}

std::shared_ptr<mitl2gta::compilation::compiled_node_t const> const &
compiled_node_registry_t::node(mitl2gta::compilation::node_id_t id) const {
  auto const &iter = _id_to_node_map.find(id);
  if (iter == _id_to_node_map.end()) {
    throw std::invalid_argument("Id not allocated by the registry");
  }

  return iter->second;
}
} // namespace compilation
} // namespace mitl2gta
