/*
 * This file is a part of the TChecker project.
 *
 * See files AUTHORS and LICENSE for copyright details.
 *
 */

#include <boost/dynamic_bitset.hpp>
#include <cassert>

#include "tchecker/dbm/db.hh"
#include "tchecker/gta_program/static_analysis.hh"

namespace tchecker {

namespace gta {

namespace details {

/*!
 * \class guard_static_parser_t
 * \brief visitor which statically parses a guard
 */

class guard_static_parser_t : tchecker::gta::typed_guard_visitor_t {
public:
  /*!
   * \brief Constructor
   */
  guard_static_parser_t() = default;

  /*!
   \brief Copy constructor (DELETED)
   */
  guard_static_parser_t(tchecker::gta::details::guard_static_parser_t const &) = delete;

  /*!
   \brief Move constructor (DELETED)
   */
  guard_static_parser_t(tchecker::gta::details::guard_static_parser_t &&) = delete;

  /*!
   \brief Destructor
   */
  virtual ~guard_static_parser_t() = default;

  /*!
   \brief Assignment operator (DELETED)
   */
  tchecker::gta::details::guard_static_parser_t & operator=(tchecker::gta::details::guard_static_parser_t const &) = delete;

  /*!
   \brief Move assignment operator (DELETED)
   */
  tchecker::gta::details::guard_static_parser_t & operator=(tchecker::gta::details::guard_static_parser_t &&) = delete;

protected:
  void parse(std::shared_ptr<tchecker::gta::typed_guard_t const> const & guard) { guard->visit(*this); }

  /*!
   \brief Visitors
   */
  virtual void visit(tchecker::gta::typed_guard_t const & guard)
  {
    guard.typed_lterm()->visit(*this);
    guard.typed_rterm()->visit(*this);
  }

  virtual void visit(tchecker::gta::typed_guard_unary_lterm_t const & lterm)
  {
    _parse_ctx.lclock = lterm.typed_var()->clk_id();
    _parse_ctx.rclock = tchecker::REFCLOCK_ID;
  }

  virtual void visit(tchecker::gta::typed_guard_binary_lterm_t const & lterm)
  {
    _parse_ctx.lclock = lterm.typed_var_l()->clk_id();
    _parse_ctx.rclock = lterm.typed_var_r()->clk_id();
  }

  virtual void visit(tchecker::gta::typed_guard_integer_rterm_t const & rterm) { _parse_ctx.rval = rterm.val()->val(); }

  virtual void visit(tchecker::gta::typed_guard_constant_rterm_t const & rterm)
  {
    switch (rterm.c()) {
    case tchecker::gta::RTERM_INF:
      _parse_ctx.rval = tchecker::dbm::extended::INF_VALUE;
      break;
    case tchecker::gta::RTERM_MINUS_INF:
      _parse_ctx.rval = tchecker::dbm::extended::MINUS_INF_VALUE;
      break;
    default:
      throw std::runtime_error("Incomplete switch case");
    }
  }

  virtual void visit(tchecker::gta::typed_guard_clk_variable_t const &) { assert(false); }
  virtual void visit(tchecker::gta::typed_guard_integer_t const &) { assert(false); }

  /*! \struct parse_contxt_t
   * \brief The context updated during the visits to lterm and rterm of a guard lterm cmp rterm
   */
  struct parse_ctx_t {
    tchecker::clock_id_t lclock; /*!< ID of left clock in a guard */
    tchecker::clock_id_t rclock; /*!< ID of right clock in a guard */
    tchecker::integer_t rval;    /*!< RHS value in a guard */
  };

  tchecker::gta::details::guard_static_parser_t::parse_ctx_t _parse_ctx;
};

/*!
 \class guard_static_analyser_t
 \brief Visitor to statically analyse the truth value of a guard
 */
class guard_static_analyser_t : public tchecker::gta::details::guard_static_parser_t {
public:
  /*!
   * \brief Constructor
   * \param prophecy_clocks : The subset of clocks which are declared prophecy
   */
  guard_static_analyser_t(boost::dynamic_bitset<> const & prophecy_clocks) : _prophecy_clocks(prophecy_clocks) {}

