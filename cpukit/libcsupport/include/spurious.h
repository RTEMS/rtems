/**
 * @file rtems/spurious.h
 *
 * This file describes the Spurious Interrupt Driver for all boards.
 */

/*
 *  COPYRIGHT (c) 1989-2011.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 */

#ifndef _RTEMS_SPURIOUS_H
#define _RTEMS_SPURIOUS_H

#include <rtems/rtems/types.h> /* rtems_id */
#include <rtems/io.h> /* rtems_device_driver */

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
  uint32_t   *
);

#ifdef __cplusplus
}
#endif

#endif
/* end of include file */
