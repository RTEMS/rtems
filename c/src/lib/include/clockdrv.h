/*  clock.h
 *
 *  This file describes the Clock Driver for all boards.
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

#ifndef __CLOCK_DRIVER_h
#define __CLOCK_DRIVER_h

#ifdef __cplusplus
extern "C" {
#endif

/* variables */

extern volatile rtems_unsigned32 Clock_driver_ticks;

/* functions */

rtems_task Exit_task();
void exit_task_init();

void Install_clock( rtems_isr_entry );
void ReInstall_clock( rtems_isr_entry );
void Clock_exit();

rtems_isr Clock_isr(
  rtems_vector_number
);

/* driver entries */

#define CLOCK_DRIVER_TABLE_ENTRY \
  { Clock_initialize, NULL, NULL, NULL, NULL, NULL }

rtems_device_driver Clock_initialize(
  rtems_device_major_number,
  rtems_device_minor_number,
  void *,
  rtems_id,
  rtems_unsigned32 *
);

#ifdef __cplusplus
}
#endif

#endif
/* end of include file */
