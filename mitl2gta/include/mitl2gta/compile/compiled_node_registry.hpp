/*
 * This file is a part of MITL2GTA project.
 *
 * See AUTHORS and LICENSE for copyright details.
 *
 */

#pragma once

#include <map>
#include <memory>

#include "mitl2gta/compile/compiled_node.hpp"
#include "mitl2gta/compile/id_allocation.hpp"

namespace mitl2gta {

namespace compilation {

using id_to_node_map_t =
    std::map<mitl2gta::compilation::node_id_t,
             std::shared_ptr<mitl2gta::compilation::compiled_node_t const>>;

class compiled_node_registry_t {

public:
  compiled_node_registry_t();

  compiled_node_registry_t(compiled_node_registry_t const &) = delete;
  compiled_node_registry_t(compiled_node_registry_t &&) = default;
  compiled_node_registry_t &
  operator=(compiled_node_registry_t const &) = delete;
  compiled_node_registry_t &operator=(compiled_node_registry_t &&) = default;

  std::shared_ptr<mitl2gta::compilation::compiled_node_t const> const &
  node(mitl2gta::compilation::node_id_t id) const;

  mitl2gta::compilation::node_id_t request_atomic_proposition();

  mitl2gta::compilation::node_id_t
  request_atomic_constant(enum mitl2gta::mitl::constant_value_t const val);

  mitl2gta::compilation::node_id_t
  request_negation(mitl2gta::compilation::node_id_t const child);

  mitl2gta::compilation::node_id_t
  request_or(mitl2gta::compilation::node_id_t const lchild,
             mitl2gta::compilation::node_id_t const rchild);

  mitl2gta::compilation::node_id_t
  request_and(mitl2gta::compilation::node_id_t const lchild,
              mitl2gta::compilation::node_id_t const rchild);

  mitl2gta::compilation::node_id_t
  request_untimed_next(mitl2gta::compilation::node_id_t const child);

  mitl2gta::compilation::node_id_t
  request_timed_next(mitl2gta::nonempty_interval_t const interval,
                     mitl2gta::compilation::node_id_t const child);

  mitl2gta::compilation::node_id_t
  request_untimed_until(mitl2gta::compilation::node_id_t const lchild,
                        mitl2gta::compilation::node_id_t const rchild);

  mitl2gta::compilation::node_id_t
  request_timed_until(mitl2gta::nonempty_interval_t const interval,
                      mitl2gta::compilation::node_id_t const lchild,
                      mitl2gta::compilation::node_id_t const rchild);

  mitl2gta::compilation::node_id_t
  request_untimed_finally(mitl2gta::compilation::node_id_t const child);

  mitl2gta::compilation::node_id_t
  request_timed_finally(mitl2gta::nonempty_interval_t const interval,
                        mitl2gta::compilation::node_id_t const child);

  mitl2gta::compilation::node_id_t
  request_untimed_globally(mitl2gta::compilation::node_id_t const child);

  mitl2gta::compilation::node_id_t
  request_timed_globally(mitl2gta::nonempty_interval_t const interval,
                         mitl2gta::compilation::node_id_t const child);

  mitl2gta::compilation::node_id_t
  request_untimed_yesterday(mitl2gta::compilation::node_id_t const child);

  mitl2gta::compilation::node_id_t
  request_timed_yesterday(mitl2gta::nonempty_interval_t const interval,
                          mitl2gta::compilation::node_id_t const child);

  mitl2gta::compilation::node_id_t
  request_untimed_since(mitl2gta::compilation::node_id_t const lchild,
                        mitl2gta::compilation::node_id_t const rchild);

  mitl2gta::compilation::node_id_t
  request_timed_since(mitl2gta::nonempty_interval_t const interval,
                      mitl2gta::compilation::node_id_t const lchild,
                      mitl2gta::compilation::node_id_t const rchild);

private:
  template <class T, typename... Args>
  mitl2gta::compilation::node_id_t create_and_add_node(Args... args);

  mitl2gta::compilation::compiled_node_id_allocator_t _id_allocator;
  mitl2gta::compilation::id_to_node_map_t _id_to_node_map;
};

} // namespace compilation

} // namespace mitl2gta
