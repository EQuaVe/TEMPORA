/*
 * This file is a part of the TChecker project.
 *
 * See files AUTHORS and LICENSE for copyright details.
 *
 */

#ifndef TCHECKER_GTA_PROGRAM_TYPED_GUARD_HH
#define TCHECKER_GTA_PROGRAM_TYPED_GUARD_HH

#include <vector>

#include "tchecker/gta_program/guard.hh"

/*!
 \file typed_guard.hh
 \brief Typed Guard
 */

namespace tchecker {

namespace gta {

class typed_guard_visitor_t; // forward declaration

/*
 * \brief Type of guards
 */
enum guard_term_type_t {
  GUARD_TYPE_BAD,            // Bad Type
  GUARD_TYPE_CLK_VAR,        // Clock variable
  GUARD_TYPE_INTEGER,        // Integer
  GUARD_TYPE_UNARY_LTERM,    // Unary lterm
  GUARD_TYPE_BINARY_LTERM,   // Binary lterm
  GUARD_TYPE_INTEGER_RTERM,  // Integer rterm
  GUARD_TYPE_CONSTANT_RTERM, // Special constant rterm
  GUARD_TYPE_SIMPLE,         // guard of the form x # c
  GUARD_TYPE_DIAGONAL        // guard of the form x - y # c
};

/*!
 \brief Output operator for guards types
 \param os : output stream
 \param type : guard type
 \post type has been output to os
 \return os after output
 */
std::ostream & operator<<(std::ostream & os, enum tchecker::gta::guard_term_type_t const type);

/*!
 * brief Translates a lterm type to the guard type
 * param lterm_type : Guard type for lterm
 * \pre lterm_type is either GUARD_TYPE_UNARY_LTERM or GUARD_TYPE_BINARY_LTERM
 * \return GUARD_TYPE_UNARY_LTERM -> GUARD_TYPE_SIMPLE and GUARD_TYPE_BINARY_LTERM -> GUARD_TYPE_DIAGONAL
 * \throw std::invalid_argument if lterm_type is not a valid type for lterm
 */
enum tchecker::gta::guard_term_type_t guard_type_from_lterm_type(enum tchecker::gta::guard_term_type_t const lterm_type);

/*!
 \class typed_guard_term_t
 \brief Guard term along with its type
 */
class typed_guard_term_t : public virtual tchecker::gta::guard_term_t {
public:
  /*!
   \brief Constructor
   \param type : expression type
   */
  explicit typed_guard_term_t(enum tchecker::gta::guard_term_type_t const type);

  /*!
   \brief Destructor
   */
  virtual ~typed_guard_term_t() = default;

  /*!
   \brief Accessor
   \return this guard's type
   */
  inline enum tchecker::gta::guard_term_type_t type() const { return _type; }

  /*!
   \brief Clone
   \return a clone of this
   */
  virtual tchecker::gta::typed_guard_term_t * clone() const = 0;

  /*!
   \brief Visit
   \param v : typed guard visitor
   */
  virtual void visit(tchecker::gta::typed_guard_visitor_t & v) const = 0;

