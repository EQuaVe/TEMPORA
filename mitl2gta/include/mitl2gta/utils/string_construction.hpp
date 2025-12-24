/*
 * This file is a part of MITL2GTA project.
 *
 * See AUTHORS and LICENSE for copyright details.
 *
 */

#pragma once

#include <map>
#include <string>
#include <vector>

namespace mitl2gta {

template <class It>
std::string join(It begin, It end, std::string const &delimiter) {
  bool prepend_delim = false;
  std::string result;

  for (It it = begin; it != end; it++) {
    if (prepend_delim) {
      result += delimiter;
    }
    result += *it;
    prepend_delim = true;
  }

  return result;
}

std::vector<std::string> filter_nonempty(std::vector<std::string> const &arr);

namespace gta {

using attributes_t = std::map<std::string, std::string>;

std::string attributes_to_string(mitl2gta::gta::attributes_t const &attributes);

} // namespace gta

} // namespace mitl2gta
