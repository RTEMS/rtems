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
extern rtems_device_major_number rtems_clock_major;
extern rtems_device_minor_number rtems_clock_minor;

/* default clock driver entry */

#define CLOCK_DRIVER_TABLE_ENTRY \
  { Clock_initialize, NULL, NULL, NULL, NULL, Clock_control }
 
rtems_device_driver Clock_initialize(
  rtems_device_major_number,
  rtems_device_minor_number,
  void *
);

rtems_device_driver Clock_control(
  rtems_device_major_number major,
  rtems_device_minor_number minor,
  void *pargp
);

#ifdef __cplusplus
}
#endif

#endif
/* end of include file */
