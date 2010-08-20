/*  bsp.h
 *
 *  This include file contains all board IO definitions.
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 *
 *  $Id$
 *
 *  Yann Sionneau <yann.sionneau@telecom-sudparis.eu>, (GSoC 2010)
 *  Telecom SudParis
 */

#ifndef _BSP_H
#define _BSP_H

#include <stdint.h>
#include <bspopts.h>

#include <rtems.h>
#include <rtems/console.h>
#include <rtems/clockdrv.h>


#ifdef __cplusplus
extern "C" {
#endif

#define BSP_DIRTY_MEMORY 1

#define BSP_HAS_FRAME_BUFFER 1

#define GPIO_DRIVER_TABLE_ENTRY { gpio_initialize, \
gpio_open, gpio_close, gpio_read, gpio_write, gpio_control}

  /*
   * lm32 requires certain aligment of mbuf because unaligned uint32_t
   * accesses are not handled properly.
   */

#define CPU_U32_FIX

#if defined(RTEMS_NETWORKING)
#include <rtems/rtems_bsdnet.h>
struct rtems_bsdnet_ifconfig;
extern int rtems_minimac_driver_attach (struct rtems_bsdnet_ifconfig *config,
                                        int attaching);
#define RTEMS_BSP_NETWORK_DRIVER_ATTACH rtems_minimac_driver_attach
#define RTEMS_BSP_NETWORK_DRIVER_NAME "minimac0"
#endif

  /*
   *  Simple spin delay in microsecond units for device drivers.
   *  This is very dependent on the clock speed of the target.
   */

#define rtems_bsp_delay( microseconds ) \
  { \
  }

/* functions */
lm32_isr_entry set_vector(                     /* returns old vector */
  rtems_isr_entry     handler,                  /* isr routine        */
  rtems_vector_number vector,                   /* vector number      */
  int                 type                      /* RTEMS or RAW intr  */
);

#ifdef __cplusplus
}
#endif

#endif
/* end of include file */
