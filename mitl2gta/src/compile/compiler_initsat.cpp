/*
 * This file is a part of MITL2GTA project.
 *
 * See AUTHORS and LICENSE for copyright details.
 *
 */

#include <algorithm>
#include <cassert>
#include <iostream>
#include <map>
#include <stack>
#include <variant>

#include "mitl2gta/compile/atomic_constant.hpp"
#include "mitl2gta/compile/atomic_proposition.hpp"
#include "mitl2gta/compile/booleans.hpp"
#include "mitl2gta/compile/compiled_node.hpp"
#include "mitl2gta/compile/compiler_initsat.hpp"
#include "mitl2gta/compile/finally.hpp"
#include "mitl2gta/compile/initsat.hpp"
#include "mitl2gta/compile/master.hpp"
#include "mitl2gta/compile/next.hpp"
#include "mitl2gta/compile/since.hpp"
#include "mitl2gta/compile/until.hpp"
#include "mitl2gta/compile/yesterday.hpp"
#include "mitl2gta/gta_system/variable.hpp"
#include "mitl2gta/memory/memory_handler.hpp"
#include "mitl2gta/mitl_formula/static_analysis.hpp"
#include "mitl2gta/sharer_automaton/translation.hpp"
#include "mitl2gta/transducer/translation.hpp"

