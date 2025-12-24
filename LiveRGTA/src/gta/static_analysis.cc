/*
 * This file is a part of the TChecker project.
 *
 * See files AUTHORS and LICENSE for copyright details.
 *
 */

#include <cassert>
#include <optional>

#include "tchecker/basictypes.hh"
#include "tchecker/gta/static_analysis.hh"
#include "tchecker/gta_program/typed_gta_program.hh"
#include "tchecker/variables/variables.hh"

namespace tchecker {

namespace gta {

namespace details {

/*!
 * \class minimal_xd_set_t
 * \brief compute a minimal set of prophecy clocks which are present
 * in some diagonal constraint between two prophecy clocks in some gta program
 */
class minimal_xd_set_t : public tchecker::gta::typed_guard_visitor_t {
public:
  /*!
   * \brief Constructor
   * \param gta_system : GTA system
   */
  minimal_xd_set_t(tchecker::gta::system_t const & gta_system) : _gta_system(gta_system)
  {
    _minimal_xd_set.resize(_gta_system.clocks_count(tchecker::VK_FLATTENED));
  }

  /*!
   \brief Copy constructor (DELETED)
   */
  minimal_xd_set_t(tchecker::gta::details::minimal_xd_set_t const &) = delete;

  /*!
   \brief Move constructor (DELETED)
   */
  minimal_xd_set_t(tchecker::gta::details::minimal_xd_set_t &&) = delete;

  /*!
   \brief Destructor
   */
  virtual ~minimal_xd_set_t() = default;

  /*!
   \brief Assignment operator (DELETED)
   */
  tchecker::gta::details::minimal_xd_set_t & operator=(tchecker::gta::details::minimal_xd_set_t const &) = delete;

  /*!
   \brief Move assignment operator (DELETED)
   */
  tchecker::gta::details::minimal_xd_set_t & operator=(tchecker::gta::details::minimal_xd_set_t &&) = delete;

  /*!
   * \brief Compute the minimal X_D set of the gta program
   * \return the set as a bitset
   */
  boost::dynamic_bitset<> const & compute_minimal_xd_set()
  {
    _minimal_xd_set.reset();

    for (tchecker::edge_id_t const id : _gta_system.edges_identifiers()) {
      if (_gta_system.is_edge_unsatisfiable(id)) {
        continue;
      }
      tchecker::gta::typed_gta_program_t const & gta_prog = _gta_system.gta_program(id);

      for (auto iter = gta_prog.begin(); iter != gta_prog.end(); iter = gta_prog.next(iter)) {
        if (iter.first != nullptr) {
          for (auto const & guard : *(iter.first->get_value())) {
            guard->visit(*this);
          }
        }
      }
    }
    return _minimal_xd_set;
  }

private:
  /*!
   \brief Visitors
   */
  void visit(tchecker::gta::typed_guard_clk_variable_t const & guard_term) { assert(false); }
  void visit(tchecker::gta::typed_guard_integer_t const & guard_term) { assert(false); }
  void visit(tchecker::gta::typed_guard_integer_rterm_t const & guard_term) { assert(false); }
  void visit(tchecker::gta::typed_guard_constant_rterm_t const & guard_term) { assert(false); }

  void visit(tchecker::gta::typed_guard_unary_lterm_t const & guard_term) { return; }
  void visit(tchecker::gta::typed_guard_binary_lterm_t const & guard_term)
  {
    bool is_left_var_prophecy = guard_term.typed_var_l()->clk_type() == tchecker::gta::GTA_CLK_TYPE_PROPHECY;
    bool is_right_var_prophecy = guard_term.typed_var_r()->clk_type() == tchecker::gta::GTA_CLK_TYPE_PROPHECY;

    // clks x and y belong to minimal x_d if both are prophecy clks and present in
    // some diagonal constraint
    if (is_left_var_prophecy && is_right_var_prophecy) {
      tchecker::clock_id_t const left_var_id = guard_term.typed_var_l()->clk_id();
      tchecker::clock_id_t const right_var_id = guard_term.typed_var_r()->clk_id();

      _minimal_xd_set[left_var_id] = 1;
      _minimal_xd_set[right_var_id] = 1;
    }
  }
  void visit(tchecker::gta::typed_guard_t const & guard_term) { guard_term.typed_lterm()->visit(*this); }

