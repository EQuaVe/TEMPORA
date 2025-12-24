/*
 * This file is a part of MITL2GTA project.
 *
 * See AUTHORS and LICENSE for copyright details.
 *
 */

#include "mitl2gta/utils/extended_int.hpp"
#include <sstream>

namespace mitl2gta {

mitl2gta::extended_integer_t negate(mitl2gta::extended_integer_t const x) {
  if (x == mitl2gta::EXTENDED_INF) {
    return mitl2gta::EXTENDED_MINUS_INF;
  }

  else if (x == mitl2gta::EXTENDED_MINUS_INF) {
    return mitl2gta::EXTENDED_INF;
  }

  return -x;
}
std::ostream &output_extended_int(std::ostream &os,
                                  mitl2gta::extended_integer_t const x) {

  if (x == mitl2gta::EXTENDED_INF) {
    return os << "inf";
  }

  else if (x == mitl2gta::EXTENDED_MINUS_INF) {
    return os << "-inf";
  }

  return os << x;
}

std::string extended_int_string(mitl2gta::extended_integer_t const x) {
  std::stringstream os;
  mitl2gta::output_extended_int(os, x);
  return os.str();
}

} // namespace mitl2gta
