# UniqueSortingAlgorithms

                              Binary Quicksort

 * An implementation of the "binary quicksort" algorithm, another name for MSD-
 * radix sort in base 2.  The algorithm is a non-comparison sort for sorting
 * integers that works by sorting the numbers one bit at a time, starting with
 * the most significant digit and ending with the least-significant digit.
 * Internally, the algorithm works by first sorting the numbers into ascending
 * order by the first bit, then recursively sorting each half of the numbers by
 * their second digit, etc.  For example, given the numbers 0-7 in scrambled
 * order, like this:
 *
 *         3   1   4   0   5   7   2   6
 *
 * We would write these numbers in binary to get
 *
 *       011 001 100 000 101 111 010 110
 *
 * First, we sort the numbers by just their first digit.  For example:
 *
 *       011 001 000 010 100 101 111 110
 *       ^               ^
 *       |               |
 *       |               +--- Numbers beginning with a 1
 *       |
 *       +------------------- Numbers beginning with a 0
 *
 *
 * Notice that we now have the array split into two regions - one region
 * consisting of values beginning with a 0 and one region consisting of values
 * beginning with a 1.  We now recursively descend into these two regions and
 * sort each one by their second digit.  This gives the following:
 *
 *       001 000 011 010 100 101 111 110
 *
 * From here we sort by the third digit:
 *
 *       000 001 010 011 100 101 110 111
 *
 * And the numbers are now in order.
 *
 * In order to convert this sketch of an algorithm into a concrete
 * implementation, we will need to devise a means for sorting a range of
 * numbers by just a single digit.  One option for doing this out-of-place is
 * to build two queues, one for numbers with a 0 in that bit and one for
 * numbers with a 1 in at that bit, then add each number in the range to the
 * appropriate queue.  We can then dequeue the numbers from the 0 queue and the
 * 1 queue, in order, and insert them back into the original array.  For
 * example, given these numbers, which we want to sort by the most significant
 * bit:
 *
 *       011 001 100 000 101 111 010 110
 *
 * We would build two queues and insert the numbers as follows:
 *
 *   Begins with 0: 011 001 000 010
 *   Begins with 1: 100 101 111 110
 *
 * We would then concatenate these two queues and store the result back into
 * the original array:
 *
 *      011 001 000 010 100 101 111 110
 *
 * While this works, it has the drawback that it requires O(n) auxiliary
 * storage.  Fortunately, there is a faster algorithm for solving this problem
 * that works in-place.  The intuition behind the algorithm is that we want to
 * reorder the elements so that the array ends up being split into two regions-
 * a region on the left consisting of all values that contain a 0 at the
 * appropriate bit, and a region on the right consisting of all values that
 * contain a 1 at the appropriate bit.  We can therefore build an algorithm
 * that scans inward from the two sides of the array looking for numbers that
 * violate this invariant, then exchange the numbers that are out of place.
 * For example, suppose we have this sequence of 0s and 1s:
 *
 *        0  1  0  1  0  1  1  0  1
 *
 * We would march two pointers in from the left and right side looking for a
 * point where elements are found that are out of place.  This would first find
 * this pair:
 *
 *        0  1  0  1  0  1  1  0  1
 *           ^                 ^
 *
 * We exchange those numbers, as shown here:
 *
 *        0  0  0  1  0  1  1  1  1
 *           ^                 ^
 *
 * and then march inward to find this next pair:
 *
 *        0  0  0  1  0  1  1  1  1
 *                 ^  ^
 *
 * which we swap to obtain
 *
 *        0  0  0  0  1  1  1  1  1
 *                 ^  ^
 *
 * and at this point are done.  This algorithm is very similar to the partition
 * step found in most quicksort implementations, and for this reason the MSD-
 * radix sort is often called binary quicksort.
 *
 * To analyze the runtime of the algorithm, note that running the partitioning
 * step on k integers takes O(k) time, since each element is visited at most
 * O(1) times.  Initially, this means that we do O(n) work to partition the
 * values by their first bit, then a total of O(n) work to partition each half
 * by the second bit, etc.  More generally, if the largest possible value that
 * can appear is U, there will be O(log U) partioning steps required, one for
 * each of the bits in U.  This means that the total runtime is O(n log U).
 * In cases where U = O(n), this is asympototically better than or comparable
 * to comparison sorts like quicksort or heap sort.
 
                            Cartesian Tree Sort
 
 * An implementation of a sort-style STL algorithm that uses a Cartesian tree
 * sort.  Cartesian tree sort is an adaptive, out-of-place sorting algorithm
 * with O(n) best-case behavior, O(n lg n) worst-case behavior, and O(n)
 * memory usage.
 *
 * A Cartesian tree is a tree created from a set of data that obeys the
 * following structural invariants:
 *
 * 1. The tree obeys in the min (or max) heap property - each node is less (or
 *    greater) than its children.
 * 2. An inorder traversal of the nodes yields the values in the same order in
 *    which they appear in the initial sequence.
 *
 * It's easy to see that this tree is unique by a quick induction on the size
 * of the input sequence.  As a base case, if the input sequence is empty,
 * then the empty tree is the unique Cartesian tree over that sequence.  For
 * the inductive case, assume that for all trees containing n' < n elements,
 * there is a unique Cartesian tree for each sequence of n' nodes.  Now take
 * any sequence of n elements.  Because a Cartesian tree is a min-heap, the
 * smallest element of the sequence must be the root of the Cartesian tree.
 * Because an inorder traversal of the elements must yield the input sequence,
 * we know that all nodes to the left of the min element must be in its left
 * subtree and similarly for the nodes to the right.  Since the left and right
 * subtree are both Cartesian trees with at most n - 1 elements in them (since
 * the min element is at the root), by the induction hypothesis there is a
 * unique Cartesian tree that could be the left or right subtree.  Since all
 * our decisions were forced, we end up with a unique tree, completing the
 * induction.
 *
 * An interesting note is that Cartesian trees are not necessarily height-
 * balanced.  In particular, any sequence in sorted or reverse-sorted order
 * will have a Cartesian tree that degrades to a linked list.  For example:
 *
 *                                    1 2 3 4 5
 *
 * has the following Cartesian tree:
 *
 *                                1
 *                                 \
 *                                  2
 *                                   \
 *                                    3
 *                                     \
 *                                      4
 *                                       \
 *                                        5
 *
 * In general, these Cartesian trees have height O(n).
 *
 * Interestingly, it's possible to build a Cartesian tree from a sequence of
 * data in linear time.  The algorithm as follows.  Beginning with the empty
 * tree, scan across the sequence from the left to the right adding new nodes
 * as follows:
 *
 * 1. Position the node as the right child of the rightmost node.
 * 2. Scan upward from the node's parent up to the root of the tree until a
 *    node is found whose value is less than the current value.
 * 3. If such a node is found, set its right child to be the new node, and
 *    set the new node's left child to be the previous right child.
 * 4. If no such node is found, set the new child to be the root, and set the
 *    new node's left child to be the previous tree.
 *
 * At first, this algorithm might not seem to run in linear time.  After all,
 * if the tree can become so imbalanced that it has height O(n) and we're
 * doing O(n) insertions, then it seems like the runtime should be O(n^2).
 * This bound is correct, but it isn't tight.  In particular, we can show that
 * the amortized cost of any insert is O(1), giving the net operations a total
 * runtime of O(n).  Define the potential of the tree to be the number of
 * nodes in its right spine.  The actual cost of any insertion is O(k), where
 * k is the number of nodes we considered on the way up from the rightmost
 * node to the node's new parent.  However, after we find where the node
 * belonds, we change the tree by moving k - 1 nodes into the left subtree of
 * the newly-inserted node.  This means that the tree's potential decreases by
 * k - 1.  We then increase the number of nodes in the right spine by one by
 * adding the new node there.  This means that the change in potential is
 * 1 - (k - 1) = 2 - k, giving an amortized cost of k + 2 - k = 2 = O(1) as
 * requested.
 *
 * Once we have built a Cartesian tree from a range of elements, we can sort
 * that range efficiently using a modified version of heapsort.  Construct a
 * binary heap that holds nodes in Cartesian trees and initialize it to the
 * root of the Cartesian tree.  Then, until the heap is empty, continuously
 * dequeue an element from the heap, add its root element to the next spot in
 * the sorted sequence, and then add the node's children to the heap.  This
 * visits each node in the sequence once, never visits a node until all of
 * its parents in the heap are visited, and visits the exposed roots in sorted
 * order.  This guarantees that the nodes come back sorted.  To see why,
 * suppose for the sake of contradiction two nodes come back out of order.
 * Call these nodes A and B with A > B.  Since the Cartesian tree is a min-
 * heap, B must not be an ancestor of A.  Since A came back first, B must not
 * have been an exposed root, or it would have come out of the heap before A.
 * But since B is not exposed, one of its ancestors must still be in the heap,
 * and since its ancestor has a value no greater than B's it would have come
 * out of the heap before A, a contradiction.
 *
 * Let's now consider the runtime of this phase.  We know that we will be
 * making O(n) insertions and dequeues from the binary heap, so each operation
 * takes at most O(lg n) time.  This gives us a worst-case runtime of
 * O(n lg n), matching the runtime guarantee of heapsort.  However, this bound
 * may not be tight.  In particular, suppose that our Cartesian tree is the
 * degenerate linked list described above.  Then initially the queue will have
 * exactly one element in it, and every time we dequeue the node and add its
 * children we'll only add a singleton node to the queue.  This means that the
 * queue will always have exactly one element in it, and so all the heap
 * operations will take O(1) time for a net runtime of O(n).  In fact, in any
 * Cartesian tree where each node has one child, we'll get this runtime.
 *
 * The interesting part about this algorithm is that it's possible to
 * explicitly quantify how much faster than O(n lg n) the algorithm will run
 * by using a measure called oscillation.  For any element x in the input
 * sequence, define cross(x) to be the number of adjacent pairs of entries
 * (y, z) in the input sequence such that either y <= x <= z or y >= x >= z.
 * Then we define the oscillation of the input sequence (denoted k) as the
 * average of cross(x) over all entries in the sequence.  It can be shown that
 * the overall runtime of the algorithm is O(n lg k), where k is this measure
 * of oscillation.  An interesting detail is that if the input is broken down
 * into S different sorted subsequences, k = O(S).  Consequently, if the
 * number of sorted subsequences in the input sequence is small (say, O(1)),
 * the algorithm will run in o(n lg n) time.  This result is due to the paper
 * "Heapsort, Adapted for Presorted Files," which first introduced Cartesian
 * tree sort.  Because the first step of the algorithm (building up the
 * Cartesian tree) runs in O(n), the overall runtime of the algorithm is
 * O(n lg k), which is at best O(n) and at worst O(n lg n).
 
                              Introsort
 
 * An implementation of the introsort (introspective sort) algorithm, a
 * hybrid of quicksort, heapsort, and insertion sort that has particularly
 * good runtime behavior.  It is one of the fastest comparison sorting
 * algorithms in use today, and is the usual implementation of the std::sort
 * algorithm provided with the C++ STL.
 *
 * Introsort aims to get the benefits of quicksort (good locality, in-place,
 * fast runtime) without running into any of its degenerate cases.  To do so,
 * the algorithm begins by guessing what the appropriate depth for the
 * quicksort recursion should be, then fires off a quicksort routine.  If
 * quicksort ever makes too many recursive calls, the introsort routine
 * switches over to using heapsort to sort the range.  This means that in
 * the best case, the algorithm runs a standard quicksort with minimal
 * bookkeeping overhead and thus runs extremely quickly.  In the worst case,
 * the algorithm switches to heapsort and avoids the O(n^2) worst-case of
 * quicksort.
 *
 * The algorithm also contains an additional optimization.  Rather than
 * using the O(n lg n) sorts (quicksort and heapsort) to completely sort the
 * input, instead introsort picks some "block size" and then uses the sorts
 * only on subranges larger than the block size.  It then makes a final pass
 * over the input using insertion sort to fix up the range.  Since insertion
 * sort runs extremely quickly (O(n)) when all of the elements in the range
 * are known to be a constant number of positions from their final locations,
 * this step runs rapidly.  It also decreases the overall work necessary by
 * the algorithm, since heapsort and quicksort are expensive on small ranges.
 *
 * This implementation of introsort uses the provided STL implementation of
 * heapsort (make_heap, sort_heap) for simplicity, but has its own versions
 * of the quicksort and insertion sort routines.  It is based on David
 * Musser's original paper on introsort (which can be found at
 * http://www.cs.rpi.edu/~musser/gp/introsort.ps), though it does not use
 * directly any of the code it contains.
 
                              Smoothsort
 
 * An implementation of Dijkstra's Smoothsort algorithm, a modification of
 * heapsort that runs in O(n lg n) in the worst case, but O(n) if the data
 * are already sorted.
