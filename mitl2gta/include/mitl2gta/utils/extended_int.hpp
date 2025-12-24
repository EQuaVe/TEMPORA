/*
 * This file is a part of MITL2GTA project.
 *
 * See AUTHORS and LICENSE for copyright details.
 *
 */

#include <limits>
#include <ostream>

namespace mitl2gta {

using extended_integer_t = int;

mitl2gta::extended_integer_t const EXTENDED_INF =
    std::numeric_limits<extended_integer_t>::max();

mitl2gta::extended_integer_t const EXTENDED_MINUS_INF =
    std::numeric_limits<extended_integer_t>::min();

mitl2gta::extended_integer_t negate(mitl2gta::extended_integer_t const x);

std::ostream &output_extended_int(std::ostream &os,
                                  mitl2gta::extended_integer_t const x);

std::string extended_int_string(mitl2gta::extended_integer_t const x);

} // namespace mitl2gta