  /*!
   \brief Untyped visit
   \param v : guard visitor
   */
  virtual void visit(tchecker::gta::guard_visitor_t & v) const = 0;

protected:
  enum tchecker::gta::guard_term_type_t const _type; /*!< Guard type */
};

/*!
 \class typed_guard_clk_variable_t
 \brief Class to represent a clock variable in a guard with type information
 */
class typed_guard_clk_variable_t : public virtual tchecker::gta::typed_guard_term_t,
                                   public virtual tchecker::gta::guard_variable_t {
public:
  /*!
   * \brief Constructor
   * \param name : Clock name
   * \param clk_id : Clock ID
   * \param clk_type : GTA Clock type
   */
  typed_guard_clk_variable_t(std::string const & name, tchecker::clock_id_t const clk_id,
                             enum tchecker::gta::clk_type_t const clk_type);

  /*!
   * \brief Destructor
   */
  ~typed_guard_clk_variable_t() = default;

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
  virtual tchecker::gta::typed_guard_clk_variable_t * clone() const override;

  /*!
   \brief Visit
   \param v : typed guard visitor
   */
  virtual void visit(tchecker::gta::typed_guard_visitor_t & v) const override;

  /*!
   \brief Untyped visit
   \param v : guard visitor
   */
  virtual void visit(tchecker::gta::guard_visitor_t & v) const override;

protected:
  tchecker::clock_id_t const _clk_id;
  enum tchecker::gta::clk_type_t const _clk_type;
};

/*!
 \class typed_guard_integer_t
 \brief Class to represent a integer in a typed guard
 */
class typed_guard_integer_t : public virtual tchecker::gta::typed_guard_term_t, public virtual tchecker::gta::guard_integer_t {
public:
  /*!
   * \brief Constructor
   * \param val : Value of integer
   */
  typed_guard_integer_t(tchecker::integer_t const val);

  /*!
   * \brief Destructor
   */
  ~typed_guard_integer_t() = default;

  /*!
   \brief Clone
   \return clone of this
   */
  virtual tchecker::gta::typed_guard_integer_t * clone() const override;

  /*!
   \brief Visit
   \param v : typed guard visitor
   */
  virtual void visit(tchecker::gta::typed_guard_visitor_t & v) const override;

  /*!
   \brief Untyped visit
   \param v : guard visitor
   */
  virtual void visit(tchecker::gta::guard_visitor_t & v) const override;
};

/*!
 \class typed_guard_lterm_t
 \brief Class to represent a lterm in a guard
 */
class typed_guard_lterm_t : public virtual tchecker::gta::typed_guard_term_t, public virtual tchecker::gta::guard_lterm_t {
public:
  /*!
   * \brief Destructor
   */
  ~typed_guard_lterm_t() = default;

  /*!
   \brief Clone
   \return clone of this
   */
  virtual tchecker::gta::typed_guard_lterm_t * clone() const override = 0;
};

/*!
 \class typed_guard_unary_lterm_t
 \brief Class to represent a typed unary lterm
 */
class typed_guard_unary_lterm_t : public virtual tchecker::gta::typed_guard_lterm_t,
                                  public virtual tchecker::gta::guard_unary_lterm_t {
public:
  /*!
   * \brief Constructor
   * \param var : Clock variable
   */
  typed_guard_unary_lterm_t(std::shared_ptr<tchecker::gta::typed_guard_clk_variable_t> const var);

  /*!
   * \brief Destructor
   */
  ~typed_guard_unary_lterm_t() = default;

  /*!
   \brief Accessor
   \return The typed variable of lterm
   */
  inline std::shared_ptr<tchecker::gta::typed_guard_clk_variable_t> const typed_var() const { return _typed_var; }

  /*!
   \brief Clone
   \return clone of this
   */
  virtual tchecker::gta::typed_guard_unary_lterm_t * clone() const override;

  /*!
   \brief Visit
   \param v : typed guard visitor
   */
  virtual void visit(tchecker::gta::typed_guard_visitor_t & v) const override;

  /*!
   \brief Untyped visit
   \param v : guard visitor
   */
  virtual void visit(tchecker::gta::guard_visitor_t & v) const override;

protected:
  std::shared_ptr<tchecker::gta::typed_guard_clk_variable_t> const _typed_var;
};

/*!
 \class typed_guard_binary_lterm_t
 \brief Class to represent a typed binary lterm of the form x - y
 */
class typed_guard_binary_lterm_t : public virtual tchecker::gta::typed_guard_lterm_t,
                                   public virtual tchecker::gta::guard_binary_lterm_t {
public:
  /*!
   * \brief Constructor
   * \param var_l : Left clock variable x in x - y
   * \param var_r : Right clock variable y in x - y
   */
  typed_guard_binary_lterm_t(std::shared_ptr<tchecker::gta::typed_guard_clk_variable_t> const var_l,
                             std::shared_ptr<tchecker::gta::typed_guard_clk_variable_t> const var_r);

  /*!
   * \brief Destructor
   */
  ~typed_guard_binary_lterm_t() = default;

  /*!
   \brief Accessor
   \return The left variable x in x - y lterm
   */
  inline std::shared_ptr<tchecker::gta::typed_guard_clk_variable_t> const typed_var_l() const { return _typed_var_l; }

  /*!
   \brief Accessor
   \return The right variable y in x - y lterm
   */
  inline std::shared_ptr<tchecker::gta::typed_guard_clk_variable_t> const typed_var_r() const { return _typed_var_r; }

  /*!
   \brief Clone
   \return clone of this
   */
  virtual tchecker::gta::typed_guard_binary_lterm_t * clone() const override;

  /*!
   \brief Visit
   \param v : typed guard visitor
   */
  virtual void visit(tchecker::gta::typed_guard_visitor_t & v) const override;

  /*!
   \brief Untyped visit
   \param v : guard visitor
   */
  virtual void visit(tchecker::gta::guard_visitor_t & v) const override;

protected:
  std::shared_ptr<tchecker::gta::typed_guard_clk_variable_t> const _typed_var_l;
  std::shared_ptr<tchecker::gta::typed_guard_clk_variable_t> const _typed_var_r;
};

/*!
\class typed_guard_rterm_t
 \brief Class to represent a rterm in a guard
 */
class typed_guard_rterm_t : public virtual tchecker::gta::typed_guard_term_t, public virtual tchecker::gta::guard_rterm_t {
public:
  /*!
   * \brief Destructor
   */
  ~typed_guard_rterm_t() = default;

