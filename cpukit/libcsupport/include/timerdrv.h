/*  timerdrv.h
 *
 *  This file describes the Timer Driver for all boards.
 *
 *  COPYRIGHT (c) 1989, 1990, 1991, 1992, 1993, 1994.
 *  On-Line Applications Research Corporation (OAR).
 *  All rights assigned to U.S. Government, 1994.
 *
 *  This material may be reproduced by or for the U.S. Government pursuant
 *  to the copyright license under the clause at DFARS 252.227-7013.  This
 *  notice must appear in all copies of this file and its derivatives.
 *
 *  $Id$
 */

#ifndef __TIMER_DRIVER_h
#define __TIMER_DRIVER_h

#ifdef __cplusplus
extern "C" {
#endif

/* functions */

void Timer_initialize( void );

rtems_unsigned32 Read_timer( void );

rtems_status_code Empty_function( void );

void Set_find_average_overhead(
  rtems_boolean find_flag
);

#ifdef __cplusplus
}
#endif

#endif
/* end of include file */
