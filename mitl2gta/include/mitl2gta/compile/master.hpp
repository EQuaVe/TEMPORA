/*
 * This file is a part of MITL2GTA project.
 *
 * See AUTHORS and LICENSE for copyright details.
 *
 */

#pragma once

#include "mitl2gta/compile/id_allocation.hpp"
#include "mitl2gta/transducer/system.hpp"
#include "mitl2gta/transducer/transducer.hpp"

namespace mitl2gta {

namespace compilation {

mitl2gta::transducer::transducer_t
construct_master(mitl2gta::compilation::node_id_t const root,
                 mitl2gta::transducer::transducer_construction_tools_t &tools);

mitl2gta::transducer::transducer_t construct_init_sat_master(
    mitl2gta::compilation::node_id_t const root,
    mitl2gta::transducer::system_for_t const system_for,
    mitl2gta::transducer::transducer_construction_tools_t &tools);

} // namespace compilation
} // namespace mitl2gta
