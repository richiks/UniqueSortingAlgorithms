/**
 * @headerfile smoothsort.h
 * @author: Richik Vivek Sen (rsen9@gatech.edu)
 * @date 10/28/2019
 * @brief Header file implementing Smoothsort
 */

#ifndef SMOOTHSORT_H
#define SMOOTHSORT_H

#include <iterator>
#include <algorithm>
#include <bitset>

/**
 * Function: Smoothsort(RandomIterator begin, RandomIterator end);
 * -----------------------------------------------------------------------
 * Sorts the input range into ascending order using the smoothsort
 * algorithm.
 */
template <typename RandomIterator>
void Smoothsort(RandomIterator begin, RandomIterator end);

/**
 * Function: Smoothsort(RandomIterator begin, RandomIterator end,
 *                      Comparator comp);
 * -----------------------------------------------------------------------
 * Sorts the input range into ascending order according to the strict
 * total ordering comp using the smoothsort algorithm.
 */
template <typename RandomIterator, typename Comparator>
void Smoothsort(RandomIterator begin, RandomIterator end, Comparator comp);

/* * * * * Implementation Below This Point * * * * */
namespace smoothsort_detail {
  /* A constant containing the number of Leonardo numbers that can fit into
   * 32 bits.  For a 64-bit machine, you'll need to update this value and the
   * list below.
   */
  const size_t kNumLeonardoNumbers = 46;

  /* A list of all the Leonardo numbers below 2^32, precomputed for
   * efficiency.
   * Source: http://oeis.org/classic/b001595.txt
   */
  const size_t kLeonardoNumbers[kNumLeonardoNumbers] = {
    1u, 1u, 3u, 5u, 9u, 15u, 25u, 41u, 67u, 109u, 177u, 287u, 465u, 753u,
    1219u, 1973u, 3193u, 5167u, 8361u, 13529u, 21891u, 35421u, 57313u, 92735u,
    150049u, 242785u, 392835u, 635621u, 1028457u, 1664079u, 2692537u,
    4356617u, 7049155u, 11405773u, 18454929u, 29860703u, 48315633u, 78176337u,
    126491971u, 204668309u, 331160281u, 535828591u, 866988873u, 1402817465u,
    2269806339u, 3672623805u
  };

  /* A structure containing a bitvector encoding of the trees in a Leonardo
   * heap.  The representation is as a bitvector shifted down so that its
   * first digit is a one, along with the amount that it was shifted.
   */
  struct HeapShape {
    /* A bitvector capable of holding all the Leonardo numbers. */
    std::bitset<kNumLeonardoNumbers> trees;

    /* The shift amount, which is also the size of the smallest tree. */
    size_t smallestTreeSize;
  };

  /**
   * Function: RandomIterator SecondChild(RandomIterator root)
   * ---------------------------------------------------------------------
   * Given an iterator to the root of Leonardo heap, returns an iterator
   * to the root of that tree's second child.  It's assumed that the heap
   * is well-formed and that size > 1.
   */
  template <typename RandomIterator>
  RandomIterator SecondChild(RandomIterator root) {
    /* The second child root is always one step before the root. */
    return root - 1;
  }

  /**
   * Function: RandomIterator FirstChild(RandomIterator root, size_t size)
   * ---------------------------------------------------------------------
   * Given an iterator to the root of Leonardo heap, returns an iterator
   * to the root of that tree's first child.  It's assumed that the heap
   * is well-formed and that size > 1.
   */
  template <typename RandomIterator>
  RandomIterator FirstChild(RandomIterator root, size_t size) {
    /* Go to the second child, then step backwards L(size - 2) steps to
     * skip over it.
     */
    return SecondChild(root) - kLeonardoNumbers[size - 2];
  }

  /**
   * Function: RandomIterator LargerChild(RandomIterator root, size_t size,
   *                                      Comparator comp);
   * --------------------------------------------------------------------
   * Given an iterator to the root of a max-heap Leonardo tree, returns
   * an iterator to its larger child.  It's assumed that the heap is
   * well-formatted and that the heap has order > 1.
   */
  template <typename RandomIterator, typename Comparator>
  RandomIterator LargerChild(RandomIterator root, size_t size, Comparator comp) {
    /* Get pointers to the first and second child. */
    RandomIterator first  = FirstChild(root, size);
    RandomIterator second = SecondChild(root);

    /* Determine which is greater. */
    return comp(*first, *second)? second : first;
  }

