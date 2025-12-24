/*
 * This file is a part of the TChecker project.
 *
 * See files AUTHORS and LICENSE for copyright details.
 *
 */

#ifndef TCHECKER_GTA_SYSTEM_HH
#define TCHECKER_GTA_SYSTEM_HH

/*!
 \file system.hh
 \brief System of processes for generalized timed automata
 */

#include <boost/dynamic_bitset/dynamic_bitset.hpp>

#include "tchecker/basictypes.hh"
#include "tchecker/dbm/gtadbm.hh"
#include "tchecker/gta_program/static_analysis.hh"
#include "tchecker/gta_program/typed_gta_program.hh"
#include "tchecker/syncprod/system.hh"
#include "tchecker/ta/system.hh"

namespace tchecker {

namespace gta {

class system_t : private tchecker::ta::system_t {
public:
  /*!
   \brief Constructor
   \param sysdecl : system declaration
   \throw std::invalid_argument if system contains an array of clocks
   */
  system_t(tchecker::parsing::system_declaration_t const & sysdecl);

  /*!
   \brief Constructor
   \param system : system of timed processes
   \post this is a copy of system
   \throw std::invalid_argument if system contains an array of clocks
   */
  system_t(tchecker::system::system_t const & system);

  /*!
   \brief Constructor
   \param system : system of synchronized timed processes
   \post this is a copy of system
   \throw std::invalid_argument if system contains an array of clocks
   */
  system_t(tchecker::syncprod::system_t const & system);

  /*!
   \brief Constructor
   \param system : system of timed automata
   \post this is a copy of system
   \throw std::invalid_argument if system contains an array of clocks
   */
  system_t(tchecker::ta::system_t const & system);

  /*!
   \brief Copy constructor
   */
  system_t(tchecker::gta::system_t const &);

  /*!
   \brief Move constructor
   */
  system_t(tchecker::gta::system_t &&) = default;

  /*!
   \brief Destructor
   */
  ~system_t() = default;

  /*!
   \brief Assignment operator
   */
  tchecker::gta::system_t & operator=(tchecker::gta::system_t const &);

  /*!
   \brief Move-assignment operator
   */
  tchecker::gta::system_t & operator=(tchecker::gta::system_t &&) = default;

  // System
  using tchecker::ta::system_t::attributes;
  using tchecker::ta::system_t::name;

  /*!
   \brief Accessor
   \return map of known attributes that are interpreted by this system
  */
  static tchecker::system::attribute_keys_map_t const & known_attributes();

  // Clocks
  using tchecker::ta::system_t::clock_attributes;
  using tchecker::ta::system_t::clock_id;
  using tchecker::ta::system_t::clock_name;
  using tchecker::ta::system_t::clock_variables;
  using tchecker::ta::system_t::clocks_count;
  using tchecker::ta::system_t::clocks_identifiers;
  using tchecker::ta::system_t::is_clock;

  /*!
   \brief Accessor
   \param clk_id : clock id
   \return true if the clock is a prophecy clock
  */

  bool is_prophecy_clock(tchecker::clock_id_t const clk_id) const;

  /*!
   \brief Accessor
   \param clk_id : clock id
   \return the subset of prophecy clocks
  */
  inline boost::dynamic_bitset<> const & prophecy_clks() const { return _prophecy_clocks; }

  /*!
   \brief Accessor
   \return the subset of histroy clocks initialized to zero
  */
  inline boost::dynamic_bitset<> const & history_zero_clks() const { return _history_zero_clocks; }

  /*!
   \brief Accessor
   \param clk_id : clock id
   \return the subset of histroy clocks initialized to inf
  */
  inline boost::dynamic_bitset<> const & history_inf_clks() const { return _history_inf_clocks; }

  // Edges
  using tchecker::ta::system_t::asynchronous_incoming_edges;
  using tchecker::ta::system_t::asynchronous_outgoing_edges;
  using tchecker::ta::system_t::edge;
  using tchecker::ta::system_t::edges;
  using tchecker::ta::system_t::edges_count;
  using tchecker::ta::system_t::edges_identifiers;
  using tchecker::ta::system_t::guard;
  using tchecker::ta::system_t::guard_bytecode;
  using tchecker::ta::system_t::incoming_edges;
  using tchecker::ta::system_t::incoming_edges_maps;
  using tchecker::ta::system_t::incoming_event;
  using tchecker::ta::system_t::is_asynchronous;
  using tchecker::ta::system_t::is_edge;
  using tchecker::ta::system_t::outgoing_edges;
  using tchecker::ta::system_t::outgoing_edges_maps;
  using tchecker::ta::system_t::outgoing_event;
  using tchecker::ta::system_t::statement;
  using tchecker::ta::system_t::statement_bytecode;

  /*!
   * \brief Returns if the edge has an unsatisfiable gta program
   * \param id : edge identifier
   * \pre id is an edge identifier (checked by assertion)
   * \return true if some gta guard in edge is unsatisfiable*/
  bool is_edge_unsatisfiable(tchecker::edge_id_t const id) const;

  /*!
   \brief Accessor
   \param id : edge identifier
   \pre id is an edge identifier (checked by assertion)
   \pre edge with identifer id is satisfiable (checked by assertion)
   \return typed gta program for edge id
   */
  tchecker::gta::typed_gta_program_t const & gta_program(tchecker::edge_id_t const id) const;