  /*!
   \brief Copy constructor (DELETED)
   */
  guard_static_analyser_t(tchecker::gta::details::guard_static_analyser_t const &) = delete;

  /*!
   \brief Move constructor (DELETED)
   */
  guard_static_analyser_t(tchecker::gta::details::guard_static_analyser_t &&) = delete;

  /*!
   \brief Destructor
   */
  virtual ~guard_static_analyser_t() = default;

  /*!
   \brief Assignment operator (DELETED)
   */
  tchecker::gta::details::guard_static_analyser_t & operator=(tchecker::gta::details::guard_static_analyser_t const &) = delete;

  /*!
   \brief Move assignment operator (DELETED)
   */
  tchecker::gta::details::guard_static_analyser_t & operator=(tchecker::gta::details::guard_static_analyser_t &&) = delete;

  tchecker::gta::guard_truth_val_t analyse_guard(std::shared_ptr<tchecker::gta::typed_guard_t const> const & guard)
  {
    parse(guard);

    // lterm <= INF is trivially true
    if (guard->op() == tchecker::gta::GUARD_LE && _parse_ctx.rval == tchecker::dbm::extended::INF_VALUE) {
      return tchecker::gta::TRIVIALLY_TRUE_GUARD;
    }

    // lterm >= -INF is trivially true
    if (guard->op() == tchecker::gta::GUARD_GE && _parse_ctx.rval == tchecker::dbm::extended::MINUS_INF_VALUE) {
      return tchecker::gta::TRIVIALLY_TRUE_GUARD;
    }
    // lterm > INF is trivially false
    if (guard->op() == tchecker::gta::GUARD_GT && _parse_ctx.rval == tchecker::dbm::extended::INF_VALUE) {
      return tchecker::gta::TRIVIALLY_FALSE_GUARD;
    }

    // lterm < -INF is trivially false
    if (guard->op() == tchecker::gta::GUARD_LT && _parse_ctx.rval == tchecker::dbm::extended::MINUS_INF_VALUE) {
      return tchecker::gta::TRIVIALLY_FALSE_GUARD;
    }

    // guard is x op c
    if (guard->type() == tchecker::gta::GUARD_TYPE_SIMPLE) {
      assert(_parse_ctx.lclock < _prophecy_clocks.size() && _parse_ctx.rclock == tchecker::REFCLOCK_ID);
      bool const is_clk_prop = _prophecy_clocks[_parse_ctx.lclock];

      if (is_clk_prop) {
        switch (guard->op()) {
        case tchecker::gta::GUARD_GE:
        case tchecker::gta::GUARD_EQ:
          // prop >=/== c where c > 0 is trivially false
          if (_parse_ctx.rval > 0) {
            return tchecker::gta::TRIVIALLY_FALSE_GUARD;
          }
          break;

        case tchecker::gta::GUARD_GT:
          // prop > c where c >= 0 is trivially false
          if (_parse_ctx.rval >= 0) {
            return tchecker::gta::TRIVIALLY_FALSE_GUARD;
          }
          break;

        case tchecker::gta::GUARD_LE:
          // prop <= c where c >= 0 is trivially true
          if (_parse_ctx.rval >= 0) {
            return tchecker::gta::TRIVIALLY_TRUE_GUARD;
          }
          break;

        case tchecker::gta::GUARD_LT:
          // prop < c where c > 0 is trivially true
          if (_parse_ctx.rval > 0) {
            return tchecker::gta::TRIVIALLY_TRUE_GUARD;
          }
          break;

        default:
          throw std::runtime_error("Incomplete switch statement");
        }
      }
      else { // When clock is a history clock
        switch (guard->op()) {
        case tchecker::gta::GUARD_GE:
          // hist >= c where c <= 0 is trivially true
          if (_parse_ctx.rval <= 0) {
            return tchecker::gta::TRIVIALLY_TRUE_GUARD;
          }
          break;

        case tchecker::gta::GUARD_GT:
          // hist > c where c < 0 is trivially true
          if (_parse_ctx.rval < 0) {
            return tchecker::gta::TRIVIALLY_TRUE_GUARD;
          }
          break;

        case tchecker::gta::GUARD_LE:
        case tchecker::gta::GUARD_EQ:
          // hist <=/== c where c < 0 is trivially false
          if (_parse_ctx.rval < 0) {
            return tchecker::gta::TRIVIALLY_FALSE_GUARD;
          }
          break;

        case tchecker::gta::GUARD_LT:
          // hist < c where c <= 0 is trivially false
          if (_parse_ctx.rval <= 0) {
            return tchecker::gta::TRIVIALLY_FALSE_GUARD;
          }
          break;

        default:
          throw std::runtime_error("Incomplete switch statement");
        }
      }
    }

    // While we can also analyse for non-diagonal constraints, the checks are more error-prone. We might add them in the future
    return tchecker::gta::NON_TRIVIAL_GUARD;
  }

private:
  boost::dynamic_bitset<> const & _prophecy_clocks;
};

/*!
 \class guard_to_constraint_t
 \brief Visitor to append clock constraints of a guard to a container
 */
class guard_to_constraint_t : public tchecker::gta::details::guard_static_parser_t {
public:
  /*!
   * \brief Constructor
   */
  guard_to_constraint_t() = default;

