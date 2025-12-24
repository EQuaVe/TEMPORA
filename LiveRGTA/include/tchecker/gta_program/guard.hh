/*
 * This file is a part of the TChecker project.
 *
 * See files AUTHORS and LICENSE for copyright details.
 *
 */

#ifndef TCHECKER_GTA_PROGRAM_GUARD_HH
#define TCHECKER_GTA_PROGRAM_GUARD_HH

#include <iostream>
#include <memory>
#include <vector>

#include "tchecker/basictypes.hh"

/*!
 \file guard.hh
 \brief Syntax tree for guards in GTA
 */

namespace tchecker {

namespace gta {

class guard_visitor_t; // forward declaration

/*!
  \class guard_term_t
  \brief Base class for terms present in a guard
*/
class guard_term_t {
public:
  /*!
  \brief Destructor
  */
  virtual ~guard_term_t() = default;

  /*!
   \brief Clone
   \return A clone of this
   */
  virtual tchecker::gta::guard_term_t * clone() const = 0;

  /*!
   \brief Visit
   \param v : guard visitor
   */
  virtual void visit(tchecker::gta::guard_visitor_t & v) const = 0;

  /*!
   \brief Output the guard
   \param os : output stream
   \return os after this term has been output
   */
  virtual std::ostream & output(std::ostream & os) const = 0;

  /*!
   \brief Accessor
   \return this guard as a string
   */
  std::string to_string() const;

private:
  friend std::ostream & operator<<(std::ostream & os, tchecker::gta::guard_term_t const & guard);
};

/*!
 \brief Output operator for guard terms
 \param os : output stream
 \param term : guard term
 \post term has been output to os
 \return os after term has been output
 */
std::ostream & operator<<(std::ostream & os, tchecker::gta::guard_term_t const & term);

/*!
  brief Type of binary comparision operator for GTA
*/
enum guard_operator_t { GUARD_EQ, GUARD_LE, GUARD_LT, GUARD_GE, GUARD_GT };

/*!
 \brief Output operator for comparision operators
 \param os : output stream
 \param op : comparision operator
 \post op has been output to os
 \return os after op has been output
 */
std::ostream & operator<<(std::ostream & os, enum tchecker::gta::guard_operator_t op);

/*!
  \class guard_variable_t
  \brief class to represent variables occuring in guard
*/
class guard_variable_t : public virtual tchecker::gta::guard_term_t {
public:
  /*!
   \brief Constructor
   \param name : variable name
   \pre name is not empty
   \throw std::invalid_argument : if name is empty
   */
  explicit guard_variable_t(std::string const & name);

  /*!
  \brief Destructor
  */
  virtual ~guard_variable_t() = default;

  /*
  \brief Accessor
  \return Name
  */
  inline std::string const & name() const { return _name; }

  /*!
   \brief Clone
   \return A clone of this
   */
  virtual tchecker::gta::guard_variable_t * clone() const override;

  /*!
   \brief Visit
   \param v : guard visitor
   \post v.visit(*this) has been called
   */
  virtual void visit(tchecker::gta::guard_visitor_t & v) const override;

  /*!
   \brief Output the variable name
   \param os : output stream
   \post this has been output to os
   \return os after this variable has been output
   */
  virtual std::ostream & output(std::ostream & os) const override;

protected:
  std::string const _name; /*!< Variable name*/
};

/*!
  \class guard_integer_t
  \brief class to represent integers occuring in guard
*/
class guard_integer_t : public virtual tchecker::gta::guard_term_t {
public:
  /*!
   \brief Constructor
   \param val : integer constant
   */
  explicit guard_integer_t(tchecker::integer_t const val);

  /*!
  \brief Destructor
  */
  virtual ~guard_integer_t() = default;

  /*
  \brief Accessor
  \return value of the integer
  */
  inline tchecker::integer_t const & val() const { return _val; }

  /*!
   \brief Clone
   \return A clone of this
   */
  virtual tchecker::gta::guard_integer_t * clone() const override;

