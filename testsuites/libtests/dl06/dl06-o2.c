/*
 * Copyright (c) 2018 Chris Johns <chrisj@rtems.org>.  All rights reserved.
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rtems.org/license/LICENSE.
 */

#include "dl06-o2.h"

#include <stdlib.h>
#include <math.h>

#include <rtems/test.h>

#define printf(...) rtems_printf(&rtems_test_printer, __VA_ARGS__);

/*
 * Call function that are not part of the RTEMS kernel base image.
 */

void dl_o2_func1 (unsigned short s[7])
{
  printf("libm: lcong48\n")
  lcong48 (s);
}

double dl_o2_func2 (double d1, double d2)
{
  printf("libm: atan2\n")
  return atan2 (d1, d2);
}

double dl_o2_func3 (double d)
{
  printf("libm: tan\n")
  return tan (d);
}
