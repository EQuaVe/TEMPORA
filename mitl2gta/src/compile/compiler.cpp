/*
 * This file is a part of MITL2GTA project.
 *
 * See AUTHORS and LICENSE for copyright details.
 *
 */

#include <algorithm>
#include <cassert>
#include <map>
#include <stack>
#include <variant>

#include "mitl2gta/compile/compiled_node.hpp"
#include "mitl2gta/compile/compiler.hpp"
#include "mitl2gta/compile/master.hpp"
#include "mitl2gta/gta_system/variable.hpp"
#include "mitl2gta/memory/memory_handler.hpp"
#include "mitl2gta/sharer_automaton/translation.hpp"
#include "mitl2gta/transducer/translation.hpp"

namespace mitl2gta {

namespace compilation {

namespace {

class node_compiler_visitor_t final
    : virtual mitl2gta::mitl::formula_visitor_t {

public:
  node_compiler_visitor_t(
      mitl2gta::mitl::formula_t const &root,
      mitl2gta::compilation::compiled_node_registry_t &node_registry)
      : _node_registry(node_registry) {
    root.visit(*this);
    _root_id = _parsed_node_to_id.at(&root);
  }

  node_compiler_visitor_t(node_compiler_visitor_t const &) = delete;
  node_compiler_visitor_t(node_compiler_visitor_t &&) = default;
  node_compiler_visitor_t &operator=(node_compiler_visitor_t const &) = delete;
  node_compiler_visitor_t &operator=(node_compiler_visitor_t &&) = delete;

  ~node_compiler_visitor_t() = default;

  mitl2gta::compilation::node_id_t root_id() const { return _root_id; }

private:
  virtual void visit(mitl2gta::mitl::atomic_proposition_t const &node) {
    if (_parsed_node_to_id.find(&node) != _parsed_node_to_id.end()) {
      return;
    }

    auto const &iter = _atomic_propositions_to_id.find(node.name());
    if (iter != _atomic_propositions_to_id.end()) {
      mitl2gta::compilation::node_id_t const id = iter->second;
      _parsed_node_to_id.insert({&node, id});
      return;
    }

    mitl2gta::compilation::node_id_t const id =
        _node_registry.request_atomic_proposition();

    _atomic_propositions_to_id.insert({node.name(), id});
    _parsed_node_to_id.insert({&node, id});
  }

  virtual void visit(mitl2gta::mitl::atomic_constant_t const &node) {
    if (_parsed_node_to_id.find(&node) != _parsed_node_to_id.end()) {
      return;
    }

    mitl2gta::compilation::node_id_t const id =
        _node_registry.request_atomic_constant(node.value());

    _parsed_node_to_id.insert({&node, id});
  }

  virtual void visit(mitl2gta::mitl::negation_t const &node) {
    if (_parsed_node_to_id.find(&node) != _parsed_node_to_id.end()) {
      return;
    }

    node.subformula()->visit(*this);

    mitl2gta::compilation::node_id_t const child_id =
        _parsed_node_to_id.at(node.subformula().get());

    mitl2gta::compilation::node_id_t const id =
        _node_registry.request_negation(child_id);

    _parsed_node_to_id.insert({&node, id});
  }

  virtual void visit(mitl2gta::mitl::or_t const &node) {
    if (_parsed_node_to_id.find(&node) != _parsed_node_to_id.end()) {
      return;
    }
    node.left_subformula()->visit(*this);
    mitl2gta::compilation::node_id_t const lchild_id =
        _parsed_node_to_id.at(node.left_subformula().get());

    node.right_subformula()->visit(*this);
    mitl2gta::compilation::node_id_t const rchild_id =
        _parsed_node_to_id.at(node.right_subformula().get());

    mitl2gta::compilation::node_id_t const id =
        _node_registry.request_or(lchild_id, rchild_id);

    _parsed_node_to_id.insert({&node, id});
  }

  virtual void visit(mitl2gta::mitl::and_t const &node) {
    if (_parsed_node_to_id.find(&node) != _parsed_node_to_id.end()) {
      return;
    }
    node.left_subformula()->visit(*this);
    mitl2gta::compilation::node_id_t const lchild_id =
        _parsed_node_to_id.at(node.left_subformula().get());

    node.right_subformula()->visit(*this);
    mitl2gta::compilation::node_id_t const rchild_id =
        _parsed_node_to_id.at(node.right_subformula().get());

    mitl2gta::compilation::node_id_t const id =
        _node_registry.request_and(lchild_id, rchild_id);

    _parsed_node_to_id.insert({&node, id});
  }

  virtual void visit(mitl2gta::mitl::untimed_next_t const &node) {
    if (_parsed_node_to_id.find(&node) != _parsed_node_to_id.end()) {
      return;
    }

    node.subformula()->visit(*this);

    mitl2gta::compilation::node_id_t const child_id =
        _parsed_node_to_id.at(node.subformula().get());

    mitl2gta::compilation::node_id_t const id =
        _node_registry.request_untimed_next(child_id);

    _parsed_node_to_id.insert({&node, id});
  }

  virtual void visit(mitl2gta::mitl::timed_next_t const &node) {
    if (_parsed_node_to_id.find(&node) != _parsed_node_to_id.end()) {
      return;
    }

    node.subformula()->visit(*this);
    mitl2gta::compilation::node_id_t const child_id =
        _parsed_node_to_id.at(node.subformula().get());

    mitl2gta::compilation::node_id_t const id =
        _node_registry.request_timed_next(node.interval(), child_id);

    _parsed_node_to_id.insert({&node, id});
  }

  virtual void visit(mitl2gta::mitl::untimed_yesterday_t const &node) {
    if (_parsed_node_to_id.find(&node) != _parsed_node_to_id.end()) {
      return;
    }

    node.subformula()->visit(*this);

    mitl2gta::compilation::node_id_t const child_id =
        _parsed_node_to_id.at(node.subformula().get());

    mitl2gta::compilation::node_id_t const id =
        _node_registry.request_untimed_yesterday(child_id);

    _parsed_node_to_id.insert({&node, id});
  }

  virtual void visit(mitl2gta::mitl::timed_yesterday_t const &node) {
    if (_parsed_node_to_id.find(&node) != _parsed_node_to_id.end()) {
      return;
    }

    node.subformula()->visit(*this);
    mitl2gta::compilation::node_id_t const child_id =
        _parsed_node_to_id.at(node.subformula().get());

    mitl2gta::compilation::node_id_t const id =
        _node_registry.request_timed_yesterday(node.interval(), child_id);

    _parsed_node_to_id.insert({&node, id});
  }

  virtual void visit(mitl2gta::mitl::untimed_finally_t const &node) {
    if (_parsed_node_to_id.find(&node) != _parsed_node_to_id.end()) {
      return;
    }

    node.subformula()->visit(*this);

    mitl2gta::compilation::node_id_t const child_id =
        _parsed_node_to_id.at(node.subformula().get());

    mitl2gta::compilation::node_id_t const id =
        _node_registry.request_untimed_finally(child_id);

    _parsed_node_to_id.insert({&node, id});
  }

  virtual void visit(mitl2gta::mitl::timed_finally_t const &node) {
    if (_parsed_node_to_id.find(&node) != _parsed_node_to_id.end()) {
      return;
    }

    node.subformula()->visit(*this);
    mitl2gta::compilation::node_id_t const child_id =
        _parsed_node_to_id.at(node.subformula().get());

    mitl2gta::compilation::node_id_t const id =
        _node_registry.request_timed_finally(node.interval(), child_id);

    _parsed_node_to_id.insert({&node, id});
  }

  virtual void visit(mitl2gta::mitl::untimed_globally_t const &node) {
    if (_parsed_node_to_id.find(&node) != _parsed_node_to_id.end()) {
      return;
    }

    node.subformula()->visit(*this);

    mitl2gta::compilation::node_id_t const child_id =
        _parsed_node_to_id.at(node.subformula().get());

    mitl2gta::compilation::node_id_t const id =
        _node_registry.request_untimed_globally(child_id);

    _parsed_node_to_id.insert({&node, id});
  }

  virtual void visit(mitl2gta::mitl::timed_globally_t const &node) {
    if (_parsed_node_to_id.find(&node) != _parsed_node_to_id.end()) {
      return;
    }

    node.subformula()->visit(*this);
    mitl2gta::compilation::node_id_t const child_id =
        _parsed_node_to_id.at(node.subformula().get());

    mitl2gta::compilation::node_id_t const id =
        _node_registry.request_timed_globally(node.interval(), child_id);

    _parsed_node_to_id.insert({&node, id});
  }

  virtual void visit(mitl2gta::mitl::untimed_until_t const &node) {
    if (_parsed_node_to_id.find(&node) != _parsed_node_to_id.end()) {
      return;
    }

    node.left_subformula()->visit(*this);
    mitl2gta::compilation::node_id_t const lchild_id =
        _parsed_node_to_id.at(node.left_subformula().get());

    node.right_subformula()->visit(*this);
    mitl2gta::compilation::node_id_t const rchild_id =
        _parsed_node_to_id.at(node.right_subformula().get());

    mitl2gta::compilation::node_id_t const id =
        _node_registry.request_untimed_until(lchild_id, rchild_id);

    _parsed_node_to_id.insert({&node, id});
  }

  virtual void visit(mitl2gta::mitl::timed_until_t const &node) {
    if (_parsed_node_to_id.find(&node) != _parsed_node_to_id.end()) {
      return;
    }

    node.left_subformula()->visit(*this);
    mitl2gta::compilation::node_id_t const lchild_id =
        _parsed_node_to_id.at(node.left_subformula().get());

    node.right_subformula()->visit(*this);
    mitl2gta::compilation::node_id_t const rchild_id =
        _parsed_node_to_id.at(node.right_subformula().get());

    mitl2gta::compilation::node_id_t const id =
        _node_registry.request_timed_until(node.interval(), lchild_id,
                                           rchild_id);

    _parsed_node_to_id.insert({&node, id});
  }

  virtual void visit(mitl2gta::mitl::untimed_since_t const &node) {
    if (_parsed_node_to_id.find(&node) != _parsed_node_to_id.end()) {
      return;
    }

    node.left_subformula()->visit(*this);
    mitl2gta::compilation::node_id_t const lchild_id =
        _parsed_node_to_id.at(node.left_subformula().get());

    node.right_subformula()->visit(*this);
    mitl2gta::compilation::node_id_t const rchild_id =
        _parsed_node_to_id.at(node.right_subformula().get());

    mitl2gta::compilation::node_id_t const id =
        _node_registry.request_untimed_since(lchild_id, rchild_id);

    _parsed_node_to_id.insert({&node, id});
  }

  virtual void visit(mitl2gta::mitl::timed_since_t const &node) {
    if (_parsed_node_to_id.find(&node) != _parsed_node_to_id.end()) {
      return;
    }

    node.left_subformula()->visit(*this);
    mitl2gta::compilation::node_id_t const lchild_id =
        _parsed_node_to_id.at(node.left_subformula().get());

    node.right_subformula()->visit(*this);
    mitl2gta::compilation::node_id_t const rchild_id =
        _parsed_node_to_id.at(node.right_subformula().get());

    mitl2gta::compilation::node_id_t const id =
        _node_registry.request_timed_since(node.interval(), lchild_id,
                                           rchild_id);

    _parsed_node_to_id.insert({&node, id});
  }

  mitl2gta::compilation::compiled_node_registry_t &_node_registry;

  std::map<mitl2gta::mitl::formula_t const *, mitl2gta::compilation::node_id_t>
      _parsed_node_to_id;

  std::map<std::string, mitl2gta::compilation::node_id_t>
      _atomic_propositions_to_id;

  mitl2gta::compilation::node_id_t _root_id;
};

} // namespace

compiler_t::compiler_t(
    std::shared_ptr<mitl2gta::mitl::formula_t const> const &root_ptr,
    mitl2gta::compilation::verification_mode_t const verification_mode)
    : _verification_mode(verification_mode) {
  node_compiler_visitor_t const node_compiler_visitor(*root_ptr,
                                                      _nodes_registry);

  _root_id = node_compiler_visitor.root_id();

  if (verification_mode ==
      mitl2gta::compilation::verification_mode_t::MODEL_CHECKING) {
    _root_id = _nodes_registry.request_negation(_root_id);
  }
}

mitl2gta::gta::system_t compiler_t::translate_to_gta_system(
    mitl2gta::sharer::sharer_mode_t const mode,
    mitl2gta::sharer::p_until_q_sharer_states_t const p_until_q_sharer_states,
    mitl2gta::compilation::translation_specs_t const &specs) const {
  mitl2gta::compilation::node_to_truth_value_predictor_t id_to_truth_predictor;

  mitl2gta::transducer::system_t system(specs.system_for);

  translate_to_transducer_system(system, mode, p_until_q_sharer_states, specs,
                                 id_to_truth_predictor);

  mitl2gta::compilation::placeholder_memory_map_t placeholder_memory =
      allocate_placeholder_memory(id_to_truth_predictor,
                                  system.memory_handler());

  mitl2gta::memory::memory_mapping_t mapping(
      specs.mapping_mode, system, id_to_truth_predictor, placeholder_memory);

  // TODO: Print memories of atomic propositions
  // if (_verification_mode ==
  //     mitl2gta::compilation::verification_mode_t::MODEL_CHECKING) {
  //   for (auto const &[ap, id] : _atomic_propositions_to_id) {
  //     mitl2gta::memory::memory_id_t const mem_id = placeholder_memory.at(id);
  //     std::string const mem_name = mapping.mapped_var(mem_id);
  //     std::cout << ap << ":" << mem_name << std::endl;
  //   }
  // }

  mitl2gta::transducer::synchronized_gta_system_t translator(
      system, id_to_truth_predictor, placeholder_memory, mapping,
      _verification_mode);

  return translator.gta_system();
}

void compiler_t::translate_to_transducer_system(
    mitl2gta::transducer::system_t &system,
    mitl2gta::sharer::sharer_mode_t const mode,
    mitl2gta::sharer::p_until_q_sharer_states_t const p_until_q_sharer_states,
    mitl2gta::compilation::translation_specs_t const &specs,
    mitl2gta::compilation::node_to_truth_value_predictor_t &mapping_to_fill)
    const {
  mitl2gta::sharer::sharer_registry_t sharer_registry(mode);

  mitl2gta::transducer::transducer_construction_tools_t tools{
      system.state_id_allocator_t(), system.memory_handler(),
      system.clock_registry(), sharer_registry};

  std::vector<mitl2gta::compilation::node_id_t> topological_ordering =
      topological_sort();

  for (auto it = topological_ordering.rbegin();
       it != topological_ordering.rend(); ++it) {
    mapping_to_fill.insert(
        {*it, _nodes_registry.node(*it)->generate_truth_value_predictor(
                  tools, specs)});
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

  system.add_transducer(
      mitl2gta::compilation::construct_master(_root_id, tools));
}

mitl2gta::compilation::placeholder_memory_map_t
compiler_t::allocate_placeholder_memory(
    mitl2gta::compilation::node_to_truth_value_predictor_t &mapping,
    mitl2gta::memory::memory_handler_t &handler) const {
  mitl2gta::compilation::placeholder_memory_map_t res;

  mitl2gta::boundval_t lower = std::min(mitl2gta::boolean::truth_val_t::FALSE,
                                        mitl2gta::boolean::truth_val_t::TRUE);
  mitl2gta::boundval_t upper = std::max(mitl2gta::boolean::truth_val_t::FALSE,
                                        mitl2gta::boolean::truth_val_t::TRUE);

  mitl2gta::gta::integer_variable_range_t range(lower, upper);
  mitl2gta::memory::memory_requirements_t transducer_req{
      range, lower, mitl2gta::memory::memory_type_t::LOCAL_TO_SINGLE_ROUND};

  mitl2gta::memory::memory_requirements_t ap_req{
      range, lower, mitl2gta::memory::memory_type_t::PERSISTENT};

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

std::vector<mitl2gta::compilation::node_id_t>
compiler_t::topological_sort() const {
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

    for (auto const &succ :
         _nodes_registry.node(curr_id)->direct_dependencies()) {
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

} // namespace compilation

} // namespace mitl2gta
