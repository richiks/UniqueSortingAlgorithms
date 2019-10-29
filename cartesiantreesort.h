/**
 * @headerfile cartesiantreesort.h
 * @author: Richik Vivek Sen (rsen9@gatech.edu)
 * @date 10/28/2019
 * @brief Header file implementing Cartesian Tree Sort
 */

#ifndef CARTESIANTREESORT_H
#define CARTESIANTREESORT_H

/**
 * void CartesianTreeSort(ForwardIterator begin, ForwardIterator end);
 * Usage: CartesianTreeSort(v.begin(), v.end());
 * ---------------------------------------------------------------------------
 * Sorts the range [begin, end) into ascending order according to the default
 * ordering using the Cartesian tree sort algorithm.
 */
template <typename ForwardIterator>
void CartesianTreeSort(ForwardIterator begin, ForwardIterator end);

/**
 * void CartesianTreeSort(ForwardIterator begin, ForwardIterator end,
 *                        Comparator comp);
 * Usage: CartesianTreeSort(v.begin(), v.end(), std::greater<int>());
 * ---------------------------------------------------------------------------
 * Sorts the range [begin, end) into ascending order according to specified
 * comparator using the Cartesian tree sort algorithm.
 */
template <typename ForwardIterator, typename Comparator>
void CartesianTreeSort(ForwardIterator begin, ForwardIterator end,
                       Comparator comp);

/* * * * * Implementation Below This Point * * * * */
#include <iterator>   // For iterator_traits
#include <functional> // For less
#include <memory>     // For auto_ptr
#include <stack>
#include <queue>
#include <vector>

namespace cartesiantreesort_detail {
  /* A utility struct representing a node in a Cartesian tree. */
  template <typename T> struct Node {
    const T value;      // The node's value
    Node* left, *right; // Pointers to the proper subtrees

    /* Constructor: Node(const T& value);
     * Usage: Node* node = new Node(value);
     * -----------------------------------------------------------------------
     * Constructs a new Node having the specified value and no children.
     */
    explicit Node(const T& value) : value(value) {
      /* Initially this node is isolated. */
      left = right = NULL;
    }

    /* Destructor: ~Node();
     * Usage: (implicit)
     * -----------------------------------------------------------------------
     * Deallocates the tree rooted at this Node.
     */
    ~Node() {
      delete left;
      delete right;
    }
  };

  /* Node<T>* MakeCartesianTree(InputIterator begin, InputIterator end,
   *                            Comparator comp);
   * Usage: Node<T>* tree = MakeCartesianTree(begin, end, comp);
   * -------------------------------------------------------------------------
   * Constructs and returns a Cartesian tree containing the specified values
   * and sorted as a min-heap with respect to the given comparator.  The
   * return type of this function is a bit messy because it has to introspect
   * on the iterator type to figure out what's being stored.
   */
  template <typename InputIterator, typename Comparator>
  Node<typename std::iterator_traits<InputIterator>::value_type>*
  MakeCartesianTree(InputIterator begin, InputIterator end,
                    Comparator comp) {
    /* For sanity's sake, typedef the type being iterated over. */
    typedef typename std::iterator_traits<InputIterator>::value_type T;

    /* Keep track of the root of the tree, which is initially NULL because the
     * tree is empty.
     */
    Node<T>* root = NULL;

    /* In addition to this, we'll maintain a stack of the nodes on the right
     * spine of the tree, in the order in which you would encounter them if
     * you marched upward from the rightmost node to the root.
     */
    std::stack< Node<T>* > rightSpine;

    /* To avoid edge cases later on, we'll add NULL to the right spine.  This
     * does make some sense mathematically, since if we walk from the
     * rightmost node to the root and upward we'd walk off the tree at some
     * point.
     */
    rightSpine.push(NULL);

    /* Scan across the elements, adding them one at a time. */
    for (; begin != end; ++begin) {
      /* Construct the new node to insert. */
      Node<T>* node = new Node<T>(*begin);

      /* Starting at the rightmost node, walk upward along the right spine
       * until we find a node that can serve as the parent.  Because the spine
       * is never empty (NULL will always be there), we don't need to worry
       * about an empty stack.
       */
      Node<T>* curr;
      for (curr = rightSpine.top(); curr != NULL; rightSpine.pop(), curr = rightSpine.top())
        if (comp(curr->value, node->value))
          break;

      /* At this point, there are two cases to consider.  First, this new node
       * might be the new minimum.  In that case, we make it the global tree
       * root, and to preserve the inorder walk requirement make the old tree
       * its right child.
       */
      if (curr == NULL) {
        node->left = root;
        root = node;
      }
      /* Otherwise, we need to pull the current node's right subtree so that
       * it's the left subtree of the current node, and then set the new node
       * as the right child of the current node.
       */
      else {
        node->left = curr->right;
        curr->right = node;
      }

      /* This new node is now on the right spine, so we'll add it to the stack
       * of nodes stored there.
       */
      rightSpine.push(node);
    }

    /* Hand back the resulting tree. */
    return root;
  }

