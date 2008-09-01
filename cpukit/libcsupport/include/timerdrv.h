/**
 * @file rtems/timerdrv.h
 *
 *  This file describes the Timer Driver for all boards.
 */
 
/*
 *
 *  COPYRIGHT (c) 1989-1999.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 *
 *  $Id$
 */

#ifndef _RTEMS_TIMERDRV_H
#define _RTEMS_TIMERDRV_H

#ifdef __cplusplus
extern "C" {
#endif

/* functions */

void benchmark_timer_initialize( void );

uint32_t   benchmark_timer_read( void );

rtems_status_code benchmark_timer_empty_function( void );

void benchmark_timer_disable_subtracting_average_overhead(
  bool find_flag
);

#ifdef __cplusplus
}
#endif

#endif
