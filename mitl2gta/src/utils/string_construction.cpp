/*
 * This file is a part of MITL2GTA project.
 *
 * See AUTHORS and LICENSE for copyright details.
 *
 */

#include "mitl2gta/utils/string_construction.hpp"
#include <vector>

namespace mitl2gta {

namespace gta {

std::string
attributes_to_string(mitl2gta::gta::attributes_t const &attributes) {
  std::vector<std::string> key_value_pairs;

  for (auto const &[key, value] : attributes) {
    key_value_pairs.emplace_back(key + ":" + value);
  }

  std::string joined_key_value_pairs =
      mitl2gta::join(key_value_pairs.begin(), key_value_pairs.end(), " : ");

  return "{" + joined_key_value_pairs + "}";
}

} // namespace gta

std::vector<std::string> filter_nonempty(std::vector<std::string> const &arr) {
  std::vector<std::string> res;
  for (auto const &s : arr) {
    if (s.empty()) {
      continue;
    }
    res.emplace_back(s);
  }
  return res;
}

} // namespace mitl2gta
