/*
 * Copyright (c) 2018 Chris Johns <chrisj@rtems.org>.  All rights reserved.
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rtems.org/license/LICENSE.
 */

#include "dl06-o2.h"

#include <dlfcn.h>
#include <math.h>

#include <rtems/test.h>

#define printf(...) rtems_printf(&rtems_test_printer, __VA_ARGS__);

typedef void (*func1_t)(unsigned short s[7]);

static void* find_sym(const char* name)
{
  void* sym = dlsym(RTLD_DEFAULT, name);
  if (sym == NULL)
    printf("dlsym failed: not found: %s\n", name);
  return sym;
}

/*
 * Yes a decl in the source. This is a modules main and I could not find which
 * header main is defined in.
 */
int rtems_main (int argc, const char* argv[]);

int rtems_main (int argc, const char* argv[])
{
  func1_t        f1;
  double         d;
  unsigned short s[7] = { 12, 34, 56, 78, 90, 13, 57 };

  printf("Loaded module: argc:%d [%s]\n", argc, __FILE__);

  f1 = find_sym ("dl_o2_func1");
  if (f1 == NULL)
    return 0;

  f1 (s);

  d = dl_o2_func2 (7.1, 33.0);
  (void) d;

  d = dl_o2_func3 (0.778899);
  (void) d;


  return argc;
}