  /*!
   \brief Clone
   \return clone of this
   */
  virtual tchecker::gta::typed_guard_rterm_t * clone() const override = 0;
};

/*!
 \class typed_guard_integer_rterm_t
 \brief Class to represent a rterm integer
 */
class typed_guard_integer_rterm_t : public virtual tchecker::gta::typed_guard_rterm_t,
                                    public virtual tchecker::gta::guard_integer_rterm_t {
public:
  /*!
   * \brief Constructor
   * \param val : typed integer term
   */
  typed_guard_integer_rterm_t(std::shared_ptr<tchecker::gta::typed_guard_integer_t> const val);

  /*!
   * \brief Destructor
   */
  ~typed_guard_integer_rterm_t() = default;

  /*!
   \brief Accessor
   \return Typed integer term
   */
  inline std::shared_ptr<tchecker::gta::typed_guard_integer_t> const typed_val() const { return _typed_val; }

  /*!
   \brief Clone
   \return clone of this
   */
  virtual tchecker::gta::typed_guard_integer_rterm_t * clone() const override;

  /*!
   \brief Visit
   \param v : typed guard visitor
   */
  virtual void visit(tchecker::gta::typed_guard_visitor_t & v) const override;

  /*!
   \brief Untyped visit
   \param v : guard visitor
   */
  virtual void visit(tchecker::gta::guard_visitor_t & v) const override;

protected:
  std::shared_ptr<tchecker::gta::typed_guard_integer_t> const _typed_val;
};

/*!
 \class typed_guard_constant_rterm_t
 \brief Class to represent a rterm special constant in extended integers
 */
class typed_guard_constant_rterm_t : public virtual tchecker::gta::typed_guard_rterm_t,
                                     public virtual tchecker::gta::guard_constant_rterm_t {
public:
  /*!
   * \brief Constructor
   * \param c : constant value
   */
  typed_guard_constant_rterm_t(enum tchecker::gta::guard_rterm_special_constants_t const c);

  /*!
   * \brief Destructor
   */
  ~typed_guard_constant_rterm_t() = default;

  /*!
   \brief Clone
   \return clone of this
   */
  virtual tchecker::gta::typed_guard_constant_rterm_t * clone() const override;

  /*!
   \brief Visit
   \param v : typed guard visitor
   */
  virtual void visit(tchecker::gta::typed_guard_visitor_t & v) const override;

  /*!
   \brief Untyped visit
   \param v : guard visitor
   */
  virtual void visit(tchecker::gta::guard_visitor_t & v) const override;
};

/*!
 \class typed_guard_t
 \brief Class to a guard of the form lterm op rterm
 */
class typed_guard_t : public virtual tchecker::gta::typed_guard_term_t, public virtual tchecker::gta::guard_t {
public:
  /*!
   * \brief Constructor
   * \param c : constant value
   */
  typed_guard_t(std::shared_ptr<tchecker::gta::typed_guard_lterm_t> const lterm, enum tchecker::gta::guard_operator_t const op,
                std::shared_ptr<tchecker::gta::typed_guard_rterm_t> const rterm);

