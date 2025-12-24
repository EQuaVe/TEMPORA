/*
 * This file is a part of the TChecker project.
 *
 * See files AUTHORS and LICENSE for copyright details.
 *
 */

#ifndef TCHECKER_GTA_PROGRAM_STATIC_ANALYSIS_HH
#define TCHECKER_GTA_PROGRAM_STATIC_ANALYSIS_HH

#include <boost/dynamic_bitset.hpp>
#include <boost/dynamic_bitset/dynamic_bitset.hpp>

#include "tchecker/gta_program/typed_action.hh"
#include "tchecker/gta_program/typed_gta_program.hh"
#include "tchecker/gta_program/typed_guard.hh"
#include "tchecker/utils/alternating_list.hh"
#include "tchecker/variables/clocks.hh"

/*!
 \file static_analysis.hh
 \brief Static analysis for guards and actions of a gta program
 */

namespace tchecker {

namespace gta {

using compiled_guards_ptr_t = std::shared_ptr<tchecker::clock_constraint_container_t const>;
using compiled_actions_ptr_t = tchecker::gta::typed_actions_ptr_t;

using compiled_guards_node_ptr_t = std::shared_ptr<
    tchecker::alternating_list_node_t<tchecker::gta::compiled_guards_ptr_t const, tchecker::gta::typed_actions_ptr_t const>>;

using compiled_actions_node_ptr_t = std::shared_ptr<
    tchecker::alternating_list_node_t<tchecker::gta::compiled_actions_ptr_t const, tchecker::gta::compiled_guards_ptr_t const>>;

/*!
  \class compiled_gta_program_t
  \brief Class to represent a compiled gta program with clock constraints for guards and typed actions for actions
*/
class compiled_gta_program_t {
public:
  /*!
   * brief type of compiled gta program list
   */
  using compiled_gta_program_list_t =
      tchecker::alternating_list_t<tchecker::gta::compiled_guards_ptr_t const, tchecker::gta::compiled_actions_ptr_t const>;

  /*!
   * \brief Constructor
   */
  compiled_gta_program_t() = default;

  /*!
   * \brief Destructor
   */
  ~compiled_gta_program_t() = default;

  /*!
   * \brief Copy Constructor (deleted)
   */
  compiled_gta_program_t(tchecker::gta::compiled_gta_program_t const &) = delete;

  /*!
   * \brief Copy operator (deleted)
   */
  compiled_gta_program_t & operator=(const compiled_gta_program_t &) = delete;

  /*!
   * \brief Move Constructor
   */
  compiled_gta_program_t(compiled_gta_program_t &&) = default;

  /*!
   * \brief Move operator
   */
  compiled_gta_program_t & operator=(compiled_gta_program_t &&) = default;

  /*!
   \brief Output the compiled gta program
   \param os : output stream
   \return os after the compiled gta program has been output
   */
  std::ostream & output(std::ostream & os) const;

  /*!
   \brief Accessor
   \return this compiled gta program as a string
   */
  std::string to_string() const;

  /*!
   * \brief Append compiled guards to gta program list
   * \param constraints : a pointer to the container of clock constraints
   * \pre constraints is not nullptr, and the list is either empty or ends at actions
   * \post constraints has been appended to the list
   * \throw std::invalid_argument if guards is nullptr, and std::runtime_error
   * if the list is not empty and does not end at actions
   */
  void append_compiled_guards(tchecker::gta::compiled_guards_ptr_t const & constraints);

  /*!
   * \brief Append compiled actions to the list
   * \param actions : a pointer to the compiled actions to be appended
   * \pre actions is not nullptr, and the list ends at guards
   * \post actions has been appended to the list
   * \throw std::invalid_argument if actions is nullptr, and std::runtime_error
   * if the list does not end at constraints
   */
  void append_compiled_actions(tchecker::gta::compiled_actions_ptr_t const & actions);

  /*!
   * \brief Accessor
   * \return the constraints at the beginning of list
   * \note returns nullptr if the list is empty
   */
  tchecker::gta::compiled_guards_node_ptr_t const beginning_constraints() const;

