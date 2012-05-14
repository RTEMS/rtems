/**
 * @file rtems/cpuuse.h
 *
 * This include file contains information necessary to utilize
 * and install the cpu usage reporting mechanism.
 */

/*
 *  COPYRIGHT (c) 1989-2011.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 */

#ifndef __RTEMS_CPUUSE_h
#define __RTEMS_CPUUSE_h

#include <rtems.h>
#include <rtems/bspIo.h>

#ifndef __RTEMS_USE_TICKS_FOR_STATISTICS__
  #include <rtems/score/timestamp.h>
#endif

#ifdef __cplusplus
extern "C" {
#endif

#ifndef __RTEMS_USE_TICKS_FOR_STATISTICS__
  extern Timestamp_Control  CPU_usage_Uptime_at_last_reset;
#else
  extern uint32_t           CPU_usage_Ticks_at_last_reset;
#endif

/*
 * rtems_cpu_usage_report_with_handler
 */

void rtems_cpu_usage_report_with_plugin(
  void                  *context,
  rtems_printk_plugin_t  handler
);

/*
 *  rtems_cpu_usage_report
 */

void rtems_cpu_usage_report( void );

/*
 *  rtems_cpu_usage_reset
 */

void rtems_cpu_usage_reset( void );

#ifdef __cplusplus
}
#endif

#endif
/* end of include file */