  /* A utility comparator class that compares Node<T>*s by the reverse of
   * their comparison by some comparator.  The rationale is that we will use
   * this comparator in a priority_queue of Node<T>*s, and will need some way
   * to ensure that the nodes are compared so that the smallest elements come
   * back first.
   */
  template <typename T, typename Comparator>
  class NodeComparator {
  public:
    /* Constructor: NodeComparator(Comparator comp);
     * Usage: NodeComparator comp(rawComp);
     * -----------------------------------------------------------------------
     * Constructs a new NodeComparator that uses the specified comparator on
     * the values in Node<T>*s.
     */
    explicit NodeComparator(Comparator comp) : comp(comp) {
      // Handled in initializer list
    }

    /* Comparator: bool operator() (const Node<T>* lhs, const Node<T>* rhs) const;
     * Usage: comp(one, two);
     * -----------------------------------------------------------------------
     * Returns whether the first node compares at least as large as the second
     * node using the stored comparator.
     */
    bool operator() (const Node<T>* lhs, const Node<T>* rhs) const {
      /* Check if lhs >= rhs by seeing if lhs < rhs returns false. */
      return !comp(lhs->value, rhs->value);
    }

  private:
    Comparator comp; // The actual comparator to use
  };
}

/* Actual implementation of Cartesian tree sort, using a parameterized
 * comparator.
 */
template <typename ForwardIterator, typename Comparator>
void CartesianTreeSort(ForwardIterator begin, ForwardIterator end,
                       Comparator comp) {
  /* As an edge case, check if the input is empty.  This avoids a problem
   * later on in this function where we might try enqueueing a NULL tree node
   * into the queue.
   */
  if (begin == end) return;

  /* Grant access to our helper types and classes. */
  using namespace cartesiantreesort_detail;

  /* Again, for sanity's sake, typedef the type being iterated over. */
  typedef typename std::iterator_traits<ForwardIterator>::value_type T;

  /* A type representing a priority queue that compares the value fields of
   * Cartesian tree nodes.
   */
  typedef std::priority_queue<Node<T>*, std::vector<Node<T>*>,
                              NodeComparator<T, Comparator> > PQueue;

  /* Construct a priority queue, wrapping up the comparator provided by the
   * client.  Due to the Most Vexing Parse, we have to parenthesize the
   * argument so this isn't accidentally interpreted as a function declaration.
   */
  PQueue pq((NodeComparator<T, Comparator>(comp)));

  /* Obtain a Cartesian tree over the input.  We'll store the result in a
   * const auto_ptr to ensure that
   *
   * 1. The memory is reclaimed when the function exits and the auto_ptr
   *    leaves scope.
   * 2. The memory isn't accidentally transferred elsewhere, because the
   *    auto_ptr is const.
   */
  const std::auto_ptr< Node<T> > tree(MakeCartesianTree(begin, end, comp));

  /* Initialize the priority queue to hold the Cartesian tree of the input. */
  pq.push(tree.get());

  /* Now, scan across the sequence, placing the smallest known value at the
   * next open position and updating the queue accordingly.
   */
  for (ForwardIterator itr = begin; itr != end; ++itr) {
    /* Grab the next node from the queue. */
    Node<T>* curr = pq.top(); pq.pop();

    /* Store its value back into the sequence. */
    *itr = curr->value;

    /* Add any non-NULL subtrees of the current tree back into the queue. */
    if (curr->left) pq.push(curr->left);
    if (curr->right) pq.push(curr->right);
  }
}

/* Non-comparator version implemented in terms of the comparator version. */
template <typename ForwardIterator>
void CartesianTreeSort(ForwardIterator begin, ForwardIterator end) {
  CartesianTreeSort(begin, end,
                    std::less<typename std::iterator_traits<ForwardIterator>::value_type>());
}

#endif // CARTESIANTREESORT_H
