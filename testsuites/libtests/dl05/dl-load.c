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

#define TEST_TRACE 0
#if TEST_TRACE
 #define DEBUG_TRACE (RTEMS_RTL_TRACE_DETAIL | \
                      RTEMS_RTL_TRACE_WARNING | \
                      RTEMS_RTL_TRACE_LOAD | \
                      RTEMS_RTL_TRACE_UNLOAD | \
                      RTEMS_RTL_TRACE_SYMBOL | \
                      RTEMS_RTL_TRACE_RELOC | \
                      RTEMS_RTL_TRACE_ALLOCATOR | \
                      RTEMS_RTL_TRACE_UNRESOLVED | \
                      RTEMS_RTL_TRACE_ARCHIVES | \
                      RTEMS_RTL_TRACE_GLOBAL_SYM | \
                      RTEMS_RTL_TRACE_DEPENDENCY)
 #define DL_DEBUG_TRACE DEBUG_TRACE /* RTEMS_RTL_TRACE_ALL */
#else
 #define DL_DEBUG_TRACE 0
#endif

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

#if DL_DEBUG_TRACE
  rtems_rtl_trace_set_mask (DL_DEBUG_TRACE);
#endif

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
