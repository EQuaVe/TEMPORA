/*
 * This file is a part of the TChecker project.
 *
 * See files AUTHORS and LICENSE for copyright details.
 *
 */

#include <cassert>
#include <memory>
#include <stdexcept>

#include "tchecker/basictypes.hh"
#include "tchecker/gta_program/guard.hh"
#include "tchecker/gta_program/typechecking.hh"
#include "tchecker/gta_program/typed_guard.hh"
#include "tchecker/utils/log.hh"
#include "tchecker/variables/variables.hh"

namespace tchecker {

namespace gta {

namespace details {

/*!
 \class guard_typechecker_t
 \brief Guard typechecking visitor
 */
class guard_typechecker_t : tchecker::gta::guard_visitor_t {
public:
  /*!
   * \brief Constructor
   * \param clocks : Array of clock variables in system
   * \param prophecy_clocks : The subset of clocks which are declared prophecy
   * \pre sizes of flattened clocks and prophecy clocks match
   * \throw std::invalid_argument if the sizes of clock and prophecy_clocks do not match
   */
  guard_typechecker_t(tchecker::clock_variables_t const & clocks, boost::dynamic_bitset<> const & prophecy_clocks)
      : _clocks(clocks), _prophecy_clocks(prophecy_clocks)
  {
    if (clocks.size(tchecker::VK_FLATTENED) != prophecy_clocks.size()) {
      throw std::invalid_argument("Sizes of clocks and prophecy clocks do not match");
    }
  }

  /*!
   \brief Copy constructor (DELETED)
   */
  guard_typechecker_t(tchecker::gta::details::guard_typechecker_t const &) = delete;

  /*!
   \brief Move constructor (DELETED)
   */
  guard_typechecker_t(tchecker::gta::details::guard_typechecker_t &&) = delete;

  /*!
   \brief Destructor
   */
  virtual ~guard_typechecker_t() = default;

  /*!
   \brief Assignment operator (DELETED)
   */
  tchecker::gta::details::guard_typechecker_t & operator=(tchecker::gta::details::guard_typechecker_t const &) = delete;

  /*!
   \brief Move assignment operator (DELETED)
   */
  tchecker::gta::details::guard_typechecker_t & operator=(tchecker::gta::details::guard_typechecker_t &&) = delete;

  /*!
   * \brief typechecks a conjunction of guards
   * \param guards : untyped conjunction of guards
   * \return a pointer to the typed conjunction of guards. The pointer is null
   * if typechecker is unable to assign type to any of the guards
   */
  tchecker::gta::typed_guards_conjunction_ptr_t typecheck(tchecker::gta::guards_conjunction_ptr_t const & guards)
  {
    tchecker::gta::typed_guards_conjunction_ptr_t res = std::make_shared<tchecker::gta::typed_guards_conjunction_t>();
    for (auto const & guard : (*guards)) {
      guard->visit(*this);

      if (curr_guard_type() == tchecker::gta::GUARD_TYPE_BAD) {
        res = nullptr;
        break;
      }

      assert(curr_guard_type() == tchecker::gta::GUARD_TYPE_SIMPLE || curr_guard_type() == tchecker::gta::GUARD_TYPE_DIAGONAL);
      res->push_back(std::dynamic_pointer_cast<tchecker::gta::typed_guard_t>(_typed_guard_term));
    }

    reset();
    return res;
  }

  /*!
   \brief Visitors
   */
  virtual void visit(tchecker::gta::guard_variable_t const & clk_var)
  {
    std::string const clk_name = clk_var.name();

    bool const is_clk_variable = _clocks.is_variable(clk_name);
    if (not is_clk_variable) {
      std::cerr << tchecker::log_error << "Clock not declared " << clk_name << std::endl;
      _typed_guard_term = nullptr;
      return;
    }

    tchecker::clock_id_t const clk_id = _clocks.id(clk_name);
    bool const is_prophecy_clk = _prophecy_clocks.at(clk_id);
    tchecker::gta::clk_type_t const clk_type =
        is_prophecy_clk ? tchecker::gta::GTA_CLK_TYPE_PROPHECY : tchecker::gta::GTA_CLK_TYPE_HISTORY;

    _typed_guard_term = std::make_shared<tchecker::gta::typed_guard_clk_variable_t>(clk_name, clk_id, clk_type);
  }