  /*!
   \brief Copy constructor (DELETED)
   */
  guard_to_constraint_t(tchecker::gta::details::guard_to_constraint_t const &) = delete;

  /*!
   \brief Move constructor (DELETED)
   */
  guard_to_constraint_t(tchecker::gta::details::guard_to_constraint_t &&) = delete;

  /*!
   \brief Destructor
   */
  virtual ~guard_to_constraint_t() = default;

  /*!
   \brief Assignment operator (DELETED)
   */
  tchecker::gta::details::guard_to_constraint_t & operator=(tchecker::gta::details::guard_to_constraint_t const &) = delete;

  /*!
   \brief Move assignment operator (DELETED)
   */
  tchecker::gta::details::guard_to_constraint_t & operator=(tchecker::gta::details::guard_to_constraint_t &&) = delete;

  /*!
   * \brief Appends clock constraints for a guard
   * \param guard : Typed guard in a GTA
   * \param cc : Clock constraints to which new constraints are added
   */
  void add_constraints_for_guard(std::shared_ptr<tchecker::gta::typed_guard_t const> const & guard,
                                 tchecker::clock_constraint_container_t & cc)
  {
    parse(guard);

    switch (guard->op()) {

    case tchecker::gta::GUARD_LE: {
      cc.emplace_back(
          tchecker::clock_constraint_t(_parse_ctx.lclock, _parse_ctx.rclock, tchecker::ineq_cmp_t::LE, _parse_ctx.rval));

      break;
    }

    case tchecker::gta::GUARD_LT: {
      cc.emplace_back(
          tchecker::clock_constraint_t(_parse_ctx.lclock, _parse_ctx.rclock, tchecker::ineq_cmp_t::LT, _parse_ctx.rval));

      break;
    }

    case tchecker::gta::GUARD_EQ: {
      if (guard->type() == tchecker::gta::guard_term_type_t::GUARD_TYPE_DIAGONAL) {
        throw std::runtime_error("Diagonal constraints cannot have equal operator");
      }

      // x == c <==> x - 0 <= c && 0 - x <= -c
      cc.emplace_back(
          tchecker::clock_constraint_t(_parse_ctx.lclock, _parse_ctx.rclock, tchecker::ineq_cmp_t::LE, _parse_ctx.rval));

      tchecker::integer_t minus_rval = tchecker::dbm::extended::negate_val(_parse_ctx.rval);
      cc.emplace_back(tchecker::clock_constraint_t(_parse_ctx.rclock, _parse_ctx.lclock, tchecker::ineq_cmp_t::LE, minus_rval));
      break;
    }

    case tchecker::gta::GUARD_GE: {
      if (guard->type() == tchecker::gta::guard_term_type_t::GUARD_TYPE_DIAGONAL) {
        throw std::runtime_error("Diagonal constraints cannot have greater than or equal operator");
      }

      // x >= c <==> 0 - x <= -c
      tchecker::integer_t const minus_rval = tchecker::dbm::extended::negate_val(_parse_ctx.rval);
      cc.emplace_back(
          tchecker::clock_constraint_t(tchecker::REFCLOCK_ID, _parse_ctx.lclock, tchecker::ineq_cmp_t::LE, minus_rval));

      break;
    }

    case tchecker::gta::GUARD_GT: {
      if (guard->type() == tchecker::gta::guard_term_type_t::GUARD_TYPE_DIAGONAL) {
        throw std::runtime_error("Diagonal constraints cannot have greater than or equal operator");
      }
      // x > c <==> 0 - x < -c
      tchecker::integer_t const minus_rval = tchecker::dbm::extended::negate_val(_parse_ctx.rval);
      cc.emplace_back(
          tchecker::clock_constraint_t(tchecker::REFCLOCK_ID, _parse_ctx.lclock, tchecker::ineq_cmp_t::LT, minus_rval));

      break;
    }

    default:
      throw std::runtime_error("Incomplete switch case");
    }
  }
};

class compute_released_clocks_t final : tchecker::gta::typed_action_visitor_t {
public:
  compute_released_clocks_t(boost::dynamic_bitset<> & released_clks) : _released_clks(released_clks) {}

