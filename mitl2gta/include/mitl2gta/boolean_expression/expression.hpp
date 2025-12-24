/*
 * This file is a part of MITL2GTA project.
 *
 * See AUTHORS and LICENSE for copyright details.
 *
 */

#pragma once

#include <memory>
#include <variant>

#include "mitl2gta/memory/memory_handler.hpp"
#include "mitl2gta/mitl_formula/parsed_formula.hpp"

namespace mitl2gta {

namespace boolean {

enum truth_val_t {
  UNCERTAIN = -1,
  FALSE = 0,
  TRUE = 1,
};

struct constant_t;
struct variable_t;
struct negation_t;
struct and_t;
struct or_t;

using expression_t =
    std::variant<constant_t, variable_t, negation_t, and_t, or_t>;

struct constant_t {
  mitl2gta::mitl::constant_value_t val;
};

struct variable_t {
  mitl2gta::memory::memory_id_t mem_id;
};

struct negation_t {
  std::unique_ptr<expression_t> child;
};

struct and_t {
  std::unique_ptr<expression_t> lchild;
  std::unique_ptr<expression_t> rchild;
};

struct or_t {
  std::unique_ptr<expression_t> lchild;
  std::unique_ptr<expression_t> rchild;
};

} // namespace boolean

} // namespace mitl2gta
