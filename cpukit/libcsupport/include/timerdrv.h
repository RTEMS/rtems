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

void Timer_initialize( void );

uint32_t   Read_timer( void );

rtems_status_code Empty_function( void );

void Set_find_average_overhead(
  rtems_boolean find_flag
);

#ifdef __cplusplus
}
#endif

#endif