  tchecker::gta::system_t const & _gta_system;
  boost::dynamic_bitset<> _minimal_xd_set;
};

/*!
 \class xd_safety_check_t
 \brief Checks if gta system is xd-safe w.r.t given xd set
 */
class xd_safety_check_t : tchecker::gta::typed_action_visitor_t, tchecker::gta::typed_guard_visitor_t {
public:
  /*!
   * \brief Constructor
   * \param gta_system : GTA system
   * \param xd_clocks : The subset of clocks which are in the set X_D
   * \pre sizes of flattened clocks in gta system and X_D(passed as bitset) match, checked by assertion
   */
  xd_safety_check_t(tchecker::gta::system_t const & gta_system, boost::dynamic_bitset<> const & xd_clocks)
      : _gta_system(gta_system), _xd_clocks(xd_clocks), _is_xd_safe(true)
  {
    assert(_gta_system.clocks_count(tchecker::VK_FLATTENED) == _xd_clocks.size());
    _prophecy_clks_checked_for_zero_or_minus_inf.resize(xd_clocks.size());
  }

  /*!
   \brief Copy constructor (DELETED)
   */
  xd_safety_check_t(tchecker::gta::details::xd_safety_check_t const &) = delete;

  /*!
   \brief Move constructor (DELETED)
   */
  xd_safety_check_t(tchecker::gta::details::xd_safety_check_t &&) = delete;

  /*!
   \brief Destructor
   */
  virtual ~xd_safety_check_t() = default;

  /*!
   \brief Assignment operator (DELETED)
   */
  tchecker::gta::details::xd_safety_check_t & operator=(tchecker::gta::details::xd_safety_check_t const &) = delete;

  /*!
   \brief Move assignment operator (DELETED)
   */
  tchecker::gta::details::xd_safety_check_t & operator=(tchecker::gta::details::xd_safety_check_t &&) = delete;

  /*!
   * \brief Resets the necessary attributes after visiting a gta program
   */
  void reset_after_gta_prog_visit() { _prophecy_clks_checked_for_zero_or_minus_inf.reset(); }

  /*!
   * \brief Checks if the GTA is xd-safe
   * \return boolean denoting if gta is safe
   */
  bool is_xd_safe()
  {
    for (tchecker::edge_id_t const id : _gta_system.edges_identifiers()) {
      if (_gta_system.is_edge_unsatisfiable(id)) {
        continue;
      }
      tchecker::gta::typed_gta_program_t const & gta_prog = _gta_system.gta_program(id);

      for (auto iter = gta_prog.begin(); iter != gta_prog.end(); iter = gta_prog.next(iter)) {
        if (iter.first != nullptr) {
          for (std::shared_ptr<tchecker::gta::typed_guard_t const> const & guard : *(iter.first->get_value())) {
            guard->visit(*this);
          }
        }

        else if (iter.second != nullptr) {
          for (std::shared_ptr<tchecker::gta::typed_action_t const> const & action : *(iter.second->get_value())) {
            action->visit(*this);
          }
        }
      }

      reset_after_gta_prog_visit();
    }
    return _is_xd_safe;
  }

private:
  /*!
   * \struct guard_parse_ctx_t
   * \brief struct to store the context during parsing of guard
   */
  struct guard_parse_ctx_t {
    /*!
     * \brief enum to represent the rterm value in guard
     */
    enum rterm_constants {
      ZERO,           /*!< ZERO */
      MINUS_INF,      /*!< -INF */
      UN_INTERESTING, /*!< Any other constant which is not of interest for xd safety */
    };
    std::optional<enum rterm_constants> _rterm_constant; /*!< Filled during parsing rterm */

    // has value 'id' if the lterm is a unary term with clock 'id', and None otherwise
    std::optional<tchecker::clock_id_t> _lterm_unary_clk; /*!< Filled during parsing lterm */

    /*!
     * \brief resets the context
     */
    void reset()
    {
      _rterm_constant.reset();
      _lterm_unary_clk.reset();
    }
  };

  /*!
   \brief Visitors
   */
  virtual void visit(tchecker::gta::typed_action_release_t const & action)
  {
    tchecker::clock_id_t released_clk_id = action.typed_clk_var()->clk_id();
    bool clk_released_without_validation = !(_prophecy_clks_checked_for_zero_or_minus_inf[released_clk_id]);
    bool clk_is_in_xd = _xd_clocks[released_clk_id];

    if (clk_released_without_validation && clk_is_in_xd) {
      _is_xd_safe = false;
    }

    // After a clock is released, its value is not constrained to be 0 or minus infinity
    _prophecy_clks_checked_for_zero_or_minus_inf[released_clk_id] = 0;
  }

  virtual void visit(tchecker::gta::typed_action_rename_prophecy_t const & action)
  {
    tchecker::clock_id_t const lhs_var_id = action.typed_lhs_clk_var()->clk_id();
    tchecker::clock_id_t const rhs_var_id = action.typed_rhs_clk_var()->clk_id();

    bool const is_left_var_in_xd = _xd_clocks[lhs_var_id];
    bool const is_right_var_in_xd = _xd_clocks[rhs_var_id];

    // x = y preserves xd set if x in xd <==> y in xd
    bool const rename_preserves_xd =
        (is_left_var_in_xd && is_right_var_in_xd) || ((!is_left_var_in_xd) && (!is_right_var_in_xd));

    if (not rename_preserves_xd) {
      _is_xd_safe = false;
    }

    // The constraints for the rhs clk is passed to lhs clk
    _prophecy_clks_checked_for_zero_or_minus_inf[lhs_var_id] = _prophecy_clks_checked_for_zero_or_minus_inf[rhs_var_id];
  }

