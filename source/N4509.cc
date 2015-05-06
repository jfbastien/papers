#include <atomic>
#include <iostream>

// It is implementation-defined what this returns, as long as:
// if (atomic<T>::is_lock_free())
//   assert(is_always_lock_free<T>::value);
template<typename T> struct is_always_lock_free { enum { value = false }; };

// Implementations must match the C ATOMIC_*_LOCK_FREE macro values.
template<> struct is_always_lock_free<bool> { enum { value = 2 == ATOMIC_BOOL_LOCK_FREE }; };
template<> struct is_always_lock_free<char> { enum { value = 2 == ATOMIC_CHAR_LOCK_FREE }; };
template<> struct is_always_lock_free<char16_t> { enum { value = 2 == ATOMIC_CHAR16_T_LOCK_FREE }; };
template<> struct is_always_lock_free<char32_t> { enum { value = 2 == ATOMIC_CHAR32_T_LOCK_FREE }; };
template<> struct is_always_lock_free<wchar_t> { enum { value = 2 == ATOMIC_WCHAR_T_LOCK_FREE }; };
template<> struct is_always_lock_free<short> { enum { value = 2 == ATOMIC_SHORT_LOCK_FREE }; };
template<> struct is_always_lock_free<int> { enum { value = 2 == ATOMIC_INT_LOCK_FREE }; };
template<> struct is_always_lock_free<long> { enum { value = 2 == ATOMIC_LONG_LOCK_FREE }; };
template<> struct is_always_lock_free<long long> { enum { value = 2 == ATOMIC_LLONG_LOCK_FREE }; };
template<typename T> struct is_always_lock_free<T*> { enum { value = 2 == ATOMIC_POINTER_LOCK_FREE }; };

int main() {

  std::cout <<
    "bool\t" << is_always_lock_free<bool>::value << '\n' <<
    "char\t" << is_always_lock_free<char>::value << '\n' <<
    "char16_t\t" << is_always_lock_free<char16_t>::value << '\n' <<
    "char32_t\t" << is_always_lock_free<char32_t>::value << '\n' <<
    "wchar_t\t" << is_always_lock_free<wchar_t>::value << '\n' <<
    "short\t" << is_always_lock_free<short>::value << '\n' <<
    "int\t" << is_always_lock_free<int>::value << '\n' <<
    "long\t" << is_always_lock_free<long>::value << '\n' <<
    "long long\t" << is_always_lock_free<long long>::value << '\n' <<
    "void*\t" << is_always_lock_free<void*>::value << '\n';

  return 0;
}
