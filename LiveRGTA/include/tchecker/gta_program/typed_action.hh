/*
 * This file is a part of the TChecker project.
 *
 * See files AUTHORS and LICENSE for copyright details.
 *
 */

#ifndef TCHECKER_GTA_PROGRAM_TYPED_ACTION_HH
#define TCHECKER_GTA_PROGRAM_TYPED_ACTION_HH

#include <vector>

#include "tchecker/basictypes.hh"
#include "tchecker/gta_program/action.hh"

/*!
 \file typed_action.hh
 \brief Typed Action
 */

namespace tchecker {
namespace gta {

/*!
 \brief Type of actions
 */
enum action_type_t {
  ACTION_TYPE_BAD,              // Bad type
  ACTION_TYPE_CLK_VAR,          // Clock variable
  ACTION_TYPE_RELEASE_PROPHECY, // Action [x] where x is a prophecy clock
  ACTION_TYPE_RESET_HISTORY,    // Action [x] where x is a history clock
  ACTION_TYPE_RENAME_PROPHECY,  // Action x = y with x and y as prophecy clocks
  ACTION_TYPE_RENAME_HISTORY    // Action x = y with x and y as history clocks
};

/*!
 \brief Output operator for action types
 \param os : output stream
 \param type : action type
 \post type has been output to os
 \return os after output
 */
std::ostream & operator<<(std::ostream & os, enum tchecker::gta::action_type_t type);

// Forward declaration
class typed_action_visitor_t;

/*!
 \class typed_action_t
 \brief Action along with its type
 */
class typed_action_t : public virtual tchecker::gta::action_t {
public:
  /*!
   \brief Constructor
   \param type : action type
   */
  typed_action_t(enum tchecker::gta::action_type_t type);

  /*!
   \brief Destructor
   */
  virtual ~typed_action_t() = default;

  /*!
   \brief Accessor
   \return this action's type
   */
  inline enum tchecker::gta::action_type_t type() const { return _type; }

  /*!
   \brief Clone
   \return A clone of this action
   */
  virtual tchecker::gta::typed_action_t * clone() const override = 0;

  /*!
   \brief Visit
   \param v : typed action visitor
   */
  virtual void visit(tchecker::gta::typed_action_visitor_t & v) const = 0;

