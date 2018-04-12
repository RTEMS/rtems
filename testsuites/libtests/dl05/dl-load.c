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
  void        (*func)(bool );
#if __i386__
  /*
   * std::runtime_error destructor locks up in atomics.
   */
  bool        throw_runtime = false;
#else
  bool        throw_runtime = true;
#endif

  rtems_rtl_trace_set_mask(RTEMS_RTL_TRACE_ALL);

  handle = dlopen("/dl05-o5.o", RTLD_GLOBAL | RTLD_NOW);
  if (handle == NULL)
  {
    err = dlerror();
    if (err != NULL)
      printf("dlopen: %s\n", err);
  }
  rtems_test_assert(handle != NULL);

  func = dlsym(handle, "exception_dl");
  if (func == NULL) {
    err = dlerror ();
    if (err)
      printf ("dlsym: %s\n", err);
  }
  rtems_test_assert(func != NULL);

  exception_base(throw_runtime);

  func(throw_runtime);

  rtems_test_assert(dlclose(handle) == 0);

  return 0;
}