  virtual void visit(tchecker::gta::guard_integer_t const & int_term)
  {
    _typed_guard_term = std::make_shared<tchecker::gta::typed_guard_integer_t>(int_term.val());
  };

  virtual void visit(tchecker::gta::guard_unary_lterm_t const & unary_term)
  {
    unary_term.var()->visit(*this);

    if (curr_guard_type() == tchecker::gta::GUARD_TYPE_BAD) {
      return;
    }

    assert(curr_guard_type() == tchecker::gta::GUARD_TYPE_CLK_VAR);
    auto clk_var = std::dynamic_pointer_cast<tchecker::gta::typed_guard_clk_variable_t>(_typed_guard_term);

    _typed_guard_term = std::make_shared<tchecker::gta::typed_guard_unary_lterm_t>(clk_var);
  }

  virtual void visit(tchecker::gta::guard_binary_lterm_t const & binary_rterm)
  {
    binary_rterm.var_l()->visit(*this);

    if (curr_guard_type() == tchecker::gta::GUARD_TYPE_BAD) {
      return;
    }

    assert(curr_guard_type() == tchecker::gta::GUARD_TYPE_CLK_VAR);
    auto clk_var_l = std::dynamic_pointer_cast<tchecker::gta::typed_guard_clk_variable_t>(_typed_guard_term);

    binary_rterm.var_r()->visit(*this);

    if (curr_guard_type() == tchecker::gta::GUARD_TYPE_BAD) {
      return;
    }

    assert(curr_guard_type() == tchecker::gta::GUARD_TYPE_CLK_VAR);
    auto clk_var_r = std::dynamic_pointer_cast<tchecker::gta::typed_guard_clk_variable_t>(_typed_guard_term);
    _typed_guard_term = std::make_shared<tchecker::gta::typed_guard_binary_lterm_t>(clk_var_l, clk_var_r);
  }

  virtual void visit(tchecker::gta::guard_integer_rterm_t const & int_rterm)
  {
    int_rterm.val()->visit(*this);

    if (curr_guard_type() == tchecker::gta::GUARD_TYPE_BAD) {
      return;
    }

    assert(curr_guard_type() == tchecker::gta::GUARD_TYPE_INTEGER);
    auto int_term = std::dynamic_pointer_cast<tchecker::gta::typed_guard_integer_t>(_typed_guard_term);

    _typed_guard_term = std::make_shared<tchecker::gta::typed_guard_integer_rterm_t>(int_term);
  }

  virtual void visit(tchecker::gta::guard_constant_rterm_t const & const_rterm)
  {
    _typed_guard_term = std::make_shared<tchecker::gta::typed_guard_constant_rterm_t>(const_rterm.c());
  }

  virtual void visit(tchecker::gta::guard_t const & guard)
  {
    guard.lterm()->visit(*this);

    if (curr_guard_type() == tchecker::gta::GUARD_TYPE_BAD) {
      return;
    }
    assert(curr_guard_type() == GUARD_TYPE_UNARY_LTERM || curr_guard_type() == GUARD_TYPE_BINARY_LTERM);
    auto const typed_lterm = std::dynamic_pointer_cast<tchecker::gta::typed_guard_lterm_t>(_typed_guard_term);

    guard.rterm()->visit(*this);

    if (curr_guard_type() == tchecker::gta::GUARD_TYPE_BAD) {
      return;
    }
    assert(curr_guard_type() == GUARD_TYPE_INTEGER_RTERM || curr_guard_type() == GUARD_TYPE_CONSTANT_RTERM);
    auto const typed_rterm = std::dynamic_pointer_cast<tchecker::gta::typed_guard_rterm_t>(_typed_guard_term);

    _typed_guard_term = std::make_shared<tchecker::gta::typed_guard_t>(typed_lterm, guard.op(), typed_rterm);
  };

  /*!
   * \brief resets the typechecker
   */
  void reset() { _typed_guard_term = nullptr; }

private:
  enum tchecker::gta::guard_term_type_t curr_guard_type() const
  {
    if (_typed_guard_term == nullptr) {
      return tchecker::gta::GUARD_TYPE_BAD;
    }
    return _typed_guard_term->type();
  }