  /*!
   \brief Untyped visit
   \param v : action visitor
   */
  virtual void visit(tchecker::gta::action_visitor_t & v) const override = 0;

protected:
  enum tchecker::gta::action_type_t const _type; /*!< Action type */
};

/*!
 \class typed_action_clk_variable_t
 \brief Class to represent a clock variable in an action with type information
 */
class typed_action_clk_variable_t : public virtual tchecker::gta::typed_action_t,
                                    public virtual tchecker::gta::action_variable_t {
public:
  /*!
   * \brief Constructor
   * \param name : Clock name
   * \param clk_id : Clock ID
   * \param clk_type : GTA Clock type
   */
  typed_action_clk_variable_t(std::string const & name, tchecker::clock_id_t const clk_id,
                              enum tchecker::gta::clk_type_t const clk_type);

  /*!
   * \brief Destructor
   */
  ~typed_action_clk_variable_t() = default;

  /*!
   * \brief Accessor
   * \return clock id
   */
  inline tchecker::clock_id_t clk_id() const { return _clk_id; }

  /*!
   * \brief Accessor
   * \return clock type
   */
  inline tchecker::gta::clk_type_t clk_type() const { return _clk_type; }

  /*!
   \brief Clone
   \return clone of this
   */
  virtual tchecker::gta::typed_action_clk_variable_t * clone() const override;

  /*!
   \brief Visit
   \param v : typed action visitor
   */
  virtual void visit(tchecker::gta::typed_action_visitor_t & v) const override;

  /*!
   \brief Untyped visit
   \param v : action visitor
   */
  virtual void visit(tchecker::gta::action_visitor_t & v) const override;

protected:
  tchecker::clock_id_t const _clk_id;
  enum tchecker::gta::clk_type_t const _clk_type;
};

/*!
 \class typed_action_release_t
 \brief Class to represent a release operation for a prophecy clock
 */
class typed_action_release_t : public virtual tchecker::gta::typed_action_t,
                               public virtual tchecker::gta::release_reset_action_t {
public:
  /*!
   * \brief Constructor
   * \param clk_var : Typed clock variable
   * \pre Clk var must have clock type as tchecker::gta::GTA_CLK_TYPE_PROPHECY
   * \throw std::invalid_argument if clk_var is not a prophecy clock
   */
  explicit typed_action_release_t(std::shared_ptr<tchecker::gta::typed_action_clk_variable_t> const & clk_var);

  /*!
   * \brief Destructor
   */
  ~typed_action_release_t() = default;

  /*!
   * \brief Accessor
   * \return clock variable
   */
  inline std::shared_ptr<tchecker::gta::typed_action_clk_variable_t> const typed_clk_var() const { return _typed_clk_var; }

  /*!
   \brief Clone
   \return clone of this
   */
  virtual tchecker::gta::typed_action_release_t * clone() const override;

  /*!
   \brief Visit
   \param v : typed action visitor
   */
  virtual void visit(tchecker::gta::typed_action_visitor_t & v) const override;

  /*!
   \brief Untyped visit
   \param v : action visitor
   */
  virtual void visit(tchecker::gta::action_visitor_t & v) const override;

protected:
  std::shared_ptr<tchecker::gta::typed_action_clk_variable_t> const _typed_clk_var;
};

/*!
 \class typed_action_reset_t
 \brief Class to represent a reset operation for a history clock
 */
class typed_action_reset_t : public virtual tchecker::gta::typed_action_t,
                             public virtual tchecker::gta::release_reset_action_t {
public:
  /*!
   * \brief Constructor
   * \param clk_var : Typed clock variable
   * \pre clk var must have clock type as tchecker::gta::GTA_CLK_TYPE_HISTORY
   * \throw std::invalid_argument if clk_var is not a history clock
   */
  explicit typed_action_reset_t(std::shared_ptr<tchecker::gta::typed_action_clk_variable_t> const & clk_var);

  /*!
   * \brief Destructor
   */
  ~typed_action_reset_t() = default;

  /*!
   * \brief Accessor
   * \return clock variable
   */
  inline std::shared_ptr<tchecker::gta::typed_action_clk_variable_t> const typed_clk_var() const { return _typed_clk_var; }

  /*!
   \brief Clone
   \return clone of this
   */
  virtual tchecker::gta::typed_action_reset_t * clone() const override;

  /*!
   \brief Visit
   \param v : typed action visitor
   */
  virtual void visit(tchecker::gta::typed_action_visitor_t & v) const override;

  /*!
   \brief Untyped visit
   \param v : action visitor
   */
  virtual void visit(tchecker::gta::action_visitor_t & v) const override;

protected:
  std::shared_ptr<tchecker::gta::typed_action_clk_variable_t> const _typed_clk_var;
};

/*!
 \class typed_action_rename_prophecy_t
 \brief Class to represent a reset operation for a history clock
 */
class typed_action_rename_prophecy_t : public virtual tchecker::gta::typed_action_t,
                                       public virtual tchecker::gta::rename_action_t {
public:
  /*!
   * \brief Constructor
   * \param lhs_clk_var : Typed clock variable at LHS
   * \param rhs_clk_var : Typed clock variable at RHS
   * \pre both clk vars must have clock type as tchecker::gta::GTA_CLK_TYPE_PROPHECY
   * \throw std::invalid_argument if either clk_var is not a prophecy clock
   */
  typed_action_rename_prophecy_t(std::shared_ptr<tchecker::gta::typed_action_clk_variable_t> const & lhs_clk_var,
                                 std::shared_ptr<tchecker::gta::typed_action_clk_variable_t> const & rhs_clk_var);

  /*!
   * \brief Destructor
   */
  ~typed_action_rename_prophecy_t() = default;

  /*!
   * \brief Accessor
   * \return lhs clock variable
   */
  inline std::shared_ptr<tchecker::gta::typed_action_clk_variable_t> const typed_lhs_clk_var() const
  {
    return _typed_lhs_clk_var;
  }

  /*!
   * \brief Accessor
   * \return rhs clock variable
   */
  inline std::shared_ptr<tchecker::gta::typed_action_clk_variable_t> const typed_rhs_clk_var() const
  {
    return _typed_rhs_clk_var;
  }

  /*!
   \brief Clone
   \return clone of this
   */
  virtual tchecker::gta::typed_action_rename_prophecy_t * clone() const override;

  /*!
   \brief Visit
   \param v : typed action visitor
   */
  virtual void visit(tchecker::gta::typed_action_visitor_t & v) const override;

  /*!
   \brief Untyped visit
   \param v : action visitor
   */
  virtual void visit(tchecker::gta::action_visitor_t & v) const override;

protected:
  std::shared_ptr<tchecker::gta::typed_action_clk_variable_t> const _typed_lhs_clk_var;
  std::shared_ptr<tchecker::gta::typed_action_clk_variable_t> const _typed_rhs_clk_var;
};

/*!
 \class typed_action_rename_history_t
 \brief Class to represent a reset operation for a history clock
 */
class typed_action_rename_history_t : public virtual tchecker::gta::typed_action_t,
                                      public virtual tchecker::gta::rename_action_t {
public:
  /*!
   * \brief Constructor
   * \param lhs_clk_var : Typed clock variable at LHS
   * \param rhs_clk_var : Typed clock variable at RHS
   * \pre both clk vars must have clock type as tchecker::gta::GTA_CLK_TYPE_HISTORY
   * \throw std::invalid_argument if either clk_var is not a history clock
   */
  typed_action_rename_history_t(std::shared_ptr<tchecker::gta::typed_action_clk_variable_t> const & lhs_clk_var,
                                std::shared_ptr<tchecker::gta::typed_action_clk_variable_t> const & rhs_clk_var);

  /*!
   * \brief Destructor
   */
  ~typed_action_rename_history_t() = default;

  /*!
   * \brief Accessor
   * \return lhs clock variable
   */
  inline std::shared_ptr<tchecker::gta::typed_action_clk_variable_t> const typed_lhs_clk_var() const
  {
    return _typed_lhs_clk_var;
  }

  /*!
   * \brief Accessor
   * \return rhs clock variable
   */
  inline std::shared_ptr<tchecker::gta::typed_action_clk_variable_t> const typed_rhs_clk_var() const
  {
    return _typed_rhs_clk_var;
  }

  /*!
   \brief Clone
   \return clone of this
   */
  virtual tchecker::gta::typed_action_rename_history_t * clone() const override;

  /*!
   \brief Visit
   \param v : typed action visitor
   */
  virtual void visit(tchecker::gta::typed_action_visitor_t & v) const override;

  /*!
   \brief Untyped visit
   \param v : action visitor
   */
  virtual void visit(tchecker::gta::action_visitor_t & v) const override;

protected:
  std::shared_ptr<tchecker::gta::typed_action_clk_variable_t> const _typed_lhs_clk_var;
  std::shared_ptr<tchecker::gta::typed_action_clk_variable_t> const _typed_rhs_clk_var;
};
/*!
 \class typed_action_visitor_t
 \brief Visitor pattern for typed actions
 */
class typed_action_visitor_t {
public:
  /*!
   \brief Constructor
   */
  typed_action_visitor_t() = default;

