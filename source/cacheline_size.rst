=====================================================
Nxxx ``constexpr std::thread::likely_cacheline_size``
=====================================================

---------
Rationale
---------

Specific architecture implementations have a cacheline size which never
changes. Indeed, this number is deeply embedded in the caching hardware's
implementation, yet C++ doesn't expose this number as a ``constexpr``. This is
partly due to this cacheline size changing from one architecture implementation
to another within the same Instruction Set Architecture. Knowing which ISA the
compiler targets is therefore insufficient to get an exact cacheline size: one
has to specify a specific architecture implementation.

The current methods to obtain the cacheline size are all runtime methods, not
``constexpr`` values, and are either:

* ISA-dependent;
* OS-dependent;
* Rely on cache timing.

Having a ``constexpr`` value is useful, even when the ``constexpr`` is only a
likely good value instead of the exact one: this allows setting ``alignas`` on
datastructures. It isn't a guarantee, merely a good guess at what would
work. This is useful to separate members in a class which have different access
patterns based on the algorithm. A developer can reason about the caching
protocol, e.g. MOESI, and group members which the algorithms makes exclusive to
a thread of execution from elements which are shared between threads of
execution.

Typical examples where this matters are:

* Multiple-producer and multiple-consumer queue.
* The ``barrier`` example, as illustrated in Nxxx.
* Grouping an atomic variable with the data it protects.
* Generally, preventing false-sharing.

.. TODO update paper number above.

The only significant cacheline size in these circumstances is L1 cacheline size.

Nowadays, developers either hard code a likely number, such as ``64`` bytes, or
figure out exactly the right number by consulting their specific machine's
manual. The implementation is better positioned to do the exact same thing at
compile-time, we therefore propose to standardize existing practice, moving the
magic number from each developer's code base to each library implementation. A
precise value can still be obtained from the operating system or using inline
assembly at runtime, which can come in handy for allocator implementations which
can guarantee alignment.

-----------------
Proposed addition
-----------------

We propose adding the following to the standard:

Under 30.3.1 Class ``thread`` [**thread.thread.class**]:

.. code-block:: c++

  namespace std {
    class thread {
      // ...
    public:
      static constexpr size_t likely_cacheline_size = /* implementation-defined */;
      // ...
    };
  }

Under 30.3.1.6 ``thread`` static members [**thread.thread.static**]:

``constexpr size_t likely_cacheline_size = /* implementation-defined */;``

[*Note:* This number is a recommended spacing in bytes for the colocation of
objects which are used with high contention. It is also the recommended amount
of contiguous memory that the objects passed to
``std::atomic_thread_fence(std::memory_order, T...&&)`` should occupy for the
highest possible performance under low contention. — *end note*]

-------------------------
OS and ISA specific usage
-------------------------

We informatively list a few ways in which the L1 cacheline size can be obtained
on different operating systems and architectures at runtime.

On OSX one would use:

.. code-block:: c++

  sysctlbyname("hw.cachelinesize", &cacheline_size, &sizeof_cacheline_size, 0, 0)

On Windows one would use:

.. code-block:: c++

  GetLogicalProcessorInformation(&buf[0], &sizeof_buf);
  for (i = 0; i != sizeof_buf / sizeof(SYSTEM_LOGICAL_PROCESSOR_INFORMATION); ++i) {
    if (buf[i].Relationship == RelationCache && buf[i].Cache.Level == 1)
      cacheline_size = buf[i].Cache.LineSize;

On Linux one would either use:

.. code-block:: c++

  p = fopen("/sys/devices/system/cpu/cpu0/cache/index0/coherency_line_size", "r");
  fscanf(p, "%d", &cacheline_size);

or:

.. code-block:: c++

  sysconf(_SC_LEVEL1_DCACHE_LINESIZE);

On x86 one would use the ``CPUID`` Instruction with ``EAX = 80000005h``, which
leaves the result in ``ECX``, which needs further work to extract.

On ARM one would use ``mrs %[ctr], ctr_el0``, which needs further work to
extract.
