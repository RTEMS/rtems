/*  cpuuse.h
 *
 *  This include file contains information necessary to utilize
 *  and install the cpu usage reporting mechanism.
 *
 *  COPYRIGHT (c) 1989-2006.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 *
 *  $Id$
 */

#ifndef __RTEMS_CPUUSE_h
#define __RTEMS_CPUUSE_h

#ifdef __cplusplus
extern "C" {
#endif

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
