/*
 *  Simple debug test program.
 */

#include <stdio.h>

#include "serial_gdb.h"

void debug_test(void)
{
  unsigned i, sum;

  /* Initialize debugging support and insert breakpoint. */

  init_serial_gdb();

  breakpoint();

  sum = 1;
  for(i=1; i <= 100; i++) {
    sum += sum % i;
    printf("Current sum = %d\n", sum);
  }
}