  /*!
   * \brief Accessor
   * \return a pair of pointers, one to a constraints node and one to a actions node
   * If one of them is not null, it represents the tail of the list
   * \note returns (nullptr, nullptr) if the list is empty
   */
  std::pair<tchecker::gta::compiled_guards_node_ptr_t, tchecker::gta::compiled_actions_node_ptr_t> const
  end_constraints_or_actions() const;

  // Iterator
  // Iterators for the alternating list in compiled gta program
  using iterator_t = compiled_gta_program_list_t::iterator_t;

  // Consult class alternating_list for details about the functions
  iterator_t begin() const;

  iterator_t end() const;

  iterator_t next(iterator_t & curr) const;

  iterator_t backward_begin() const;

  iterator_t backward_end() const;

  iterator_t prev(iterator_t & curr) const;

private:
  compiled_gta_program_list_t _compiled_gta_program_list;
};

/*!
 \brief Output operator for compiled gta programs
 \param os : output stream
 \param compiled_gta_program : The GTA program
 \post gta program has been output to os
 \return os after gta program has been output
 */
std::ostream & operator<<(std::ostream & os, tchecker::gta::compiled_gta_program_t const & compiled_gta_program);

enum guard_truth_val_t {
  TRIVIALLY_TRUE_GUARD,
  TRIVIALLY_FALSE_GUARD,
  NON_TRIVIAL_GUARD,
};

/*!
 * \brief Analyses the guard to check if the guard is trivially true or false
 * \param guard : GTA guard
 * \param prophecy_clocks : Subset of clocks which are prophetic
 * \return If the guard is trivially true, trivially false or non-trivial
 */
tchecker::gta::guard_truth_val_t analyse_guard(std::shared_ptr<tchecker::gta::typed_guard_t const> const & guard,
                                               boost::dynamic_bitset<> const & prophecy_clocks);

/*!
 * \brief Simplifies GTA program by pruning trivially true guards
 * \param gta_prog : GTA program
 * \param prophecy_clocks : Subset of clocks which are prophetic
 * \pre gta_prog is not nullptr (checked by assertion)
 * \return the simplified gta program, and nullptr if the gta program contains a trivially false guard
 */
std::shared_ptr<tchecker::gta::typed_gta_program_t>
simplify_gta_prog(std::shared_ptr<tchecker::gta::typed_gta_program_t const> const & gta_prog,
                  boost::dynamic_bitset<> const & prophecy_clocks);

/*!
 * \brief Compute constraints corresponding to x - y == inf
 * \param x : lclock in the diagonal constraint
 * \param x_type : clock type of x
 * \param y : rclock in the diagonal constraint
 * \param y_type : clock type of y
 */
tchecker::clock_constraint_container_t diagonal_constraint_inf(tchecker::clock_id_t const x,
                                                               tchecker::gta::clk_type_t const x_type,
                                                               tchecker::clock_id_t const y,
                                                               tchecker::gta::clk_type_t const y_type);

/*!
 * \brief Returns clock constraints which correspond to a conjunction of guards
 * \param guards : conjunction of GTA guards
 * \return clock constraints corresponding to the guards
 */
tchecker::clock_constraint_container_t constraints_for_guards(tchecker::gta::typed_guards_conjunction_t const & guards);

/*!
 * \brief Returns compiled GTA program
 * \param gta_prog : GTA program
 * \return compiled gta program corresponding to gta_prog
 */
tchecker::gta::compiled_gta_program_t compile_gta_program(tchecker::gta::typed_gta_program_t const & gta_prog);

/*!
 * \brief Returns the subset of prophecy clocks released in gta program
 * \param gta_prog : typed GTA program
 * \return the subset of clocks which are released in gta_prog
 */
boost::dynamic_bitset<> released_clks(tchecker::gta::typed_gta_program_t const & gta_prog, tchecker::clock_id_t const num_clks);

} // end of namespace gta

} // end of namespace tchecker

#endif // TCHECKER_GTA_PROGRAM_STATIC_ANALYSIS_HH