  /*!
   \brief Visit
   \param v : guard visitor
   \post v.visit(*this) has been called
   */
  virtual void visit(tchecker::gta::guard_visitor_t & v) const override;

  /*!
   \brief Output the integer
   \param os : output stream
   \post this has been output to os
   \return os after this variable has been output
   */
  virtual std::ostream & output(std::ostream & os) const override;

protected:
  tchecker::integer_t const _val; /*!< Value */
};

/*!
 /class guard_lterm_t
 /brief Terms that are present in LHS of guard
 */
class guard_lterm_t : public virtual tchecker::gta::guard_term_t {
public:
  /*!
  \brief Destructor
  */
  virtual ~guard_lterm_t() = default;

  /*!
  \brief Clone
  \note Specifies that an LHS term clone is an lhs term
  */
  virtual tchecker::gta::guard_lterm_t * clone() const override = 0;
};

/*
  \class guard_unary_lterm_t
  \brief Class to represent lterm for guards of the form
  x comparision c
*/
class guard_unary_lterm_t : public virtual tchecker::gta::guard_lterm_t {
public:
  /*!
   \brief Constructor
   \param var : lterm variable
   \pre var != nullptr
   \throw std::invalid_argument if var == nullptr
   \note this stores pointer to var
   */
  explicit guard_unary_lterm_t(std::shared_ptr<tchecker::gta::guard_variable_t const> const & var);

  /*!
  \brief Destructor
  */
  virtual ~guard_unary_lterm_t() = default;

  /*
  \brief Accessor
  \return var
  */
  inline std::shared_ptr<tchecker::gta::guard_variable_t const> const & var() const { return _var; }

  /*!
   \brief Clone
   \return A clone of this
   */
  virtual tchecker::gta::guard_unary_lterm_t * clone() const override;

  /*!
   \brief Visit
   \param v : guard visitor
   \post v.visit(*this) has been called
   */
  virtual void visit(tchecker::gta::guard_visitor_t & v) const override;

  /*!
   \brief Output the term
   \param os : output stream
   \post this has been output to os
   \return os after this lterm has been output
   */
  virtual std::ostream & output(std::ostream & os) const override;

protected:
  std::shared_ptr<tchecker::gta::guard_variable_t const> _var; /*!< Variable*/
};

/*!
  \class guard_binary_lterm_t
  \brief Class to represent lterms for diagonal guards of the form
  x - y comparision c
*/
class guard_binary_lterm_t : public virtual tchecker::gta::guard_lterm_t {
public:
  /*!
   \brief Constructor
   \param var_l : left operand of the lterm
   \param var_r : right operand of the lterm
   \pre var_l != nullptr and var_r != nullptr
   \throw std::invalid_argument if var_l or var_r is nullptr
   \note this stores pointer to var_l and var_r
   */
  guard_binary_lterm_t(std::shared_ptr<tchecker::gta::guard_variable_t const> const & var_l,
                       std::shared_ptr<tchecker::gta::guard_variable_t const> const & var_r);

  /*!
  \brief Destructor
  */
  virtual ~guard_binary_lterm_t() = default;

  /*
  \brief Accessor
  \return var_l
  */
  inline std::shared_ptr<tchecker::gta::guard_variable_t const> const & var_l() const { return _var_l; }

  /*
  \brief Accessor
  \return var_r
  */
  inline std::shared_ptr<tchecker::gta::guard_variable_t const> const & var_r() const { return _var_r; }

  /*!
   \brief Clone
   \return A clone of this
   */
  virtual tchecker::gta::guard_binary_lterm_t * clone() const override;

  /*!
   \brief Visit
   \param v : guard visitor
   \post v.visit(*this) has been called
   */
  virtual void visit(tchecker::gta::guard_visitor_t & v) const override;

  /*!
   \brief Output the term
   \param os : output stream
   \post this has been output to os
   \return os after this lterm has been output
   */
  virtual std::ostream & output(std::ostream & os) const override;

protected:
  std::shared_ptr<tchecker::gta::guard_variable_t const> _var_l; /*!< Left Variable*/

