/**
 * @headerfile binaryquicksort.h
 * @author: Richik Vivek Sen (rsen9@gatech.edu)
 * @date 10/28/2019
 * @brief Header file implementing binary quicksort
 */

#ifndef BINARYQUICKSORT_H
#define BINARYQUICKSORT_H

#include <climits>   // For CHAR_BIT
#include <iterator>
#include <limits>
#include <algorithm> // For std::iter_swap, std::rotate, std::find_if

/**
 * Function: BinaryQuicksort(RandomIterator begin, RandomIterator end);
 * Usage: BinaryQuicksort(v.begin(), v.end());
 * ------------------------------------------------------------------------
 * Applies the binary quicksort algorithm to sort the specified list of
 * numbers.  It is assumes that the iterators are traversing a list of
 * integral types, and will not function properly otherwise.
 */
template <typename RandomIterator>
void BinaryQuicksort(RandomIterator begin, RandomIterator end);

/* * * * * Implementation Below This Point * * * * */

namespace binaryquicksort_detail {

  /* Utility function to partition the elements of a range by moving all
   * elements in the range having a 0 in a given position to the right and all
   * elements in the range having a 1 in a given position to the left.  The
   * function then returns an iterator to the beginning of the range that
   * contains a 1.
   *
   * This algorithm works by having begin point one step past the end of the
   * range of values known to be 0 and end point at the range of values known
   * to be 1.  The endpoints are then marched inward until they collide (in
   * which case we're done) or a pair of mismatched elements are found.
   */
  template <typename RandomIterator>
  RandomIterator PartitionAtBit(RandomIterator begin, RandomIterator end,
                                signed int bit) {
    /* Typedef defining the type of the elements being traversed. */
    typedef typename std::iterator_traits<RandomIterator>::value_type T;

    /* Compute the bitmask we'll use to test whether the bit is set. */
    const T bitmask = T(1) << bit;

    /* Move these two together until they meet or we find two elements that
     * are out of place.
     */
    while (true) {
      /* Find the first 1 after the 0s; it's either the end or we've just
       * found the element that's out of place.
       */
      while (begin < end && !(*begin & bitmask))
        ++ begin;

      /* If the begin is now sitting atop the end, we're done and all of the
       * remaining values are 1s.  We're therefore done.
       */
      if (begin == end)
        return begin;

      /* Otherwise, the end is just before the elements containing 1s.  Start
       * moving it over until we find a 0 or realize that we've already
       * found everything.
       *
       * Since we need to back up the end iterator before we read it, this
       * loop is written as a do ... while rather than as a while loop.
       */
      do {
        --end;
      } while (begin < end && !!(*end & bitmask));

      /* If the two are equal, we've found the crossover point and are done.
       * We can hand back this element as the pivot point.
       */
      if (begin == end)
        return begin;

      /* Otherwise, swap the two elements and repeat. */
      std::iter_swap(begin, end);
    }
  }

  /* Utility function which actually performs the binary quicksort algorithm,
   * beginning with the specified bit.
   */
  template <typename RandomIterator>
  void BinaryQuicksortAtBit(RandomIterator begin, RandomIterator end,
                            signed int bit) {
    /* Typedef defining the type of the elements being traversed. */
    typedef typename std::iterator_traits<RandomIterator>::value_type T;

    /* Borrowing an optimization technique from quicksort, we will have this
     * function work iteratively and recursively.  To avoid having a large
     * number of function calls made, we will iteratively process the larger
     * of the two partitions we find, and will recursively process the other.
     */

    /* If we've processed all the bits, or if the range has fewer than one
     * element in it, we're done.
     */
    while (bit >= 0 && std::distance(begin, end) > 1) {
      /* Apply the partitioning step on this bit and get the start of the
       * range of values containing the 1s.
       */
      RandomIterator pivot = PartitionAtBit(begin, end, bit);

      /* Drop the index of the bit we're processing; this will cause the next
       * loop iteration to use the right bit and will make the recursive calls
       * correct.
       */
      --bit;

      /* Determine which range is larger - the range holding the 0s or the
       * range holding the 1s.  Based on which is smaller, recursively process
       * one of the ranges.
       */
      if (std::distance(begin, pivot) < std::distance(pivot, end)) {
        /* There are fewer numbers beginning with 0; go recursively sort
         * them.
         */
        BinaryQuicksortAtBit(begin, pivot, bit);
        begin = pivot;
      } else {
        /* There are fewer numbers beginning with 1; go recursively sort
         * them.
         */
        BinaryQuicksortAtBit(pivot, end, bit);
        end = pivot;
      }
    }
  }

  /* If we are dealing with signed numbers, then negative numbers will
   * incorrectly appear at the end of the range rather than the start, since
   * the signed two's-complement representation will cause the sign bit to
   * be set, making the negative values appear larger than positive values.
   * This function applies a rotation to the final array to pull the negative
   * values (if any) to the front.
   */
  template <typename RandomIterator>
  void RotateNegativeValues(RandomIterator begin, RandomIterator end) {
    /* Typedef defining the type of the elements being traversed. */
    typedef typename std::iterator_traits<RandomIterator>::value_type T;

    /* Walk forward until we find a negative value.  If we find one, do a
     * rotate to rectify the elements.
     */
    for (RandomIterator itr = begin; itr != end; ++itr) {
      /* If the value is negative, do a rotate starting here. */
      if (*itr < T(0)) {
        std::rotate(begin, itr, end);
        return;
      }
    }
  }
}

/* Actual implementation of binary quicksort. */
template <typename RandomIterator>
void BinaryQuicksort(RandomIterator begin, RandomIterator end) {
    /* Typedef defining the type of the elements being traversed. */
    typedef typename std::iterator_traits<RandomIterator>::value_type T;

    /* Find out how many bits we need to process. */
    const signed int kNumBits = (signed int)(CHAR_BIT * sizeof(T));

    /* Run binary quicksort on the elements, starting with the MSD. */
    binaryquicksort_detail::BinaryQuicksortAtBit(begin, end, kNumBits - 1);

    /* If the numbers are signed, we need to do a rotate to pull all of the
     * negative numbers to the front of the range, since otherwise (because
     * their MSB is set) they'll be at the end instead of the front.
     */
    if (std::numeric_limits<T>::is_signed)
      binaryquicksort_detail::RotateNegativeValues(begin, end);
}

#endif
