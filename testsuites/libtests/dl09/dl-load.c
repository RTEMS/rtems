/*
 * Copyright (c) 2019 Chris Johns <chrisj@rtems.org>.
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
                      RTEMS_RTL_TRACE_DEPENDENCY | \
                      RTEMS_RTL_TRACE_LOAD_SECT | \
                      RTEMS_RTL_TRACE_BIT_ALLOC)
 #define DL09_DEBUG_TRACE DEBUG_TRACE /* RTEMS_RTL_TRACE_ALL */
 #define DL09_RTL_CMDS    1
#else
 #define DL09_DEBUG_TRACE 0
 #define DL09_RTL_CMDS    0
#endif

#include <dlfcn.h>

#include "dl-load.h"

#include <tmacros.h>

#include <rtems/malloc.h>
#include <rtems/rtl/rtl-shell.h>
#include <rtems/rtl/rtl-trace.h>

extern void rtems_shell_print_heap_info(
  const char             *c,
  const Heap_Information *h
);

typedef struct
{
  void* handle;
  void* space;
} objects;

typedef struct
{
  const char* name;
  bool        has_unresolved;
  size_t      space;
} object_def;

#define MBYTES(_m) ((_m) * 1024UL * 1024UL)
#define NUMOF(_a)  (sizeof(_a) / sizeof(_a[0]))

typedef int (*call_sig)(void);

static void dl_load_dump (void)
{
#if DL09_RTL_CMDS
  char* list[] = { "rtl", "list", NULL };
  char* sym[] = { "rtl", "sym", NULL };
  Heap_Information_block info;
  malloc_info( &info );
  printf ("RTL List:\n");
  rtems_rtl_shell_command (2, list);
  printf ("RTL Sym:\n");
  rtems_rtl_shell_command (2, sym);
  printf ("Malloc:\n");
  rtems_shell_print_heap_info("free", &info.Free);
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
  printf ("handel: %p: %sunresolved externals\n",
          handle, unresolved != 0 ? "" : "no ");
}

static void* dl_load_obj (const char* name, bool has_unresolved)
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

static void dl_object_open (object_def* od, objects* o)
{
  o->handle = dl_load_obj(od->name, od->has_unresolved);
  rtems_test_assert (o->handle != NULL);
  if (!od->has_unresolved)
    dl_check_resolved (o->handle, false);
  if (od->space != 0)
  {
    o->space = malloc (od->space);
    printf("space alloc: %s: %d: %p\n", od->name, od->space, o->space);
    rtems_test_assert (o->space != NULL);
  }
  dl_load_dump ();
}

static void dl_object_close (objects* o)
{
  dl_close (o->handle);
  printf("space dealloc: %p\n", o->space);
  free (o->space);
}

int dl_load_test(void)
{
  object_def od[5] = { { "/dl09-o1.o", true,  MBYTES(32) },
                       { "/dl09-o2.o", true,  MBYTES(32) },
                       { "/dl09-o3.o", true,  MBYTES(32) },
                       { "/dl09-o4.o", true,  MBYTES(32) },
                       { "/dl09-o5.o", false, 0          } };
  objects o[5] = { 0 };
  size_t  i;

  printf ("Test source (link in strstr): %s\n", dl_localise_file (__FILE__));

#if DL09_DEBUG_TRACE
  rtems_rtl_trace_set_mask (DL09_DEBUG_TRACE);
#endif

  for (i = 0; i < NUMOF(od); ++i)
    dl_object_open (&od[i], &o[i]);

  dl_load_dump ();

  /*
   * Check for any resolved externals.
   */
  printf ("Check is any unresolved externals exist:\n");
  dl_check_resolved (RTLD_SELF, false);

  printf ("Running rtems_main_o1:\n");
  if (dl_call (o[0].handle, "rtems_main_o1"))
    return 1;

  for (i = 0; i < NUMOF (od); ++i)
    dl_object_close (&o[i]);

  return 0;
}
