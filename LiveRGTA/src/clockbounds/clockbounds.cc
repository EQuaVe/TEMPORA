/*
 * This file is a part of the TChecker project.
 *
 * See files AUTHORS and LICENSE for copyright details.
 *
 */

#include <cassert>
#include <stdexcept>
#include <tuple>

#include "tchecker/basictypes.hh"
#include "tchecker/clockbounds/clockbounds.hh"

namespace tchecker {

namespace clockbounds {

/* map_t */

tchecker::clockbounds::map_t * allocate_map(std::size_t clock_nb)
{
  tchecker::clockbounds::map_t * map =
      tchecker::make_array_allocate_and_construct<tchecker::clockbounds::bound_t, sizeof(tchecker::clockbounds::bound_t),
                                                  tchecker::array_capacity_t<clock_id_t>>(clock_nb, std::make_tuple(clock_nb),
                                                                                          std::make_tuple(0));
  clear(*map);
  return map;
}

tchecker::clockbounds::map_t * clone_map(tchecker::clockbounds::map_t const & m)
{
  tchecker::clockbounds::map_t * clone = tchecker::clockbounds::allocate_map(m.capacity());
  for (tchecker::clock_id_t id = 0; id < m.capacity(); ++id)
    (*clone)[id] = m[id];
  return clone;
}

void deallocate_map(tchecker::clockbounds::map_t * m) { tchecker::make_array_destruct_and_deallocate(m); }

void clear(tchecker::clockbounds::map_t & map)
{
  for (tchecker::clock_id_t id = 0; id < map.capacity(); ++id)
    map[id] = tchecker::clockbounds::NO_BOUND;
}

bool update(tchecker::clockbounds::map_t & map, tchecker::clock_id_t id, tchecker::clockbounds::bound_t bound)
{
  assert(id < map.capacity());
  if (bound <= map[id])
    return false;
  map[id] = bound;
  return true;
}

bool update(tchecker::clockbounds::map_t & map, tchecker::clockbounds::map_t const & upd)
{
  assert(map.capacity() == upd.capacity());
  bool modified = false;
  for (tchecker::clock_id_t id = 0; id < map.capacity(); ++id)
    if (update(map, id, upd[id]))
      modified = true;
  return modified;
}

void copy(tchecker::clockbounds::map_t & dst, tchecker::clockbounds::map_t const & src)
{
  assert(dst.capacity() == src.capacity());
  for (tchecker::clock_id_t id = 0; id < src.capacity(); ++id)
    dst[id] = src[id];
}

std::ostream & operator<<(std::ostream & os, tchecker::clockbounds::map_t const & map)
{
  os << "[";
  for (std::size_t i = 0; i < map.capacity(); ++i) {
    if (i != 0)
      os << ",";

    if (map[i] == tchecker::clockbounds::NO_BOUND)
      os << ".";
    else
      os << map[i];
  }
  os << "]";
  return os;
}

/* local_lu_map_t */

local_lu_map_t::local_lu_map_t(tchecker::loc_id_t loc_nb, tchecker::clock_id_t clock_nb)
    : _loc_nb(0), _clock_nb(0), _L(_loc_nb, nullptr), _U(_loc_nb, nullptr)
{
  resize(loc_nb, clock_nb);
}

local_lu_map_t::local_lu_map_t(tchecker::clockbounds::local_lu_map_t const & m)
    : _loc_nb(m._loc_nb), _clock_nb(m._clock_nb), _L(_loc_nb, nullptr), _U(_loc_nb, nullptr)
{
  for (tchecker::loc_id_t id = 0; id < _loc_nb; ++id) {
    _L[id] = tchecker::clockbounds::clone_map(*m._L[id]);
    _U[id] = tchecker::clockbounds::clone_map(*m._U[id]);
  }
}

local_lu_map_t::local_lu_map_t(tchecker::clockbounds::local_lu_map_t && m)
    : _loc_nb(m._loc_nb), _clock_nb(m._clock_nb), _L(std::move(m._L)), _U(std::move(m._U))
{
  m._L.clear();
  m._U.clear();
  _loc_nb = 0;
  _clock_nb = 0;
}

local_lu_map_t::~local_lu_map_t() { clear(); }

tchecker::clockbounds::local_lu_map_t & local_lu_map_t::operator=(tchecker::clockbounds::local_lu_map_t const & m)
{
  if (this != &m) {
    clear();

    _loc_nb = m._loc_nb;
    _clock_nb = m._clock_nb;
    _L.resize(_loc_nb, nullptr);
    _U.resize(_loc_nb, nullptr);
    for (tchecker::loc_id_t id = 0; id < _loc_nb; ++id) {
      _L[id] = tchecker::clockbounds::clone_map(*m._L[id]);
      _U[id] = tchecker::clockbounds::clone_map(*m._U[id]);
    }
  }
  return *this;
}

tchecker::clockbounds::local_lu_map_t & local_lu_map_t::operator=(tchecker::clockbounds::local_lu_map_t && m)
{
  if (this != &m) {
    clear();

    _loc_nb = std::move(m._loc_nb);
    _clock_nb = std::move(m._clock_nb);
    _L = std::move(m._L);
    _U = std::move(m._U);

    m._L.clear();
    m._U.clear();
    m._loc_nb = 0;
    m._clock_nb = 0;
  }
  return *this;
}

void local_lu_map_t::clear()
{
  for (tchecker::loc_id_t id = 0; id < _loc_nb; ++id) {
    tchecker::clockbounds::deallocate_map(_L[id]);
    tchecker::clockbounds::deallocate_map(_U[id]);
  }
  _L.clear();
  _U.clear();
  _loc_nb = 0;
  _clock_nb = 0;
}

void local_lu_map_t::resize(tchecker::loc_id_t loc_nb, tchecker::clock_id_t clock_nb)
{
  clear();

  _loc_nb = loc_nb;
  _clock_nb = clock_nb;
  _L.resize(_loc_nb, nullptr);
  _U.resize(_loc_nb, nullptr);
  for (tchecker::loc_id_t id = 0; id < _loc_nb; ++id) {
    _L[id] = tchecker::clockbounds::allocate_map(_clock_nb);
    _U[id] = tchecker::clockbounds::allocate_map(_clock_nb);
    tchecker::clockbounds::clear(*_L[id]);
    tchecker::clockbounds::clear(*_U[id]);
  }
}

tchecker::loc_id_t local_lu_map_t::loc_number() const { return _loc_nb; }

tchecker::clock_id_t local_lu_map_t::clock_number() const { return _clock_nb; }

tchecker::clockbounds::map_t & local_lu_map_t::L(tchecker::loc_id_t id)
{
  assert(id < _loc_nb);
  return *_L[id];
}

tchecker::clockbounds::map_t const & local_lu_map_t::L(tchecker::loc_id_t id) const
{
  assert(id < _loc_nb);
  return *_L[id];
}

tchecker::clockbounds::map_t & local_lu_map_t::U(tchecker::loc_id_t id)
{
  assert(id < _loc_nb);
  return *_U[id];
}

tchecker::clockbounds::map_t const & local_lu_map_t::U(tchecker::loc_id_t id) const
{
  assert(id < _loc_nb);
  return *_U[id];
}

void local_lu_map_t::bounds(tchecker::loc_id_t id, tchecker::clockbounds::map_t & L, tchecker::clockbounds::map_t & U) const
{
  assert(id < _loc_nb);
  assert(L.capacity() == _clock_nb);
  assert(U.capacity() == _clock_nb);
  tchecker::clockbounds::clear(L);
  tchecker::clockbounds::clear(U);
  tchecker::clockbounds::update(L, *_L[id]);
  tchecker::clockbounds::update(U, *_U[id]);
}

void local_lu_map_t::bounds(tchecker::vloc_t const & vloc, tchecker::clockbounds::map_t & L,
                            tchecker::clockbounds::map_t & U) const
{
  assert(L.capacity() == _clock_nb);
  assert(U.capacity() == _clock_nb);
  tchecker::clockbounds::clear(L);
  tchecker::clockbounds::clear(U);
  for (tchecker::clock_id_t id : vloc) {
    assert(id < _loc_nb);
    tchecker::clockbounds::update(L, *_L[id]);
    tchecker::clockbounds::update(U, *_U[id]);
  }
}

std::ostream & operator<<(std::ostream & os, tchecker::clockbounds::local_lu_map_t const & map)
{
  tchecker::loc_id_t loc_nb = map.loc_number();
  for (tchecker::loc_id_t l = 0; l < loc_nb; ++l)
    os << l << ": L=" << map.L(l) << " U=" << map.U(l) << std::endl;

  return os;
}

/* global_lu_map_t */

global_lu_map_t::global_lu_map_t(tchecker::clock_id_t clock_nb) : _clock_nb(0), _L(nullptr), _U(nullptr) { resize(clock_nb); }

global_lu_map_t::global_lu_map_t(tchecker::clockbounds::global_lu_map_t const & m)
    : _clock_nb(m._clock_nb), _L(nullptr), _U(nullptr)
{
  _L = tchecker::clockbounds::clone_map(*m._L);
  _U = tchecker::clockbounds::clone_map(*m._U);
}

global_lu_map_t::global_lu_map_t(tchecker::clockbounds::global_lu_map_t && m)
    : _clock_nb(m._clock_nb), _L(std::move(m._L)), _U(std::move(m._U))
{
  m._L = nullptr;
  m._U = nullptr;
  m._clock_nb = 0;
}

global_lu_map_t::~global_lu_map_t() { clear(); }

tchecker::clockbounds::global_lu_map_t & global_lu_map_t::operator=(tchecker::clockbounds::global_lu_map_t const & m)
{
  if (this != &m) {
    clear();

    _clock_nb = m._clock_nb;
    _L = tchecker::clockbounds::clone_map(*m._L);
    _U = tchecker::clockbounds::clone_map(*m._U);
  }
  return *this;
}

tchecker::clockbounds::global_lu_map_t & global_lu_map_t::operator=(tchecker::clockbounds::global_lu_map_t && m)
{
  if (this != &m) {
    clear();

    _clock_nb = std::move(m._clock_nb);
    _L = std::move(m._L);
    _U = std::move(m._U);

    m._L = nullptr;
    m._U = nullptr;
    m._clock_nb = 0;
  }
  return *this;
}

void global_lu_map_t::clear()
{
  tchecker::clockbounds::deallocate_map(_L);
  tchecker::clockbounds::deallocate_map(_U);
  _clock_nb = 0;
}

void global_lu_map_t::resize(tchecker::clock_id_t clock_nb)
{
  clear();

  _clock_nb = clock_nb;
  _L = tchecker::clockbounds::allocate_map(_clock_nb);
  _U = tchecker::clockbounds::allocate_map(_clock_nb);
  tchecker::clockbounds::clear(*_L);
  tchecker::clockbounds::clear(*_U);
}

tchecker::clock_id_t global_lu_map_t::clock_number() const { return _clock_nb; }

tchecker::clockbounds::map_t & global_lu_map_t::L(void) { return *_L; }

tchecker::clockbounds::map_t const & global_lu_map_t::L(void) const { return *_L; }

tchecker::clockbounds::map_t & global_lu_map_t::U(void) { return *_U; }

tchecker::clockbounds::map_t const & global_lu_map_t::U(void) const { return *_U; }

void global_lu_map_t::bounds(tchecker::clockbounds::map_t & L, tchecker::clockbounds::map_t & U) const
{
  assert(L.capacity() == _clock_nb);
  assert(U.capacity() == _clock_nb);
  tchecker::clockbounds::clear(L);
  tchecker::clockbounds::clear(U);
  tchecker::clockbounds::update(L, *_L);
  tchecker::clockbounds::update(U, *_U);
}

void global_lu_map_t::bounds(tchecker::loc_id_t id, tchecker::clockbounds::map_t & L, tchecker::clockbounds::map_t & U) const
{
  bounds(L, U);
}

void global_lu_map_t::bounds(tchecker::vloc_t const & vloc, tchecker::clockbounds::map_t & L,
                             tchecker::clockbounds::map_t & U) const
{
  bounds(L, U);
}

std::ostream & operator<<(std::ostream & os, tchecker::clockbounds::global_lu_map_t const & map)
{
  return os << "L=" << map.L() << " U=" << map.U() << std::endl;
}

void fill_global_lu_map(tchecker::clockbounds::global_lu_map_t & global_lu_map,
                        tchecker::clockbounds::local_lu_map_t const & local_lu_map)
{
  if (global_lu_map.clock_number() != local_lu_map.clock_number())
    throw std::invalid_argument("*** fill_global_lu_map: incompatible number of clocks");

  tchecker::loc_id_t const loc_nb = local_lu_map.loc_number();

  tchecker::clockbounds::clear(global_lu_map.L());
  tchecker::clockbounds::clear(global_lu_map.U());
  for (tchecker::loc_id_t loc = 0; loc < loc_nb; ++loc) {
    tchecker::clockbounds::update(global_lu_map.L(), local_lu_map.L(loc));
    tchecker::clockbounds::update(global_lu_map.U(), local_lu_map.U(loc));
  }
}

/* local_m_map_t */

local_m_map_t::local_m_map_t(tchecker::loc_id_t loc_nb, tchecker::clock_id_t clock_nb)
    : _loc_nb(0), _clock_nb(0), _M(_loc_nb, nullptr)
{
  resize(loc_nb, clock_nb);
}

local_m_map_t::local_m_map_t(tchecker::clockbounds::local_m_map_t const & m)
    : _loc_nb(m._loc_nb), _clock_nb(m._clock_nb), _M(_loc_nb, nullptr)
{
  for (tchecker::loc_id_t id = 0; id < _loc_nb; ++id)
    _M[id] = tchecker::clockbounds::clone_map(*m._M[id]);
}

local_m_map_t::local_m_map_t(tchecker::clockbounds::local_m_map_t && m)
    : _loc_nb(m._loc_nb), _clock_nb(m._clock_nb), _M(std::move(m._M))
{
  m._M.clear();
  m._loc_nb = 0;
  m._clock_nb = 0;
}

local_m_map_t::~local_m_map_t() { clear(); }

tchecker::clockbounds::local_m_map_t & local_m_map_t::operator=(tchecker::clockbounds::local_m_map_t const & m)
{
  if (this != &m) {
    clear();

    _loc_nb = m._loc_nb;
    _clock_nb = m._clock_nb;
    _M.resize(_loc_nb, nullptr);
    for (tchecker::loc_id_t id = 0; id < _loc_nb; ++id)
      _M[id] = tchecker::clockbounds::clone_map(*m._M[id]);
  }
  return *this;
}

tchecker::clockbounds::local_m_map_t & local_m_map_t::operator=(tchecker::clockbounds::local_m_map_t && m)
{
  if (this != &m) {
    clear();

    _loc_nb = std::move(m._loc_nb);
    _clock_nb = std::move(m._clock_nb);
    _M = std::move(m._M);

    m._M.clear();
    m._loc_nb = 0;
    m._clock_nb = 0;
  }
  return *this;
}

void local_m_map_t::clear()
{
  for (tchecker::loc_id_t id = 0; id < _loc_nb; ++id)
    tchecker::clockbounds::deallocate_map(_M[id]);
  _M.clear();
  _loc_nb = 0;
  _clock_nb = 0;
}

void local_m_map_t::resize(tchecker::loc_id_t loc_nb, tchecker::clock_id_t clock_nb)
{
  clear();

  _loc_nb = loc_nb;
  _clock_nb = clock_nb;
  _M.resize(_loc_nb, nullptr);
  for (tchecker::loc_id_t id = 0; id < _loc_nb; ++id) {
    _M[id] = tchecker::clockbounds::allocate_map(_clock_nb);
    tchecker::clockbounds::clear(*_M[id]);
  }
}

tchecker::loc_id_t local_m_map_t::loc_number() const { return _loc_nb; }

tchecker::clock_id_t local_m_map_t::clock_number() const { return _clock_nb; }

tchecker::clockbounds::map_t & local_m_map_t::M(tchecker::loc_id_t id)
{
  assert(id < _loc_nb);
  return *_M[id];
}

tchecker::clockbounds::map_t const & local_m_map_t::M(tchecker::loc_id_t id) const
{
  assert(id < _loc_nb);
  return *_M[id];
}

void local_m_map_t::bounds(tchecker::loc_id_t id, tchecker::clockbounds::map_t & M) const
{
  assert(id < _loc_nb);
  assert(M.capacity() == _clock_nb);
  tchecker::clockbounds::clear(M);
  tchecker::clockbounds::update(M, *_M[id]);
}

void local_m_map_t::bounds(tchecker::vloc_t const & vloc, tchecker::clockbounds::map_t & M) const
{
  assert(M.capacity() == _clock_nb);
  tchecker::clockbounds::clear(M);
  for (tchecker::loc_id_t id : vloc) {
    assert(id < _loc_nb);
    tchecker::clockbounds::update(M, *_M[id]);
  }
}

std::ostream & operator<<(std::ostream & os, tchecker::clockbounds::local_m_map_t const & map)
{
  tchecker::loc_id_t loc_nb = map.loc_number();
  for (tchecker::loc_id_t l = 0; l < loc_nb; ++l)
    os << l << ": M=" << map.M(l) << std::endl;
  return os;
}

void fill_local_m_map(tchecker::clockbounds::local_m_map_t & local_m_map,
                      tchecker::clockbounds::local_lu_map_t const & local_lu_map)
{
  if (local_m_map.clock_number() != local_lu_map.clock_number())
    throw std::invalid_argument("*** fill_local_m_map: incompatible number of clocks");

  if (local_m_map.loc_number() != local_lu_map.loc_number())
    throw std::invalid_argument("*** fill_local_m_map: incompatible number of locations");

  tchecker::loc_id_t const loc_nb = local_lu_map.loc_number();

  for (tchecker::loc_id_t loc = 0; loc < loc_nb; ++loc) {
    tchecker::clockbounds::copy(local_m_map.M(loc), local_lu_map.L(loc));
    tchecker::clockbounds::update(local_m_map.M(loc), local_lu_map.U(loc));
  }
}

/* global_m_map_t */

global_m_map_t::global_m_map_t(tchecker::clock_id_t clock_nb) : _clock_nb(0), _M(nullptr) { resize(clock_nb); }

global_m_map_t::global_m_map_t(tchecker::clockbounds::global_m_map_t const & m) : _clock_nb(m._clock_nb), _M(nullptr)
{
  _M = tchecker::clockbounds::clone_map(*m._M);
}

global_m_map_t::global_m_map_t(tchecker::clockbounds::global_m_map_t && m) : _clock_nb(m._clock_nb), _M(std::move(m._M))
{
  m._M = nullptr;
  m._clock_nb = 0;
}

global_m_map_t::~global_m_map_t() { clear(); }

tchecker::clockbounds::global_m_map_t & global_m_map_t::operator=(tchecker::clockbounds::global_m_map_t const & m)
{
  if (this != &m) {
    clear();

    _clock_nb = m._clock_nb;
    _M = tchecker::clockbounds::clone_map(*m._M);
  }
  return *this;
}

tchecker::clockbounds::global_m_map_t & global_m_map_t::operator=(tchecker::clockbounds::global_m_map_t && m)
{
  if (this != &m) {
    clear();

    _clock_nb = std::move(m._clock_nb);
    _M = std::move(m._M);

    m._M = nullptr;
    m._clock_nb = 0;
  }
  return *this;
}

void global_m_map_t::clear()
{
  tchecker::clockbounds::deallocate_map(_M);
  _clock_nb = 0;
  _M = nullptr;
}

void global_m_map_t::resize(tchecker::clock_id_t clock_nb)
{
  clear();

  _clock_nb = clock_nb;
  _M = tchecker::clockbounds::allocate_map(_clock_nb);
  tchecker::clockbounds::clear(*_M);
}

tchecker::clock_id_t global_m_map_t::clock_number() const { return _clock_nb; }

tchecker::clockbounds::map_t & global_m_map_t::M(void) { return *_M; }

tchecker::clockbounds::map_t const & global_m_map_t::M(void) const { return *_M; }

void global_m_map_t::bounds(tchecker::clockbounds::map_t & M) const
{
  tchecker::clockbounds::clear(M);
  tchecker::clockbounds::update(M, *_M);
}

void global_m_map_t::bounds(tchecker::loc_id_t id, tchecker::clockbounds::map_t & M) const { bounds(M); }

void global_m_map_t::bounds(tchecker::vloc_t const & vloc, tchecker::clockbounds::map_t & M) const { bounds(M); }

std::ostream & operator<<(std::ostream & os, tchecker::clockbounds::global_m_map_t const & map)
{
  return os << "M=" << map.M() << std::endl;
}

void fill_global_m_map(tchecker::clockbounds::global_m_map_t & global_m_map,
                       tchecker::clockbounds::local_lu_map_t const & local_lu_map)
{
  if (global_m_map.clock_number() != local_lu_map.clock_number())
    throw std::invalid_argument("*** fill_gloal_m_map: incompatible clock number");

  tchecker::clockbounds::clear(global_m_map.M());

  tchecker::loc_id_t const loc_nb = local_lu_map.loc_number();
  for (tchecker::loc_id_t loc = 0; loc < loc_nb; ++loc) {
    tchecker::clockbounds::update(global_m_map.M(), local_lu_map.L(loc));
    tchecker::clockbounds::update(global_m_map.M(), local_lu_map.U(loc));
  }
}

/* clockbounds_t */

clockbounds_t::clockbounds_t(tchecker::loc_id_t loc_nb, tchecker::clock_id_t clock_nb)
    : _global_lu(new tchecker::clockbounds::global_lu_map_t(clock_nb)),
      _global_m(new tchecker::clockbounds::global_m_map_t(clock_nb)),
      _local_lu(new tchecker::clockbounds::local_lu_map_t(loc_nb, clock_nb)),
      _local_m(new tchecker::clockbounds::local_m_map_t(loc_nb, clock_nb))
{
}

void clockbounds_t::clear()
{
  _global_lu->clear();
  _global_m->clear();
  _local_lu->clear();
  _local_m->clear();
}

void clockbounds_t::resize(tchecker::loc_id_t loc_nb, tchecker::clock_id_t clock_nb)
{
  _global_lu->resize(clock_nb);
  _local_lu->resize(loc_nb, clock_nb);
  _global_m->resize(clock_nb);
  _local_m->resize(loc_nb, clock_nb);
}

std::shared_ptr<tchecker::clockbounds::global_lu_map_t const> clockbounds_t::global_lu_map() const { return _global_lu; }

std::shared_ptr<tchecker::clockbounds::global_lu_map_t> clockbounds_t::global_lu_map() { return _global_lu; }

std::shared_ptr<tchecker::clockbounds::global_m_map_t const> clockbounds_t::global_m_map() const { return _global_m; }

std::shared_ptr<tchecker::clockbounds::global_m_map_t> clockbounds_t::global_m_map() { return _global_m; }

std::shared_ptr<tchecker::clockbounds::local_lu_map_t const> clockbounds_t::local_lu_map() const { return _local_lu; }

std::shared_ptr<tchecker::clockbounds::local_lu_map_t> clockbounds_t::local_lu_map() { return _local_lu; }

std::shared_ptr<tchecker::clockbounds::local_m_map_t const> clockbounds_t::local_m_map() const { return _local_m; }

std::shared_ptr<tchecker::clockbounds::local_m_map_t> clockbounds_t::local_m_map() { return _local_m; }

namespace gta {

// gsim_diagonal_bounds_t
gsim_diagonal_bounds_t::gsim_diagonal_bounds_t(tchecker::clock_id_t const num_clks) : _num_clks(num_clks) {}

tchecker::clock_constraint_t gsim_diagonal_bounds_t::pop()
{
  if (empty()) {
    throw std::runtime_error("Attempting to pop from empty bounds");
  }

  auto const & top_bounds = _bounds.begin();
  auto const [x, y] = top_bounds->first;
  tchecker::clockbounds::gta::bounds_collection_t & bounds = top_bounds->second;

  assert(not bounds.empty());
  auto const & top_bound = bounds.begin();

  tchecker::clock_constraint_t const bound(x, y, top_bound->cmp, top_bound->value);

  if (bounds.size() == 1) {
    // We erase x, y from the map if this was the last constraint in the collection
    _bounds.erase(top_bounds);
    return bound;
  }

  bounds.erase(top_bound);
  return bound;
}

tchecker::clockbounds::gta::bounds_collection_t const & gsim_diagonal_bounds_t::bounds(tchecker::clock_id_t const x,
                                                                                       tchecker::clock_id_t const y) const
{
  assert(x < _num_clks && y < _num_clks);
  auto const it = _bounds.find(std::make_pair(x, y));
  if (it != _bounds.end()) {
    return it->second;
  }
  return tchecker::clockbounds::gta::EMPTY_BOUND_COLLECTION;
}

bool gsim_diagonal_bounds_t::add_diagonal_bound(tchecker::clock_id_t const x, tchecker::clock_id_t const y,
                                                tchecker::dbm::extended::db_t const bound)
{
  assert(x < _num_clks && y < _num_clks);
  auto const p = std::make_pair(x, y);
  auto const it = _bounds.find(p);
  if (it != _bounds.end()) {
    auto const [_, was_modified] = it->second.insert(bound);
    return was_modified;
  }

  _bounds.insert({p, tchecker::clockbounds::gta::bounds_collection_t{bound}});
  return true;
}

void gsim_diagonal_bounds_t::clear_diagonal_bounds(tchecker::clock_id_t const x, tchecker::clock_id_t const y)
{
  assert(x < _num_clks && y < _num_clks);
  auto const it = _bounds.find(std::make_pair(x, y));
  if (it != _bounds.end()) {
    _bounds.erase(it);
  }
}

bool gsim_diagonal_bounds_t::add_bounds_from(tchecker::clockbounds::gta::gsim_diagonal_bounds_t const & bounds)
{
  bool was_modified = false;
  for (auto const & [pair, bounds] : bounds._bounds) {
    auto const & [x, y] = pair;
    for (auto const & bound : bounds) {
      if (add_diagonal_bound(x, y, bound)) {
        was_modified = true;
      }
    }
  }

  return was_modified;
}

std::ostream & gsim_diagonal_bounds_t::output(std::ostream & os, tchecker::clock_variables_t const & clock_vars) const
{
  bool output_delim = false;
  for (auto const & [pair, bounds] : _bounds) {
    auto const & [x, y] = pair;
    for (tchecker::dbm::extended::db_t const & bound : bounds) {
      if (output_delim) {
        os << ", ";
      }

      os << clock_vars.name(x) << "-" << clock_vars.name(y);
      tchecker::dbm::extended::output(os, bound);
      output_delim = true;
    }
  }
  return os;
}

// x_minus_zero_bounds_t
x_minus_zero_bounds_t::x_minus_zero_bounds_t()
    : _subsumable_bound(tchecker::clockbounds::gta::X_MINUS_ZERO_NO_BOUND), _lt_inf_bound(false)
{
}

bool x_minus_zero_bounds_t::add_bound(tchecker::dbm::extended::db_t const bound)
{
  if (bound == tchecker::dbm::extended::LT_INFINITY) {
    bool const is_modified = !_lt_inf_bound;
    _lt_inf_bound = true;
    return is_modified;
  }
  if (_subsumable_bound >= bound) {
    return false;
  }
  _subsumable_bound = bound;
  return true;
}

bool x_minus_zero_bounds_t::add_bounds_from(tchecker::clockbounds::gta::x_minus_zero_bounds_t const & bounds)
{
  bool was_modified = false;

  if (bounds.contains_lt_inf_bound()) {
    if (add_bound(tchecker::dbm::extended::LT_INFINITY)) {
      was_modified = true;
    }
  }

  if (add_bound(bounds.less_than_inf_rvalue_bound())) {
    was_modified = true;
  }

  return was_modified;
}

void x_minus_zero_bounds_t::clear_bounds()
{
  _subsumable_bound = tchecker::clockbounds::gta::X_MINUS_ZERO_NO_BOUND;
  _lt_inf_bound = false;
}

zero_minus_x_bound_t::zero_minus_x_bound_t()
    : _subsumable_bound(tchecker::clockbounds::gta::ZERO_MINUS_X_NO_BOUND), _le_minus_inf(false)
{
}

bool zero_minus_x_bound_t::add_bound(tchecker::dbm::extended::db_t const bound)
{
  if (bound == tchecker::dbm::extended::LE_MINUS_INFINITY) {
    bool const is_modified = !_le_minus_inf;
    _le_minus_inf = true;
    return is_modified;
  }

  if (_subsumable_bound <= bound) {
    return false;
  }
  _subsumable_bound = bound;
  return true;
}

bool zero_minus_x_bound_t::add_bounds_from(tchecker::clockbounds::gta::zero_minus_x_bound_t const & bounds)
{
  bool was_modified = false;

  if (bounds.contained_le_minus_inf_bound()) {
    if (add_bound(tchecker::dbm::extended::LE_MINUS_INFINITY)) {
      was_modified = true;
    }
  }

  if (add_bound(bounds.greater_than_minus_inf_rvalue_bound())) {
    was_modified = true;
  }

  return was_modified;
}

void zero_minus_x_bound_t::clear_bounds()
{
  _subsumable_bound = tchecker::clockbounds::gta::ZERO_MINUS_X_NO_BOUND;
  _le_minus_inf = false;
}

// gsim_map_t
gsim_map_t::gsim_map_t(tchecker::clock_id_t const num_clks)
    : _x_minus_zero_bounds(num_clks), _zero_minus_x_bounds(num_clks), _diagonal_bounds(num_clks)
{
}

tchecker::clock_id_t gsim_map_t::num_clks() const { return _x_minus_zero_bounds.size(); }

bool gsim_map_t::add_diagonal_bound(tchecker::clock_id_t const x, tchecker::clock_id_t const y,
                                    tchecker::dbm::extended::db_t const bound)
{
  return _diagonal_bounds.add_diagonal_bound(x, y, bound);
}

void gsim_map_t::clear_diagonal_bounds(tchecker::clock_id_t const x, tchecker::clock_id_t const y)
{
  _diagonal_bounds.clear_diagonal_bounds(x, y);
}

bool gsim_map_t::add_bounds_from(tchecker::clockbounds::gta::gsim_map_t const & gmap)
{
  bool was_modified = false;
  tchecker::clock_id_t const clks = num_clks();

  for (tchecker::clock_id_t x = 0; x < clks; x++) {
    if (add_x_minus_zero_bounds_from(x, gmap.x_minus_zero_bounds(x))) {
      was_modified = true;
    }

    if (add_zero_minus_x_bounds_from(x, gmap.zero_minus_x_bounds(x))) {
      was_modified = true;
    }
  }

  if (_diagonal_bounds.add_bounds_from(gmap._diagonal_bounds)) {
    was_modified = true;
  }

  return was_modified;
}

std::ostream & gsim_map_t::output(std::ostream & os, tchecker::clock_variables_t const & clock_vars) const
{

  bool output_delim = false;
  os << "Non-diagonal bounds = [";

  for (tchecker::clock_id_t x = 0; x < _x_minus_zero_bounds.size(); x++) {
    tchecker::clockbounds::gta::x_minus_zero_bounds_t const & x_minus_zero_bounds = _x_minus_zero_bounds[x];

    if (x_minus_zero_bounds.less_than_inf_rvalue_bound() != tchecker::clockbounds::gta::X_MINUS_ZERO_NO_BOUND) {
      if (output_delim) {
        os << ", ";
      }

      os << clock_vars.name(x);
      tchecker::dbm::extended::output(os, x_minus_zero_bounds.less_than_inf_rvalue_bound());
      output_delim = true;
    }

    if (x_minus_zero_bounds.contains_lt_inf_bound()) {
      if (output_delim) {
        os << ", ";
      }

      os << clock_vars.name(x);
      tchecker::dbm::extended::output(os, tchecker::dbm::extended::LT_INFINITY);
      output_delim = true;
    }
  }

  for (tchecker::clock_id_t x = 0; x < _zero_minus_x_bounds.size(); x++) {
    tchecker::clockbounds::gta::zero_minus_x_bound_t const & zero_minus_x_bounds = _zero_minus_x_bounds[x];

    if (zero_minus_x_bounds.greater_than_minus_inf_rvalue_bound() != tchecker::clockbounds::gta::ZERO_MINUS_X_NO_BOUND) {
      if (output_delim) {
        os << ", ";
      }

      os << "-" << clock_vars.name(x);
      tchecker::dbm::extended::output(os, zero_minus_x_bounds.greater_than_minus_inf_rvalue_bound());
      output_delim = true;
    }

    if (zero_minus_x_bounds.contained_le_minus_inf_bound()) {
      if (output_delim) {
        os << ", ";
      }

      os << clock_vars.name(x);
      tchecker::dbm::extended::output(os, tchecker::dbm::extended::LE_MINUS_INFINITY);
      output_delim = true;
    }
  }
  os << "]" << std::endl;

  os << "Diagonal bounds = [";
  _diagonal_bounds.output(os, clock_vars);
  os << "]" << std::endl;
  return os;
}

vloc_to_bounds_t::vloc_to_bounds_t(tchecker::clockbounds::gta::loc_gmap_t const & clockbounds) : _clockbounds(clockbounds)
{
  if (clockbounds.empty()) {
    _num_clks = 0;
  }
  else {
    _num_clks = clockbounds[0].num_clks();
  }
}

tchecker::clockbounds::gta::gsim_map_t &
vloc_to_bounds_t::bounds_of_vloc(tchecker::intrusive_shared_ptr_t<const tchecker::shared_vloc_t> vloc) const
{

  auto const & iter = _memoized_bounds.find(vloc.ptr());

  if (iter == _memoized_bounds.end()) {
    tchecker::clockbounds::gta::gsim_map_t vloc_clockbounds(_num_clks);

    for (tchecker::loc_id_t const id : *vloc) {
      vloc_clockbounds.add_bounds_from(_clockbounds[id]);
    }

    auto const & [new_iter, _] = _memoized_bounds.insert(std::make_pair(vloc.ptr(), std::move(vloc_clockbounds)));

    return new_iter->second;
  }
  else {
    return iter->second;
  }
}
} // namespace gta

} // namespace clockbounds

} // namespace tchecker
