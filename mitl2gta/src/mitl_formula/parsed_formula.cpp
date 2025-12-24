/*
 * This file is a part of MITL2GTA project.
 *
 * See AUTHORS and LICENSE for copyright details.
 *
 */

#include <sstream>
#include <string>

#include "mitl2gta/mitl_formula/parsed_formula.hpp"

namespace mitl2gta {

namespace mitl {

std::ostream &operator<<(std::ostream &os, enum constant_value_t const value) {
  switch (value) {
  case mitl2gta::mitl::constant_value_t::BOOL_TRUE:
    os << "True";
    break;
  case mitl2gta::mitl::constant_value_t::BOOL_FALSE:
    os << "False";
    break;
  default:
    throw std::runtime_error("Incomplete switch statement");
  }

  return os;
}

// formula_t
std::string formula_t::to_string() const {
  std::ostringstream stream;
  output(stream);
  return stream.str();
}

// atomic_proposition_t
atomic_proposition_t::atomic_proposition_t(std::string const &name)
    : _name(name) {}

void atomic_proposition_t::visit(
    mitl2gta::mitl::formula_visitor_t &visitor) const {
  visitor.visit(*this);
}

std::ostream &atomic_proposition_t::output(std::ostream &os) const {
  return os << _name;
}

// atomic_constant_t
atomic_constant_t::atomic_constant_t(
    enum mitl2gta::mitl::constant_value_t const value)
    : _value(value) {}

void atomic_constant_t::visit(
    mitl2gta::mitl::formula_visitor_t &visitor) const {
  visitor.visit(*this);
}

std::ostream &atomic_constant_t::output(std::ostream &os) const {
  return os << _value;
}

// unary_operator_t
unary_operator_t::unary_operator_t(
    std::shared_ptr<mitl2gta::mitl::formula_t const> const subformula)
    : _subformula(subformula) {
  if (subformula.get() == nullptr) {
    throw std::invalid_argument("Null pointer passed for subformula");
  }
}

// negation_t
negation_t::negation_t(
    std::shared_ptr<mitl2gta::mitl::formula_t const> const subformula)
    : mitl2gta::mitl::unary_operator_t(subformula) {}

std::ostream &negation_t::output(std::ostream &os) const {
  os << "(!" << *_subformula << ")";
  return os;
}

void negation_t::visit(mitl2gta::mitl::formula_visitor_t &visitor) const {
  visitor.visit(*this);
}

// binary_operator_t
binary_operator_t::binary_operator_t(
    std::shared_ptr<mitl2gta::mitl::formula_t const> const left_subformula,
    std::shared_ptr<mitl2gta::mitl::formula_t const> const right_subformula)
    : _left_subformula(left_subformula), _right_subformula(right_subformula) {
  if (left_subformula.get() == nullptr) {
    throw std::invalid_argument("Null pointer passed for left subformula");
  }
  if (right_subformula.get() == nullptr) {
    throw std::invalid_argument("Null pointer passed for right subformula");
  }
};

// or_t
or_t::or_t(
    std::shared_ptr<mitl2gta::mitl::formula_t const> const left_subformula,
    std::shared_ptr<mitl2gta::mitl::formula_t const> const right_subformula)
    : binary_operator_t(left_subformula, right_subformula){};

std::ostream &or_t::output(std::ostream &os) const {
  os << "(" << *_left_subformula << "||" << *_right_subformula << ")";
  return os;
}

void or_t::visit(mitl2gta::mitl::formula_visitor_t &visitor) const {
  visitor.visit(*this);
}

// and_t
and_t::and_t(
    std::shared_ptr<mitl2gta::mitl::formula_t const> const left_subformula,
    std::shared_ptr<mitl2gta::mitl::formula_t const> const right_subformula)
    : binary_operator_t(left_subformula, right_subformula){};

std::ostream &and_t::output(std::ostream &os) const {
  os << "(" << *_left_subformula << "&&" << *_right_subformula << ")";
  return os;
}

void and_t::visit(mitl2gta::mitl::formula_visitor_t &visitor) const {
  visitor.visit(*this);
}

// timed_temporal_operator_t
timed_temporal_operator_t::timed_temporal_operator_t(
    mitl2gta::nonempty_interval_t const &interval)
    : _interval(interval) {}

// untimed_next_t
untimed_next_t::untimed_next_t(
    std::shared_ptr<mitl2gta::mitl::formula_t const> const subformula)
    : mitl2gta::mitl::unary_operator_t(subformula) {}

void untimed_next_t::visit(mitl2gta::mitl::formula_visitor_t &visitor) const {
  visitor.visit(*this);
}

std::ostream &untimed_next_t::output(std::ostream &os) const {
  os << "(X " << *_subformula << ")";
  return os;
};

// timed_next_t
timed_next_t::timed_next_t(
    mitl2gta::nonempty_interval_t const &interval,
    std::shared_ptr<mitl2gta::mitl::formula_t const> const subformula)
    : mitl2gta::mitl::timed_temporal_operator_t(interval),
      mitl2gta::mitl::unary_operator_t(subformula) {}

void timed_next_t::visit(mitl2gta::mitl::formula_visitor_t &visitor) const {
  visitor.visit(*this);
}

std::ostream &timed_next_t::output(std::ostream &os) const {
  os << "(X" << _interval << " " << *_subformula << ")";
  return os;
};

// untimed_yesterday_t
untimed_yesterday_t::untimed_yesterday_t(
    std::shared_ptr<mitl2gta::mitl::formula_t const> const subformula)
    : mitl2gta::mitl::unary_operator_t(subformula) {}

void untimed_yesterday_t::visit(
    mitl2gta::mitl::formula_visitor_t &visitor) const {
  visitor.visit(*this);
}

std::ostream &untimed_yesterday_t::output(std::ostream &os) const {
  os << "(Y " << *_subformula << ")";
  return os;
};

// timed_yesterday_t
timed_yesterday_t::timed_yesterday_t(
    mitl2gta::nonempty_interval_t const &interval,
    std::shared_ptr<mitl2gta::mitl::formula_t const> const subformula)
    : mitl2gta::mitl::timed_temporal_operator_t(interval),
      mitl2gta::mitl::unary_operator_t(subformula) {}

void timed_yesterday_t::visit(
    mitl2gta::mitl::formula_visitor_t &visitor) const {
  visitor.visit(*this);
}

std::ostream &timed_yesterday_t::output(std::ostream &os) const {
  os << "(Y" << _interval << " " << *_subformula << ")";
  return os;
};

// untimed_finally_t
untimed_finally_t::untimed_finally_t(
    std::shared_ptr<mitl2gta::mitl::formula_t const> const subformula)
    : mitl2gta::mitl::unary_operator_t(subformula) {}

void untimed_finally_t::visit(
    mitl2gta::mitl::formula_visitor_t &visitor) const {
  visitor.visit(*this);
}

std::ostream &untimed_finally_t::output(std::ostream &os) const {
  os << "(F " << *_subformula << ")";
  return os;
};

// timed_finally_t
timed_finally_t::timed_finally_t(
    mitl2gta::nonempty_interval_t const &interval,
    std::shared_ptr<mitl2gta::mitl::formula_t const> const subformula)
    : mitl2gta::mitl::timed_temporal_operator_t(interval),
      mitl2gta::mitl::unary_operator_t(subformula) {}

void timed_finally_t::visit(mitl2gta::mitl::formula_visitor_t &visitor) const {
  visitor.visit(*this);
}

std::ostream &timed_finally_t::output(std::ostream &os) const {
  os << "(F" << _interval << " " << *_subformula << ")";
  return os;
};

// untimed_globally_t
untimed_globally_t::untimed_globally_t(
    std::shared_ptr<mitl2gta::mitl::formula_t const> const subformula)
    : mitl2gta::mitl::unary_operator_t(subformula) {}

void untimed_globally_t::visit(
    mitl2gta::mitl::formula_visitor_t &visitor) const {
  visitor.visit(*this);
}

std::ostream &untimed_globally_t::output(std::ostream &os) const {
  os << "(G " << *_subformula << ")";
  return os;
};

// timed_globally_t
timed_globally_t::timed_globally_t(
    mitl2gta::nonempty_interval_t const &interval,
    std::shared_ptr<mitl2gta::mitl::formula_t const> const subformula)
    : mitl2gta::mitl::timed_temporal_operator_t(interval),
      mitl2gta::mitl::unary_operator_t(subformula) {}

void timed_globally_t::visit(mitl2gta::mitl::formula_visitor_t &visitor) const {
  visitor.visit(*this);
}

std::ostream &timed_globally_t::output(std::ostream &os) const {
  os << "(G" << _interval << " " << *_subformula << ")";
  return os;
};

// untimed_until_t
untimed_until_t::untimed_until_t(
    std::shared_ptr<mitl2gta::mitl::formula_t const> const left_subformula,
    std::shared_ptr<mitl2gta::mitl::formula_t const> const right_subformula)
    : mitl2gta::mitl::binary_operator_t(left_subformula, right_subformula) {}

void untimed_until_t::visit(mitl2gta::mitl::formula_visitor_t &visitor) const {
  visitor.visit(*this);
}

std::ostream &untimed_until_t::output(std::ostream &os) const {
  os << "(" << *_left_subformula << " U " << *_right_subformula << ")";
  return os;
};

// timed_until_t
timed_until_t::timed_until_t(
    mitl2gta::nonempty_interval_t const &interval,
    std::shared_ptr<mitl2gta::mitl::formula_t const> const left_subformula,
    std::shared_ptr<mitl2gta::mitl::formula_t const> const right_subformula)
    : mitl2gta::mitl::timed_temporal_operator_t(interval),
      mitl2gta::mitl::binary_operator_t(left_subformula, right_subformula) {}

void timed_until_t::visit(mitl2gta::mitl::formula_visitor_t &visitor) const {
  visitor.visit(*this);
}

std::ostream &timed_until_t::output(std::ostream &os) const {
  os << "(" << *_left_subformula << " U" << _interval << " "
     << *_right_subformula << ")";
  return os;
};

// untimed_since_t
untimed_since_t::untimed_since_t(
    std::shared_ptr<mitl2gta::mitl::formula_t const> const left_subformula,
    std::shared_ptr<mitl2gta::mitl::formula_t const> const right_subformula)
    : mitl2gta::mitl::binary_operator_t(left_subformula, right_subformula) {}

void untimed_since_t::visit(mitl2gta::mitl::formula_visitor_t &visitor) const {
  visitor.visit(*this);
}

std::ostream &untimed_since_t::output(std::ostream &os) const {
  os << "(" << *_left_subformula << " S " << *_right_subformula << ")";
  return os;
};

// timed_since_t
timed_since_t::timed_since_t(
    mitl2gta::nonempty_interval_t const &interval,
    std::shared_ptr<mitl2gta::mitl::formula_t const> const left_subformula,
    std::shared_ptr<mitl2gta::mitl::formula_t const> const right_subformula)
    : mitl2gta::mitl::timed_temporal_operator_t(interval),
      mitl2gta::mitl::binary_operator_t(left_subformula, right_subformula) {}

void timed_since_t::visit(mitl2gta::mitl::formula_visitor_t &visitor) const {
  visitor.visit(*this);
}

std::ostream &timed_since_t::output(std::ostream &os) const {
  os << "(" << *_left_subformula << " S" << _interval << " "
     << *_right_subformula << ")";
  return os;
};

} // namespace mitl

} // namespace mitl2gta
