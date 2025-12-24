/*
 * This file is a part of the TChecker project.
 *
 * See files AUTHORS and LICENSE for copyright details.
 *
 */

#include "tchecker/gtazg/gta_zone.hh"

namespace tchecker {

namespace gtazg {

tchecker::gtazg::gta_zone_t & gta_zone_t::operator=(tchecker::gtazg::gta_zone_t const & zone)
{
  if (_dim != zone._dim) {
    throw std::invalid_argument("Zone dimension mismatch");
  }

  if (this != &zone) {
    tchecker::gtadbm::copy(dbm_ptr(), zone.dbm_ptr(), _dim);
  }

  return *this;
}

bool gta_zone_t::is_empty() const { return tchecker::gtadbm::is_empty_0(dbm_ptr(), _dim); }

bool gta_zone_t::is_final() const
{
  if (is_empty()) {
    return false;
  }

  return tchecker::gtadbm::all_prop_clks_minus_inf(dbm_ptr(), _dim, _prop_clks, _hist_clks);
}

bool gta_zone_t::operator==(tchecker::gtazg::gta_zone_t const & zone) const
{
  if (_dim != zone._dim) {
    return false;
  }

  bool const empty1 = this->is_empty(), empty2 = zone.is_empty();
  if (empty1 || empty2) {
    return (empty1 && empty2);
  }

  return tchecker::gtadbm::is_equal(dbm_ptr(), zone.dbm_ptr(), _dim, _prop_clks, _hist_clks);
}

bool gta_zone_t::operator!=(tchecker::gtazg::gta_zone_t const & zone) const { return !(*this == zone); }

bool gta_zone_t::is_gmap_le(tchecker::gtazg::gta_zone_t const & zone,
                            tchecker::clockbounds::gta::gsim_map_t & clockbounds) const
{
  return tchecker::gtadbm::is_gmap_le(dbm_ptr(), zone.dbm_ptr(), _dim, _prop_clks, _hist_clks, clockbounds);
}

int gta_zone_t::lexical_cmp(tchecker::gtazg::gta_zone_t const & zone) const
{
  return tchecker::gtadbm::lexical_cmp(dbm_ptr(), _dim, zone.dbm_ptr(), zone._dim);
}

std::size_t gta_zone_t::hash() const { return tchecker::gtadbm::hash(dbm_ptr(), _dim); }

std::size_t gta_zone_t::prophecy_clks_lower_bound_hash() const
{
  return tchecker::gtadbm::hash_prophecy_lower_bounds(dbm_ptr(), _dim, _prop_clks);
}

std::ostream & gta_zone_t::output(std::ostream & os, tchecker::clock_index_t const & index) const
{
  return tchecker::gtadbm::output(os, dbm_ptr(), _dim, _prop_clks, _hist_clks,
                                  [&](tchecker::clock_id_t id) { return (id == 0 ? "0" : index.value(id - 1)); });
}

tchecker::dbm::extended::db_t * gta_zone_t::dbm() { return dbm_ptr(); }

tchecker::dbm::extended::db_t const * gta_zone_t::dbm() const { return dbm_ptr(); }

void gta_zone_t::to_dbm(tchecker::dbm::extended::db_t * dbm) const { tchecker::gtadbm::copy(dbm, dbm_ptr(), _dim); }

gta_zone_t::gta_zone_t(tchecker::clock_id_t dim, tchecker::gtadbm::prophecy_clks_idx_t const & prophecy_clks_idx,
                       tchecker::gtadbm::history_clks_idx_t const & history_clks_idx)
    : _dim(dim), _prop_clks(prophecy_clks_idx), _hist_clks(history_clks_idx)
{
  tchecker::gtadbm::gta_universal(dbm_ptr(), _dim, _prop_clks, _hist_clks);
}

gta_zone_t::gta_zone_t(tchecker::gtazg::gta_zone_t const & zone)
    : _dim(zone._dim), _prop_clks(zone._prop_clks), _hist_clks(zone._hist_clks)
{
  tchecker::gtadbm::copy(dbm_ptr(), zone.dbm_ptr(), _dim);
}

void gta_zone_destruct_and_deallocate(tchecker::gtazg::gta_zone_t * zone)
{
  tchecker::gtazg::gta_zone_t::destruct(zone);
  delete[] reinterpret_cast<char *>(zone);
}

bool is_mutual_sim(tchecker::gtazg::gta_zone_t const & zone1, tchecker::gtazg::gta_zone_t const & zone2,
                   tchecker::clockbounds::gta::gsim_map_t & clockbounds)
{
  return tchecker::gtadbm::is_gmap_mutual_sim(zone1.dbm(), zone2.dbm(), zone1.dim(), zone1.prophecy_clks_idx(),
                                              zone2.history_clks_idx(), clockbounds);
}
} // end of namespace gtazg

std::string to_string(tchecker::gtazg::gta_zone_t const & zone, tchecker::clock_index_t const & index)
{
  std::stringstream sstream;
  zone.output(sstream, index);
  return sstream.str();
}

} // end of namespace tchecker
