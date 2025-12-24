/*
 * This file is a part of the TChecker project.
 *
 * See files AUTHORS and LICENSE for copyright details.
 *
 */

#include "tchecker/utils/alternating_list.hh"

using test_alternate_list_t = tchecker::alternating_list_t<int, double>;

TEST_CASE("Correct usage of alternating list", "[alternating_list]")
{
  test_alternate_list_t test_list;
  int e1 = 10;
  double e2 = 10.10;
  int e3 = 20;
  double e4 = 20.20;
  SECTION("Adding elements alternatively to list")
  {
    REQUIRE_NOTHROW(test_list.append_t1(e1));
    REQUIRE_NOTHROW(test_list.append_t2(e2));
    REQUIRE_NOTHROW(test_list.append_t1(e3));
    REQUIRE_NOTHROW(test_list.append_t2(e4));
  }
}

TEST_CASE("Adding elements in wrong order", "[alternating_list]")
{
  test_alternate_list_t test_list;
  int e1 = 10;
  double e2 = 10.10;
  double err_e3 = 20.20;
  SECTION("Adding last element in incorrect order")
  {
    test_list.append_t1(e1);
    test_list.append_t2(e2);
    REQUIRE_THROWS_AS(test_list.append_t2(err_e3), std::runtime_error);
  }
}

TEST_CASE("Forward traversal of alternating list", "[alternating_list]")
{
  test_alternate_list_t test_list;
  int e1 = 10;
  double e2 = 10.10;
  int e3 = 20;
  double e4 = 20.20;

  test_list.append_t1(e1);
  test_list.append_t2(e2);
  test_list.append_t1(e3);
  test_list.append_t2(e4);

  std::shared_ptr<test_alternate_list_t::node_t1_t> node_t1 = test_list.get_head();
  std::shared_ptr<test_alternate_list_t::node_t2_t> node_t2 = node_t1->get_next();

  REQUIRE(node_t1->get_value() == e1);
  REQUIRE(node_t2->get_value() == e2);

  node_t1 = node_t2->get_next();
  node_t2 = node_t1->get_next();

  REQUIRE(node_t1->get_value() == e3);
  REQUIRE(node_t2->get_value() == e4);
}

TEST_CASE("Reverse traversal of alternating list", "[alternating_list]")
{
  test_alternate_list_t test_list;
  int e1 = 10;
  double e2 = 10.10;
  int e3 = 20;
  double e4 = 20.20;

  test_list.append_t1(e1);
  test_list.append_t2(e2);
  test_list.append_t1(e3);
  test_list.append_t2(e4);

  auto && [node_t1, node_t2] = test_list.get_tail();
  REQUIRE(node_t2 != nullptr);
  REQUIRE(node_t1 == nullptr);

  node_t1 = node_t2->get_prev();

  REQUIRE(node_t2->get_value() == e4);
  REQUIRE(node_t1->get_value() == e3);

  node_t2 = node_t1->get_prev();
  node_t1 = node_t2->get_prev();

  REQUIRE(node_t2->get_value() == e2);
  REQUIRE(node_t1->get_value() == e1);
}
