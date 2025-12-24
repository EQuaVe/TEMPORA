/*
 * This file is a part of MITL2GTA project.
 *
 * See AUTHORS and LICENSE for copyright details.
 *
 */

#pragma once

#include <string>

#include "mitl2gta/utils/id_allocator.hpp"

namespace mitl2gta {

namespace transducer {

using state_id_t = mitl2gta::id_t;

std::string name_from_id(mitl2gta::id_t const id);

class state_id_allocator_t : public mitl2gta::id_allocator_t {};

class state_t {
public:
  explicit state_t(mitl2gta::transducer::state_id_allocator_t &allocator);
  ~state_t() = default;

  state_t(state_t const &) = default;
  state_t(state_t &&) = default;
  state_t &operator=(state_t const &) = default;
  state_t &operator=(state_t &&) = default;

  inline mitl2gta::transducer::state_id_t id() const { return _id; }
  inline bool is_initial() const { return _is_initial; }
  inline bool is_final() const { return _is_final; }
  inline bool is_urgent() const { return _is_urgent; }

  mitl2gta::transducer::state_t &make_initial();
  mitl2gta::transducer::state_t &make_final();
  mitl2gta::transducer::state_t &make_urgent();

private:
  mitl2gta::transducer::state_id_t _id;
  bool _is_initial;
  bool _is_final;
  bool _is_urgent;
};

} // namespace transducer

} // namespace mitl2gta
