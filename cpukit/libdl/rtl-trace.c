/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup rtems_rtld
 *
 * @brief RTEMS Run-Time Link Editor Trace
 *
 * A configurable tracer for the RTL. See the header file for the enable and
 * disable.
 */

/*
 *  COPYRIGHT (c) 2012 Chris Johns <chrisj@rtems.org>
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

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <stdio.h>
#include <string.h>

#include <rtems/rtl/rtl-trace.h>

#if RTEMS_RTL_TRACE
static rtems_rtl_trace_mask rtems_rtl_trace_flags;

bool
rtems_rtl_trace (rtems_rtl_trace_mask mask)
{
  bool result = false;
  if (mask & rtems_rtl_trace_flags)
    result = true;
  return result;
}

rtems_rtl_trace_mask
rtems_rtl_trace_set_mask (rtems_rtl_trace_mask mask)
{
  rtems_rtl_trace_mask state = rtems_rtl_trace_flags;
  rtems_rtl_trace_flags |= mask;
  return state;
}

rtems_rtl_trace_mask
rtems_rtl_trace_clear_mask (rtems_rtl_trace_mask mask)
{
  rtems_rtl_trace_mask state = rtems_rtl_trace_flags;
  rtems_rtl_trace_flags &= ~mask;
  return state;
}

int
rtems_rtl_trace_shell_command (const rtems_printer* printer,
                               int                  argc,
                               char*                argv[])
{
  const char* table[] =
  {
    "detail",
    "warning",
    "load",
    "unload",
    "section",
    "symbol",
    "reloc",
    "global-sym",
    "load-sect",
    "allocator",
    "unresolved",
    "cache",
    "archives",
    "archive-syms",
    "dependency",
    "bit-alloc"
  };

  rtems_rtl_trace_mask set_value = 0;
  rtems_rtl_trace_mask clear_value = 0;
  bool                 set = true;
  int                  arg;
  int                  t;

  for (arg = 1; arg < argc; arg++)
  {
    if (argv[arg][0] == '-')
    {
      switch (argv[arg][1])
      {
        case 'h':
          rtems_printf (printer, "usage: %s [-hl] [set/clear] [flags]\n", argv[0]);
          return 0;
        case 'l':
          rtems_printf (printer, "%s: valid flags to set or clear are:\n", argv[0]);
          for (t = 0; t < (sizeof (table) / sizeof (const char*)); t++)
            rtems_printf (printer, "  %s\n", table[t]);
          return 0;
        default:
          rtems_printf (printer, "error: unknown option\n");
          return 1;
      }
    }
    else
    {
      if (strcmp (argv[arg], "set") == 0)
        set = true;
      if (strcmp (argv[arg], "clear") == 0)
        set = false;
      else if (strcmp (argv[arg], "all") == 0)
      {
        if (set)
          set_value = RTEMS_RTL_TRACE_ALL;
        else
          clear_value = RTEMS_RTL_TRACE_ALL;
      }
      else
      {
        for (t = 0; t < (sizeof (table) / sizeof (const char*)); t++)
        {
          if (strcmp (argv[arg], table[t]) == 0)
          {
            if (set)
              set_value = 1 << t;
            else
              clear_value = 1 << t;
            break;
          }
        }
      }

      rtems_rtl_trace_flags |= set_value;
      rtems_rtl_trace_flags &= ~clear_value;
    }
  }

  return 0;
}

#endif
