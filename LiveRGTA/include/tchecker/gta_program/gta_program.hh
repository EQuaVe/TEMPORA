/*
 * This file is a part of the TChecker project.
 *
 * See files AUTHORS and LICENSE for copyright details.
 *
 */

#ifndef TCHECKER_GTA_PROGRAM_GTA_PROGRAM_HH
#define TCHECKER_GTA_PROGRAM_GTA_PROGRAM_HH

#include "tchecker/gta_program/action.hh"
#include "tchecker/gta_program/guard.hh"
#include "tchecker/utils/alternating_list.hh"

/*!
 \file gta_program.hh
 \brief Syntax tree for a GTA program
*/

namespace tchecker {

namespace gta {

/*!
 \brief Type for a pointer to conjunction of guards
*/
using guards_conjunction_ptr_t = std::shared_ptr<tchecker::gta::guards_conjunction_t>;

/*!
 \brief Type for a pointer to list of actions
*/
using actions_ptr_t = std::shared_ptr<tchecker::gta::actions_t>;

/*!
 \brief Type for a node storing conjunction of guards in the alternating list of GTA
*/
using guards_node_t = tchecker::alternating_list_node_t<guards_conjunction_ptr_t const, actions_ptr_t const>;
using guards_node_ptr_t = std::shared_ptr<guards_node_t>;

/*!
 \brief Type for a node storing list of actions in the alternating list of GTA
*/
using actions_node_t = tchecker::alternating_list_node_t<actions_ptr_t const, guards_conjunction_ptr_t const>;
using actions_node_ptr_t = std::shared_ptr<actions_node_t>;

/*!
  \class gta_program_t
  \brief Class to represent the parsed gta program
*/
class gta_program_t {
public:
  /*!
   * brief type of gta program list
   */
  using gta_program_list_t = tchecker::alternating_list_t<guards_conjunction_ptr_t const, actions_ptr_t const>;

  /*!
   * \brief Constructor
   */
  gta_program_t() = default;

  /*!
   * \brief Destructor
   */
  ~gta_program_t() = default;

  /*!
   \brief Output the gta program
   \param os : output stream
   \return os after the gta program has been output
   */
  std::ostream & output(std::ostream & os) const;

  /*!
   \brief Accessor
   \return this gta program as a string
   */
  std::string to_string() const;

  /*!
   * \brief Append guards to the list
   * \param guards : a pointer to the conjunction of guards to be appended
   * \pre guards is not nullptr, and the list is either empty or ends at actions
   * \post guards has been appended to the list
   * \throw std::invalid_argument if guards is nullptr, and std::runtime_error
   * if the list is not empty and does not end at actions
   */
  void append_guards(tchecker::gta::guards_conjunction_ptr_t const & guards);

  /*!
   * \brief Append actions to the list
   * \param actions : a pointer to the actions to be appended
   * \pre actions is not nullptr, and the list ends at guards
   * \post actions has been appended to the list
   * \throw std::invalid_argument if actions is nullptr, and std::runtime_error
   * if the list does not end at guards
   */
  void append_actions(tchecker::gta::actions_ptr_t const & actions);

  /*!
   * \brief Accessor
   * \return the guards at the beginning of list
   * \note returns nullptr if the list is empty
   */
  inline guards_node_ptr_t const beginning_guards() const;

  /*!
   * \brief Accessor
   * \return a pair of pointers, one to a guards node and one to a actions node
   * If one of them is not null, it represents the tail of the list
   * \note returns (nullptr, nullptr) if the list is empty
   */
  inline std::pair<guards_node_ptr_t, actions_node_ptr_t> const end_guards_or_actions() const;

  // Iterator
  // Iterators for the alternating list in gta program
  using iterator_t = gta_program_list_t::iterator_t;

  // Consult class alternating_list for details about the functions
  iterator_t begin() const;

  iterator_t end() const;

  iterator_t next(iterator_t & curr) const;

private:
  gta_program_list_t _guards_actions_list;
};

/*!
 \brief Output operator for gta programs
 \param os : output stream
 \param gta_program : The GTA program
 \post gta program has been output to os
 \return os after gta program has been output
 */
std::ostream & operator<<(std::ostream & os, tchecker::gta::gta_program_t const & gta_program);

} // end of namespace gta

} // end of namespace tchecker
#endif // TCHECKER_GTA_PROGRAM_GTA_PROGRAM_HH
