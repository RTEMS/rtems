/* SPDX-License-Identifier: BSD-2-Clause */

/*
 * Copyright (c) 2014 Chris Johns <chrisj@rtems.org>.  All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
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
  if (!o2)
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
