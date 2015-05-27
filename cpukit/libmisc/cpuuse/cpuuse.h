/**
 * @file rtems/cpuuse.h
 * 
 * @defgroup libmisc_cpuuse CPU Usage
 *
 * @ingroup libmisc
 * @brief CPU Usage Report
 *
 * This include file contains information necessary to utilize
 * and install the cpu usage reporting mechanism.
 */

/*
 * COPYRIGHT (c) 1989-2011.
 * On-Line Applications Research Corporation (OAR).
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rtems.org/license/LICENSE.
 */

#ifndef __RTEMS_CPUUSE_h
#define __RTEMS_CPUUSE_h

#include <rtems.h>
#include <rtems/bspIo.h>

#include <rtems/score/timestamp.h>

/**
 *  @defgroup libmisc_cpuuse CPU Usage
 *
 *  @ingroup libmisc
 */
/**@{*/
#ifdef __cplusplus
extern "C" {
#endif

extern Timestamp_Control  CPU_usage_Uptime_at_last_reset;

/*
 * rtems_cpu_usage_report_with_handler
 */

void rtems_cpu_usage_report_with_plugin(
  void                  *context,
  rtems_printk_plugin_t  handler
);

/**
 *  @brief Report CPU usage.
 *
 *  CPU Usage Reporter
 */

void rtems_cpu_usage_report( void );

/**
 *   @brief CPU usage Top plugin
 *
 *   Report CPU Usage in top format to
 *   to a print plugin.
 */
void rtems_cpu_usage_top_with_plugin(
  void                  *context,
  rtems_printk_plugin_t  print
);

/**
 *  @brief CPU usage top.
 *
 *  CPU Usage top
 */

void rtems_cpu_usage_top( void );

/**
 *  @brief Reset CPU usage.
 *
 *  CPU Usage Reporter
 */

void rtems_cpu_usage_reset( void );

#ifdef __cplusplus
}
#endif
/**@}*/
#endif
/* end of include file */
