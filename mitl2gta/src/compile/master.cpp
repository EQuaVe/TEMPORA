/*
 * This file is a part of MITL2GTA project.
 *
 * See AUTHORS and LICENSE for copyright details.
 *
 */

#include "mitl2gta/compile/master.hpp"

namespace mitl2gta {
namespace compilation {

using mitl2gta::transducer::node_value_t;
using mitl2gta::transducer::on_node_values_t;

mitl2gta::transducer::transducer_t
construct_master(mitl2gta::compilation::node_id_t const root,
                 mitl2gta::transducer::transducer_construction_tools_t &tools) {
  mitl2gta::transducer::state_t init(tools.state_id_allocator);
  init.make_initial();

  mitl2gta::transducer::state_t acc(tools.state_id_allocator);
  acc.make_final();

  mitl2gta::transducer::edge_t const e1(
      init.id(), acc.id(), on_node_values_t{{{root, node_value_t::TRUE}}}, {},
      {}, {});

  mitl2gta::transducer::edge_t const e2(acc.id(), acc.id(), {}, {}, {}, {});

  return mitl2gta::transducer::transducer_t{{init, acc}, {e1, e2}};
}

mitl2gta::transducer::transducer_t construct_init_sat_master(
    mitl2gta::compilation::node_id_t const root,
    mitl2gta::transducer::system_for_t const system_for,
    mitl2gta::transducer::transducer_construction_tools_t &tools) {
  mitl2gta::transducer::state_t init(tools.state_id_allocator);
  init.make_initial();

  mitl2gta::transducer::state_t acc(tools.state_id_allocator);
  acc.make_final();

  std::vector<mitl2gta::transducer::edge_t> edges;

  switch (system_for) {
  case mitl2gta::transducer::system_for_t::FINITE_TRACE: {
    edges.emplace_back(mitl2gta::transducer::edge_t(
        init.id(), init.id(), on_node_values_t{{{root, node_value_t::FALSE}}},
        {}, {}, {}));

    edges.emplace_back(mitl2gta::transducer::edge_t(
        init.id(), acc.id(), on_node_values_t{{{root, node_value_t::TRUE}}}, {},
        {}, {}));

    edges.emplace_back(mitl2gta::transducer::edge_t(
        acc.id(), init.id(), on_node_values_t{{{root, node_value_t::FALSE}}},
        {}, {}, {}));

    edges.emplace_back(mitl2gta::transducer::edge_t(
        acc.id(), acc.id(), on_node_values_t{{{root, node_value_t::TRUE}}}, {},
        {}, {}));
    break;
  }

  case mitl2gta::transducer::system_for_t::INFINITE_TRACE: {
    edges.emplace_back(mitl2gta::transducer::edge_t(
        init.id(), init.id(),
        on_node_values_t{{{root, node_value_t::UNCERTAIN}}}, {}, {}, {}));
    edges.emplace_back(mitl2gta::transducer::edge_t(
        init.id(), acc.id(), on_node_values_t{{{root, node_value_t::TRUE}}}, {},
        {}, {}));
    edges.emplace_back(mitl2gta::transducer::edge_t(
        acc.id(), acc.id(), on_node_values_t{}, {}, {}, {}));
    break;
  }
  }

  return mitl2gta::transducer::transducer_t{{init, acc}, edges};
}

} // namespace compilation
} // namespace mitl2gta
