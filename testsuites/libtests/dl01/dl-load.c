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