  std::shared_ptr<tchecker::gta::guard_variable_t const> _var_r; /*!< Right Variable*/
};

/*!
  brief Type of special constants appearing in RHS of guard
*/
enum guard_rterm_special_constants_t { RTERM_INF, RTERM_MINUS_INF };

/*!
 \brief Output operator for special rterm constants
 \param os : output stream
 \param c : constant
 \post c has been output to os
 \return os after c has been output
 */
std::ostream & operator<<(std::ostream & os, enum tchecker::gta::guard_rterm_special_constants_t c);

/*!
 /class guard_rterm_t
 /brief Terms that are present in RHS of guard
 */
class guard_rterm_t : public virtual tchecker::gta::guard_term_t {
public:
  /*!
  \brief Destructor
  */
  virtual ~guard_rterm_t() = default;

  /*!
  \brief Clone
  \note Specifies that an RHS term clone is an rhs term
  */
  virtual tchecker::gta::guard_rterm_t * clone() const override = 0;
};

/*
  \class guard_integer_rterm_t
  \brief Class to represent integer rterm for guards
*/
class guard_integer_rterm_t : public virtual tchecker::gta::guard_rterm_t {
public:
  /*!
   \brief Constructor
   \param val : value of the integer
   \pre val != nullptr
   \throw std::invalid_argument if val == nullptr
   \note this stores pointer to val
   */
  explicit guard_integer_rterm_t(std::shared_ptr<tchecker::gta::guard_integer_t const> const & val);

  /*!
  \brief Destructor
  */
  virtual ~guard_integer_rterm_t() = default;

  /*
  \brief Accessor
  \return val
  */
  inline std::shared_ptr<tchecker::gta::guard_integer_t const> const & val() const { return _val; }

  /*!
   \brief Clone
   \return A clone of this
   */
  virtual tchecker::gta::guard_integer_rterm_t * clone() const override;

  /*!
   \brief Visit
   \param v : guard visitor
   \post v.visit(*this) has been called
   */
  virtual void visit(tchecker::gta::guard_visitor_t & v) const override;

  /*!
   \brief Output the term
   \param os : output stream
   \post this has been output to os
   \return os after this lterm has been output
   */
  virtual std::ostream & output(std::ostream & os) const override;

protected:
  std::shared_ptr<tchecker::gta::guard_integer_t const> _val; /*!< Value of integer constant*/
};

/*
  \class guard_constant_rterm_t
  \brief Class to represent special constant rterm for guards
*/
class guard_constant_rterm_t : public virtual tchecker::gta::guard_rterm_t {
public:
  /*!
   \brief Constructor
   \param c : special constant
   */
  explicit guard_constant_rterm_t(enum tchecker::gta::guard_rterm_special_constants_t c);

  /*!
  \brief Destructor
  */
  virtual ~guard_constant_rterm_t() = default;

  /*
  \brief Accessor
  \return c
  */
  inline enum tchecker::gta::guard_rterm_special_constants_t c() const { return _c; }

  /*!
   \brief Clone
   \return A clone of this
   */
  virtual tchecker::gta::guard_constant_rterm_t * clone() const override;

  /*!
   \brief Visit
   \param v : guard visitor
   \post v.visit(*this) has been called
   */
  virtual void visit(tchecker::gta::guard_visitor_t & v) const override;

  /*!
   \brief Output the term
   \param os : output stream
   \post this has been output to os
   \return os after this lterm has been output
   */
  virtual std::ostream & output(std::ostream & os) const override;

protected:
  enum tchecker::gta::guard_rterm_special_constants_t _c; /*!< Value of special constant*/
};

/*!
  \class guard_t
  \brief Class to represent a guard
*/
class guard_t : public virtual tchecker::gta::guard_term_t {
public:
  /*!
   \brief Constructor
   \param lterm : left term of the guard
   \param rterm : right term of the guard
   \param c : comparision operator
   \pre lterm != nullptr and rterm != nullptr
   \throw std::invalid_argument if lterm or rterm is nullptr
   \note this stores pointer to lterm and rterm
   */
  guard_t(std::shared_ptr<tchecker::gta::guard_lterm_t const> const & lterm, enum tchecker::gta::guard_operator_t op,
          std::shared_ptr<tchecker::gta::guard_rterm_t const> const & rterm);

