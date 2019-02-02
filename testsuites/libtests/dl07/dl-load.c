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
                      RTEMS_RTL_TRACE_ALLOCATOR | \
                      RTEMS_RTL_TRACE_UNRESOLVED | \
                      RTEMS_RTL_TRACE_ARCHIVES | \
                      RTEMS_RTL_TRACE_GLOBAL_SYM | \
                      RTEMS_RTL_TRACE_DEPENDENCY)
 #define DL_DEBUG_TRACE DEBUG_TRACE /* RTEMS_RTL_TRACE_ALL */
 #define DL_RTL_CMDS    1
#else
 #define DL_DEBUG_TRACE 0
 #define DL_RTL_CMDS    0
#endif

#include <dlfcn.h>

#include "dl-load.h"

#include <tmacros.h>

#include <rtems/rtl/rtl-shell.h>
#include <rtems/rtl/rtl-trace.h>

typedef int (*call_sig)(void);

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

static void dl_check_resolved(void* handle, bool has_unresolved)
{
  int unresolved = 0;
  rtems_test_assert (dlinfo (handle, RTLD_DI_UNRESOLVED, &unresolved) == 0);
  if (has_unresolved)
  {
    if (unresolved == 0)
    {
      dl_load_dump();
      rtems_test_assert (unresolved != 0);
    }
  }
  else
  {
    if (unresolved != 0)
    {
      dl_load_dump();
      rtems_test_assert (unresolved == 0);
    }
  }
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

static void dl_close (void* handle, const char* msg)
{
  int r;
  printf ("%s: handle: %p closing\n", msg, handle);
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
  void* o2;
  void* o3;
  void* o4;
  void* o5;

  printf ("Test source (link in strstr): %s\n", dl_localise_file (__FILE__));

#if DL_DEBUG_TRACE
  rtems_rtl_trace_set_mask (DL_DEBUG_TRACE);
#endif

  o1 = dl_load_obj("/dl07-o1.o", false);
  if (!o1)
    return 1;
  o2 = dl_load_obj("/dl07-o2.o", false);
  if (!o1)
    return 1;
  o3 = dl_load_obj("/dl07-o3.o", true);
  if (!o1)
    return 1;
  o4 = dl_load_obj("/dl07-o4.o", false);
  if (!o1)
    return 1;
  o5 = dl_load_obj("/dl07-o5.o", false);
  if (!o1)
    return 1;

  dl_check_resolved (o3, false);

  dl_load_dump ();

  printf ("\n\nRun mains in each module:\n\n");
  if (dl_call (o1, "rtems_main_o1"))
    return 1;
  if (dl_call (o2, "rtems_main_o2"))
    return 1;
  if (dl_call (o3, "rtems_main_o3"))
    return 1;
  if (dl_call (o4, "rtems_main_o4"))
    return 1;
  if (dl_call (o5, "rtems_main_o5"))
    return 1;

  /*
   * Try and close the dependent modules, we should get an error.
   */
  printf ("unload test: o1\n");
  rtems_test_assert (dlclose (o1) != 0);
  printf ("unload test: o2\n");
  rtems_test_assert (dlclose (o2) != 0);
  printf ("unload test: o4\n");
  rtems_test_assert (dlclose (o4) != 0);
  printf ("unload test: o5\n");
  rtems_test_assert (dlclose (o5) != 0);

  dl_close (o3, "o3");
  rtems_test_assert (dlclose (o1) != 0);
  dl_close (o4, "o4");
  rtems_test_assert (dlclose (o1) != 0);
  dl_close (o5, "o5");
  rtems_test_assert (dlclose (o1) != 0);
  dl_close (o2, "o2");
  dl_close (o1, "o1");

  return 0;
}
