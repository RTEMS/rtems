/*  spurious.h
 *
 *  This file describes the Spurious Interrupt Driver for all boards.
 *
 *  COPYRIGHT (c) 1989, 1990, 1991, 1992, 1993.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  This material may be reproduced by or for the U.S. Government pursuant
 *  to the copyright license under the clause at DFARS 252.227-7013.  This
 *  notice must appear in all copies of this file and its derivatives.
 *
 *  $Id$
 */

#ifndef __SPURIOUS_h
#define __SPURIOUS_h

#ifdef __cplusplus
extern "C" {
#endif

#define SPURIOUS_DRIVER_TABLE_ENTRY \
  { Spurious_Initialize, NULL, NULL, NULL, NULL, NULL }

rtems_device_driver Spurious_Initialize(
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
