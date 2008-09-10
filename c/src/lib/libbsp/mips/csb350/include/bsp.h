/*  bsp.h
 *
 *  This include file contains some definitions specific to the
 *  Cogent CSB350 Board.
 *
 *  COPYRIGHT (c) 1989-2000.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 *
 *  $Id$
 */

#ifndef _BSP_H
#define _BSP_H

#ifdef __cplusplus
extern "C" {
#endif

#include <bspopts.h>

#include <rtems.h>
#include <rtems/iosupp.h>
#include <rtems/console.h>
#include <rtems/clockdrv.h>
#include <libcpu/au1x00.h>

/*
 *  Define the interrupt mechanism for Time Test 27
 */
int assert_sw_irw(uint32_t irqnum);
int negate_sw_irw(uint32_t irqnum);

#define MUST_WAIT_FOR_INTERRUPT 0

#define Install_tm27_vector( handler ) \
   (void) set_vector(handler, AU1X00_IRQ_SW0, 1);

#define Cause_tm27_intr() \
  do { \
     assert_sw_irq(0); \
  } while(0)

#define Clear_tm27_intr() \
  do { \
     negate_sw_irq(0); \
  } while(0)

#if 0
#define Lower_tm27_intr() \
  mips_enable_in_interrupt_mask( 0xff01 );
#else
#define Lower_tm27_intr() \
  do { \
     continue;\
  } while(0)
#endif

/* Constants */

/* miscellaneous stuff assumed to exist */

/*
 *  Device Driver Table Entries
 */

/*
 * NOTE: Use the standard Console driver entry
 */
 
/*
 * NOTE: Use the standard Clock driver entry
 */

/*
 * Network driver configuration
 */
extern struct rtems_bsdnet_ifconfig *config;

int rtems_au1x00_emac_attach(struct rtems_bsdnet_ifconfig *config, 
                             int attaching);
#define RTEMS_BSP_NETWORK_DRIVER_NAME	"eth0"
#define RTEMS_BSP_NETWORK_DRIVER_ATTACH	rtems_au1x00_emac_attach

/* functions */

rtems_isr_entry set_vector(
  rtems_isr_entry, rtems_vector_number, int );

#ifdef __cplusplus
}
#endif

#endif