  tchecker::clock_variables_t const & _clocks;
  boost::dynamic_bitset<> const & _prophecy_clocks;

  std::shared_ptr<tchecker::gta::typed_guard_term_t> _typed_guard_term;
};

/*!
 \class action_typechecker_t
 \brief Action typechecking visitor
 */
class action_typechecker_t : tchecker::gta::action_visitor_t {
public:
  /*!
   * \brief Constructor
   * \param clocks : Array of clock variables in system
   * \param prophecy_clocks : The subset of clocks which are declared prophecy
   * \pre sizes of flattened clocks and prophecy clocks match
   * \throw std::invalid_argument if the sizes of clock and prophecy_clocks do not match
   */
  action_typechecker_t(tchecker::clock_variables_t const & clocks, boost::dynamic_bitset<> const & prophecy_clocks)
      : _clocks(clocks), _prophecy_clocks(prophecy_clocks)
  {
    if (clocks.size(tchecker::VK_FLATTENED) != prophecy_clocks.size()) {
      throw std::invalid_argument("Sizes of clocks and prophecy clocks do not match");
    }
  }

  /*!
   \brief Copy constructor (DELETED)
   */
  action_typechecker_t(tchecker::gta::details::action_typechecker_t const &) = delete;

  /*!
   \brief Move constructor (DELETED)
   */
  action_typechecker_t(tchecker::gta::details::action_typechecker_t &&) = delete;

  /*!
   \brief Destructor
   */
  virtual ~action_typechecker_t() = default;

  /*!
   \brief Assignment operator (DELETED)
   */
  tchecker::gta::details::action_typechecker_t & operator=(tchecker::gta::details::action_typechecker_t const &) = delete;

  /*!
   \brief Move assignment operator (DELETED)
   */
  tchecker::gta::details::action_typechecker_t & operator=(tchecker::gta::details::action_typechecker_t &&) = delete;

  /*!
   * \brief typechecks a list of actions
   * \param guards : untyped list of actions
   * \return a pointer to the typed list of actions. The pointer is null
   * if typechecker is unable to assign type to any of the actions
   */
  tchecker::gta::typed_actions_ptr_t typecheck(tchecker::gta::actions_ptr_t const & actions)
  {
    tchecker::gta::typed_actions_ptr_t res = std::make_shared<tchecker::gta::typed_actions_t>();
    for (auto const & action : (*actions)) {
      action->visit(*this);

      if (curr_action_type() == tchecker::gta::ACTION_TYPE_BAD) {
        res = nullptr;
        break;
      }

      res->push_back(_typed_action_term);
    }

    reset();
    return res;
  }

  /*!
   \brief Visitors
   */
  virtual void visit(tchecker::gta::action_variable_t const & clk_var)
  {
    std::string const clk_name = clk_var.name();

    bool const is_clk_variable = _clocks.is_variable(clk_name);
    if (not is_clk_variable) {
      std::cerr << tchecker::log_error << "Clock not declared " << clk_name << std::endl;
      _typed_action_term = nullptr;
      return;
    }

    tchecker::clock_id_t const clk_id = _clocks.id(clk_name);
    bool const is_prophecy_clk = _prophecy_clocks.at(clk_id);
    tchecker::gta::clk_type_t const clk_type =
        is_prophecy_clk ? tchecker::gta::GTA_CLK_TYPE_PROPHECY : tchecker::gta::GTA_CLK_TYPE_HISTORY;

    _typed_action_term = std::make_shared<tchecker::gta::typed_action_clk_variable_t>(clk_name, clk_id, clk_type);
  }

