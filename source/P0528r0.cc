#include <atomic>
#include <cstring>
#include <new>
#include <stdio.h>
#include <type_traits>

struct Padded {
  char c = 0xFF;
  // Padding here.
  int i = 0xFEEDFACE;
  Padded() = default;
};
typedef std::atomic<Padded> Atomic;
typedef std::aligned_storage<sizeof(Atomic)>::type Storage;

void peek(const char* what, void *into) {
  printf("%16s %08x %08x\n", what, *(int*)into, *(1 + (int*)into));
}

Storage* create() {
  auto* storage = new Storage();
  std::memset(storage, 0xBA, sizeof(Storage));
  asm volatile("":::"memory");
  peek("storage", storage);
  return storage;
}

Atomic* change(Storage* storage) {
  // As if we used an allocator which reuses memory.
  auto* atomic = new(storage) Atomic;
  peek("atomic placed", atomic);
  std::atomic_init(atomic, Padded()); // Which bits go in?
  peek("atomic init", atomic);
  return atomic;
}

Padded infloop_maybe(Atomic* atomic) {
  Padded desired;  // Padding unknown.
  Padded expected; // Could be different.
  peek("desired before", &desired);
  peek("expected before", &expected);
  peek("atomic before", atomic);
  while (
    !atomic->compare_exchange_strong(
      expected,
      desired // Padding bits added and removed here ˙ ͜ʟ˙
  ));
  peek("expected after", &expected);
  peek("atomic after", atomic);
  return expected; // Maybe changed here as well.
}

int main() {
  auto* storage = create();
  auto* atomic = change(storage);
  Padded p = infloop_maybe(atomic);
  peek("main", &p);
  return 0;
}
