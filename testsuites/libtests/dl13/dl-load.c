/* SPDX-License-Identifier: BSD-2-Clause */

/*
 * Copyright (C) 2024 On-Line Applications Research (OAR). All rights reserved.
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
#include "dl13-arm_mode.h"

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

typedef void (*call_t)(void);

int dl_load_test(void)
{
  void*  handle;
  call_t call;
  int    unresolved;
  char*  message = "loaded";

  printf("About to call arm_mode_func from %s\n", __func__);
  arm_mode_func();

#if DL_DEBUG_TRACE
  rtems_rtl_trace_set_mask (DL_DEBUG_TRACE);
#endif

  printf("load: /dl13-o1.o\n");

  handle = dlopen ("/dl13-o1.o", RTLD_NOW | RTLD_GLOBAL);
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

  call = dlsym (handle, "dl_o1_func");
  if (call == NULL)
  {
    printf("dlsym failed: symbol not found\n");
    return 1;
  }

  printf ("Calling dl_o1_func!\n");
  call ();

  if (dlclose (handle) < 0)
  {
    printf("dlclose failed: %s\n", dlerror());
    return 1;
  }

  printf ("handle: %p closed\n", handle);

  return 0;
}