  virtual void visit(tchecker::gta::release_reset_action_t const & action)
  {
    action.clk_var()->visit(*this);

    if (curr_action_type() == tchecker::gta::ACTION_TYPE_BAD) {
      return;
    }

    assert(curr_action_type() == tchecker::gta::ACTION_TYPE_CLK_VAR);
    auto const clk_var = std::dynamic_pointer_cast<tchecker::gta::typed_action_clk_variable_t>(_typed_action_term);

    if (clk_var->clk_type() == tchecker::gta::GTA_CLK_TYPE_PROPHECY) {
      _typed_action_term = std::make_shared<tchecker::gta::typed_action_release_t>(clk_var);
    }
    else if (clk_var->clk_type() == tchecker::gta::GTA_CLK_TYPE_HISTORY) {
      _typed_action_term = std::make_shared<tchecker::gta::typed_action_reset_t>(clk_var);
    }
  }
  virtual void visit(tchecker::gta::rename_action_t const & action)
  {
    action.lhs_clk_var()->visit(*this);

    if (curr_action_type() == tchecker::gta::ACTION_TYPE_BAD) {
      return;
    }

    assert(curr_action_type() == tchecker::gta::ACTION_TYPE_CLK_VAR);
    auto const lhs_clk_var = std::dynamic_pointer_cast<tchecker::gta::typed_action_clk_variable_t>(_typed_action_term);

    action.rhs_clk_var()->visit(*this);

    if (curr_action_type() == tchecker::gta::ACTION_TYPE_BAD) {
      return;
    }

    assert(curr_action_type() == tchecker::gta::ACTION_TYPE_CLK_VAR);
    auto const rhs_clk_var = std::dynamic_pointer_cast<tchecker::gta::typed_action_clk_variable_t>(_typed_action_term);

    if (lhs_clk_var->clk_type() != rhs_clk_var->clk_type()) {
      std::cerr << tchecker::log_error << "Type of LHS clock " << lhs_clk_var->name() << "[" << lhs_clk_var->clk_type() << "]"
                << " and RHS clock " << rhs_clk_var->name() << "[" << rhs_clk_var->clk_type() << "]"
                << " do not match in rename action" << std::endl;
      _typed_action_term = nullptr;
      return;
    }

    if (lhs_clk_var->clk_type() == tchecker::gta::GTA_CLK_TYPE_PROPHECY) {
      _typed_action_term = std::make_shared<tchecker::gta::typed_action_rename_prophecy_t>(lhs_clk_var, rhs_clk_var);
    }
    else if (lhs_clk_var->clk_type() == tchecker::gta::GTA_CLK_TYPE_HISTORY) {
      _typed_action_term = std::make_shared<tchecker::gta::typed_action_rename_history_t>(lhs_clk_var, rhs_clk_var);
    }
  }

  /*!
   * \brief resets the typechecker
   */
  void reset() { _typed_action_term = nullptr; }

private:
  enum tchecker::gta::action_type_t curr_action_type() const
  {
    if (_typed_action_term == nullptr) {
      return tchecker::gta::ACTION_TYPE_BAD;
    }
    return _typed_action_term->type();
  }

  tchecker::clock_variables_t const & _clocks;
  boost::dynamic_bitset<> const & _prophecy_clocks;

  std::shared_ptr<tchecker::gta::typed_action_t> _typed_action_term;
};
} // end of namespace details

std::shared_ptr<tchecker::gta::typed_gta_program_t> typechecking(std::shared_ptr<tchecker::gta::gta_program_t> const & gta_prog,
                                                                 tchecker::clock_variables_t const & clocks,
                                                                 boost::dynamic_bitset<> const & prophecy_clocks)
{
  auto typed_gta_prog = std::make_shared<tchecker::gta::typed_gta_program_t>();
  tchecker::gta::details::guard_typechecker_t guard_typechecker(clocks, prophecy_clocks);
  tchecker::gta::details::action_typechecker_t action_typechecker(clocks, prophecy_clocks);

  for (auto iter = gta_prog->begin(); iter != gta_prog->end(); iter = gta_prog->next(iter)) {

    if (iter.first != nullptr) {
      tchecker::gta::typed_guards_conjunction_ptr_t typed_guards = guard_typechecker.typecheck(iter.first->get_value());
      if (typed_guards == nullptr) {
        typed_gta_prog = nullptr;
        break;
      }
      typed_gta_prog->append_typed_guards(typed_guards);
    }

    if (iter.second != nullptr) {
      tchecker::gta::typed_actions_ptr_t typed_actions = action_typechecker.typecheck(iter.second->get_value());
      if (typed_actions == nullptr) {
        typed_gta_prog = nullptr;
        break;
      }
      typed_gta_prog->append_typed_actions(typed_actions);
    }
  }

  return typed_gta_prog;
}

} // namespace gta

} // namespace tchecker
