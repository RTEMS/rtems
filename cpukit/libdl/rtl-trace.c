/*
 *  COPYRIGHT (c) 2012 Chris Johns <chrisj@rtems.org>
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
 * @brief RTEMS Run-Time Link Editor Trace
 *
 * A configurable tracer for the RTL. See the header file for the enable and
 * disable.
 */

#if HAVE_CONFIG_H
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
