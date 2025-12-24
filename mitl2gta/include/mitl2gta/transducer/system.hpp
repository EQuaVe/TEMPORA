/*
 * This file is a part of MITL2GTA project.
 *
 * See AUTHORS and LICENSE for copyright details.
 *
 */

#pragma once

#include <deque>

#include "mitl2gta/transducer/transducer.hpp"

namespace mitl2gta {

namespace transducer {

enum system_for_t {
  FINITE_TRACE,
  INFINITE_TRACE,
};

class system_t {
public:
  system_t(mitl2gta::transducer::system_for_t const system_for);
  ~system_t() = default;

  system_t(system_t const &) = delete;
  system_t(system_t &&) = default;
  system_t &operator=(system_t const &) = delete;
  system_t &operator=(system_t &&) = delete;

  inline mitl2gta::transducer::system_for_t system_for() const {
    return _system_for;
  }

  inline mitl2gta::transducer::state_id_allocator_t &state_id_allocator_t() {
    return _state_id_allocator;
  }

  inline mitl2gta::memory::memory_handler_t &memory_handler() {
    return _memory_handler;
  }

  inline mitl2gta::clock::clock_registry_t &clock_registry() {
    return _clock_registry;
  }

  inline std::deque<mitl2gta::transducer::transducer_t> &transducers() {
    return _transducers;
  }

  inline std::deque<mitl2gta::transducer::transducer_t> const &
  transducers() const {
    return _transducers;
  }

  void add_transducer(mitl2gta::transducer::transducer_t const transducer);

private:
  mitl2gta::transducer::system_for_t const _system_for;

  std::deque<mitl2gta::transducer::transducer_t> _transducers;

  mitl2gta::transducer::state_id_allocator_t _state_id_allocator;
  mitl2gta::memory::memory_handler_t _memory_handler;
  mitl2gta::clock::clock_registry_t _clock_registry;
};

} // namespace transducer

} // namespace mitl2gta