  void add_released_clks(tchecker::gta::typed_actions_t const & actions)
  {
    for (std::shared_ptr<tchecker::gta::typed_action_t const> const & action : actions) {
      action->visit(*this);
    }
  }

private:
  virtual void visit(tchecker::gta::typed_action_release_t const & action)
  {
    tchecker::clock_id_t const id = action.typed_clk_var()->clk_id();
    _released_clks[id] = 1;
  }

  virtual void visit(tchecker::gta::typed_action_rename_prophecy_t const & action)
  {

    tchecker::clock_id_t const lhs_id = action.typed_lhs_clk_var()->clk_id();
    tchecker::clock_id_t const rhs_id = action.typed_rhs_clk_var()->clk_id();
    _released_clks[lhs_id] = _released_clks[rhs_id];
  }

  virtual void visit(tchecker::gta::typed_action_reset_t const &) { return; }

  virtual void visit(tchecker::gta::typed_action_rename_history_t const &) { return; }

  virtual void visit(tchecker::gta::typed_action_clk_variable_t const &) { assert(false); }

  boost::dynamic_bitset<> & _released_clks;
};

} // end of namespace details

std::ostream & compiled_gta_program_t::output(std::ostream & os) const
{

  for (tchecker::gta::compiled_gta_program_t::iterator_t curr = begin(); curr != end(); curr = next(curr)) {
    if (curr.first != nullptr) {
      tchecker::clock_constraint_container_t const & cc = *(curr.first->get_value());
      for (std::size_t i = 0; i < cc.size(); i++) {
        if (i != 0) {
          os << " && ";
        }
        os << cc[i];
      }
    }

    else if (curr.second != nullptr) {
      os << *(curr.second->get_value());
    }

    os << "; ";
  }
  return os;
}

std::string compiled_gta_program_t::to_string() const
{
  std::stringstream s;
  this->output(s);
  return s.str();
}

void compiled_gta_program_t::append_compiled_guards(tchecker::gta::compiled_guards_ptr_t const & constraints)
{
  if (constraints.get() == nullptr) {
    throw std::invalid_argument("null pointer passed for guards");
  }
  _compiled_gta_program_list.append_t1(constraints);
}

void compiled_gta_program_t::append_compiled_actions(tchecker::gta::compiled_actions_ptr_t const & actions)
{
  if (actions.get() == nullptr) {
    throw std::invalid_argument("null pointer passed for actions");
  }
  _compiled_gta_program_list.append_t2(actions);
}

tchecker::gta::compiled_guards_node_ptr_t const compiled_gta_program_t::beginning_constraints() const
{
  return _compiled_gta_program_list.get_head();
}

std::pair<tchecker::gta::compiled_guards_node_ptr_t, tchecker::gta::compiled_actions_node_ptr_t> const
compiled_gta_program_t::end_constraints_or_actions() const
{
  return _compiled_gta_program_list.get_tail();
}

tchecker::gta::compiled_gta_program_t::iterator_t compiled_gta_program_t::begin() const
{
  return _compiled_gta_program_list.begin();
}

tchecker::gta::compiled_gta_program_t::iterator_t compiled_gta_program_t::end() const
{
  return _compiled_gta_program_list.end();
}

tchecker::gta::compiled_gta_program_t::iterator_t
compiled_gta_program_t::next(tchecker::gta::compiled_gta_program_t::iterator_t & curr) const
{
  return _compiled_gta_program_list.next(curr);
}

tchecker::gta::compiled_gta_program_t::iterator_t compiled_gta_program_t::backward_begin() const
{
  return _compiled_gta_program_list.backward_begin();
}

tchecker::gta::compiled_gta_program_t::iterator_t compiled_gta_program_t::backward_end() const
{
  return _compiled_gta_program_list.backward_end();
}

tchecker::gta::compiled_gta_program_t::iterator_t compiled_gta_program_t::prev(iterator_t & curr) const
{
  return _compiled_gta_program_list.prev(curr);
}

std::ostream & operator<<(std::ostream & os, tchecker::gta::compiled_gta_program_t const & compiled_gta_program)
{
  return compiled_gta_program.output(os);
}

tchecker::gta::guard_truth_val_t analyse_guard(std::shared_ptr<tchecker::gta::typed_guard_t const> const & guard,
                                               boost::dynamic_bitset<> const & prophecy_clocks)
{
  tchecker::gta::details::guard_static_analyser_t guard_static_analyser(prophecy_clocks);
  return guard_static_analyser.analyse_guard(guard);
}

std::shared_ptr<tchecker::gta::typed_gta_program_t>
simplify_gta_prog(std::shared_ptr<tchecker::gta::typed_gta_program_t const> const & gta_prog,
                  boost::dynamic_bitset<> const & prophecy_clocks)
{
  assert(gta_prog != nullptr);
  auto simplified_gta_prog = std::make_shared<tchecker::gta::typed_gta_program_t>();

  for (tchecker::gta::typed_gta_program_t::iterator_t iter = gta_prog->begin(); iter != gta_prog->end();
       iter = gta_prog->next(iter)) {
    if (iter.first != nullptr) {
      auto pruned_guards = std::make_shared<tchecker::gta::typed_guards_conjunction_t>();

      for (std::shared_ptr<tchecker::gta::typed_guard_t const> const & guard : *(iter.first->get_value())) {
        tchecker::gta::guard_truth_val_t const truth_val = tchecker::gta::analyse_guard(guard, prophecy_clocks);

        if (truth_val == tchecker::gta::TRIVIALLY_TRUE_GUARD) {
          // We do not append trivially true guards to gta prog
        }
        else if (truth_val == tchecker::gta::TRIVIALLY_FALSE_GUARD) {
          // GTA program is unsatisfiable
          simplified_gta_prog = nullptr;
          return nullptr;
        }
        else if (truth_val == tchecker::gta::NON_TRIVIAL_GUARD) {
          pruned_guards->push_back(guard);
        }
        else {
          throw std::runtime_error("Unrecongnised truth value");
        }
      }
      simplified_gta_prog->append_typed_guards(pruned_guards);
    }
    else if (iter.second != nullptr) {
      simplified_gta_prog->append_typed_actions(iter.second->get_value());
    }
  }

  return simplified_gta_prog;
}

tchecker::clock_constraint_container_t diagonal_constraint_inf(tchecker::clock_id_t const x,
                                                               tchecker::gta::clk_type_t const x_type,
                                                               tchecker::clock_id_t const y,
                                                               tchecker::gta::clk_type_t const y_type)
{
  /*
   * x - y == inf <=\=> x - y <= inf (trivially true) && y - x <= -inf as this only includes the solutions y = -inf, x >
   * -inf AND x = inf, y < inf.
   * The equivalent condition is x == inf OR y == -inf. We can return this disjunction, but a finer analysis on types of
   * clocks can help avoid this disjunction.
   */
  tchecker::clock_constraint_container_t constraints;
  if (x_type == tchecker::gta::GTA_CLK_TYPE_HISTORY && y_type == tchecker::gta::GTA_CLK_TYPE_PROPHECY) {
    // x == inf and y cannot be -inf
    constraints.push_back(tchecker::clock_constraint_t(tchecker::REFCLOCK_ID, x, tchecker::ineq_cmp_t::LE,
                                                       tchecker::dbm::extended::MINUS_INF_VALUE));
  }

  else if (x_type == tchecker::gta::GTA_CLK_TYPE_PROPHECY && y_type == tchecker::gta::GTA_CLK_TYPE_PROPHECY) {
    // y == -inf and x cannot be inf
    constraints.push_back(tchecker::clock_constraint_t(y, tchecker::REFCLOCK_ID, tchecker::ineq_cmp_t::LE,
                                                       tchecker::dbm::extended::MINUS_INF_VALUE));
  }

  else {
    // If both x and y are not simultaneously history or prophecy, x - y == inf <==> y - x <= -inf
    constraints.push_back(
        tchecker::clock_constraint_t(y, x, tchecker::ineq_cmp_t::LE, tchecker::dbm::extended::MINUS_INF_VALUE));
  };
  return constraints;
}

tchecker::clock_constraint_container_t constraints_for_guards(tchecker::gta::typed_guards_conjunction_t const & guards)
{
  tchecker::clock_constraint_container_t cc;
  tchecker::gta::details::guard_to_constraint_t guard_to_constraint;

  for (std::shared_ptr<tchecker::gta::typed_guard_t const> const & guard : guards) {
    guard_to_constraint.add_constraints_for_guard(guard, cc);
  }

  return cc;
}

tchecker::gta::compiled_gta_program_t compile_gta_program(tchecker::gta::typed_gta_program_t const & gta_prog)

{
  tchecker::gta::compiled_gta_program_t compiled_gta_program;

  for (tchecker::gta::typed_gta_program_t::iterator_t iter = gta_prog.begin(); iter != gta_prog.end();
       iter = gta_prog.next(iter)) {
    if (iter.first != nullptr) {
      tchecker::gta::typed_guards_conjunction_t const & guards = *(iter.first->get_value());
      auto cc = std::make_shared<tchecker::clock_constraint_container_t const>(constraints_for_guards(guards));
      compiled_gta_program.append_compiled_guards(cc);
    }
    else if (iter.second != nullptr) {
      compiled_gta_program.append_compiled_actions(iter.second->get_value());
    }
  }
  return compiled_gta_program;
}

boost::dynamic_bitset<> released_clks(tchecker::gta::typed_gta_program_t const & gta_prog, tchecker::clock_id_t const num_clks)
{
  boost::dynamic_bitset<> released_clks(num_clks);
  tchecker::gta::details::compute_released_clocks_t compute_released_clocks(released_clks);

  for (tchecker::gta::typed_gta_program_t::iterator_t it = gta_prog.begin(); it != gta_prog.end(); it = gta_prog.next(it)) {
    if (it.second != nullptr) {
      compute_released_clocks.add_released_clks(*(it.second->get_value()));
    }
  }

  return released_clks;
}

} // end of namespace gta

} // namespace tchecker
