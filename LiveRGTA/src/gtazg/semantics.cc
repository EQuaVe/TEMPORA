/*
 * This file is a part of the TChecker project.
 *
 * See files AUTHORS and LICENSE for copyright details.
 *
 */

#include "tchecker/gtazg/semantics.hh"
#include "tchecker/dbm/gtadbm.hh"

namespace tchecker {

namespace gtazg {

namespace details {

class dbm_update_after_action_t final : tchecker::gta::typed_action_visitor_t {
public:
  dbm_update_after_action_t(tchecker::gtadbm::prophecy_clks_idx_t const & prophecy_clks_idx,
                            tchecker::gtadbm::history_clks_idx_t const & history_clks_idx)
      : _prophecy_clks_idx(prophecy_clks_idx), _history_clks_idx(history_clks_idx)
  {
  }

  void update(tchecker::dbm::extended::db_t * dbm, tchecker::clock_id_t dim, tchecker::gta::typed_actions_t const & actions)
  {
    _dbm = dbm;
    _dim = dim;

    for (auto const & action : actions) {
      action->visit(*this);
    }
  }

private:
  // Visitors
  virtual void visit(tchecker::gta::typed_action_release_t const & action)
  {
    assert(_dbm != nullptr);
    tchecker::gtadbm::clk_idx_t const release_clk_idx = action.typed_clk_var()->clk_id() + 1;
    tchecker::gtadbm::release(_dbm, _dim, release_clk_idx, _prophecy_clks_idx, _history_clks_idx);
  }

  virtual void visit(tchecker::gta::typed_action_reset_t const & action)
  {
    assert(_dbm != nullptr);
    tchecker::gtadbm::clk_idx_t const reset_clk_idx = action.typed_clk_var()->clk_id() + 1;
    tchecker::gtadbm::reset(_dbm, _dim, reset_clk_idx, _prophecy_clks_idx, _history_clks_idx);
  }

  virtual void visit(tchecker::gta::typed_action_rename_prophecy_t const & action)
  {
    assert(_dbm != nullptr);
    tchecker::gtadbm::clk_idx_t const lhs_clk_idx = action.typed_lhs_clk_var()->clk_id() + 1;
    tchecker::gtadbm::clk_idx_t const rhs_clk_idx = action.typed_rhs_clk_var()->clk_id() + 1;
    tchecker::gtadbm::rename(_dbm, _dim, lhs_clk_idx, rhs_clk_idx, _prophecy_clks_idx, _history_clks_idx);
  }

  virtual void visit(tchecker::gta::typed_action_rename_history_t const & action)
  {
    assert(_dbm != nullptr);
    tchecker::gtadbm::clk_idx_t const lhs_clk_idx = action.typed_lhs_clk_var()->clk_id() + 1;
    tchecker::gtadbm::clk_idx_t const rhs_clk_idx = action.typed_rhs_clk_var()->clk_id() + 1;
    tchecker::gtadbm::rename(_dbm, _dim, lhs_clk_idx, rhs_clk_idx, _prophecy_clks_idx, _history_clks_idx);
  }

  virtual void visit(tchecker::gta::typed_action_clk_variable_t const &) { assert(false); }

  tchecker::gtadbm::prophecy_clks_idx_t const & _prophecy_clks_idx;
  tchecker::gtadbm::history_clks_idx_t const & _history_clks_idx;

  tchecker::dbm::extended::db_t * _dbm;
  tchecker::clock_id_t _dim;
};

} // end of namespace details

/* standard_semantics_t */

tchecker::state_status_t standard_semantics_t::initial(tchecker::dbm::extended::db_t * dbm, tchecker::clock_id_t dim,
                                                       bool delay_allowed,
                                                       tchecker::gtadbm::prophecy_clks_idx_t const & prophecy_clks_idx,
                                                       tchecker::gtadbm::history_clks_idx_t const & zero_history_clks_idx,
                                                       tchecker::gtadbm::history_clks_idx_t const & infinitiy_history_clks_idx)
{
  tchecker::gtadbm::init(dbm, dim, prophecy_clks_idx, zero_history_clks_idx, infinitiy_history_clks_idx);

  return tchecker::STATE_OK;
}

void standard_semantics_t::delay_before_transition(tchecker::dbm::extended::db_t * dbm, tchecker::clock_id_t dim,
                                                   bool src_delay_allowed,
                                                   tchecker::gtadbm::prophecy_clks_idx_t const & prophecy_clks_idx,
                                                   tchecker::gtadbm::history_clks_idx_t const & history_clks_idx)
{
  if (src_delay_allowed) {
    tchecker::gtadbm::open_up(dbm, dim, prophecy_clks_idx, history_clks_idx);
  }
}

tchecker::state_status_t
standard_semantics_t::execute_gta_program(tchecker::dbm::extended::db_t * dbm, tchecker::clock_id_t dim,
                                          tchecker::gtadbm::prophecy_clks_idx_t const & prophecy_clks_idx,
                                          tchecker::gtadbm::history_clks_idx_t const & history_clks_idx,
                                          tchecker::gta::compiled_gta_program_t const & gta_prog)
{

  tchecker::gtazg::details::dbm_update_after_action_t action_updater(prophecy_clks_idx, history_clks_idx);

  for (auto iter = gta_prog.begin(); iter != gta_prog.end(); iter = gta_prog.next(iter)) {
    if (iter.first != nullptr) {
      tchecker::clock_constraint_container_t const & cc = *(iter.first->get_value());
      tchecker::dbm::status_t const res = tchecker::gtadbm::constrain(dbm, dim, cc, prophecy_clks_idx, history_clks_idx);

      if (res == tchecker::dbm::EMPTY) {
        assert(tchecker::gtadbm::is_empty_0(dbm, dim));
        return tchecker::STATE_CLOCKS_GUARD_VIOLATED;
      }
    }

    else if (iter.second != nullptr) {
      tchecker::gta::typed_actions_t const & actions = *(iter.second->get_value());
      action_updater.update(dbm, dim, actions);
    }
  }

  return tchecker::STATE_OK;
}

void standard_semantics_t::delay_after_transition(tchecker::dbm::extended::db_t * dbm, tchecker::clock_id_t dim,
                                                  bool tgt_delay_allowed,
                                                  tchecker::gtadbm::prophecy_clks_idx_t const & prophecy_clks_idx,
                                                  tchecker::gtadbm::history_clks_idx_t const & history_clks_idx)
{
  return;
}

/* factory */

tchecker::gtazg::standard_semantics_t * semantics_factory() { return new tchecker::gtazg::standard_semantics_t{}; }

} // namespace gtazg

} // end of namespace tchecker
