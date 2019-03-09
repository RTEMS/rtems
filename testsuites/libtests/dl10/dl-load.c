/*
 * Copyright (c) 2014 Chris Johns <chrisj@rtems.org>.
 * All rights reserved.
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rtems.org/license/LICENSE.
 */

#define TEST_TRACE 0
#if TEST_TRACE
 #define DEBUG_TRACE (RTEMS_RTL_TRACE_DETAIL | \
                      RTEMS_RTL_TRACE_WARNING | \
                      RTEMS_RTL_TRACE_LOAD | \
                      RTEMS_RTL_TRACE_UNLOAD | \
                      RTEMS_RTL_TRACE_SYMBOL | \
                      RTEMS_RTL_TRACE_RELOC | \
                      RTEMS_RTL_TRACE_LOAD_SECT | \
                      RTEMS_RTL_TRACE_ALLOCATOR | \
                      RTEMS_RTL_TRACE_UNRESOLVED | \
                      RTEMS_RTL_TRACE_ARCHIVES | \
                      RTEMS_RTL_TRACE_DEPENDENCY)
 /* RTEMS_RTL_TRACE_ALL */
#define DL_DEBUG_TRACE DEBUG_TRACE
#define DL_DEBUG_CMD   1
#else
#define DL_DEBUG_TRACE 0
#define DL_DEBUG_CMD    0
#endif

#include <dlfcn.h>

#include "dl-load.h"

#include <tmacros.h>

#include <rtems/rtl/rtl-shell.h>
#include <rtems/rtl/rtl-trace.h>

typedef int (*call_sig)(void);

static void dl_load_dump (void)
{
#if DL_DEBUG_CMD
  char* list[] = { "rtl", "list", NULL };
  char* sym[] = { "rtl", "sym", NULL };
  printf ("RTL List:\n");
  rtems_rtl_shell_command (2, list);
  printf ("RTL Sym:\n");
  rtems_rtl_shell_command (2, sym);
#endif
}

static bool dl_check_resolved(void* handle, bool has_unresolved)
{
  int unresolved = 0;
  if (dlinfo (handle, RTLD_DI_UNRESOLVED, &unresolved) == 0)
    return 1;
  if (has_unresolved)
  {
    if (unresolved == 0)
    {
      dl_load_dump();
      return false;
    }
  }
  else
  {
    if (unresolved != 0)
    {
      dl_load_dump();
      return false;
    }
  }
  printf ("handel: %p: no unresolved externals\n", handle);
  return true;
}

static void* dl_load_obj(const char* name, bool has_unresolved)
{
  void* handle;

  printf("load: %s\n", name);

  handle = dlopen (name, RTLD_NOW | RTLD_GLOBAL);
  if (!handle)
  {
    printf("dlopen failed: %s\n", dlerror());
    return NULL;
  }

  dl_check_resolved (handle, has_unresolved);

  printf ("handle: %p loaded\n", handle);

  return handle;
}

static void dl_close (void* handle)
{
  int r;
  printf ("handle: %p closing\n", handle);
  r = dlclose (handle);
  if (r != 0)
    printf("dlclose failed: %s\n", dlerror());
  rtems_test_assert (r == 0);
}

static int dl_call (void* handle, const char* func)
{
  call_sig call = dlsym (handle, func);
  if (call == NULL)
  {
    printf("dlsym failed: symbol not found: %s\n", func);
    return 1;
  }
  call ();
  return 0;
}

int dl_load_test(void)
{
  void* o1;

  printf ("Test source (link in strstr): %s\n", dl_localise_file (__FILE__));

#if DL_DEBUG_TRACE
  rtems_rtl_trace_set_mask (DL_DEBUG_TRACE);
#endif

  o1 = dl_load_obj("/dl10-o1.o", false);
  if (!o1)
    return 1;

  if (!dl_check_resolved (o1, false))
    return 1;

  dl_load_dump ();

  return 0;
}
