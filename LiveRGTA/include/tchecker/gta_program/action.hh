/*
 * This file is a part of the TChecker project.
 *
 * See files AUTHORS and LICENSE for copyright details.
 *
 */

#ifndef TCHECKER_GTA_PROGRAM_ACTION_HH
#define TCHECKER_GTA_PROGRAM_ACTION_HH

#include <iostream>
#include <memory>
#include <vector>
/*!
 \file action.hh
 \brief Syntax tree for actions in GTA program
 */

namespace tchecker {

namespace gta {

class action_visitor_t; // forward declaration

/*!
  \class action_t
  \brief Base class to represent an action in a program
*/
class action_t {
public:
  /*!
  \brief Destructor
  */
  virtual ~action_t() = default;

  /*!
   \brief Clone
   \return A clone of this
   */
  virtual tchecker::gta::action_t * clone() const = 0;

  /*!
   \brief Visit
   \param v : action visitor
   */
  virtual void visit(tchecker::gta::action_visitor_t & v) const = 0;

  /*!
   \brief Output the action
   \param os : output stream
   \return os after this action has been output
   */
  virtual std::ostream & output(std::ostream & os) const = 0;

  /*!
   \brief Accessor
   \return this action as a string
   */
  std::string to_string() const;

private:
  friend std::ostream & operator<<(std::ostream & os, tchecker::gta::action_t const & action);
};

/*!
 \brief Output operator for action
 \param os : output stream
 \param action : gta action
 \post action has been output to os
 \return os after action has been output
 */
std::ostream & operator<<(std::ostream & os, tchecker::gta::action_t const & action);

/*!
  \class action_variable_t
  \brief Class to represent a variable in an action
*/
class action_variable_t : public virtual action_t {
public:
  /*!
   brief Constructor
   \param name : variable name
   \pre name is not empty
   \throw std::invalid_argument : if name is empty
  */
  explicit action_variable_t(std::string const & name);

  /*!
  \brief Destructor
  */
  virtual ~action_variable_t() = default;

  /*!
  \brief Accessor
  \return Name
   */
  inline std::string const & name() const { return _name; };

  /*!
   \brief Clone
   \return A clone of this
   */
  virtual tchecker::gta::action_variable_t * clone() const override;

  /*!
   \brief Visit
   \param v : action visitor
   */
  virtual void visit(tchecker::gta::action_visitor_t & v) const override;

  /*!
   \brief Output the variable
   \param os : output stream
   \return os after this variable has been output
   */
  virtual std::ostream & output(std::ostream & os) const override;

protected:
  std::string const _name;
};
/*!
  \class release_reset_action_t
  \brief Class to represent a release or reset of a clock variable
*/
class release_reset_action_t : public virtual action_t {
public:
  /*!
   brief Constructor
   \param clk_var : clock variable
   \pre clk_var is not null
   \throw std::invalid_argument : if clk_var is empty
  */
  explicit release_reset_action_t(std::shared_ptr<tchecker::gta::action_variable_t> const & clk_var);

  /*!
  \brief Destructor
  */
  virtual ~release_reset_action_t() = default;

  /*!
  \brief Accessor
  \return clock variable
  */
  inline std::shared_ptr<tchecker::gta::action_variable_t> const & clk_var() const { return _clk_var; };

  /*!
   \brief Clone
   \return A clone of this
   */
  virtual tchecker::gta::release_reset_action_t * clone() const override;

  /*!
   \brief Visit
   \param v : action visitor
   */
  virtual void visit(tchecker::gta::action_visitor_t & v) const override;

  /*!
   \brief Output the action
   \param os : output stream
   \return os after this action has been output
   */
  virtual std::ostream & output(std::ostream & os) const override;

protected:
  std::shared_ptr<tchecker::gta::action_variable_t> _clk_var;
};

/*!
  \class rename_action_t
  \brief Class to represent a rename operation
*/
class rename_action_t : public virtual action_t {
public:
  /*!
   brief Constructor
   \param lhs_clk_var : LHS clock var
   \param rhs_clk_var : RHS clock var
   \pre lhs_clk_var and rhs_clk_var are not empty null
   \throw std::invalid_argument : If either of the clock variables are null
  */
  explicit rename_action_t(std::shared_ptr<tchecker::gta::action_variable_t> const & lhs_clk_var,
                           std::shared_ptr<tchecker::gta::action_variable_t> const & rhs_clk_var);

  /*!
  \brief Destructor
  */
  virtual ~rename_action_t() = default;

  /*!
  \brief Accessor
  \return LHS clock variable
  */
  std::shared_ptr<tchecker::gta::action_variable_t> const lhs_clk_var() const { return _lhs_clk_var; }

  /*!
  \brief Accessor
  \return LHS clock variable
  */
  std::shared_ptr<tchecker::gta::action_variable_t> const rhs_clk_var() const { return _rhs_clk_var; }

  /*!
   \brief Clone
   \return A clone of this
   */
  virtual tchecker::gta::rename_action_t * clone() const override;

  /*!
   \brief Visit
   \param v : action visitor
   */
  virtual void visit(tchecker::gta::action_visitor_t & v) const override;

  /*!
   \brief Output the action
   \param os : output stream
   \return os after this action has been output
   */
  virtual std::ostream & output(std::ostream & os) const override;

protected:
  std::shared_ptr<tchecker::gta::action_variable_t> const _lhs_clk_var;
  std::shared_ptr<tchecker::gta::action_variable_t> const _rhs_clk_var;
};

/*!
 \class action_visitor_t
 \brief Action for gta guards
 */
class action_visitor_t {
public:
  /*!
   \brief Constructor
   */
  action_visitor_t() = default;

  /*!
   \brief Copy constructor
   */
  action_visitor_t(tchecker::gta::action_visitor_t const &) = default;

  /*!
   \brief Move constructor
   */
  action_visitor_t(tchecker::gta::action_visitor_t &&) = default;

  /*!
   \brief Destructor
   */
  virtual ~action_visitor_t() = default;

  /*!
   \brief Assignment operator
   */
  tchecker::gta::action_visitor_t & operator=(tchecker::gta::action_visitor_t const &) = default;

  /*!
   \brief Move assignment operator
   */
  tchecker::gta::action_visitor_t & operator=(tchecker::gta::action_visitor_t &&) = default;

  /*!
   \brief Visitors
   */
  virtual void visit(tchecker::gta::action_variable_t const & action) = 0;
  virtual void visit(tchecker::gta::release_reset_action_t const & action) = 0;
  virtual void visit(tchecker::gta::rename_action_t const & action) = 0;
};

/*!
 \brief Type to represent a list of actions present in a GTA program
 */
using actions_t = std::vector<std::shared_ptr<tchecker::gta::action_t const>>;

/*!
 \brief Output operator for actions list
 \param os : output stream
 \param actions_list : actions list
 \post actions has been output to os
 \return os after actions have been output
 */
std::ostream & operator<<(std::ostream & os, tchecker::gta::actions_t & actions_list);

} // end of namespace gta
} // end of namespace tchecker

#endif
