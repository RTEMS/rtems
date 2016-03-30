/**
 * @file
 *
 * @brief Clock Driver for all Boards
 *
 * This file describes the Clock Driver for all boards.
 */

/*
 *  COPYRIGHT (c) 1989-2011.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 */

#ifndef _RTEMS_CLOCKDRV_H
#define _RTEMS_CLOCKDRV_H

#include <rtems/io.h> /* rtems_device_driver */

#ifdef __cplusplus
extern "C" {
#endif

/* variables */

extern volatile uint32_t   Clock_driver_ticks;

/* default clock driver entry */

#define CLOCK_DRIVER_TABLE_ENTRY \
  { Clock_initialize, NULL, NULL, NULL, NULL, NULL }

rtems_device_driver Clock_initialize(
  rtems_device_major_number,
  rtems_device_minor_number,
  void *
);

void Clock_exit(void);

#ifdef __cplusplus
}
#endif

#endif
/* end of include file */
