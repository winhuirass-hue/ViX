#ifndef _STDLIB_H
#define _STDLIB_H

#include <stddef.h>
#include "malloc.h"
#include "string.h"

#define EXIT_SUCCESS 0
#define EXIT_FAILURE 1

static inline void abort(void) {
  for (;;) {
    __asm__ __volatile__("cli; hlt");
  }
}

#endif
