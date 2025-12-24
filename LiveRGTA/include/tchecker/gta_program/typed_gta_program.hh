/*
 * This file is a part of the TChecker project.
 *
 * See files AUTHORS and LICENSE for copyright details.
 *
 */

#ifndef TCHECKER_GTA_PROGRAM_TYPED_GTA_PROGRAM_HH
#define TCHECKER_GTA_PROGRAM_TYPED_GTA_PROGRAM_HH

#include <iostream>
#include <memory>

#include "tchecker/gta_program/typed_action.hh"
#include "tchecker/gta_program/typed_guard.hh"
#include "tchecker/utils/alternating_list.hh"

/*!
 \file typed_gta_program.hh
 \brief GTA program with type information
*/

namespace tchecker {

namespace gta {

/*!
 \brief Type for a pointer to conjunction of typed guards
*/
using typed_guards_conjunction_ptr_t = std::shared_ptr<tchecker::gta::typed_guards_conjunction_t>;

/*!
 \brief Type for a pointer to list of typed actions
*/
using typed_actions_ptr_t = std::shared_ptr<tchecker::gta::typed_actions_t>;

/*
 \brief Type for a node storing conjunction of typed guards in the alternating list of GTA
*/
using typed_guards_node_t = tchecker::alternating_list_node_t<typed_guards_conjunction_ptr_t const, typed_actions_ptr_t const>;
using typed_guards_node_ptr_t = std::shared_ptr<tchecker::gta::typed_guards_node_t>;

/*
 \brief Type for a node storing list of typed actions in the alternating list of GTA
*/
using typed_actions_node_t = tchecker::alternating_list_node_t<typed_actions_ptr_t const, typed_guards_conjunction_ptr_t const>;
using typed_actions_node_ptr_t = std::shared_ptr<typed_actions_node_t>;

/*!
  \class typed_gta_program_t
  \brief Class to represent a gta program with typed guards and actions
*/
class typed_gta_program_t {
public:
  /*!
   */
  using typed_gta_program_list_t =
      tchecker::alternating_list_t<typed_guards_conjunction_ptr_t const, typed_actions_ptr_t const>;

  /*!
   * \brief Constructor
   */
  typed_gta_program_t() = default;

  /*!
   * \brief Destructor
   */
  ~typed_gta_program_t() = default;

  /*!
   \brief Output the typed gta program
   \param os : output stream
   \return os after the typed gta program has been output
   */
  std::ostream & output(std::ostream & os) const;

  /*!
   \brief Accessor
   \return this typed gta program as a string
   */
  std::string to_string() const;

  /*!
   * \brief Append typed guards to the list
   * \param guards : a pointer to the conjunction of typed guards to be appended
   * \pre guards is not nullptr, and the list is either empty or ends at actions
   * \post guards has been appended to the list
   * \throw std::invalid_argument if guards is nullptr, and std::runtime_error
   * if the list is not empty and does not end at actions
   */
  void append_typed_guards(tchecker::gta::typed_guards_conjunction_ptr_t const & guards);

  /*!
   * \brief Append typed actions to the list
   * \param actions : a pointer to the typed actions to be appended
   * \pre actions is not nullptr, and the list ends at guards
   * \post actions has been appended to the list
   * \throw std::invalid_argument if actions is nullptr, and std::runtime_error
   * if the list does not end at guards
   */
  void append_typed_actions(tchecker::gta::typed_actions_ptr_t const & actions);

  /*!
   * \brief Accessor
   * \return the typed guards at the beginning of list
   * \note returns nullptr if the list is empty
   */
  inline typed_guards_node_ptr_t const beginning_guards() const;

  /*!
   * \brief Accessor
   * \return a pair of pointers, one to a guards node and one to a actions node
   * If one of them is not null, it represents the tail of the list
   * \note returns (nullptr, nullptr) if the list is empty
   */
  inline std::pair<typed_guards_node_ptr_t, typed_actions_node_ptr_t> const end_guards_or_actions() const;

  // Iterator
  // Iterators for the alternating list in typed gta program
  using iterator_t = typed_gta_program_list_t::iterator_t;

  // Consult class alternating_list for details about the functions
  iterator_t begin() const;

  iterator_t end() const;

  iterator_t next(iterator_t & curr) const;

private:
  typed_gta_program_list_t _typed_guards_actions_list;
};

/*!
 \brief Output operator for typed gta programs
 \param os : output stream
 \param typed_gta_program : The GTA program
 \post gta program has been output to os
 \return os after gta program has been output
 */
std::ostream & operator<<(std::ostream & os, tchecker::gta::typed_gta_program_t const & typed_gta_program);

} // end of namespace gta

} // end of namespace tchecker
#endif // TCHECKER_GTA_PROGRAM_TYPED_GTA_PROGRAM_HH
