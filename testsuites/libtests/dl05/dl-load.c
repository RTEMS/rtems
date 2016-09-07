/*
 * Copyright (c) 2016 Chris Johns <chrisj@rtems.org>.  All rights reserved.
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rtems.org/license/LICENSE.
 */

#ifdef HAVE_CONFIG_H
  #include "config.h"
#endif

#include "tmacros.h"

#include <stdio.h>

#include <dlfcn.h>

#include <rtems/rtl/rtl-trace.h>

#include "dl-load.h"

int dl_load_test(void)
{
  void*       handle;
  const char* err;
  void        (*func)(void);

  rtems_rtl_trace_set_mask(RTEMS_RTL_TRACE_ALL);
  handle = dlopen("/dl-o5.o", RTLD_GLOBAL | RTLD_NOW);
  err = dlerror();
  if (err != NULL)
    printf("dlopen: %s\n", err);
  rtems_test_assert(handle != NULL);
  func = dlsym(handle, "exception_dl");
  err = dlerror ();
  if (err)
    printf ("dlsym: %s\n", err);
  rtems_test_assert(func != NULL);

  exception_base(false);
  func();

  rtems_test_assert(dlclose(handle) == 0);

  return 0;
}