  virtual void visit(tchecker::gta::typed_action_reset_t const &) { return; }
  virtual void visit(tchecker::gta::typed_action_rename_history_t const &) { return; }
  virtual void visit(tchecker::gta::typed_action_clk_variable_t const &) { assert(false); }

  virtual void visit(tchecker::gta::typed_guard_clk_variable_t const &) { assert(false); }

  virtual void visit(tchecker::gta::typed_guard_integer_t const & int_term) { assert(false); }

  virtual void visit(tchecker::gta::typed_guard_unary_lterm_t const & lterm)
  {
    _ctx._lterm_unary_clk = std::make_optional<tchecker::clock_id_t>(lterm.typed_var()->clk_id());
  }

  virtual void visit(tchecker::gta::typed_guard_binary_lterm_t const &) { return; }

  virtual void visit(tchecker::gta::typed_guard_integer_rterm_t const & rterm)
  {
    std::shared_ptr<tchecker::gta::typed_guard_integer_t const> const rterm_int_var = rterm.typed_val();
    if (rterm_int_var->val() == 0) {
      _ctx._rterm_constant = std::make_optional<enum guard_parse_ctx_t::rterm_constants>(guard_parse_ctx_t::ZERO);
    }
    else {
      _ctx._rterm_constant = std::make_optional<enum guard_parse_ctx_t::rterm_constants>(guard_parse_ctx_t::UN_INTERESTING);
    }
  }

  virtual void visit(tchecker::gta::typed_guard_constant_rterm_t const & rterm)
  {
    if (rterm.c() == tchecker::gta::RTERM_MINUS_INF) {
      _ctx._rterm_constant = std::make_optional<enum guard_parse_ctx_t::rterm_constants>(guard_parse_ctx_t::MINUS_INF);
    }
    else {
      _ctx._rterm_constant = std::make_optional<enum guard_parse_ctx_t::rterm_constants>(guard_parse_ctx_t::UN_INTERESTING);
    }
  }

  virtual void visit(tchecker::gta::typed_guard_t const & guard)
  {
    _ctx.reset();
    guard.typed_lterm()->visit(*this);
    guard.typed_rterm()->visit(*this);

    assert(_ctx._rterm_constant.has_value());

    // If lterm is a unary clock
    if (_ctx._lterm_unary_clk.has_value()) {
      switch (_ctx._rterm_constant.value()) {
      case guard_parse_ctx_t::ZERO:
        // x == 0 or x >= 0 checks if clk x has value 0
        if (guard.op() == tchecker::gta::GUARD_EQ || guard.op() == tchecker::gta::GUARD_GE) {
          _prophecy_clks_checked_for_zero_or_minus_inf[_ctx._lterm_unary_clk.value()] = 1;
        }
        break;

      case guard_parse_ctx_t::MINUS_INF:
        // x == -inf or x <= -inf checks if clk x has value -inf
        if (guard.op() == tchecker::gta::GUARD_EQ || guard.op() == tchecker::gta::GUARD_LE) {
          _prophecy_clks_checked_for_zero_or_minus_inf[_ctx._lterm_unary_clk.value()] = 1;
        }
        break;

      case guard_parse_ctx_t::UN_INTERESTING:
        break;

      default:
        throw std::runtime_error("rterm constant not handled");
      }
    }
  }

  tchecker::gta::system_t const & _gta_system;                          /*!< GTA system */
  boost::dynamic_bitset<> const & _xd_clocks;                           /*!< X_D set of clks */
  boost::dynamic_bitset<> _prophecy_clks_checked_for_zero_or_minus_inf; /*!< Prophecy clocks constrained to be 0 or -inf */
  guard_parse_ctx_t _ctx;                                               /*!< The context stored during parsing of a guard */
  bool _is_xd_safe; /*!< Maintains if the gta program is xd safe while visiting the gta programs */
};
} // end of namespace details

bool is_xd_safe(tchecker::gta::system_t const & gta_system)
{
  tchecker::gta::details::minimal_xd_set_t x(gta_system);
  boost::dynamic_bitset<> const & minimal_xd_set = x.compute_minimal_xd_set();
  return tchecker::gta::details::xd_safety_check_t(gta_system, minimal_xd_set).is_xd_safe();
}
} // end of namespace gta
} // end of namespace tchecker
