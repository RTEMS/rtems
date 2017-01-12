/**
 * @file
 *
 * @brief CPU Usage Report
 * @ingroup libmisc_cpuuse CPU Usage
 */

/*
 *  COPYRIGHT (c) 1989-2010.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#include <inttypes.h>

#include <rtems/cpuuse.h>
#include <rtems/printer.h>
#include <rtems/score/threadimpl.h>
#include <rtems/score/todimpl.h>

#include "cpuuseimpl.h"

typedef struct {
  const rtems_printer *printer;
  Timestamp_Control    total;
  Timestamp_Control    uptime_at_last_reset;
} cpu_usage_context;

static bool cpu_usage_visitor( Thread_Control *the_thread, void *arg )
{
  cpu_usage_context *ctx;
  char               name[ 38 ];
  uint32_t           ival;
  uint32_t           fval;
  Timestamp_Control  uptime;
  Timestamp_Control  used;
  uint32_t           seconds;
  uint32_t           nanoseconds;

  ctx = arg;
  _Thread_Get_name( the_thread, name, sizeof( name ) );

  _Thread_Get_CPU_time_used( the_thread, &used );
  _TOD_Get_uptime( &uptime );
  _Timestamp_Subtract( &ctx->uptime_at_last_reset, &uptime, &ctx->total );
  _Timestamp_Divide( &used, &ctx->total, &ival, &fval );
  seconds = _Timestamp_Get_seconds( &used );
  nanoseconds = _Timestamp_Get_nanoseconds( &used ) /
    TOD_NANOSECONDS_PER_MICROSECOND;

  rtems_printf(
    ctx->printer,
    " 0x%08" PRIx32 " | %-38s |"
      "%7" PRIu32 ".%06" PRIu32 " |%4" PRIu32 ".%03" PRIu32 "\n",
    the_thread->Object.id,
    name,
    seconds, nanoseconds,
    ival, fval
  );

  return false;
}

/*
 *  rtems_cpu_usage_report
 */
void rtems_cpu_usage_report_with_plugin(
  const rtems_printer *printer
)
{
  cpu_usage_context  ctx;
  uint32_t           seconds;
  uint32_t           nanoseconds;

  ctx.printer = printer;

  /*
   *  When not using nanosecond CPU usage resolution, we have to count
   *  the number of "ticks" we gave credit for to give the user a rough
   *  guideline as to what each number means proportionally.
   */
  _Timestamp_Set_to_zero( &ctx.total );
  ctx.uptime_at_last_reset = CPU_usage_Uptime_at_last_reset;

  rtems_printf(
     printer,
     "-------------------------------------------------------------------------------\n"
     "                              CPU USAGE BY THREAD\n"
     "------------+----------------------------------------+---------------+---------\n"
     " ID         | NAME                                   | SECONDS       | PERCENT\n"
     "------------+----------------------------------------+---------------+---------\n"
  );

  rtems_task_iterate( cpu_usage_visitor, &ctx );

  seconds = _Timestamp_Get_seconds( &ctx.total );
  nanoseconds = _Timestamp_Get_nanoseconds( &ctx.total ) /
    TOD_NANOSECONDS_PER_MICROSECOND;
  rtems_printf(
     printer,
     "------------+----------------------------------------+---------------+---------\n"
     " TIME SINCE LAST CPU USAGE RESET IN SECONDS:                    %7" PRIu32 ".%06" PRIu32 "\n"
     "-------------------------------------------------------------------------------\n",
     seconds, nanoseconds
  );
}

void rtems_cpu_usage_report( void )
{
  rtems_printer printer;
  rtems_print_printer_printk( &printer );
  rtems_cpu_usage_report_with_plugin( &printer );
}
