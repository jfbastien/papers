#include <atomic>
#include <iostream>

namespace std {

  namespace detail {
    // It is implementation-defined what this returns, as long as:
    //
    // if (std::atomic<T>()::is_lock_free())
    //   assert(std::atomic<T>::is_always_lock_free);
    //
    // An implementation may therefore have more variable template
    // specializations than the ones shown below.
    template<typename T> static constexpr bool is_always_lock_free = false;

    // Implementations must match the C ATOMIC_*_LOCK_FREE macro values.
    template<> static constexpr bool is_always_lock_free<bool> = 2 == ATOMIC_BOOL_LOCK_FREE;
    template<> static constexpr bool is_always_lock_free<char> = 2 == ATOMIC_CHAR_LOCK_FREE;
    template<> static constexpr bool is_always_lock_free<signed char> = 2 == ATOMIC_CHAR_LOCK_FREE;
    template<> static constexpr bool is_always_lock_free<unsigned char> = 2 == ATOMIC_CHAR_LOCK_FREE;
    template<> static constexpr bool is_always_lock_free<char16_t> = 2 == ATOMIC_CHAR16_T_LOCK_FREE;
    template<> static constexpr bool is_always_lock_free<char32_t> = 2 == ATOMIC_CHAR32_T_LOCK_FREE;
    template<> static constexpr bool is_always_lock_free<wchar_t> = 2 == ATOMIC_WCHAR_T_LOCK_FREE;
    template<> static constexpr bool is_always_lock_free<short> = 2 == ATOMIC_SHORT_LOCK_FREE;
    template<> static constexpr bool is_always_lock_free<unsigned short> = 2 == ATOMIC_SHORT_LOCK_FREE;
    template<> static constexpr bool is_always_lock_free<int> = 2 == ATOMIC_INT_LOCK_FREE;
    template<> static constexpr bool is_always_lock_free<unsigned int> = 2 == ATOMIC_INT_LOCK_FREE;
    template<> static constexpr bool is_always_lock_free<long> = 2 == ATOMIC_LONG_LOCK_FREE;
    template<> static constexpr bool is_always_lock_free<unsigned long> = 2 == ATOMIC_LONG_LOCK_FREE;
    template<> static constexpr bool is_always_lock_free<long long> = 2 == ATOMIC_LLONG_LOCK_FREE;
    template<> static constexpr bool is_always_lock_free<unsigned long long> = 2 == ATOMIC_LLONG_LOCK_FREE;
    template<typename T> static constexpr bool is_always_lock_free<T*> = 2 == ATOMIC_POINTER_LOCK_FREE;
    template<> static constexpr bool is_always_lock_free<std::nullptr_t> = 2 == ATOMIC_POINTER_LOCK_FREE;
    // The macros do not support float, double, long double, but C++ does
    // support atomics of these types. An implementation shall ensure that these
    // types, as well as user-defined types, guarantee the above invariant that
    // is_always_lock_free implies is_lock_free for the same type.
  }

  template<typename T>
  struct atomic_n4509 {
    static constexpr bool is_always_lock_free = detail::is_always_lock_free<T>;
  };

}

template<typename T> using atomic = std::atomic_n4509<T>;

int main() {
  std::cout <<
    "bool\t" << atomic<bool>::is_always_lock_free << '\n' <<
    "char\t" << atomic<char>::is_always_lock_free << '\n' <<
    "signed char\t" << atomic<signed char>::is_always_lock_free << '\n' <<
    "unsigned char\t" << atomic<unsigned char>::is_always_lock_free << '\n' <<
    "char16_t\t" << atomic<char16_t>::is_always_lock_free << '\n' <<
    "char32_t\t" << atomic<char32_t>::is_always_lock_free << '\n' <<
    "wchar_t\t" << atomic<wchar_t>::is_always_lock_free << '\n' <<
    "short\t" << atomic<short>::is_always_lock_free << '\n' <<
    "unsigned short\t" << atomic<unsigned short>::is_always_lock_free << '\n' <<
    "int\t" << atomic<int>::is_always_lock_free << '\n' <<
    "unsigned int\t" << atomic<unsigned int>::is_always_lock_free << '\n' <<
    "long\t" << atomic<long>::is_always_lock_free << '\n' <<
    "unsigned long\t" << atomic<unsigned long>::is_always_lock_free << '\n' <<
    "long long\t" << atomic<long long>::is_always_lock_free << '\n' <<
    "unsigned long long\t" << atomic<unsigned long long>::is_always_lock_free << '\n' <<
    "void*\t" << atomic<void*>::is_always_lock_free << '\n' <<
    "std::nullptr_t\t" << atomic<std::nullptr_t>::is_always_lock_free << '\n';

  return 0;
}
