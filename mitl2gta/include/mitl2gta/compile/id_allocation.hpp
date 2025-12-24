/*
 * This file is a part of MITL2GTA project.
 *
 * See AUTHORS and LICENSE for copyright details.
 *
 */

#pragma once

#include "mitl2gta/utils/id_allocator.hpp"

namespace mitl2gta {

namespace compilation {

using node_id_t = mitl2gta::id_t;

class compiled_node_id_allocator_t : public mitl2gta::id_allocator_t {};

} // namespace compilation
} // namespace mitl2gta
