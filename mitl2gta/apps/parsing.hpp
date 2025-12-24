/*
 * This file is a part of MITL2GTA project.
 *
 * See AUTHORS and LICENSE for copyright details.
 *
 */

#include <argp.h>
#include <filesystem>

#include "mitl2gta/compile/compiled_node.hpp"
#include "mitl2gta/mitl_formula/factory.hpp"
#include "mitl2gta/sharer_automaton/translation.hpp"
#include "mitl2gta/transducer/system.hpp"

enum translate_mode_t {
  ALL_TRANSDUCERS,
  INIT_SAT_AUTOMATA,
};

class parsed_config_t {
public:
  parsed_config_t(
      mitl2gta::compilation::verification_mode_t verifying_mode,
      translate_mode_t translation_mode,
      mitl2gta::transducer::system_for_t system_for,
      std::set<mitl2gta::compilation::node_type_t> nodes_for,
      std::optional<std::filesystem::path> formula_input,
      std::optional<std::filesystem::path> gta_output,
      mitl2gta::mitl::parsing_mode_t parsing_mode,
      mitl2gta::sharer::sharer_mode_t sharing_mode,
      mitl2gta::sharer::p_until_q_sharer_states_t p_until_q_sharer_states,
      mitl2gta::memory::mapping_mode_t mapping_mode);

  parsed_config_t(parsed_config_t const &) = delete;
  parsed_config_t(parsed_config_t &&) = default;
  parsed_config_t &operator=(parsed_config_t const &) = delete;
  parsed_config_t &operator=(parsed_config_t &&) = delete;

  inline mitl2gta::compilation::verification_mode_t verifying_mode() const {
    return _verifying_mode;
  }

  inline translate_mode_t translation_mode() const { return _translation_mode; }

  inline mitl2gta::transducer::system_for_t system_for() const {
    return _system_for;
  }

  inline std::set<mitl2gta::compilation::node_type_t> const &nodes_for() const {
    return _nodes_for;
  }

  inline std::optional<std::filesystem::path> formula_input() const {
    return _formula_input;
  }

  std::optional<std::filesystem::path> const &model_input() const {
    return _model_input;
  }

  std::optional<std::filesystem::path> const &gta_output() const {
    return _gta_output;
  }

  mitl2gta::mitl::parsing_mode_t const parsing_mode() const {
    return _parsing_mode;
  }

  mitl2gta::sharer::sharer_mode_t const sharing_mode() const {
    return _sharing_mode;
  }

  mitl2gta::sharer::p_until_q_sharer_states_t const
  p_until_q_sharer_states() const {
    return _p_until_q_sharer_states;
  }

  mitl2gta::memory::mapping_mode_t const mapping_mode() const {
    return _mapping_mode;
  }

private:
  mitl2gta::compilation::verification_mode_t const _verifying_mode;

  translate_mode_t const _translation_mode;

  mitl2gta::transducer::system_for_t const _system_for;

  std::set<mitl2gta::compilation::node_type_t> const _nodes_for;

  std::optional<std::filesystem::path> const _formula_input;

  std::optional<std::filesystem::path> const _model_input;

  std::optional<std::filesystem::path> const _gta_output;

  mitl2gta::mitl::parsing_mode_t const _parsing_mode;

  mitl2gta::sharer::sharer_mode_t const _sharing_mode;

  mitl2gta::sharer::p_until_q_sharer_states_t const _p_until_q_sharer_states;

  mitl2gta::memory::mapping_mode_t const _mapping_mode;
};

std::optional<parsed_config_t> parse_args(int argc, char *argv[]);
