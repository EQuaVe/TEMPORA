/*
 * This file is a part of MITL2GTA project.
 *
 * See AUTHORS and LICENSE for copyright details.
 *
 */

#include <set>
#include <variant>

#include "mitl2gta/boolean_expression/expression.hpp"
#include "mitl2gta/memory/memory_handler.hpp"

namespace mitl2gta {

namespace boolean {

std::set<mitl2gta::memory::memory_id_t>
ids_in_expr(mitl2gta::boolean::expression_t const &expr) {
  return std::visit(
      [](auto &&arg) -> std::set<mitl2gta::memory::memory_id_t> {
        using T = std::decay_t<decltype(arg)>;

        if constexpr (std::is_same_v<T, mitl2gta::boolean::constant_t>) {
          return std::set<mitl2gta::memory::memory_id_t>();
        }

        else if constexpr (std::is_same_v<T, mitl2gta::boolean::variable_t>) {
          return std::set<mitl2gta::memory::memory_id_t>{arg.mem_id};
        }

        else if constexpr (std::is_same_v<T, mitl2gta::boolean::negation_t>) {
          return mitl2gta::boolean::ids_in_expr(*(arg.child));
        }

        else if constexpr (std::is_same_v<T, mitl2gta::boolean::or_t>) {
          std::set<mitl2gta::memory::memory_id_t> lchild_ids =
              mitl2gta::boolean::ids_in_expr(*(arg.lchild));

          std::set<mitl2gta::memory::memory_id_t> rchild_ids =
              mitl2gta::boolean::ids_in_expr(*(arg.rchild));

          std::set<mitl2gta::memory::memory_id_t> res = lchild_ids;
          res.insert(rchild_ids.begin(), rchild_ids.end());
          return res;
        }

        else if constexpr (std::is_same_v<T, mitl2gta::boolean::and_t>) {
          std::set<mitl2gta::memory::memory_id_t> lchild_ids =
              mitl2gta::boolean::ids_in_expr(*(arg.lchild));

          std::set<mitl2gta::memory::memory_id_t> rchild_ids =
              mitl2gta::boolean::ids_in_expr(*(arg.rchild));

          std::set<mitl2gta::memory::memory_id_t> res = lchild_ids;
          res.insert(rchild_ids.begin(), rchild_ids.end());
          return res;
        }
      },
      expr);
}

} // namespace boolean
} // namespace mitl2gta
