/*  spurious.h
 *
 *  This file describes the Spurious Interrupt Driver for all boards.
 *
 *  COPYRIGHT (c) 1989, 1990, 1991, 1992, 1993.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
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