namespace mitl2gta {

namespace compilation {

compiler_initsat_t::compiler_initsat_t(
    std::shared_ptr<mitl2gta::mitl::formula_t const> const &root_ptr,
    mitl2gta::compilation::verification_mode_t const verification_mode)
    : _verification_mode(verification_mode),
      _subformula_to_placement_map(
          mitl2gta::mitl::compute_node_placements(root_ptr)) {
  _translate_ctx = TRANSLATING_NESTED;
  root_ptr->visit(*this);

  _translate_ctx = TRANSLATING_TOPLEVEL;
  root_ptr->visit(*this);
  _root_id = _parsed_node_to_toplevel_id.at(root_ptr.get());

  if (verification_mode ==
      mitl2gta::compilation::verification_mode_t::MODEL_CHECKING) {
    std::shared_ptr<mitl2gta::compilation::compiled_toplevel_negation_t const>
        new_root_ptr = std::make_shared<
            mitl2gta::compilation::compiled_toplevel_negation_t>(_root_id,
                                                                 _allocator);

    _id_to_top_level_node.insert({new_root_ptr->id(), new_root_ptr});

    _root_id = new_root_ptr->id();
  }
}

mitl2gta::gta::system_t compiler_initsat_t::translate_to_gta_system(
    mitl2gta::sharer::sharer_mode_t const mode,
    mitl2gta::sharer::p_until_q_sharer_states_t const p_until_q_sharer_states,
    mitl2gta::compilation::translation_specs_t const &specs) {
  mitl2gta::compilation::node_to_truth_value_predictor_t id_to_truth_predictor;

  mitl2gta::transducer::system_t system(specs.system_for);

  translate_to_transducer_system(system, mode, p_until_q_sharer_states, specs,
                                 id_to_truth_predictor);

  std::map<mitl2gta::compilation::node_id_t, mitl2gta::memory::memory_id_t>
      placeholder_memory = allocate_placeholder_memory(id_to_truth_predictor,
                                                       system.memory_handler());

  mitl2gta::memory::memory_mapping_t mapping(
      specs.mapping_mode, system, id_to_truth_predictor, placeholder_memory);

  if (_verification_mode ==
      mitl2gta::compilation::verification_mode_t::MODEL_CHECKING) {
    for (auto const &[ap, id] : _atomic_propositions_to_id) {
      mitl2gta::memory::memory_id_t const mem_id = placeholder_memory.at(id);
      std::string const mem_name = mapping.mapped_var(mem_id);
      std::cout << ap << ":" << mem_name << std::endl;
    }
  }

  mitl2gta::transducer::synchronized_gta_system_t translator(
      system, id_to_truth_predictor, placeholder_memory, mapping,
      _verification_mode);

  return translator.gta_system();
}

std::shared_ptr<mitl2gta::compilation::compiled_node_t const>
compiler_initsat_t::get_node(mitl2gta::compilation::node_id_t const id) const {
  if (_id_to_nested_node.find(id) != _id_to_nested_node.end()) {
    return _id_to_nested_node.at(id);
  }

  else if (_id_to_top_level_node.find(id) != _id_to_top_level_node.end()) {
    return _id_to_top_level_node.at(id);
  }

  else {
    throw std::runtime_error("Node not found");
  }
}

std::vector<mitl2gta::compilation::node_id_t>
compiler_initsat_t::topological_sort() const {
  std::set<mitl2gta::compilation::node_id_t> visited;
  std::vector<mitl2gta::compilation::node_id_t> ordering;
  std::stack<mitl2gta::compilation::node_id_t> current;

  current.push(_root_id);

  while (!current.empty()) {
    mitl2gta::compilation::node_id_t const curr_id = current.top();
    int num_succs = 0;

    if (visited.find(curr_id) != visited.end()) {
      current.pop();
      continue;
    }

    std::shared_ptr<mitl2gta::compilation::compiled_node_t const> node =
        get_node(curr_id);

    for (auto const &succ : node->direct_dependencies()) {
      if (visited.find(succ) != visited.end()) {
        continue;
      }

      num_succs++;
      current.push(succ);
    }

    if (num_succs == 0) {
      ordering.push_back(curr_id);
      visited.insert(curr_id);
      current.pop();
    }
  }

  std::reverse(ordering.begin(), ordering.end());

  return ordering;
}

void compiler_initsat_t::translate_to_transducer_system(
    mitl2gta::transducer::system_t &system,
    mitl2gta::sharer::sharer_mode_t const mode,
    mitl2gta::sharer::p_until_q_sharer_states_t const p_until_q_sharer_states,
    mitl2gta::compilation::translation_specs_t const &specs,
    mitl2gta::compilation::node_to_truth_value_predictor_t &mapping_to_fill) {
  mitl2gta::sharer::sharer_registry_t sharer_registry(mode);

  mitl2gta::transducer::transducer_construction_tools_t tools{
      system.state_id_allocator_t(), system.memory_handler(),
      system.clock_registry(), sharer_registry};

  std::vector<mitl2gta::compilation::node_id_t> topological_ordering =
      topological_sort();

  for (auto it = topological_ordering.rbegin();
       it != topological_ordering.rend(); ++it) {
    mapping_to_fill.insert(
        {*it, get_node(*it)->generate_truth_value_predictor(tools, specs)});
  }

  for (auto it = topological_ordering.rbegin();
       it != topological_ordering.rend(); ++it) {
    std::deque<mitl2gta::sharer::sharer_automaton_t> dependencies =
        sharer_registry.dependencies(*it);
    for (auto it = dependencies.rbegin(); it != dependencies.rend(); ++it) {
      system.add_transducer(mitl2gta::sharer::to_transducer(
          *it, p_until_q_sharer_states, specs.system_for, tools));
    }

    mitl2gta::compilation::truth_value_predictor_t &predictor =
        mapping_to_fill.at(*it);
    if (!std::holds_alternative<
            mitl2gta::compilation::predicted_by_transducer_t>(predictor)) {
      continue;
    }

    auto const &transducer_wrapper =
        std::get<mitl2gta::compilation::predicted_by_transducer_t>(predictor);

    system.add_transducer(transducer_wrapper.transducer);
  }

  system.add_transducer(mitl2gta::compilation::construct_init_sat_master(
      _root_id, specs.system_for, tools));
}

std::map<mitl2gta::compilation::node_id_t, mitl2gta::memory::memory_id_t>
compiler_initsat_t::allocate_placeholder_memory(
    mitl2gta::compilation::node_to_truth_value_predictor_t &mapping,
    mitl2gta::memory::memory_handler_t &handler) const {
  std::map<mitl2gta::compilation::node_id_t, mitl2gta::memory::memory_id_t> res;

  mitl2gta::boundval_t constexpr uncertain_lower =
      std::min({mitl2gta::boolean::truth_val_t::UNCERTAIN,
                mitl2gta::boolean::truth_val_t::FALSE,
                mitl2gta::boolean::truth_val_t::TRUE});
  mitl2gta::boundval_t constexpr uncertain_upper =
      std::max({mitl2gta::boolean::truth_val_t::UNCERTAIN,
                mitl2gta::boolean::truth_val_t::FALSE,
                mitl2gta::boolean::truth_val_t::TRUE});

  mitl2gta::gta::integer_variable_range_t uncertain_range(uncertain_lower,
                                                          uncertain_upper);

  mitl2gta::memory::memory_requirements_t transducer_req{
      uncertain_range, uncertain_lower,
      mitl2gta::memory::memory_type_t::LOCAL_TO_SINGLE_ROUND};

  mitl2gta::boundval_t constexpr boolean_lower =
      std::min(mitl2gta::boolean::truth_val_t::FALSE,
               mitl2gta::boolean::truth_val_t::TRUE);
  mitl2gta::boundval_t constexpr boolean_upper =
      std::max(mitl2gta::boolean::truth_val_t::FALSE,
               mitl2gta::boolean::truth_val_t::TRUE);

  mitl2gta::gta::integer_variable_range_t boolean_range(boolean_lower,
                                                        boolean_upper);

  mitl2gta::memory::memory_requirements_t ap_req{
      boolean_range, boolean_lower,
      mitl2gta::memory::memory_type_t::PERSISTENT};

  for (auto const &[id, predictor] : mapping) {
    if (std::holds_alternative<
            mitl2gta::compilation::predicted_by_transducer_t>(predictor)) {
      mitl2gta::memory::memory_id_t const allocated_mem =
          handler.request_memory(transducer_req);

      res.insert({id, allocated_mem});
    }

    else if (std::holds_alternative<
                 mitl2gta::compilation::atomic_proposition_t>(predictor)) {
      mitl2gta::memory::memory_id_t const allocated_mem =
          handler.request_memory(ap_req);

      res.insert({id, allocated_mem});
    }
  }
  return res;
}

void compiler_initsat_t::insert_new_toplevel_node(
    mitl2gta::mitl::formula_t const *parsed_node_ptr,
    std::shared_ptr<mitl2gta::compilation::compiled_node_t const>
        compiled_node) {
  assert(_parsed_node_to_toplevel_id.find(parsed_node_ptr) ==
         _parsed_node_to_toplevel_id.end());
  assert(_id_to_top_level_node.find(compiled_node->id()) ==
         _id_to_top_level_node.end());

  _parsed_node_to_toplevel_id.insert({parsed_node_ptr, compiled_node->id()});
  _id_to_top_level_node.insert({compiled_node->id(), compiled_node});
}

void compiler_initsat_t::insert_new_nested_node(
    mitl2gta::mitl::formula_t const *parsed_node_ptr,
    std::shared_ptr<mitl2gta::compilation::compiled_node_t const>
        compiled_node) {
  assert(_parsed_node_to_nested_id.find(parsed_node_ptr) ==
         _parsed_node_to_nested_id.end());
  assert(_id_to_top_level_node.find(compiled_node->id()) ==
         _id_to_top_level_node.end());

  _parsed_node_to_nested_id.insert({parsed_node_ptr, compiled_node->id()});
  _id_to_nested_node.insert({compiled_node->id(), compiled_node});
}

void compiler_initsat_t::visit(
    mitl2gta::mitl::atomic_proposition_t const &node) {
  switch (_translate_ctx) {
  case TRANSLATING_NESTED: {
    if (_parsed_node_to_nested_id.find(&node) !=
        _parsed_node_to_nested_id.end()) {
      return;
    }

    auto const &it = _atomic_propositions_to_id.find(node.name());
    if (it != _atomic_propositions_to_id.end()) {
      _parsed_node_to_nested_id.insert({&node, it->second});
      return;
    }

    std::shared_ptr<mitl2gta::compilation::compiled_atomic_proposition_t const>
        new_node = std::make_shared<
            mitl2gta::compilation::compiled_atomic_proposition_t>(_allocator);

    _atomic_propositions_to_id.insert({node.name(), new_node->id()});

    insert_new_nested_node(&node, new_node);
    return;
  }
  case TRANSLATING_TOPLEVEL: {
    if (_parsed_node_to_toplevel_id.find(&node) !=
        _parsed_node_to_toplevel_id.end()) {
      return;
    }

    mitl2gta::compilation::node_id_t const self_copy =
        _atomic_propositions_to_id.at(node.name());

    std::shared_ptr<mitl2gta::compilation::compiled_toplevel_node_value_t const>
        new_node = std::make_shared<
            mitl2gta::compilation::compiled_toplevel_node_value_t>(self_copy,
                                                                   _allocator);
    insert_new_toplevel_node(&node, new_node);
    return;
  }
  }
  throw std::runtime_error("Incomplete switch statement");
}

void compiler_initsat_t::visit(mitl2gta::mitl::atomic_constant_t const &node) {
  switch (_translate_ctx) {
  case TRANSLATING_NESTED: {
    if (_parsed_node_to_nested_id.find(&node) !=
        _parsed_node_to_nested_id.end()) {
      return;
    }
    std::shared_ptr<mitl2gta::compilation::compiled_atomic_constant_t const>
        new_node =
            std::make_shared<mitl2gta::compilation::compiled_atomic_constant_t>(
                node.value(), _allocator);
    insert_new_nested_node(&node, new_node);
    return;
  }

  case TRANSLATING_TOPLEVEL: {
    if (_parsed_node_to_toplevel_id.find(&node) !=
        _parsed_node_to_toplevel_id.end()) {
      return;
    }
    std::shared_ptr<mitl2gta::compilation::compiled_atomic_constant_t const>
        new_node =
            std::make_shared<mitl2gta::compilation::compiled_atomic_constant_t>(
                node.value(), _allocator);
    insert_new_toplevel_node(&node, new_node);
    return;
  }
  }
}

void compiler_initsat_t::visit(mitl2gta::mitl::negation_t const &node) {
  switch (_translate_ctx) {
  case TRANSLATING_NESTED: {
    if (_subformula_to_placement_map.at(&node) ==
        mitl2gta::mitl::subformula_placement_t::TOP_LEVEL) {
      node.subformula()->visit(*this);
      return;
    }

    if (_parsed_node_to_nested_id.find(&node) !=
        _parsed_node_to_nested_id.end()) {
      return;
    }

    node.subformula()->visit(*this);

    mitl2gta::compilation::node_id_t const child =
        _parsed_node_to_nested_id.find(node.subformula().get())->second;

    std::shared_ptr<mitl2gta::compilation::compiled_negation_t const> new_node =
        std::make_shared<mitl2gta::compilation::compiled_negation_t>(
            child, _allocator);

    insert_new_nested_node(&node, new_node);
    return;
  }

  case TRANSLATING_TOPLEVEL: {
    if (_subformula_to_placement_map.at(&node) ==
        mitl2gta::mitl::subformula_placement_t::NESTED) {
      return;
    }

    if (_parsed_node_to_toplevel_id.find(&node) !=
        _parsed_node_to_toplevel_id.end()) {
      return;
    }

    node.subformula()->visit(*this);

    mitl2gta::compilation::node_id_t const child =
        _parsed_node_to_toplevel_id.at(node.subformula().get());

    std::shared_ptr<mitl2gta::compilation::compiled_toplevel_negation_t const>
        new_node = std::make_shared<
            mitl2gta::compilation::compiled_toplevel_negation_t>(child,
                                                                 _allocator);

    insert_new_toplevel_node(&node, new_node);
    return;
  }
  }
  throw std::runtime_error("Incomplete switch statement");
}

void compiler_initsat_t::visit(mitl2gta::mitl::or_t const &node) {
  switch (_translate_ctx) {
  case TRANSLATING_NESTED: {
    if (_subformula_to_placement_map.at(&node) ==
        mitl2gta::mitl::subformula_placement_t::TOP_LEVEL) {
      node.left_subformula()->visit(*this);
      node.right_subformula()->visit(*this);
      return;
    }

    if (_parsed_node_to_nested_id.find(&node) !=
        _parsed_node_to_nested_id.end()) {
      return;
    }

    node.left_subformula()->visit(*this);
    node.right_subformula()->visit(*this);

    mitl2gta::compilation::node_id_t const lchild =
        _parsed_node_to_nested_id.find(node.left_subformula().get())->second;

    mitl2gta::compilation::node_id_t const rchild =
        _parsed_node_to_nested_id.find(node.right_subformula().get())->second;

    std::shared_ptr<mitl2gta::compilation::compiled_or_t const> new_node =
        std::make_shared<mitl2gta::compilation::compiled_or_t>(lchild, rchild,
                                                               _allocator);

    insert_new_nested_node(&node, new_node);
    return;
  }

  case TRANSLATING_TOPLEVEL: {
    if (_subformula_to_placement_map.at(&node) ==
        mitl2gta::mitl::subformula_placement_t::NESTED) {
      return;
    }

    if (_parsed_node_to_toplevel_id.find(&node) !=
        _parsed_node_to_toplevel_id.end()) {
      return;
    }

    node.left_subformula()->visit(*this);

    mitl2gta::compilation::node_id_t const lchild =
        _parsed_node_to_toplevel_id.find(node.left_subformula().get())->second;

    node.right_subformula()->visit(*this);

    mitl2gta::compilation::node_id_t const rchild =
        _parsed_node_to_toplevel_id.find(node.right_subformula().get())->second;

    std::shared_ptr<mitl2gta::compilation::compiled_toplevel_or_t const>
        new_node =
            std::make_shared<mitl2gta::compilation::compiled_toplevel_or_t>(
                lchild, rchild, _allocator);

    insert_new_toplevel_node(&node, new_node);
    return;
  }
  }
  throw std::runtime_error("Incomplete switch statement");
}

void compiler_initsat_t::visit(mitl2gta::mitl::and_t const &node) {
  switch (_translate_ctx) {
  case TRANSLATING_NESTED: {
    if (_subformula_to_placement_map.at(&node) ==
        mitl2gta::mitl::subformula_placement_t::TOP_LEVEL) {
      node.left_subformula()->visit(*this);
      node.right_subformula()->visit(*this);
      return;
    }

    if (_parsed_node_to_nested_id.find(&node) !=
        _parsed_node_to_nested_id.end()) {
      return;
    }

    node.left_subformula()->visit(*this);
    node.right_subformula()->visit(*this);

    mitl2gta::compilation::node_id_t const lchild =
        _parsed_node_to_nested_id.find(node.left_subformula().get())->second;

    mitl2gta::compilation::node_id_t const rchild =
        _parsed_node_to_nested_id.find(node.right_subformula().get())->second;

    std::shared_ptr<mitl2gta::compilation::compiled_and_t const> new_node =
        std::make_shared<mitl2gta::compilation::compiled_and_t>(lchild, rchild,
                                                                _allocator);

    insert_new_nested_node(&node, new_node);
    return;
  }

  case TRANSLATING_TOPLEVEL: {
    if (_subformula_to_placement_map.at(&node) ==
        mitl2gta::mitl::subformula_placement_t::NESTED) {
      return;
    }

    if (_parsed_node_to_toplevel_id.find(&node) !=
        _parsed_node_to_toplevel_id.end()) {
      return;
    }

    node.left_subformula()->visit(*this);

    mitl2gta::compilation::node_id_t const lchild =
        _parsed_node_to_toplevel_id.find(node.left_subformula().get())->second;

    node.right_subformula()->visit(*this);

    mitl2gta::compilation::node_id_t const rchild =
        _parsed_node_to_toplevel_id.find(node.right_subformula().get())->second;

    std::shared_ptr<mitl2gta::compilation::compiled_toplevel_and_t const>
        new_node =
            std::make_shared<mitl2gta::compilation::compiled_toplevel_and_t>(
                lchild, rchild, _allocator);

    insert_new_toplevel_node(&node, new_node);
    return;
  }
  }
  throw std::runtime_error("Incomplete switch statement");
}

void compiler_initsat_t::visit(mitl2gta::mitl::untimed_next_t const &node) {
  switch (_translate_ctx) {
  case TRANSLATING_NESTED: {
    if (_parsed_node_to_nested_id.find(&node) !=
        _parsed_node_to_nested_id.end()) {
      return;
    }

    node.subformula()->visit(*this);

    if (_subformula_to_placement_map.at(&node) ==
        mitl2gta::mitl::subformula_placement_t::TOP_LEVEL) {
      return;
    }

    mitl2gta::compilation::node_id_t const child =
        _parsed_node_to_nested_id.find(node.subformula().get())->second;

    std::shared_ptr<mitl2gta::compilation::compiled_untimed_next_t const>
        new_node =
            std::make_shared<mitl2gta::compilation::compiled_untimed_next_t>(
                child, _allocator);

    insert_new_nested_node(&node, new_node);
    return;
  }
  case TRANSLATING_TOPLEVEL: {
    if (_parsed_node_to_toplevel_id.find(&node) !=
        _parsed_node_to_toplevel_id.end()) {
      return;
    }

    switch (_subformula_to_placement_map.at(&node)) {
    case mitl2gta::mitl::subformula_placement_t::NESTED: {
      return;
    }
    case mitl2gta::mitl::subformula_placement_t::TOP_AND_NESTED: {
      mitl2gta::compilation::node_id_t const self_copy =
          _parsed_node_to_nested_id.at(&node);
      std::shared_ptr<
          mitl2gta::compilation::compiled_toplevel_node_value_t const>
          new_node = std::make_shared<
              mitl2gta::compilation::compiled_toplevel_node_value_t>(
              self_copy, _allocator);
      insert_new_toplevel_node(&node, new_node);
      return;
    }
    case mitl2gta::mitl::subformula_placement_t::TOP_LEVEL: {
      mitl2gta::compilation::node_id_t const child =
          _parsed_node_to_nested_id.at(node.subformula().get());
      std::shared_ptr<
          mitl2gta::compilation::compiled_untimed_toplevel_next_t const>
          new_node = std::make_shared<
              mitl2gta::compilation::compiled_untimed_toplevel_next_t>(
              child, _allocator);
      insert_new_toplevel_node(&node, new_node);
      return;
    }
    }
    return;
  }
  }
  throw std::runtime_error("Incomplete switch statement");
}

void compiler_initsat_t::visit(mitl2gta::mitl::timed_next_t const &node) {
  switch (_translate_ctx) {
  case TRANSLATING_NESTED: {
    if (_parsed_node_to_nested_id.find(&node) !=
        _parsed_node_to_nested_id.end()) {
      return;
    }

    node.subformula()->visit(*this);

    if (_subformula_to_placement_map.at(&node) ==
        mitl2gta::mitl::subformula_placement_t::TOP_LEVEL) {
      return;
    }

    mitl2gta::compilation::node_id_t const child =
        _parsed_node_to_nested_id.find(node.subformula().get())->second;

    std::shared_ptr<mitl2gta::compilation::compiled_timed_next_t const>
        new_node =
            std::make_shared<mitl2gta::compilation::compiled_timed_next_t>(
                node.interval(), child, _allocator);

    insert_new_nested_node(&node, new_node);
    return;
  }
  case TRANSLATING_TOPLEVEL: {
    if (_parsed_node_to_toplevel_id.find(&node) !=
        _parsed_node_to_toplevel_id.end()) {
      return;
    }

    switch (_subformula_to_placement_map.at(&node)) {
    case mitl2gta::mitl::subformula_placement_t::NESTED: {
      return;
    }
    case mitl2gta::mitl::subformula_placement_t::TOP_AND_NESTED: {
      mitl2gta::compilation::node_id_t const self_copy =
          _parsed_node_to_nested_id.at(&node);
      std::shared_ptr<
          mitl2gta::compilation::compiled_toplevel_node_value_t const>
          new_node = std::make_shared<
              mitl2gta::compilation::compiled_toplevel_node_value_t>(
              self_copy, _allocator);
      insert_new_toplevel_node(&node, new_node);
      return;
    }
    case mitl2gta::mitl::subformula_placement_t::TOP_LEVEL: {
      mitl2gta::compilation::node_id_t const child =
          _parsed_node_to_nested_id.at(node.subformula().get());
      std::shared_ptr<
          mitl2gta::compilation::compiled_timed_toplevel_next_t const>
          new_node = std::make_shared<
              mitl2gta::compilation::compiled_timed_toplevel_next_t>(
              node.interval(), child, _allocator);
      insert_new_toplevel_node(&node, new_node);
      return;
    }
    }
    return;
  }
  }
  throw std::runtime_error("Incomplete switch statement");
}

void compiler_initsat_t::visit(
    mitl2gta::mitl::untimed_yesterday_t const &node) {
  switch (_translate_ctx) {
  case TRANSLATING_NESTED: {
    if (_parsed_node_to_nested_id.find(&node) !=
        _parsed_node_to_nested_id.end()) {
      return;
    }

    node.subformula()->visit(*this);

    if (_subformula_to_placement_map.at(&node) ==
        mitl2gta::mitl::subformula_placement_t::TOP_LEVEL) {
      return;
    }

    mitl2gta::compilation::node_id_t const child =
        _parsed_node_to_nested_id.find(node.subformula().get())->second;

    std::shared_ptr<mitl2gta::compilation::compiled_untimed_yesterday_t const>
        new_node = std::make_shared<
            mitl2gta::compilation::compiled_untimed_yesterday_t>(child,
                                                                 _allocator);

    insert_new_nested_node(&node, new_node);
    return;
  }
  case TRANSLATING_TOPLEVEL: {
    if (_parsed_node_to_toplevel_id.find(&node) !=
        _parsed_node_to_toplevel_id.end()) {
      return;
    }
    std::shared_ptr<mitl2gta::compilation::compiled_atomic_constant_t const>
        new_node =
            std::make_shared<mitl2gta::compilation::compiled_atomic_constant_t>(
                mitl2gta::mitl::constant_value_t::BOOL_FALSE, _allocator);
    insert_new_toplevel_node(&node, new_node);
    return;
  }
  }
  throw std::runtime_error("Incomplete switch statement");
}

void compiler_initsat_t::visit(mitl2gta::mitl::timed_yesterday_t const &node) {
  switch (_translate_ctx) {
  case TRANSLATING_NESTED: {
    if (_parsed_node_to_nested_id.find(&node) !=
        _parsed_node_to_nested_id.end()) {
      return;
    }

    node.subformula()->visit(*this);

    if (_subformula_to_placement_map.at(&node) ==
        mitl2gta::mitl::subformula_placement_t::TOP_LEVEL) {
      return;
    }

    mitl2gta::compilation::node_id_t const child =
        _parsed_node_to_nested_id.find(node.subformula().get())->second;

    std::shared_ptr<mitl2gta::compilation::compiled_timed_yesterday_t const>
        new_node =
            std::make_shared<mitl2gta::compilation::compiled_timed_yesterday_t>(
                node.interval(), child, _allocator);

    insert_new_nested_node(&node, new_node);
    return;
  }
  case TRANSLATING_TOPLEVEL: {
    if (_parsed_node_to_toplevel_id.find(&node) !=
        _parsed_node_to_toplevel_id.end()) {
      return;
    }
    std::shared_ptr<mitl2gta::compilation::compiled_atomic_constant_t const>
        new_node =
            std::make_shared<mitl2gta::compilation::compiled_atomic_constant_t>(
                mitl2gta::mitl::constant_value_t::BOOL_FALSE, _allocator);
    insert_new_toplevel_node(&node, new_node);
    return;
  }
  }
  throw std::runtime_error("Incomplete switch statement");
}

void compiler_initsat_t::visit(mitl2gta::mitl::untimed_finally_t const &node) {
  switch (_translate_ctx) {
  case TRANSLATING_NESTED: {
    if (_parsed_node_to_nested_id.find(&node) !=
        _parsed_node_to_nested_id.end()) {
      return;
    }

    node.subformula()->visit(*this);

    if (_subformula_to_placement_map.at(&node) ==
        mitl2gta::mitl::subformula_placement_t::TOP_LEVEL) {
      return;
    }

    mitl2gta::compilation::node_id_t const child =
        _parsed_node_to_nested_id.find(node.subformula().get())->second;

    std::shared_ptr<mitl2gta::compilation::compiled_atomic_constant_t const>
        true_node =
            std::make_shared<mitl2gta::compilation::compiled_atomic_constant_t>(
                mitl2gta::mitl::constant_value_t::BOOL_TRUE, _allocator);

    _id_to_nested_node.insert({true_node->id(), true_node});

    std::shared_ptr<mitl2gta::compilation::compiled_untimed_until_t const>
        new_node = std::make_shared<
            mitl2gta::compilation::compiled_untimed_until_t const>(
            true_node->id(), child, _allocator);

    insert_new_nested_node(&node, new_node);
    return;
  }
  case TRANSLATING_TOPLEVEL: {
    if (_parsed_node_to_toplevel_id.find(&node) !=
        _parsed_node_to_toplevel_id.end()) {
      return;
    }

    switch (_subformula_to_placement_map.at(&node)) {
    case mitl2gta::mitl::subformula_placement_t::NESTED: {
      return;
    }
    case mitl2gta::mitl::subformula_placement_t::TOP_AND_NESTED: {
      mitl2gta::compilation::node_id_t const self_copy =
          _parsed_node_to_nested_id.at(&node);
      std::shared_ptr<
          mitl2gta::compilation::compiled_toplevel_node_value_t const>
          new_node = std::make_shared<
              mitl2gta::compilation::compiled_toplevel_node_value_t>(
              self_copy, _allocator);
      insert_new_toplevel_node(&node, new_node);
      return;
    }
    case mitl2gta::mitl::subformula_placement_t::TOP_LEVEL: {
      mitl2gta::compilation::node_id_t const child =
          _parsed_node_to_nested_id.at(node.subformula().get());
      std::shared_ptr<
          mitl2gta::compilation::compiled_untimed_toplevel_finally_t const>
          new_node = std::make_shared<
              mitl2gta::compilation::compiled_untimed_toplevel_finally_t>(
              child, _allocator);
      insert_new_toplevel_node(&node, new_node);
      return;
    }
    }
    return;
  }
  }
  throw std::runtime_error("Incomplete switch statement");
}

void compiler_initsat_t::visit(mitl2gta::mitl::timed_finally_t const &node) {
  switch (_translate_ctx) {
  case TRANSLATING_NESTED: {
    if (_parsed_node_to_nested_id.find(&node) !=
        _parsed_node_to_nested_id.end()) {
      return;
    }

    node.subformula()->visit(*this);

    if (_subformula_to_placement_map.at(&node) == mitl2gta::mitl::TOP_LEVEL) {
      return;
    }

    mitl2gta::compilation::node_id_t const child =
        _parsed_node_to_nested_id.find(node.subformula().get())->second;

    mitl2gta::interval_type_t interval_type =
        mitl2gta::interval_type(node.interval());

    if (interval_type == mitl2gta::interval_type_t::UPPER_BOUNDED) {
      std::shared_ptr<
          mitl2gta::compilation::compiled_upper_bounded_finally_t const>
          new_node = std::make_shared<
              mitl2gta::compilation::compiled_upper_bounded_finally_t>(
              node.interval().upper_bound(), child, _allocator);
      insert_new_nested_node(&node, new_node);
      return;
    }

    std::shared_ptr<mitl2gta::compilation::compiled_atomic_constant_t const>
        true_node =
            std::make_shared<mitl2gta::compilation::compiled_atomic_constant_t>(
                mitl2gta::mitl::constant_value_t::BOOL_TRUE, _allocator);

    _id_to_nested_node.insert({true_node->id(), true_node});

    std::shared_ptr<mitl2gta::compilation::compiled_node_t const> new_node;
    if (interval_type == mitl2gta::interval_type_t::UNBOUNDED) {
      new_node =
          std::make_shared<mitl2gta::compilation::compiled_untimed_until_t>(
              true_node->id(), child, _allocator);
    }

    else if (interval_type == mitl2gta::interval_type_t::LOWER_BOUNDED) {
      new_node = std::make_shared<
          mitl2gta::compilation::compiled_lower_bounded_until_t>(
          node.interval().lower_bound(), true_node->id(), child, _allocator);
    }

    else if (interval_type == mitl2gta::interval_type_t::LOWER_UPPER_BOUNDED) {
      new_node =
          std::make_shared<mitl2gta::compilation::compiled_timed_until_t>(
              node.interval(), true_node->id(), child, _allocator);
    }

    else {
      throw std::runtime_error("Incomplete if-else statement");
    }

    insert_new_nested_node(&node, new_node);
    return;
  }
  case TRANSLATING_TOPLEVEL: {
    if (_parsed_node_to_toplevel_id.find(&node) !=
        _parsed_node_to_toplevel_id.end()) {
      return;
    }

    switch (_subformula_to_placement_map.at(&node)) {
    case mitl2gta::mitl::subformula_placement_t::NESTED: {
      return;
    }
    case mitl2gta::mitl::subformula_placement_t::TOP_AND_NESTED: {
      mitl2gta::compilation::node_id_t const self_copy =
          _parsed_node_to_nested_id.at(&node);
      std::shared_ptr<
          mitl2gta::compilation::compiled_toplevel_node_value_t const>
          new_node = std::make_shared<
              mitl2gta::compilation::compiled_toplevel_node_value_t>(
              self_copy, _allocator);
      insert_new_toplevel_node(&node, new_node);
      return;
    }
    case mitl2gta::mitl::subformula_placement_t::TOP_LEVEL: {
      mitl2gta::compilation::node_id_t const child =
          _parsed_node_to_nested_id.at(node.subformula().get());
      std::shared_ptr<
          mitl2gta::compilation::compiled_timed_toplevel_finally_t const>
          new_node = std::make_shared<
              mitl2gta::compilation::compiled_timed_toplevel_finally_t>(
              node.interval(), child, _allocator);
      insert_new_toplevel_node(&node, new_node);
      return;
    }
    }
    return;
  }
  }
  throw std::runtime_error("Incomplete switch statement");
}

void compiler_initsat_t::visit(mitl2gta::mitl::untimed_globally_t const &node) {
  switch (_translate_ctx) {
  case TRANSLATING_NESTED: {
    if (_parsed_node_to_nested_id.find(&node) !=
        _parsed_node_to_nested_id.end()) {
      return;
    }

    node.subformula()->visit(*this);
    if (_subformula_to_placement_map.at(&node) ==
        mitl2gta::mitl::subformula_placement_t::TOP_LEVEL) {
      return;
    }

    mitl2gta::compilation::node_id_t const child =
        _parsed_node_to_nested_id.find(node.subformula().get())->second;

    std::shared_ptr<mitl2gta::compilation::compiled_negation_t const>
        notp_node =
            std::make_shared<mitl2gta::compilation::compiled_negation_t>(
                child, _allocator);
    _id_to_nested_node.insert({notp_node->id(), notp_node});

    std::shared_ptr<mitl2gta::compilation::compiled_atomic_constant_t const>
        true_node =
            std::make_shared<mitl2gta::compilation::compiled_atomic_constant_t>(
                mitl2gta::mitl::constant_value_t::BOOL_TRUE, _allocator);

    _id_to_nested_node.insert({true_node->id(), true_node});

    std::shared_ptr<mitl2gta::compilation::compiled_untimed_until_t const>
        true_until_not_p_node =
            std::make_shared<mitl2gta::compilation::compiled_untimed_until_t>(
                true_node->id(), notp_node->id(), _allocator);
    _id_to_nested_node.insert(
        {true_until_not_p_node->id(), true_until_not_p_node});

    std::shared_ptr<mitl2gta::compilation::compiled_negation_t const> new_node =
        std::make_shared<mitl2gta::compilation::compiled_negation_t>(
            true_until_not_p_node->id(), _allocator);
    insert_new_nested_node(&node, new_node);
    return;
  }
  case TRANSLATING_TOPLEVEL: {
    if (_parsed_node_to_toplevel_id.find(&node) !=
        _parsed_node_to_toplevel_id.end()) {
      return;
    }

    switch (_subformula_to_placement_map.at(&node)) {
    case mitl2gta::mitl::subformula_placement_t::NESTED: {
      return;
    }
    case mitl2gta::mitl::subformula_placement_t::TOP_AND_NESTED: {
      mitl2gta::compilation::node_id_t const self_copy =
          _parsed_node_to_nested_id.at(&node);
      std::shared_ptr<
          mitl2gta::compilation::compiled_toplevel_node_value_t const>
          new_node = std::make_shared<
              mitl2gta::compilation::compiled_toplevel_node_value_t>(
              self_copy, _allocator);
      insert_new_toplevel_node(&node, new_node);
      return;
    }
    case mitl2gta::mitl::subformula_placement_t::TOP_LEVEL: {
      mitl2gta::compilation::node_id_t const child =
          _parsed_node_to_nested_id.at(node.subformula().get());
      std::shared_ptr<
          mitl2gta::compilation::compiled_untimed_toplevel_globally_t const>
          new_node = std::make_shared<
              mitl2gta::compilation::compiled_untimed_toplevel_globally_t>(
              child, _allocator);
      insert_new_toplevel_node(&node, new_node);
      return;
    }
    }
    return;
  }
  }
  throw std::runtime_error("Incomplete switch statement");
}

void compiler_initsat_t::visit(mitl2gta::mitl::timed_globally_t const &node) {
  switch (_translate_ctx) {
  case TRANSLATING_NESTED: {
    if (_parsed_node_to_nested_id.find(&node) !=
        _parsed_node_to_nested_id.end()) {
      return;
    }

    node.subformula()->visit(*this);
    if (_subformula_to_placement_map.at(&node) ==
        mitl2gta::mitl::subformula_placement_t::TOP_LEVEL) {
      return;
    }
    mitl2gta::compilation::node_id_t const child =
        _parsed_node_to_nested_id.find(node.subformula().get())->second;

    std::shared_ptr<mitl2gta::compilation::compiled_negation_t const>
        notp_node =
            std::make_shared<mitl2gta::compilation::compiled_negation_t>(
                child, _allocator);
    _id_to_nested_node.insert({notp_node->id(), notp_node});

    std::shared_ptr<mitl2gta::compilation::compiled_node_t const>
        finally_notp_node;

    switch (mitl2gta::interval_type(node.interval())) {
    case mitl2gta::interval_type_t::UPPER_BOUNDED: {
      finally_notp_node = std::make_shared<
          mitl2gta::compilation::compiled_upper_bounded_finally_t>(
          node.interval().upper_bound(), notp_node->id(), _allocator);
      break;
    }
    case mitl2gta::interval_type_t::LOWER_BOUNDED: {
      std::shared_ptr<mitl2gta::compilation::compiled_atomic_constant_t const>
          true_node = std::make_shared<
              mitl2gta::compilation::compiled_atomic_constant_t>(
              mitl2gta::mitl::constant_value_t::BOOL_TRUE, _allocator);

      _id_to_nested_node.insert({true_node->id(), true_node});

      finally_notp_node = std::make_shared<
          mitl2gta::compilation::compiled_lower_bounded_until_t>(
          node.interval().lower_bound(), true_node->id(), notp_node->id(),
          _allocator);
      break;
    }
    default: {
      std::shared_ptr<mitl2gta::compilation::compiled_atomic_constant_t const>
          true_node = std::make_shared<
              mitl2gta::compilation::compiled_atomic_constant_t>(
              mitl2gta::mitl::constant_value_t::BOOL_TRUE, _allocator);

      _id_to_nested_node.insert({true_node->id(), true_node});

      finally_notp_node =
          std::make_shared<mitl2gta::compilation::compiled_timed_until_t>(
              node.interval(), true_node->id(), notp_node->id(), _allocator);
      break;
    }
    }

    _id_to_nested_node.insert({finally_notp_node->id(), finally_notp_node});

    std::shared_ptr<mitl2gta::compilation::compiled_negation_t const> new_node =
        std::make_shared<mitl2gta::compilation::compiled_negation_t>(
            finally_notp_node->id(), _allocator);
    insert_new_nested_node(&node, new_node);
    return;
  }
  case TRANSLATING_TOPLEVEL: {
    if (_parsed_node_to_toplevel_id.find(&node) !=
        _parsed_node_to_toplevel_id.end()) {
      return;
    }

    switch (_subformula_to_placement_map.at(&node)) {
    case mitl2gta::mitl::subformula_placement_t::NESTED: {
      return;
    }
    case mitl2gta::mitl::subformula_placement_t::TOP_AND_NESTED: {
      mitl2gta::compilation::node_id_t const self_copy =
          _parsed_node_to_nested_id.at(&node);
      std::shared_ptr<
          mitl2gta::compilation::compiled_toplevel_node_value_t const>
          new_node = std::make_shared<
              mitl2gta::compilation::compiled_toplevel_node_value_t>(
              self_copy, _allocator);
      insert_new_toplevel_node(&node, new_node);
      return;
    }
    case mitl2gta::mitl::subformula_placement_t::TOP_LEVEL: {
      mitl2gta::compilation::node_id_t const child =
          _parsed_node_to_nested_id.at(node.subformula().get());
      std::shared_ptr<
          mitl2gta::compilation::compiled_timed_toplevel_globally_t const>
          new_node = std::make_shared<
              mitl2gta::compilation::compiled_timed_toplevel_globally_t>(
              node.interval(), child, _allocator);
      insert_new_toplevel_node(&node, new_node);
      return;
    }
    }
    return;
  }
  }
  throw std::runtime_error("Incomplete switch statement");
}

void compiler_initsat_t::visit(mitl2gta::mitl::untimed_until_t const &node) {
  switch (_translate_ctx) {
  case TRANSLATING_NESTED: {
    if (_parsed_node_to_nested_id.find(&node) !=
        _parsed_node_to_nested_id.end()) {
      return;
    }

    node.left_subformula()->visit(*this);
    node.right_subformula()->visit(*this);

    if (_subformula_to_placement_map.at(&node) ==
        mitl2gta::mitl::subformula_placement_t::TOP_LEVEL) {
      return;
    }

    mitl2gta::compilation::node_id_t const lchild =
        _parsed_node_to_nested_id.find(node.left_subformula().get())->second;

    mitl2gta::compilation::node_id_t const rchild =
        _parsed_node_to_nested_id.find(node.right_subformula().get())->second;

    std::shared_ptr<mitl2gta::compilation::compiled_untimed_until_t const>
        new_node =
            std::make_shared<mitl2gta::compilation::compiled_untimed_until_t>(
                lchild, rchild, _allocator);

    insert_new_nested_node(&node, new_node);
    return;
  }
  case TRANSLATING_TOPLEVEL: {
    if (_parsed_node_to_toplevel_id.find(&node) !=
        _parsed_node_to_toplevel_id.end()) {
      return;
    }

    switch (_subformula_to_placement_map.at(&node)) {
    case mitl2gta::mitl::subformula_placement_t::NESTED: {
      return;
    }
    case mitl2gta::mitl::subformula_placement_t::TOP_AND_NESTED: {
      mitl2gta::compilation::node_id_t const self_copy =
          _parsed_node_to_nested_id.at(&node);
      std::shared_ptr<
          mitl2gta::compilation::compiled_toplevel_node_value_t const>
          new_node = std::make_shared<
              mitl2gta::compilation::compiled_toplevel_node_value_t>(
              self_copy, _allocator);
      insert_new_toplevel_node(&node, new_node);
      return;
    }
    case mitl2gta::mitl::subformula_placement_t::TOP_LEVEL: {
      mitl2gta::compilation::node_id_t const lchild =
          _parsed_node_to_nested_id.at(node.left_subformula().get());

      mitl2gta::compilation::node_id_t const rchild =
          _parsed_node_to_nested_id.at(node.right_subformula().get());

      std::shared_ptr<
          mitl2gta::compilation::compiled_untimed_toplevel_until_t const>
          new_node = std::make_shared<
              mitl2gta::compilation::compiled_untimed_toplevel_until_t>(
              lchild, rchild, _allocator);
      insert_new_toplevel_node(&node, new_node);
      return;
    }
      throw std::runtime_error("Incomplete switch statement");
    }
    return;
  }
  }
  throw std::runtime_error("Incomplete switch statement");
}

void compiler_initsat_t::visit(mitl2gta::mitl::timed_until_t const &node) {
  switch (_translate_ctx) {
  case TRANSLATING_NESTED: {
    if (_parsed_node_to_nested_id.find(&node) !=
        _parsed_node_to_nested_id.end()) {
      return;
    }

    node.left_subformula()->visit(*this);
    node.right_subformula()->visit(*this);

    if (_subformula_to_placement_map.at(&node) ==
        mitl2gta::mitl::subformula_placement_t::TOP_LEVEL) {
      return;
    }

    mitl2gta::compilation::node_id_t const lchild =
        _parsed_node_to_nested_id.find(node.left_subformula().get())->second;

    mitl2gta::compilation::node_id_t const rchild =
        _parsed_node_to_nested_id.find(node.right_subformula().get())->second;

    switch (mitl2gta::interval_type(node.interval())) {

    case mitl2gta::interval_type_t::UPPER_BOUNDED: {
      std::shared_ptr<
          mitl2gta::compilation::compiled_upper_bounded_until_t const>
          new_node = std::make_shared<
              mitl2gta::compilation::compiled_upper_bounded_until_t>(
              node.interval().upper_bound(), lchild, rchild, _allocator);
      insert_new_nested_node(&node, new_node);
      break;
    }

    case mitl2gta::interval_type_t::LOWER_BOUNDED: {
      std::shared_ptr<
          mitl2gta::compilation::compiled_lower_bounded_until_t const>
          new_node = std::make_shared<
              mitl2gta::compilation::compiled_lower_bounded_until_t>(
              node.interval().lower_bound(), lchild, rchild, _allocator);
      insert_new_nested_node(&node, new_node);
      break;
    }

    default: {
      std::shared_ptr<mitl2gta::compilation::compiled_timed_until_t const>
          new_node =
              std::make_shared<mitl2gta::compilation::compiled_timed_until_t>(
                  node.interval(), lchild, rchild, _allocator);
      insert_new_nested_node(&node, new_node);
      break;
    }
    }
    return;
  }
  case TRANSLATING_TOPLEVEL: {
    if (_parsed_node_to_toplevel_id.find(&node) !=
        _parsed_node_to_toplevel_id.end()) {
      return;
    }

    switch (_subformula_to_placement_map.at(&node)) {
    case mitl2gta::mitl::subformula_placement_t::NESTED: {
      return;
    }
    case mitl2gta::mitl::subformula_placement_t::TOP_AND_NESTED: {
      mitl2gta::compilation::node_id_t const self_copy =
          _parsed_node_to_nested_id.at(&node);
      std::shared_ptr<
          mitl2gta::compilation::compiled_toplevel_node_value_t const>
          new_node = std::make_shared<
              mitl2gta::compilation::compiled_toplevel_node_value_t>(
              self_copy, _allocator);
      insert_new_toplevel_node(&node, new_node);
      return;
    }
    case mitl2gta::mitl::subformula_placement_t::TOP_LEVEL: {
      mitl2gta::compilation::node_id_t const lchild =
          _parsed_node_to_nested_id.at(node.left_subformula().get());

      mitl2gta::compilation::node_id_t const rchild =
          _parsed_node_to_nested_id.at(node.right_subformula().get());
      std::shared_ptr<
          mitl2gta::compilation::compiled_timed_toplevel_until_t const>
          new_node = std::make_shared<
              mitl2gta::compilation::compiled_timed_toplevel_until_t>(
              node.interval(), lchild, rchild, _allocator);
      insert_new_toplevel_node(&node, new_node);
      return;
    }
      return;
    }
  }
  }
  throw std::runtime_error("Incomplete switch statement");
}

void compiler_initsat_t::visit(mitl2gta::mitl::untimed_since_t const &node) {
  switch (_translate_ctx) {
  case TRANSLATING_NESTED: {
    if (_parsed_node_to_nested_id.find(&node) !=
        _parsed_node_to_nested_id.end()) {
      return;
    }

    node.left_subformula()->visit(*this);
    node.right_subformula()->visit(*this);

    if (_subformula_to_placement_map.at(&node) ==
        mitl2gta::mitl::subformula_placement_t::TOP_LEVEL) {
      return;
    }

    mitl2gta::compilation::node_id_t const lchild =
        _parsed_node_to_nested_id.find(node.left_subformula().get())->second;

    mitl2gta::compilation::node_id_t const rchild =
        _parsed_node_to_nested_id.find(node.right_subformula().get())->second;

    std::shared_ptr<mitl2gta::compilation::compiled_untimed_since_t const>
        new_node =
            std::make_shared<mitl2gta::compilation::compiled_untimed_since_t>(
                lchild, rchild, _allocator);

    insert_new_nested_node(&node, new_node);
    return;
  }
  case TRANSLATING_TOPLEVEL: {
    if (_parsed_node_to_toplevel_id.find(&node) !=
        _parsed_node_to_toplevel_id.end()) {
      return;
    }

    switch (_subformula_to_placement_map.at(&node)) {
    case mitl2gta::mitl::subformula_placement_t::NESTED: {
      return;
    }
    default: {
      mitl2gta::compilation::node_id_t const rchild =
          _parsed_node_to_nested_id.at(node.right_subformula().get());
      std::shared_ptr<
          mitl2gta::compilation::compiled_toplevel_node_value_t const>
          new_node = std::make_shared<
              mitl2gta::compilation::compiled_toplevel_node_value_t>(
              rchild, _allocator);
      insert_new_toplevel_node(&node, new_node);
      return;
    }
      return;
    }
  }
  }
  throw std::runtime_error("Incomplete switch statement");
}

void compiler_initsat_t::visit(mitl2gta::mitl::timed_since_t const &node) {
  switch (_translate_ctx) {
  case TRANSLATING_NESTED: {
    if (_parsed_node_to_nested_id.find(&node) !=
        _parsed_node_to_nested_id.end()) {
      return;
    }

    node.left_subformula()->visit(*this);
    node.right_subformula()->visit(*this);

    if (_subformula_to_placement_map.at(&node) ==
        mitl2gta::mitl::subformula_placement_t::TOP_LEVEL) {
      return;
    }
    mitl2gta::compilation::node_id_t const lchild =
        _parsed_node_to_nested_id.find(node.left_subformula().get())->second;

    mitl2gta::compilation::node_id_t const rchild =
        _parsed_node_to_nested_id.find(node.right_subformula().get())->second;

    switch (mitl2gta::interval_type(node.interval())) {
    case mitl2gta::interval_type_t::UPPER_BOUNDED: {
      std::shared_ptr<
          mitl2gta::compilation::compiled_upper_bounded_since_t const>
          new_node = std::make_shared<
              mitl2gta::compilation::compiled_upper_bounded_since_t>(
              node.interval().upper_bound(), lchild, rchild, _allocator);
      insert_new_nested_node(&node, new_node);
      break;
    }

    case mitl2gta::interval_type_t::LOWER_BOUNDED: {
      std::shared_ptr<
          mitl2gta::compilation::compiled_lower_bounded_since_t const>
          new_node = std::make_shared<
              mitl2gta::compilation::compiled_lower_bounded_since_t>(
              node.interval().lower_bound(), lchild, rchild, _allocator);
      insert_new_nested_node(&node, new_node);
      break;
    }

    default: {
      std::shared_ptr<mitl2gta::compilation::compiled_timed_since_t const>
          new_node =
              std::make_shared<mitl2gta::compilation::compiled_timed_since_t>(
                  node.interval(), lchild, rchild, _allocator);
      insert_new_nested_node(&node, new_node);
      break;
    }
    }
    return;
  }

  case TRANSLATING_TOPLEVEL: {
    if (_parsed_node_to_toplevel_id.find(&node) !=
        _parsed_node_to_toplevel_id.end()) {
      return;
    }

    switch (_subformula_to_placement_map.at(&node)) {
    case mitl2gta::mitl::subformula_placement_t::NESTED: {
      return;
    }
    default: {
      if (node.interval().lower_bound() == mitl2gta::ZERO_BOUND_CLOSED) {
        mitl2gta::compilation::node_id_t const rchild =
            _parsed_node_to_nested_id.at(node.right_subformula().get());
        std::shared_ptr<
            mitl2gta::compilation::compiled_toplevel_node_value_t const>
            new_node = std::make_shared<
                mitl2gta::compilation::compiled_toplevel_node_value_t>(
                rchild, _allocator);
        insert_new_toplevel_node(&node, new_node);
        return;
      }
      std::shared_ptr<mitl2gta::compilation::compiled_atomic_constant_t const>
          new_node = std::make_shared<
              mitl2gta::compilation::compiled_atomic_constant_t>(
              mitl2gta::mitl::constant_value_t::BOOL_FALSE, _allocator);
      insert_new_toplevel_node(&node, new_node);
    }
      return;
    }
  }
  }
  throw std::runtime_error("Incomplete switch statement");
}

} // namespace compilation

} // namespace mitl2gta