  /*!
   * \brief Destructor
   */
  ~typed_guard_t() = default;

  /*!
   \brief Accessor
   \return Typed lterm in guard
   */
  inline std::shared_ptr<tchecker::gta::typed_guard_lterm_t> const typed_lterm() const { return _typed_lterm; }

  /*!
   \brief Accessor
   \return Typed rterm in guard
   */
  inline std::shared_ptr<tchecker::gta::typed_guard_rterm_t> const typed_rterm() const { return _typed_rterm; }

  /*!
   \brief Clone
   \return clone of this
   */
  virtual tchecker::gta::typed_guard_t * clone() const override;

  /*!
   \brief Visit
   \param v : typed guard visitor
   */
  virtual void visit(tchecker::gta::typed_guard_visitor_t & v) const override;

  /*!
   \brief Untyped visit
   \param v : guard visitor
   */
  virtual void visit(tchecker::gta::guard_visitor_t & v) const override;

protected:
  std::shared_ptr<tchecker::gta::typed_guard_lterm_t> const _typed_lterm;
  std::shared_ptr<tchecker::gta::typed_guard_rterm_t> const _typed_rterm;
};

/*!
 \class typed_guard_visitor_t
 \brief Visitor pattern for typed guards
 */
class typed_guard_visitor_t {
public:
  /*!
   \brief Constructor
   */
  typed_guard_visitor_t() = default;

  /*!
   \brief Copy constructor
   */
  typed_guard_visitor_t(tchecker::gta::typed_guard_visitor_t const &) = default;

  /*!
   \brief Destructor
   */
  virtual ~typed_guard_visitor_t() = default;

  /*!
   \brief Assignment operator
   */
  tchecker::gta::typed_guard_visitor_t & operator=(tchecker::gta::typed_guard_visitor_t const &) = default;

  /*!
   \brief Move assignment operator
   */
  tchecker::gta::typed_guard_visitor_t & operator=(tchecker::gta::typed_guard_visitor_t &&) = default;

  /*!
   \brief Visitors
   */
  virtual void visit(tchecker::gta::typed_guard_clk_variable_t const &) = 0;
  virtual void visit(tchecker::gta::typed_guard_integer_t const &) = 0;
  virtual void visit(tchecker::gta::typed_guard_unary_lterm_t const &) = 0;
  virtual void visit(tchecker::gta::typed_guard_binary_lterm_t const &) = 0;
  virtual void visit(tchecker::gta::typed_guard_integer_rterm_t const &) = 0;
  virtual void visit(tchecker::gta::typed_guard_constant_rterm_t const &) = 0;
  virtual void visit(tchecker::gta::typed_guard_t const &) = 0;
};

/*!
 \brief Type to represent a conjunction of typed guard constraints
*/
using typed_guards_conjunction_t = std::vector<std::shared_ptr<tchecker::gta::typed_guard_t const>>;

/*!
 \brief Output operator for typed guard conjunctions
 \param os : output stream
 \param typed_guards : typed guard conjunctions
 \post guards has been output to os
 \return os after guards have been output
 */
std::ostream & operator<<(std::ostream & os, tchecker::gta::typed_guards_conjunction_t & typed_guards);

} // end of namespace gta

} // end of namespace tchecker

#endif // TCHECKER_GTA_PROGRAM_TYPED_GUARD_HH
