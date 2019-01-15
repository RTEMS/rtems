/*
 * Copyright (c) 2014 Chris Johns <chrisj@rtems.org>.  All rights reserved.
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rtems.org/license/LICENSE.
 */

#include <stdio.h>

#include <dlfcn.h>

#include "dl-load.h"

#include <rtems/rtl/rtl-shell.h>
#include <rtems/rtl/rtl-trace.h>

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
                      RTEMS_RTL_TRACE_DEPENDENCY)
 #define DL_DEBUG_TRACE DEBUG_TRACE /* RTEMS_RTL_TRACE_ALL */
 #define DL_RTL_CMDS    1
#else
 #define DL_DEBUG_TRACE 0
 #define DL_RTL_CMDS    0
#endif

static void dl_load_dump (void)
{
#if DL_RTL_CMDS
  char* list[] = { "rtl", "list", NULL };
  char* sym[] = { "rtl", "sym", NULL };
  printf ("RTL List:\n");
  rtems_rtl_shell_command (2, list);
  printf ("RTL Sym:\n");
  rtems_rtl_shell_command (2, sym);
#endif
}

typedef int (*call_t)(int argc, const char* argv[]);

static const char* call_1[] = { "Line 1", "Line 2" };
static const char* call_2[] = { "Call 2, line 1",
                                "Call 2, line 2",
                                "Call 2, line 3" };

int dl_load_test(void)
{
  void*  handle;
  call_t call;
  int    call_ret;
  int    unresolved;
  char*  message = "loaded";

#if DL_DEBUG_TRACE
  rtems_rtl_trace_set_mask (DL_DEBUG_TRACE);
#endif

  printf("load: /dl01-o1.o\n");

  handle = dlopen ("/dl01-o1.o", RTLD_NOW | RTLD_GLOBAL);
  if (!handle)
  {
    printf("dlopen failed: %s\n", dlerror());
    return 1;
  }

  if (dlinfo (handle, RTLD_DI_UNRESOLVED, &unresolved) < 0)
    message = "dlinfo error checking unresolved status";
  else if (unresolved)
    message = "has unresolved externals";

  printf ("handle: %p %s\n", handle, message);

  dl_load_dump ();

  call = dlsym (handle, "rtems_main");
  if (call == NULL)
  {
    printf("dlsym failed: symbol not found\n");
    return 1;
  }

  call_ret = call (2, call_1);
  if (call_ret != 2)
  {
    printf("dlsym call failed: ret value bad\n");
    return 1;
  }

  call_ret = call (3, call_2);
  if (call_ret != 3)
  {
    printf("dlsym call failed: ret value bad\n");
    return 1;
  }

  if (dlclose (handle) < 0)
  {
    printf("dlclose failed: %s\n", dlerror());
    return 1;
  }

  printf ("handle: %p closed\n", handle);

  return 0;
}
