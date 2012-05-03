/*  bsp.h
 *
 *  This include file contains all board IO definitions.
 *
 *  XXX : put yours in here
 *
 *  COPYRIGHT (c) 1989-1999.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 *
 *  Jukka Pietarinen <jukka.pietarinen@mrf.fi>, 2008,
 *  Micro-Research Finland Oy
 */

#ifndef _BSP_H
#define _BSP_H

#include <stdint.h>
#include <bspopts.h>

#include <rtems.h>
#include <rtems/console.h>
#include <rtems/clockdrv.h>

#if defined(RTEMS_NETWORKING)
#include <rtems/rtems_bsdnet.h>
#endif

#ifdef __cplusplus
extern "C" {
#endif

#define BSP_DIRTY_MEMORY 1

  /*
   * lm32 requires certain aligment of mbuf because unaligned uint32_t
   * accesses are not handled properly.
   */

#define CPU_U32_FIX

#if defined(RTEMS_NETWORKING)
extern int rtems_tsmac_driver_attach(struct rtems_bsdnet_ifconfig *config,
				     int attaching);

#define RTEMS_BSP_NETWORK_DRIVER_NAME "TSMAC0"
#define RTEMS_BSP_NETWORK_DRIVER_ATTACH rtems_tsmac_driver_attach

  /*
   * Due to a hardware design error (RJ45 connector with 10baseT magnetics)
   * we are forced to use 10baseT mode.
   */

#define TSMAC_FORCE_10BASET
#endif

  /*
   *  Simple spin delay in microsecond units for device drivers.
   *  This is very dependent on the clock speed of the target.
   */

#define rtems_bsp_delay( microseconds ) \
  { \
  }

/* functions */
#if 0
rtems_isr_entry set_vector(                     /* returns old vector */
  rtems_isr_entry     handler,                  /* isr routine        */
  rtems_vector_number vector,                   /* vector number      */
  int                 type                      /* RTEMS or RAW intr  */
);
#endif

#ifdef __cplusplus
}
#endif

#endif
/* end of include file */
