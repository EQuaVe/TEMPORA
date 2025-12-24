/*
 * This file is a part of MITL2GTA project.
 *
 * See AUTHORS and LICENSE for copyright details.
 *
 */

#include <filesystem>
#include <optional>

class io_handler_t {
public:
  io_handler_t(std::optional<std::filesystem::path> const formula_input,
               std::optional<std::filesystem::path> const gta_output);

  std::string read_formula() const;

  void write_system(std::string const &system) const;

private:
  std::optional<std::filesystem::path> const _formula_input;

  std::optional<std::filesystem::path> const _gta_output;
};
