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
 #define DL_DEBUG_TRACE (RTEMS_RTL_TRACE_ALL & ~RTEMS_RTL_TRACE_ALLOCATOR)
 #define DL_RTL_CMDS    1
#else
 #define DL_DEBUG_TRACE 0
 #define DL_RTL_CMDS    0
#endif

typedef int (*call_t)(int argc, const char* argv[]);

static const char* call_args[] = { "1", "2", "3", "4" };

static void* dl_load_obj(const char* name)
{
  void* handle;
  int   unresolved;
  char* message = "loaded";

  printf("load: %s\n", name);

  handle = dlopen (name, RTLD_NOW | RTLD_GLOBAL);
  if (!handle)
  {
    printf("dlopen failed: %s\n", dlerror());
    return NULL;
  }

  if (dlinfo (handle, RTLD_DI_UNRESOLVED, &unresolved) < 0)
    message = "dlinfo error checking unresolved status";
  else if (unresolved)
    message = "has unresolved externals";

  printf ("handle: %p %s\n", handle, message);

  return handle;
}

int dl_load_test(void)
{
  void*  o1;
  void*  o2;
  call_t call;
  int    call_ret;
  int    ret;

#if DL_DEBUG_TRACE
  rtems_rtl_trace_set_mask (DL_DEBUG_TRACE);
#endif

  o1 = dl_load_obj("/dl02-o1.o");
  if (!o1)
    return 1;
  o2 = dl_load_obj("/dl02-o2.o");
  if (!o1)
    return 1;

#if DL_RTL_CMDS
  {
    char* list[] = { "rtl", "list", NULL };
    rtems_rtl_shell_command (2, list);
    char* sym[] = { "rtl", "sym", NULL };
    rtems_rtl_shell_command (2, sym);
  }
#endif

  call = dlsym (o1, "rtems_main");
  if (call == NULL)
  {
    printf("dlsym failed: symbol not found\n");
    return 1;
  }

  call_ret = call (4, call_args);
  if (call_ret != 4)
  {
    printf("dlsym call failed: ret value bad\n");
    return 1;
  }

  ret = 0;

  if (dlclose (o1) < 0)
  {
    printf("dlclose o1 failed: %s\n", dlerror());
    ret = 1;
  }

  printf ("handle: %p closed\n", o1);

  if (dlclose (o2) < 0)
  {
    printf("dlclose o1 failed: %s\n", dlerror());
    ret = 1;
  }

  printf ("handle: %p closed\n", o2);

  return ret;
}