  /**
   * Function: RebalanceSingleHeap(RandomIterator root, size_t size,
   *                               Comparator comp);
   * --------------------------------------------------------------------
   * Given an iterator to the root of a single Leonardo tree that needs
   * rebalancing, rebalances that tree using the standard "bubble-down"
   * approach.
   */
  template <typename RandomIterator, typename Comparator>
  void RebalanceSingleHeap(RandomIterator root, size_t size, Comparator comp) {
    /* Loop until the current node has no children, which happens when the order
     * of the tree is 0 or 1.
     */
    while (size > 1) {
      /* Get pointers to the first and second child. */
      RandomIterator first  = FirstChild(root, size);
      RandomIterator second = SecondChild(root);

      /* Determine which child is larger and remember the order of its tree. */
      RandomIterator largerChild;
      size_t childSize;
      if (comp(*first, *second)) {
        largerChild = second; // Second child is larger...
        childSize = size - 2; // ... and has order k - 2.
      } else {
        largerChild = first;  // First child is larger...
        childSize = size - 1; // ... and has order k - 1.
      }

      /* If the root is bigger than this child, we're done. */
      if (!comp(*root, *largerChild))
        return;

      /* Otherwise, swap down and update our order. */
      std::iter_swap(root, largerChild);
      root = largerChild;
      size = childSize;
    }
  }

  /**
   * Function: LeonardoHeapRectify(RandomIterator begin, RandomIterator end,
   *                               HeapShape shape, Comparator comp);
   * ---------------------------------------------------------------------
   * Given an implicit Leonardo heap spanning [begin, end) that has just
   * had an element inserted into it at the very end, along with the
   * size list for that heap, rectifies the heap structure by shuffling
   * the new root down to the proper position and rebalancing the target
   * heap.
   */
  template <typename RandomIterator, typename Comparator>
  void LeonardoHeapRectify(RandomIterator begin, RandomIterator end,
                           HeapShape shape, Comparator comp) {
    /* Back up the end iterator one step to get to the root of the rightmost
     * heap.
     */
    RandomIterator itr = end - 1;

    /* Keep track of the size of the last heap size that we visited.  We need
     * this so that once we've positioned the new node atop the correct heap
     * we remember how large it is.
     */
    size_t lastHeapSize;

    /* Starting at the last heap and working backward, check whether we need
     * to swap the root of the current heap with the previous root.
     */
    while (true) {
      /* Cache the size of the heap we're currently on top of. */
      lastHeapSize = shape.smallestTreeSize;

      /* If this is the very first heap in the tree, we're done. */
      if (size_t(std::distance(begin, itr)) == kLeonardoNumbers[lastHeapSize] - 1)
        break;

      /* We want to swap the previous root with this one if it's strictly
       * greater than both the root of this tree and both its children.
       * In order to avoid weird edge cases when the current heap has
       * size zero or size one, we'll compute what value will be compared
       * against.
       */
      RandomIterator toCompare = itr;

      /* If we aren't an order-0 or order-1 tree, we have two children, and
       * need to check which of the three values is largest.
       */
      if (shape.smallestTreeSize > 1) {
        /* Get the largest child and see if we need to change what we're
         * comparing against.
         */
        RandomIterator largeChild = LargerChild(itr, shape.smallestTreeSize,
                                                comp);

        /* Update what element is being compared against. */
        if (comp(*toCompare, *largeChild))
          toCompare = largeChild;
      }

      /* Get a pointer to the root of the second heap by backing up the size
       * of this heap.
       */
      RandomIterator priorHeap = itr - kLeonardoNumbers[lastHeapSize];

      /* If we ran out of trees or the new tree root is less than the element
       * we're comparing, we now have the new node at the top of the correct
       * heap.
       */
      if (!comp(*toCompare, *priorHeap))
        break;

      /* Otherwise, do the swap and adjust our location. */
      std::iter_swap(itr, priorHeap);
      itr = priorHeap;

      /* Scan down until we find the heap before this one.  We do this by
       * continously shifting down the tree bitvector and bumping up the size
       * of the smallest tree until we hit a new tree.
       */
      do {
        shape.trees >>= 1;
        ++shape.smallestTreeSize;
      } while (!shape.trees[0]);
    }

    /* Finally, rebalance the current heap. */
    RebalanceSingleHeap(itr, lastHeapSize, comp);
  }

