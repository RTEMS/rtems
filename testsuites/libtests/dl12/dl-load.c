/* SPDX-License-Identifier: BSD-2-Clause */

/*
 * Copyright (c) 2014 Chris Johns <chrisj at rtems.org>.  All rights reserved.
 * Copyright (c) 2019 On-Line Applications Research
 * Copyright (c) 2024 Ranulfo Raphael
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

typedef const void *(*call_t)(void);

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
  void*         handle;
  call_t        call;
  const void*   dl_o1_str_addr;
  const void*   dl_o2_str_addr;

#if DL_DEBUG_TRACE
  rtems_rtl_trace_set_mask (DL_DEBUG_TRACE);
#endif

  handle = dl_load_obj("/dl12-inc.o");
  if (!handle)
    return 1;

#if DL_RTL_CMDS
  {
    char* list[] = { "rtl", "list", NULL };
    rtems_rtl_shell_command (2, list);
    char* sym[] = { "rtl", "sym", NULL };
    rtems_rtl_shell_command (2, sym);
  }
#endif

  call = dlsym (handle, "dl_o1_func");
  if (call == NULL)
  {
    printf("dlsym failed: symbol not found\n");
    return 1;
  }

  printf("\nAbout to call dl_o1_func at address %p\n", call);

  dl_o1_str_addr = call ();

  printf("\ndl_o1_func string literal address: %p\n", dl_o1_str_addr);

  call = dlsym (handle, "dl_o2_func");
  if (call == NULL)
  {
    printf("dlsym failed: symbol not found\n");
    return 1;
  }

  printf("\nAbout to call dl_o2_func at address %p\n", call);

  dl_o2_str_addr = call ();

  printf("\ndl_o2_func string literal address: %p\n", dl_o2_str_addr);

  if (dl_o1_str_addr == dl_o2_str_addr) {
    printf("\nTwo symbols from different object files have the same address!\n");
    return 1;
  }

  if (dlclose (handle) < 0)
  {
    printf("dlclose obj failed: %s\n", dlerror());
    return 1;
  }

  printf ("\nhandle: %p closed\n", handle);

  return 0;
}
