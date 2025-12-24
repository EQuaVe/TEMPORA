/*
 * This file is a part of MITL2GTA project.
 *
 * See AUTHORS and LICENSE for copyright details.
 *
 */

#include "parsing.hpp"

namespace {

mitl2gta::compilation::verification_mode_t VERIFYING_MODE =
    mitl2gta::compilation::verification_mode_t::MODEL_CHECKING;

translate_mode_t TRANSLATION_MODE = INIT_SAT_AUTOMATA;

mitl2gta::transducer::system_for_t SYSTEM_FOR =
    mitl2gta::transducer::system_for_t::INFINITE_TRACE;

std::set<mitl2gta::compilation::node_type_t> NODES_FOR;

std::optional<std::filesystem::path> formula_input = std::nullopt;

std::optional<std::filesystem::path> gta_output = std::nullopt;

mitl2gta::mitl::parsing_mode_t PARSING_MODE =
    mitl2gta::mitl::parsing_mode_t::SUBFORMULA_SHARING_ENABLED;

mitl2gta::sharer::sharer_mode_t SHARING_MODE =
    mitl2gta::sharer::sharer_mode_t::SHARING_ENABLED;

mitl2gta::sharer::p_until_q_sharer_states_t P_UNTIL_Q_SHARER_STATES =
    mitl2gta::sharer::p_until_q_sharer_states_t::TWO_STATE;

mitl2gta::memory::mapping_mode_t MAPPING_MODE =
    mitl2gta::memory::mapping_mode_t::OPTIMISED;

int const FORMULA_INPUT_OPTION = static_cast<int>('f');
int const VERIFYING_MODE_OPTION = static_cast<int>('m');
int const GTA_OUTPUT_OPTION = static_cast<int>('o');
int const TRACE_TYPE_OPTION = static_cast<int>('t');

int const SUBFORMULA_SHARING_OPTION = 128;
int const SHARING_MODE_OPTION = 129;
int const MAPPING_OPTIMISED_OPTION = 130;
int const BOOLEAN_LABELS_OPTION = 131;
int const INIT_SAT_OPTION = 132;
int const P_UNTIL_Q_STATES_OPTION = 133;

error_t parse_opt(int key, char *arg, struct argp_state *state) {
  switch (key) {
  case TRACE_TYPE_OPTION: {
    std::string argument(arg);
    if (argument == "finite") {
      SYSTEM_FOR = mitl2gta::transducer::system_for_t::FINITE_TRACE;
    }

    else if (argument == "infinite") {
      SYSTEM_FOR = mitl2gta::transducer::system_for_t::INFINITE_TRACE;
    }

    else {
      argp_error(state, "Unknown argument for trace type. Valid "
                        "arguments are [finite|infinite]");
    }
    break;
  }

  case FORMULA_INPUT_OPTION: {
    formula_input = std::filesystem::path(arg);
    break;
  }

  case VERIFYING_MODE_OPTION: {
    std::string argument(arg);
    if (argument == "satisfiability") {
      VERIFYING_MODE =
          mitl2gta::compilation::verification_mode_t::SATISFIABILITY;

      NODES_FOR.insert(mitl2gta::compilation::node_type_t::ATOMIC_PROPOSITION);
    }

    else if (argument == "model_checking") {
      VERIFYING_MODE =
          mitl2gta::compilation::verification_mode_t::MODEL_CHECKING;
    }

    else {
      argp_error(state, "Unknown argument for verification mode. Valid "
                        "arguments are [satisfiability|model_checking]");
    }

    break;
  }

  case GTA_OUTPUT_OPTION: {
    gta_output = std::filesystem::path(arg);
    break;
  }

  case SUBFORMULA_SHARING_OPTION: {
    std::string argument(arg);
    if (argument == "on") {
      PARSING_MODE = mitl2gta::mitl::parsing_mode_t::SUBFORMULA_SHARING_ENABLED;
    }

    else if (argument == "off") {
      PARSING_MODE =
          mitl2gta::mitl::parsing_mode_t::SUBFORMULA_SHARING_DISABLED;
    }

    else {
      argp_error(state, "Unknown argument for subformula sharing. Valid "
                        "arguments are [on|off]");
    }
    break;
  }

  case MAPPING_OPTIMISED_OPTION: {
    std::string argument(arg);
    if (argument == "on") {
      MAPPING_MODE = mitl2gta::memory::mapping_mode_t::OPTIMISED;
    }

    else if (argument == "off") {
      MAPPING_MODE = mitl2gta::memory::mapping_mode_t::ONE_TO_ONE;
    }

    else {
      argp_error(state, "Unknown argument for optimised memory mapping. Valid "
                        "arguments are [on|off]");
    }
    break;
  }

  case SHARING_MODE_OPTION: {
    std::string argument(arg);
    if (argument == "on") {
      SHARING_MODE = mitl2gta::sharer::sharer_mode_t::SHARING_ENABLED;
    }

    else if (argument == "off") {
      SHARING_MODE = mitl2gta::sharer::sharer_mode_t::SHARING_DISABLED;
    }

    else {
      argp_error(state, "Unknown argument for transducer sharing. Valid "
                        "arguments are [on|off]");
    }
    break;
  }

  case P_UNTIL_Q_STATES_OPTION: {
    std::string argument(arg);
    if (argument == "two") {
      P_UNTIL_Q_SHARER_STATES =
          mitl2gta::sharer::p_until_q_sharer_states_t::TWO_STATE;
    }

    else if (argument == "three") {
      P_UNTIL_Q_SHARER_STATES =
          mitl2gta::sharer::p_until_q_sharer_states_t::THREE_STATE;
    }

    else {
      argp_error(state, "Unknown argument for until operator states. Valid "
                        "arguments are [two|three]");
    }
    break;
  }

  case BOOLEAN_LABELS_OPTION: {
    std::string argument(arg);
    if (argument == "on") {
    }

    else if (argument == "off") {
      NODES_FOR.insert({mitl2gta::compilation::node_type_t::ATOMIC_CONSTANT,
                        mitl2gta::compilation::node_type_t::BOOLEAN_OPERATOR});
    }

    else {
      argp_error(state, "Unknown argument for boolean labels. Valid "
                        "arguments are [on|off]");
    }
    break;
  }

  case INIT_SAT_OPTION: {
    std::string argument(arg);
    if (argument == "on") {
      TRANSLATION_MODE = INIT_SAT_AUTOMATA;
    }

    else if (argument == "off") {
      TRANSLATION_MODE = ALL_TRANSDUCERS;
    }

    else {
      argp_error(state,
                 "Unknown argument for initial satisfiability mode. Valid "
                 "arguments are [on|off]");
    }
    break;
  }
  }
  return 0;
}

struct argp_option options[] = {
    {"input_formula_file", FORMULA_INPUT_OPTION, "<formula_input_filepath>", 0,
     "Input file containing the formula. If this option is not passed, "
     "formula "
     "is read from standard input"},

