/*
 * This file is a part of MITL2GTA project.
 *
 * See AUTHORS and LICENSE for copyright details.
 *
 */

#pragma once

#include <cstddef>

namespace mitl2gta {

using id_t = std::size_t;

class id_allocator_t {
public:
  id_allocator_t();

  id_allocator_t(id_allocator_t const &) = delete;
  id_allocator_t(id_allocator_t &&) = default;
  id_allocator_t &operator=(id_allocator_t const &) = delete;
  id_allocator_t &operator=(id_allocator_t &&) = default;

  mitl2gta::id_t allocate_id();

private:
  mitl2gta::id_t _next_id;
};

} // namespace mitl2gta
