/* SPDX-License-Identifier: BSD-2-Clause */

/*
 * Copyright (C) 2023 On-Line Applications Research Corporation (OAR).
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

#include <errno.h>
#include <stdio.h>

#include <dlfcn.h>

#include "dl-load.h"

#include <rtems/rtl/rtl-shell.h>
#include <rtems/rtl/rtl-trace.h>

#define TEST_TRACE 0
#if TEST_TRACE
 #define SHOW_GLOBAL_SYMS 1
 #if SHOW_GLOBAL_SYMS
  #define TRACE_GLOBAL_SYMBOL RTEMS_RTL_TRACE_GLOBAL_SYM
 #else
  #define TRACE_GLOBAL_SYMBOL 0
 #endif
 #define DEBUG_TRACE (RTEMS_RTL_TRACE_DETAIL | \
                      RTEMS_RTL_TRACE_WARNING | \
                      RTEMS_RTL_TRACE_LOAD | \
                      RTEMS_RTL_TRACE_UNLOAD | \
                      TRACE_GLOBAL_SYMBOL | \
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

typedef int (*int_call_t)(void);
typedef int* (*ptr_call_t)(void);

void* get_errno_ptr(void);
int get_errno(void);

int dl_load_test(void)
{
  void*  handle;
  int_call_t int_call;
  ptr_call_t ptr_call;
  int    int_call_ret;
  int*   ptr_call_ret;
  int    unresolved;
  char*  message = "loaded";

#if DL_DEBUG_TRACE
  rtems_rtl_trace_set_mask (DL_DEBUG_TRACE);
#endif

  printf("load: /dl11-o1.o\n");

  handle = dlopen ("/dl11-o1.o", RTLD_NOW | RTLD_GLOBAL);
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

  ptr_call = dlsym (handle, "get_errno_ptr");
  if (ptr_call == NULL)
  {
    printf("dlsym failed: symbol get_errno_ptr not found\n");
    return 1;
  }

  int_call = dlsym (handle, "get_errno_val");
  if (int_call == NULL)
  {
    printf("dlsym failed: symbol get_errno_val not found\n");
    return 1;
  }

  ptr_call_ret = ptr_call ();
  if (ptr_call_ret != get_errno_ptr())
  {
    printf("dlsym ptr_call failed: ret value bad\n");
    return 1;
  }

  errno = 12345;
  int_call_ret = int_call ();
  if (int_call_ret != get_errno())
  {
    printf("dlsym int_call failed: ret value bad\n");
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

/*
 * Disasseble these to see how the platform accesses TLS
 */
void* get_errno_ptr(void)
{
  return &errno;
}

int get_errno(void)
{
  return errno;
}