    {"verification_mode", VERIFYING_MODE_OPTION,
     "[satisfiability|model_checking]", 0,
     "Veryfication mode for translation. Default: Model checking"},

    {"gta_output_file", GTA_OUTPUT_OPTION, "<gta_output_filepath>", 0,
     "Output file for the GTA. If this option is not passed, system "
     "is written to standard output"},

    {"trace_type", TRACE_TYPE_OPTION, "[finite|infinite]", 0,
     "Trace type for verification.\nDefault:Infinite"},

    {"subformula_sharing", SUBFORMULA_SHARING_OPTION, "[on|off]", 0,
     "Switch subformula sharing on or off.\nDefault: on"},

    {"transducer_sharing", SHARING_MODE_OPTION, "[on|off]", 0,
     "Switch transducer sharing on or off.\nDefault: on"},

    {"p_until_q_states", P_UNTIL_Q_STATES_OPTION, "[two|three]", 0,
     "Set number of states for 0-infty fragment of until operator for "
     "infinite traces\nDefault:two"},

    {"optimised_memory_mapping", MAPPING_OPTIMISED_OPTION, "[on|off]", 0,
     "Switch optimisation for memory mapping on or off.\nDefault: on"},

    {"boolean_labels", BOOLEAN_LABELS_OPTION, "[on|off]", 0,
     "Switch boolean labels on edges on or off.\nDefault: on"},

    {"init_sat", INIT_SAT_OPTION, "[on|off]", 0,
     "Switch generation of automata for initial satisfiability on or "
     "off.\nDefault: on"},

    {0},
};

struct argp argparser = {options, parse_opt};

} // namespace

// parsed_config_t
parsed_config_t::parsed_config_t(
    mitl2gta::compilation::verification_mode_t verifying_mode,
    translate_mode_t translation_mode,
    mitl2gta::transducer::system_for_t system_for,
    std::set<mitl2gta::compilation::node_type_t> nodes_for,
    std::optional<std::filesystem::path> formula_input,
    std::optional<std::filesystem::path> gta_output,
    mitl2gta::mitl::parsing_mode_t parsing_mode,
    mitl2gta::sharer::sharer_mode_t sharing_mode,
    mitl2gta::sharer::p_until_q_sharer_states_t p_until_q_sharer_states,
    mitl2gta::memory::mapping_mode_t mapping_mode)
    : _verifying_mode(verifying_mode), _translation_mode(translation_mode),
      _system_for(system_for), _nodes_for(std::move(nodes_for)),
      _formula_input(std::move(formula_input)),
      _gta_output(std::move(gta_output)), _parsing_mode(parsing_mode),
      _sharing_mode(sharing_mode),
      _p_until_q_sharer_states(p_until_q_sharer_states),
      _mapping_mode(mapping_mode) {}

std::optional<parsed_config_t> parse_args(int argc, char *argv[]) {

  error_t res = argp_parse(&argparser, argc, argv, 0, 0, 0);
  if (res != 0) {
    return std::nullopt;
  }

  return std::make_optional<parsed_config_t>(
      VERIFYING_MODE, TRANSLATION_MODE, SYSTEM_FOR, NODES_FOR, formula_input,
      gta_output, PARSING_MODE, SHARING_MODE, P_UNTIL_Q_SHARER_STATES,
      MAPPING_MODE);
}
