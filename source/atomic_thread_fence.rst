=============================================
Nxxx ``std::atomic_thread_fence(mo, T...&&)``
=============================================
.. TODO Update number above.

:Author: Olivier Giroux
:Contact: ogiroux@nvidia.com
:Author: JF Bastien
:Contact: jfb@google.com
:Date: 2015-05-06
:URL: https://github.com/jfbastien/papers/blob/atomic_thread_fence/source/atomic_thread_fence.rst
.. TODO Update the URL above.
.. TODO Also add OpenMP discussion

Fences allow programmers to express a conservative appromation to the ideal precise pair-wise 
relations of operations required to be ordered in the happens-before relation.  This
is conservative because fences use the sequenced-before relation to select vast extents
of the program into the happens-before relation.  That makes it easy to over-constrain 
the order of memory operations in the implementation of synchronization primitives that 
use more than a single atomic memory location.

The flush primitive of OpenMP is more expressive, more precise, than the fences of C++11 and 
C++14 in at least one sense: it can optionally restrict the memory operations to a user-specified
set of memory locations.  This is often enough for short lock-free algorithms to achieve
the fully-precise expression of pair-wise ordering.  This capability isn’t only relevant in OpenMP
and isn’t only to the benefit of the compiler, but also in expert hands it can unlock the maximum
memory performance out of all popular hardware platforms.  

An example of this important optimization can be seen in a likely implementation of N4392’s
std::barrier object.  This algorithm makes ordered modifications on the atomic sub-objects of a 
larger non-atomic synchronization object, but the internal modifications need only be ordered with
respect to each other, not all surrounding objects.

.. code-block:: c++

  struct barrier {
      // Some member functions elided.
      void arrive_and_wait() {
          int const myepoch = epoch.load(memory_order_relaxed);
          int const result = arrived.fetch_add(1, memory_order_acq_rel) + 1;
          if(result == expected) {
              expected = nexpected.load(memory_order_relaxed);
              arrived.store(0, memory_order_relaxed);
              //only need to order {expected, arrived} -> {epoch}
              epoch.store(myepoch + 1, memory_order_release);
          }
          else
              while(epoch.load(memory_order_acquire) == myepoch)
                  ;
      }
  private:
      int expected;
      atomic<int> arrived, nexpected, epoch;
  };

The release operation on the epoch atomic is likely to require the compiler to
insert a fence that has an effect that goes beyond the intended constraint,
which is to order only the operations on the barrier object. Since the barrier
object is likely to be smaller than a cache line and the library's
implementation can control its alignment using ``alignas``, then it would be
possible to compile this program without a fence in this location on
architectures that are cache-line coherent. To concisely express the bound on
the set of memory operations whose order is constrained, we propose to overload
``std::atomic_thread_fence`` with a variant that takes a reference to the object
containing sub-objects to be ordered by the fence:

.. code-block:: c++

     template<class... T>
     void atomic_thread_fence(memory_order, T... &&objects) noexcept;

For this overload, operations on objects that are not sub-objects of the
object(s) in the variadic template argument(s) are *un-sequenced* with the
fence. The compiler would likely apply restrictions on alignment, and may
generate a dynamic test leading to a fence for under-aligned objects.

In all cases, a trivially conforming implementation may implement the new
overload in terms of the existing ``std::atomic_thread_fence`` using the same
memory order:

.. code-block:: c++

     template<class... T>
     void atomic_thread_fence(memory_order, T... &&) noexcept {
       atomic_thread_fence(memory_order);
     }

The above barrier example's inner-code would use the new overload as follows:

.. code-block:: c++

          if(result == expected) {
              expected = nexpected.load(memory_order_relaxed);
              arrived.store(0, memory_order_relaxed);
	      atomic_thread_fence(memory_order_release, *this);
              epoch.store(myepoch + 1, memory_order_relaxed);
          }

This enables hardware-specific optimizations which cannot be expressed in C++
today. If the synchronized object(s) are know to reside in memory that's not
visible to other threads of execution, then a weaker type of fence than the
hardware's global fence can be used.

We conjecture that this mechanism could also be used to identify transaction
boundaries in hardware implementations which support transactional memory, and
implement subsequent atomic operations as hardware transactions instead.
