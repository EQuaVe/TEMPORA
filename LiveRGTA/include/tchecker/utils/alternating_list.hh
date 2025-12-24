/*
 * This file is a part of the TChecker project.
 *
 * See files AUTHORS and LICENSE for copyright details.
 *
 */

#ifndef TCHECKER_ALTERNATING_LIST_HH
#define TCHECKER_ALTERNATING_LIST_HH

#include <cassert>
#include <memory>
#include <stdexcept>
/*!
 \file alternating_list.hh
 \brief Alternating list where the types of elements
 alternate between two types A and B
 */
namespace tchecker {

/*!
  \class alternating_list_node_t
  \param SELF Type for value stored
  \param ALTERNATE Type for element of next/prev node in the alternating list
  \brief Class to represent a single node in the alternating list
 */
template <typename SELF, typename ALTERNATE>
class alternating_list_node_t : public std::enable_shared_from_this<alternating_list_node_t<SELF, ALTERNATE>> {
public:
  /*!
   \brief Type of prev node in list
   */
  using prev_node_t = alternating_list_node_t<ALTERNATE, SELF>;

  /*!
   \brief Type of next node in list
   */
  using next_node_t = alternating_list_node_t<ALTERNATE, SELF>;

  /*!
    \brief Constructor to create an isolated node using
    the value.
    \note The prev and next pointers are initialised to nullptr
  */
  explicit alternating_list_node_t(SELF & value) : _value(value){};

  /*!
   \brief Destructor
  */
  ~alternating_list_node_t() = default;

  /*!
   \brief Copy constructor
  */
  alternating_list_node_t(tchecker::alternating_list_node_t<SELF, ALTERNATE> const &) = default;

  /*!
   \brief Move assignment operator
  */
  alternating_list_node_t & operator=(tchecker::alternating_list_node_t<SELF, ALTERNATE> &&) = default;

  /*!
    \brief Value accessor
  */
  SELF get_value() const { return _value; }

  /*!
    \brief Prev accessor
  */
  std::shared_ptr<next_node_t> get_prev() const { return _prev.lock(); }

  /*!
    \brief Next accessor
  */
  std::shared_ptr<next_node_t> get_next() const { return _next; }

private:
  /*!
    \brief Value setter
    \param value: value stored in node
  */
  void set_value(SELF & value) { _value = value; }

  /*!
    \brief Prev setter
  */
  void set_prev(std::shared_ptr<prev_node_t> & prev) { _prev = prev; }

  /*!
    \brief Next setter
  */
  void set_next(std::shared_ptr<next_node_t> & next) { _next = next; }

  /*
    \brief Append a node
    \param next : Pointer to the node to append
    \pre this does not have a next node, and next does not
    have a previous node
    \throw std::invalid_argument : if either this node
    \note this stores a shared ptr to next and next shares a weak ptr
    to this
  */
  void append(std::shared_ptr<next_node_t> & next)
  {
    if (_next.get() != nullptr) {
      throw std::invalid_argument("Invalid append, this has a next node");
    }
    if (next->get_prev().get() != nullptr) {
      throw std::invalid_argument("Invalid append, next node already has a previous node");
    }

    this->set_next(next);
    std::shared_ptr<alternating_list_node_t<SELF, ALTERNATE>> this_ptr = this->shared_from_this();
    next->set_prev(this_ptr);
  }

  SELF _value;
  std::weak_ptr<prev_node_t> _prev;
  std::shared_ptr<next_node_t> _next;