  /*!
  \brief Destructor
  */
  virtual ~guard_t() = default;

  /*
  \brief Accessor
  \return lterm
  */
  inline std::shared_ptr<tchecker::gta::guard_lterm_t const> const & lterm() const { return _lterm; }

  /*
  \brief Accessor
  \return comparision operator
  */
  inline tchecker::gta::guard_operator_t op() const { return _op; }

  /*
  \brief Accessor
  \return rterm
  */
  inline std::shared_ptr<tchecker::gta::guard_rterm_t const> const & rterm() const { return _rterm; }

  /*!
   \brief Clone
   \return A clone of this
   */
  virtual tchecker::gta::guard_t * clone() const override;

  /*!
   \brief Visit
   \param v : guard visitor
   \post v.visit(*this) has been called
   */
  virtual void visit(tchecker::gta::guard_visitor_t & v) const override;

  /*!
   \brief Output the guard
   \param os : output stream
   \post this has been output to os
   \return os after this guard has been output
   */
  virtual std::ostream & output(std::ostream & os) const override;

protected:
  std::shared_ptr<tchecker::gta::guard_lterm_t const> _lterm; /*!< Left Variable*/
  enum tchecker::gta::guard_operator_t _op;
  std::shared_ptr<tchecker::gta::guard_rterm_t const> _rterm; /*!< Right Variable*/
};

/*!
 \brief Type to represent a conjunction of guard constraints
*/
using guards_conjunction_t = std::vector<std::shared_ptr<tchecker::gta::guard_t const>>;

/*!
 \brief Output operator for guard conjunctions
 \param os : output stream
 \param guards : guard conjunctions
 \post guards has been output to os
 \return os after guards have been output
 */
std::ostream & operator<<(std::ostream & os, tchecker::gta::guards_conjunction_t & guards);

/*!
 \class guard_visitor_t
 \brief Visitor for gta guards
 */
class guard_visitor_t {
public:
  /*!
   \brief Constructor
   */
  guard_visitor_t() = default;

  /*!
   \brief Copy constructor
   */
  guard_visitor_t(tchecker::gta::guard_visitor_t const &) = default;

  /*!
   \brief Move constructor
   */
  guard_visitor_t(tchecker::gta::guard_visitor_t &&) = default;

  /*!
   \brief Destructor
   */
  virtual ~guard_visitor_t() = default;

  /*!
   \brief Assignment operator
   */
  tchecker::gta::guard_visitor_t & operator=(tchecker::gta::guard_visitor_t const &) = default;

  /*!
   \brief Move assignment operator
   */
  tchecker::gta::guard_visitor_t & operator=(tchecker::gta::guard_visitor_t &&) = default;

  /*!
   \brief Visitors
   */
  virtual void visit(tchecker::gta::guard_variable_t const & guard) = 0;
  virtual void visit(tchecker::gta::guard_integer_t const & guard) = 0;
  virtual void visit(tchecker::gta::guard_unary_lterm_t const & guard) = 0;
  virtual void visit(tchecker::gta::guard_binary_lterm_t const & guard) = 0;
  virtual void visit(tchecker::gta::guard_integer_rterm_t const & guard) = 0;
  virtual void visit(tchecker::gta::guard_constant_rterm_t const & guard) = 0;
  virtual void visit(tchecker::gta::guard_t const & guard) = 0;
};
} // end of namespace gta

} // end of namespace tchecker
#endif // TCHECKER_GTA_PROGRAM_GUARD_HH
