/*
 *  COPYRIGHT (c) 2012, 2018 Chris Johns <chrisj@rtems.org>
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 */
/**
 * @file
 *
 * @ingroup rtems_rtld
 *
 * @brief RTEMS Run-Time Link Editor Dynamic Loading API Shell Support.
 *
 * Shell command wrappers for the Dynamic Loading API.
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <dlfcn.h>
#include <rtems/rtl/dlfcn-shell.h>

static void*
convert_ascii_to_voidp (const char* arg)
{
  if (strcmp (arg, "base") == 0)
    return RTLD_DEFAULT;
  return (void*) strtoul (arg, NULL, 16);
}

int
shell_dlopen (int argc, char* argv[])
{
  int arg;
  for (arg = 1; arg < argc; arg++)
  {
    void* handle = dlopen (argv[arg], RTLD_NOW | RTLD_GLOBAL);
    if (handle)
    {
      int   unresolved;
      char* message = "loaded";
      if (dlinfo (handle, RTLD_DI_UNRESOLVED, &unresolved) < 0)
        message = "dlinfo error checking unresolved status";
      else if (unresolved)
        message = "has unresolved externals";
      printf ("handle: %p %s\n", handle, message);
    }
    else
      printf ("error: %s\n", dlerror ());
  }
  return 0;
}

int
shell_dlclose (int argc, char* argv[])
{
  return 0;
}

static bool
lookup_dlsym (void** value, int argc, char* argv[])
{
  if (argc >= 3)
  {
    void* handle = convert_ascii_to_voidp (argv[1]);
    if (handle)
    {
      *value = dlsym (handle, argv[2]);
      if (*value)
        return true;
      else
        printf ("error: invalid handle or symbol\n");
    }
    else
      printf ("error: invalid handle");
  }
  else
    printf ("error: requires handle and symbol name\n");
  return false;
}

int
shell_dlsym (int argc, char* argv[])
{
  void* value;
  if (lookup_dlsym (&value, argc, argv))
  {
    printf ("%s = %p\n", argv[2], value);
    return 0;
  }
  return -1;
}

typedef int (*call_p)(int argc, char* argv[]);

int
shell_dlcall (int argc, char* argv[])
{
  void* value;
  if (lookup_dlsym (&value, argc, argv))
  {
    call_p call = value;
    int    r;
    printf ("(*%p)(%d, ....)\n", value, argc - 3);
    r = call (argc - 3, argv + 3);
    printf ("return %d\n", r);
    return 0;
  }
  return -1;
}