  /**
   * Function: LeonardoHeapAdd(RandomIterator begin, RandomIterator end,
   *                           RandomIterator heapEnd,
   *                           HeapShape& shape, Comparator comp);
   * ----------------------------------------------------------------------
   * Given an implicit Leonardo heap spanning [begin, end) in a range spanned
   * by [begin, heapEnd], along with the shape and a comparator, increases the
   * size of that heap by one by inserting the element at *end.
   */
  template <typename RandomIterator, typename Comparator>
  void LeonardoHeapAdd(RandomIterator begin, RandomIterator end,
                       RandomIterator heapEnd,
                       HeapShape& shape, Comparator comp) {
    /* There are three cases to consider, which are analogous to the cases
     * in the proof that it is possible to partition the input into heaps
     * of decreasing size:
     *
     * Case 0: If there are no elements in the heap, add a tree of order 1.
     * Case 1: If the last two heaps have sizes that differ by one, we
     *         add the new element by merging the last two heaps.
     * Case 2: Otherwise, if the last heap has Leonardo number 1, add
     *         a singleton heap of Leonardo number 0.
     * Case 3: Otherwise, add a singleton heap of Leonardo number 1.
     */

    /* Case 0 represented by the first bit being a zero; it should always be
     * one during normal operation.
     */
    if (!shape.trees[0]) {
      shape.trees[0] = true;
      shape.smallestTreeSize = 1;
    }
    /* Case 1 would be represented by the last two bits of the bitvector both
     * being set.
     */
    else if (shape.trees[1] && shape.trees[0]) {
      /* First, remove those two trees by shifting them off the bitvector. */
      shape.trees >>= 2;

      /* Set the last bit of the bitvector; we just added a tree of this
       * size.
       */
      shape.trees[0] = true;

      /* Finally, increase the size of the smallest tree by two, since the new
       * Leonardo tree has order one greater than both of them.
       */
      shape.smallestTreeSize += 2;
    }
    /* Case two is represented by the size of the smallest tree being 1. */
    else if (shape.smallestTreeSize == 1) {
      /* Shift the bits up one spot so that we have room for the zero bit. */
      shape.trees <<= 1;
      shape.smallestTreeSize = 0;

      /* Set the bit. */
      shape.trees[0] = true;
    }
    /* Case three is everything else. */
    else {
      /* We currently have a forest encoded with a format that looks like
       * (W, n) for bitstring W and exponent n.  We want to convert this to
       * (W00...01, 1) by shifting up n - 1 spaces, then setting the last bit.
       */
      shape.trees <<= shape.smallestTreeSize - 1;
      shape.trees[0] = true;

      /* Set the smallest tree size to one, since that is the new smallest
       * tree size.
       */
      shape.smallestTreeSize = 1;
    }

    /* At this point, we've set up a new tree.  We need to see if this tree is
     * at the final size it's going to take.  If so, we'll do a full rectify
     * on it.  Otherwise, all we need to do is maintain the heap property.
     */
    bool isLast = false;
    switch (shape.smallestTreeSize) {
      /* If this last heap has order 0, then it's in its final position only
       * if it's the very last element of the array.
       */
    case 0:
      if (end + 1 == heapEnd)
        isLast = true;
      break;

      /* If this last heap has order 1, then it's in its final position if
       * it's the last element, or it's the penultimate element and it's not
       * about to be merged.  For simplicity
       */
    case 1:
      if (end + 1 == heapEnd || (end + 2 == heapEnd && !shape.trees[1]))
        isLast = true;
      break;

      /* Otherwise, this heap is in its final position if there isn't enough
       * room for the next Leonardo number and one extra element.
       */
    default:
      if (size_t(std::distance(end + 1, heapEnd)) < kLeonardoNumbers[shape.smallestTreeSize - 1] + 1)
        isLast = true;
      break;
    }

    /* If this isn't a final heap, then just rebalance the current heap. */
    if (!isLast)
      RebalanceSingleHeap(end, shape.smallestTreeSize, comp);
    /* Otherwise do a full rectify to put this node in its place. */
    else
      LeonardoHeapRectify(begin, end + 1, shape, comp);
  }
  /**
   * Function: LeonardoHeapRemove(RandomIterator begin, RandomIterator end,
   *                              HeapShape& shape,  Comparator comp);
   * ----------------------------------------------------------------------
   * Given an implicit Leonardo heap spanning [begin, end), along with the
   * size list and a comparator, dequeues the element at end - 1 and
   * rebalances the heap.  Since the largest element of the heap is already
   * at end, this essentially keeps the max element in its place and does
   * a rebalance if necessary.
   */
  template <typename RandomIterator, typename Comparator>
  void LeonardoHeapRemove(RandomIterator begin, RandomIterator end,
                          HeapShape& shape, Comparator comp) {
    /* There are two cases to consider:
     *
     * Case 1: The last heap is of order zero or one.  In this case,
     *         removing it doesn't expose any new trees and we can just
     *         drop it from the list of trees.
     * Case 2: The last heap is of order two or greater.  In this case,
     *         we exposed two new heaps, which may require rebalancing.
     */

    /* Case 1. */
    if (shape.smallestTreeSize <= 1) {
      /* Keep scanning up the list looking for the next tree. */
      do {
        shape.trees >>= 1;
        ++shape.smallestTreeSize;
      } while (shape.trees.any() && !shape.trees[0]);
      return;
    }

    /* Break open the last heap to expose two subheaps of order
     * k - 2 and k - 1.  This works by mapping the encoding (W1, n) to the
     * encoding (W011, n - 2).
     */
    const size_t heapOrder = shape.smallestTreeSize;
    shape.trees[0] = false;
    shape.trees <<= 2;
    shape.trees[1] = shape.trees[0] = true;
    shape.smallestTreeSize -= 2;

    /* We now do the insertion-sort/rebalance operation on the larger exposed heap to
     * put it in its proper place, then on the smaller of the two.  But first, we need
     * to find where they are.  This can be done by just looking up the first and second
     * children of the former root, which was at end - 1.
     */
    RandomIterator leftHeap  = FirstChild(end - 1, heapOrder);
    RandomIterator rightHeap = SecondChild(end - 1);

    /* Rebalance the left heap.  For this step we'll pretend that there is
     * one fewer heap than there actually is, since we're ignoring the
     * rightmost heap.
     */
    HeapShape allButLast = shape;
    ++allButLast.smallestTreeSize;
    allButLast.trees >>= 1;

    /* We add one to the position of the left heap because the function
     * assumes an exclusive range, while leftHeap is actually an iterator
     * directly to where the root is.
     */
    LeonardoHeapRectify(begin, leftHeap + 1,  allButLast, comp);
    LeonardoHeapRectify(begin, rightHeap + 1, shape, comp);
  }
}

/* Actual smoothsort implementation. */
template <typename RandomIterator, typename Comparator>
void Smoothsort(RandomIterator begin, RandomIterator end, Comparator comp) {
  /* Edge case: Check that the range isn't empty or a singleton. */
  if (begin == end || begin + 1 == end) return;

  /* Construct a shape object describing the empty heap. */
  smoothsort_detail::HeapShape shape;
  shape.smallestTreeSize = 0;

  /* Convert the input into an implicit Leonardo heap. */
  for (RandomIterator itr = begin; itr != end; ++itr)
    smoothsort_detail::LeonardoHeapAdd(begin, itr, end, shape, comp);

  /* Continuously dequeue from the implicit Leonardo heap until we've
   * consumed all the elements.
   */
  for (RandomIterator itr = end; itr != begin; --itr)
    smoothsort_detail::LeonardoHeapRemove(begin, itr, shape, comp);
}

/* Non-comparator version just uses the default comparator. */
template <typename RandomIterator>
void Smoothsort(RandomIterator begin, RandomIterator end) {
  Smoothsort(begin, end,
             std::less<typename std::iterator_traits<RandomIterator>::value_type>());
}

#endif // SMOOTHSORT_H
