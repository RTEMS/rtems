/*
 * Copyright (c) 2014 Chris Johns <chrisj@rtems.org>.  All rights reserved.
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rtems.org/license/LICENSE.
 */

#include "dl-o2.h"

#include <rtems/test.h>

#define printf(...) rtems_printf(&rtems_test_printer, __VA_ARGS__);

int dl_o2_func1 (int argc, char* argv[])
{
  int arg;
  printf("Loaded module: argc:%d [%s]\n", argc, __FILE__);
  for (arg = 0; arg < argc; ++arg)
    printf("  %d: %s\n", arg, argv[arg]);
  return argc;
}

double dl_o2_func2 (double d1, double d2)
{
  return d1 * d2;
}

int dl_o2_func3 (dl_o2_call_t callback, int count)
{
  return callback ("string in dl_o2", count + 1);
}

