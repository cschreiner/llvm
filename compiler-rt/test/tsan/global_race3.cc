// RUN: %clangxx_tsan -O1 %s -o %t && %deflake %run %t | FileCheck %s
#include "test.h"

namespace XXX {
  struct YYY {
    static int ZZZ[10];
  };
  int YYY::ZZZ[10];
}

void *Thread(void *a) {
  barrier_wait(&barrier);
  XXX::YYY::ZZZ[0] = 1;
  return 0;
}

int main() {
  barrier_init(&barrier, 2);
  // On FreeBSD, the %p conversion specifier works as 0x%x and thus does not
  // match to the format used in the diagnotic message.
  fprintf(stderr, "addr3=0x%012lx\n", (unsigned long) XXX::YYY::ZZZ);
  pthread_t t;
  pthread_create(&t, 0, Thread, 0);
  XXX::YYY::ZZZ[0] = 0;
  barrier_wait(&barrier);
  pthread_join(t, 0);
}

// CHECK: addr3=[[ADDR3:0x[0-9,a-f]+]]
// CHECK: WARNING: ThreadSanitizer: data race
// CHECK: Location is global 'XXX::YYY::ZZZ' of size 40 at [[ADDR3]] ({{.*}}+0x{{[0-9,a-f]+}})