  /*!
   \brief Accessor
   \param id : edge identifier
   \pre id is an edge identifier (checked by assertion)
   \pre edge with identifer id is satisfiable (checked by assertion)
   \return compiled gta program for edge
   */
  tchecker::gta::compiled_gta_program_t const & compiled_gta_program(tchecker::edge_id_t const id) const;

  // Events
  using tchecker::ta::system_t::event_attributes;
  using tchecker::ta::system_t::event_id;
  using tchecker::ta::system_t::event_name;
  using tchecker::ta::system_t::events_count;
  using tchecker::ta::system_t::events_identifiers;
  using tchecker::ta::system_t::is_event;

  // Bounded integer variables
  using tchecker::ta::system_t::integer_variables;
  using tchecker::ta::system_t::intvar_attributes;
  using tchecker::ta::system_t::intvar_id;
  using tchecker::ta::system_t::intvar_name;
  using tchecker::ta::system_t::intvars_count;
  using tchecker::ta::system_t::intvars_identifiers;
  using tchecker::ta::system_t::is_intvar;

  // Labels
  using tchecker::ta::system_t::is_label;
  using tchecker::ta::system_t::label_id;
  using tchecker::ta::system_t::label_name;
  using tchecker::ta::system_t::labels;
  using tchecker::ta::system_t::labels_count;
  using tchecker::ta::system_t::labels_identifiers;

  // Locations
  using tchecker::ta::system_t::committed_locations;
  using tchecker::ta::system_t::initial_locations;
  using tchecker::ta::system_t::invariant;
  using tchecker::ta::system_t::invariant_bytecode;
  using tchecker::ta::system_t::is_committed;
  using tchecker::ta::system_t::is_initial_location;
  using tchecker::ta::system_t::is_location;
  using tchecker::ta::system_t::is_urgent;
  using tchecker::ta::system_t::location;
  using tchecker::ta::system_t::locations;
  using tchecker::ta::system_t::locations_count;
  using tchecker::ta::system_t::locations_identifiers;

  // Processes
  using tchecker::syncprod::system_t::is_process;
  using tchecker::syncprod::system_t::process_attributes;
  using tchecker::syncprod::system_t::process_id;
  using tchecker::syncprod::system_t::process_name;
  using tchecker::syncprod::system_t::processes_count;
  using tchecker::syncprod::system_t::processes_identifiers;

  // Synchronizations
  using tchecker::syncprod::system_t::synchronization;
  using tchecker::syncprod::system_t::synchronizations;
  using tchecker::syncprod::system_t::synchronizations_count;
  using tchecker::syncprod::system_t::synchronizations_identifiers;

  // Virtual machine
  using tchecker::ta::system_t::vm;

  // Cast
  using tchecker::ta::system_t::as_syncprod_system;
  using tchecker::ta::system_t::as_system_system;

  /*!
   \brief Cast
   \return this as a tchecker::ta::system_t instance
   */
  constexpr inline tchecker::ta::system_t const & as_ta_system() const { return *this; }

private:
  /*!
   * \struct edge_gta_programs_t
   * \brief Stores the typed gta program and the compiled gta program
   */
  struct edge_gta_program_t {
    std::shared_ptr<tchecker::gta::typed_gta_program_t const> _typed_gta_prog;
    compiled_gta_program_t _compiled_gta_prog;
  };

  /*!
   * \brief type for a map from edge identifiers -> compile gta program(s) for edge
   */
  using gta_programs_t = std::vector<edge_gta_program_t>;

  /*!
   \brief Compute data from ta::system_t
   \throw std::invalid_argument if
   - system contains an array of clocks
   */
  void compute_from_ta_system();

  /*!
   \brief Set clock type
   \param clk_id : clock identifier
   \param guards : range of clock types (as strings)
   \post Exactly one of the three bitsets: _prophecy_clocks, _history_zero_clocks, _history_inf_clocks at index clk_id is set to
   1, depending on declaration of type. Default declaration is history_zero
   \note type values of "prophecy" and "history_zero" and "history_inf" are supported
   \note Unknown types are reported as warnings on std::cerr
   \note the last type in the range overrides all the previous types
   */
  void set_clock_type(tchecker::clock_id_t const & id,
                      tchecker::range_t<tchecker::system::attributes_t::const_iterator_t> const & clk_type);

  /*!
   \brief Set gta program (attribute gta_program)
   \param id : edge identifier
   \param gta_program : range of gta_programs (as strings)
   \post gta program has been parsed, typed and set as edge_id gta program
   \note all compilation errors have been reported to std::cerr
   \note previous gta program (if any) has been replaced by the new one
   \throw std::invalid_argument : if compilation of gta program fails
   */
  void set_gta_program(tchecker::edge_id_t id,
                       tchecker::range_t<tchecker::system::attributes_t::const_iterator_t> const & gta_programs);

  boost::dynamic_bitset<> _prophecy_clocks;     /*!< Prophecy Clocks */
  boost::dynamic_bitset<> _history_zero_clocks; /*!< History Clocks initialized to zero */
  boost::dynamic_bitset<> _history_inf_clocks;  /*!< History Clocks initialized to inf*/
  boost::dynamic_bitset<> _unsatisfiable_edges; /*!< Edges with unsatisfiable gta program */
  gta_programs_t _gta_progs;                    /*!< Map identifers -> gta program */
};

} // end of namespace gta

} // end of namespace tchecker

#endif // TCHECKER_GTA_SYSTEM_HH