  /*!
   \brief Copy constructor
   */
  typed_action_visitor_t(tchecker::gta::typed_action_visitor_t const &) = default;

  /*!
   \brief Move constructor
   */
  typed_action_visitor_t(tchecker::gta::typed_action_visitor_t &&) = default;

  /*!
   \brief Destructor
   */
  virtual ~typed_action_visitor_t() = default;

  /*!
   \brief Assignment operator
   */
  tchecker::gta::typed_action_visitor_t & operator=(tchecker::gta::typed_action_visitor_t const &) = default;

  /*!
   \brief Move assignment operator
   */
  tchecker::gta::typed_action_visitor_t & operator=(tchecker::gta::typed_action_visitor_t &&) = default;

  /*!
   \brief Visitors
   */
  virtual void visit(tchecker::gta::typed_action_clk_variable_t const &) = 0;
  virtual void visit(tchecker::gta::typed_action_release_t const &) = 0;
  virtual void visit(tchecker::gta::typed_action_reset_t const &) = 0;
  virtual void visit(tchecker::gta::typed_action_rename_prophecy_t const &) = 0;
  virtual void visit(tchecker::gta::typed_action_rename_history_t const &) = 0;
};

/*!
 \brief Type to represent a list of typed actions present in a GTA program
 */
using typed_actions_t = std::vector<std::shared_ptr<tchecker::gta::typed_action_t const>>;

/*!
 \brief Output operator for typed actions list
 \param os : output stream
 \param typed_actions_list : typed actions list
 \post actions has been output to os
 \return os after actions have been output
 */
std::ostream & operator<<(std::ostream & os, tchecker::gta::typed_actions_t & typed_actions_list);

} // end of namespace gta
} // end of namespace tchecker
#endif // TCHECKER_GTA_PROGRAM_TYPED_ACTION_HH