  template <typename T1, typename T2> friend class alternating_list_node_t;
  template <typename T1, typename T2> friend class alternating_list_t;
};

/*!
  \class alternating_list_t
  \param T1 Type for elements at odd positions in list
  \param T2 Type for elements at even positions in list
  \brief A list where the types of elements alternate between
  two types T1 and T2
 */
template <typename T1, typename T2> class alternating_list_t {
public:
  /*!
   \brief Type to represent nodes storing value of type T1
  */
  using node_t1_t = tchecker::alternating_list_node_t<T1, T2>;

  /*!
   \brief Type to represent nodes storing value of type T2
  */
  using node_t2_t = tchecker::alternating_list_node_t<T2, T1>;

  /*!
   \brief Type to represent the head of the list
  */
  using head_t = std::shared_ptr<node_t1_t>;

  /*!
   \brief Type to represent tail of the list when last element is of type T1
  */
  using tail_as_t1_t = std::shared_ptr<node_t1_t>;

  /*!
   \brief Type to represent tail of the list when last element is of type T2
  */
  using tail_as_t2_t = std::shared_ptr<node_t2_t>;

  /*!
   \brief Constructor for an empty list
  */
  alternating_list_t() {}

  /*!
    \brief Destructor
  */
  ~alternating_list_t() = default;

  /*!
   \brief Copy constructor
  */
  alternating_list_t(tchecker::alternating_list_t<T1, T2> const &) = default;

  /*!
   \brief Move constructor
  */
  alternating_list_t(tchecker::alternating_list_t<T1, T2> &&) = default;

  /*!
   \brief Move assignment operator
  */
  alternating_list_t & operator=(tchecker::alternating_list_t<T1, T2> &&) = default;

  /*!
    \brief Accessor for head of list
  */
  head_t get_head() const { return _head; }

  /*
  \brief Accessor for tail
  \return a pair of pointer to nodes, one of type T1 and other of
  type T2
  \note If the list is empty, both the pointers will be nullptr
  */
  std::pair<std::shared_ptr<node_t1_t>, std::shared_ptr<node_t2_t>> get_tail() const
  {
    return std::make_pair(_tail_as_t1, _tail_as_t2);
  }

  /*
    \brief Appends a value of type T1 to end of list
    \param value : Value to be added to end of list
    \pre List is empty or the last element is of type T2
    \post A node with the value has been added to the list
    \throw std::runtime_error if both of the above conditions
    are violated
  */
  void append_t1(T1 & value)
  {
    enum tail_type_t tail_type = get_tail_type();
    switch (tail_type) {
    case NONE_T: {
      std::shared_ptr<node_t1_t> node = std::make_shared<node_t1_t>(value);
      _head = node;
      _tail_as_t1 = node;
      _tail_as_t2 = nullptr;
      break;
    }
    case TYPE_T2_T: {
      std::shared_ptr<node_t1_t> node = std::make_shared<node_t1_t>(value);
      _tail_as_t2->append(node);
      _tail_as_t1 = node;
      _tail_as_t2 = nullptr;
      break;
    }
    case TYPE_T1_T: {
      throw std::runtime_error("Tail type is T1");
    }
    default: {
      throw std::runtime_error("Tail type not supported");
    }
    }
  };

  /*
    \brief Appends a value of type T2 to end of list
    \param value : value to be added to end of list
    \pre List is non-empty and last element is of type T1
    \post A node with the value has been added to the list
    \throw std::runtime_error if either of the above conditions
    are violated
  */
  void append_t2(T2 & value)
  {
    enum tail_type_t tail_type = get_tail_type();

    switch (tail_type) {
    case TYPE_T1_T: {
      std::shared_ptr<node_t2_t> node = std::make_shared<node_t2_t>(value);
      _tail_as_t1->append(node);
      _tail_as_t2 = node;
      _tail_as_t1 = nullptr;
      break;
    }
    case NONE_T: {
      throw std::runtime_error("Cannot append, list empty");
    }
    case TYPE_T2_T: {
      throw std::runtime_error("Tail type is T2");
    }
    default: {
      throw std::runtime_error("Tail type not supported");
    }
    }
  };

  // Iterator

  /*
   * \brief Type of iterator, as a pair of pointers one to node of type T1
   * and other to node of type T2. We maintain the invariant that at most
   * one of the two pointers in non-null, representing the current node
   * in the iteration
   */
  using iterator_t = std::pair<std::shared_ptr<node_t1_t const>, std::shared_ptr<node_t2_t const>>;

  /*
   * \brief First iterator
   * \return a pair of pointers, with the first being non-null
   * if the list is non-empty, and [nullptr, nullptr] if list is
   * empty
   */
  iterator_t begin() const { return std::make_pair(_head, nullptr); }

  /*
   * \brief Next to end of list iterator
   * \return [nullptr, nullptr]
   */
  constexpr iterator_t end() const { return std::make_pair(nullptr, nullptr); }

  /*
   * \brief Returns the next node as a pair, with the non-null pointer being the next node
   * \param curr : current iterator
   * the current node
   * \pre curr is a valid iterator and curr != end()
   * \return A pair, where one of the nodes is set to the next node of the non-null node in curr and
   * the other node is set to null
   * \throw std::invalid_argument if either none or both the nodes in curr are nullptr
   * \note If there is no next node, returns (nullptr, nullptr)
   */
  iterator_t next(iterator_t & curr) const
  {
    auto & [node_t1_ptr, node_t2_ptr] = curr;

    std::shared_ptr<node_t1_t> next_node_t1_ptr;
    std::shared_ptr<node_t2_t> next_node_t2_ptr;

    if (node_t1_ptr != nullptr && node_t2_ptr == nullptr) {
      next_node_t2_ptr = node_t1_ptr->get_next();
      return std::make_pair(next_node_t1_ptr, next_node_t2_ptr);
    }

    else if (node_t2_ptr != nullptr && node_t1_ptr == nullptr) {
      next_node_t1_ptr = node_t2_ptr->get_next();
      return std::make_pair(next_node_t1_ptr, next_node_t2_ptr);
    }

    throw std::invalid_argument("Invalid iterator, exactly one pointer one must be non-null in the pair");
  }

  /*
   * \brief First iterator for backward iteration
   * \return a pair of pointers, with the one representing tail being non-null if the list is non-empty,
   * and [nullptr, nullptr] if list is empty
   */
  iterator_t backward_begin() const { return get_tail(); }

  /*
   * \brief Next to end of backward list iterator
   * \return [nullptr, nullptr]
   */
  constexpr iterator_t backward_end() const { return std::make_pair(nullptr, nullptr); }

  /*
   * \brief Returns the prev node as a pair, with the non-null pointer being the prev node
   * \param curr : current iterator
   * \pre curr is a valid iterator and curr != end()
   * \return A pair, where one of the nodes is set to the prev node of the non-null node in curr and
   * the other node is set to null
   * \throw std::invalid_argument if either none or both the nodes in curr are nullptr
   * \note If there is no prev node, returns (nullptr, nullptr)
   */
  iterator_t prev(iterator_t & curr) const
  {
    auto & [node_t1_ptr, node_t2_ptr] = curr;

    std::shared_ptr<node_t1_t> prev_node_t1_ptr;
    std::shared_ptr<node_t2_t> prev_node_t2_ptr;

    if (node_t1_ptr != nullptr && node_t2_ptr == nullptr) {
      prev_node_t2_ptr = node_t1_ptr->get_prev();
      return std::make_pair(prev_node_t1_ptr, prev_node_t2_ptr);
    }

    else if (node_t2_ptr != nullptr && node_t1_ptr == nullptr) {
      prev_node_t1_ptr = node_t2_ptr->get_prev();
      return std::make_pair(prev_node_t1_ptr, prev_node_t2_ptr);
    }

    throw std::invalid_argument("Invalid iterator, exactly one pointer one must be non-null in the pair");
  }

private:
  /*
    \brief Enum to represent the possibilities for tail
  */
  enum tail_type_t { NONE_T, TYPE_T1_T, TYPE_T2_T };

  /*
   \return the type of tail
  */
  enum tail_type_t get_tail_type()
  {
    if (_tail_as_t1.get() == nullptr && _tail_as_t2.get() == nullptr) {
      return NONE_T;
    }

    else if (_tail_as_t1.get() != nullptr && _tail_as_t2.get() == nullptr) {
      return TYPE_T1_T;
    }

    else if (_tail_as_t2.get() != nullptr && _tail_as_t1.get() == nullptr) {
      return TYPE_T2_T;
    }

    else {
      throw std::runtime_error("Inconsistent state of alternating list");
    }
  }
  head_t _head;
  tail_as_t1_t _tail_as_t1;
  tail_as_t2_t _tail_as_t2;
};

} // end of namespace tchecker

#endif // TCHECKER_ALTERNATING_LIST_HH
