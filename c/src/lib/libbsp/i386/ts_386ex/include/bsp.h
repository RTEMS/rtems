/*
 *  This include file definitions related to an Intel i386ex board.
 *
 *  COPYRIGHT (c) 1989-1999.
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
#include <rtems/bspIo.h>
#include <libcpu/cpu.h>
#include <bsp/irq.h>

/*
 * Network driver configuration
 */

struct rtems_bsdnet_ifconfig;
extern int rtems_ne_driver_attach (struct rtems_bsdnet_ifconfig *config);

extern void Wait_X_ms (unsigned);

#define RTEMS_BSP_NETWORK_DRIVER_NAME         "ne1"
#define RTEMS_BSP_NETWORK_DRIVER_ATTACH       rtems_ne_driver_attach

#define NE2000_BYTE_TRANSFERS

/*
 * Constants relating to the 8254 (or 8253) programmable interval timers.
 */

#define TIMER_CONFIG   0xF834
#define IO_TIMER1      0xF040

  /* Port address of the control port and timer channels */

#define TIMER_CNTR0    (IO_TIMER1 + 0) /* timer 0 counter port           */
#define TIMER_CNTR1    (IO_TIMER1 + 1) /* timer 1 counter port           */
#define TIMER_CNTR2    (IO_TIMER1 + 2) /* timer 2 counter port           */
#define TIMER_MODE     (IO_TIMER1 + 3) /* timer mode port                */
#define TIMER_SEL0     0x00            /* select counter 0               */
#define TIMER_SEL1     0x40            /* select counter 1               */
#define TIMER_SEL2     0x80            /* select counter 2               */
#define TIMER_INTTC    0x00            /* mode 0, intr on terminal cnt   */
#define TIMER_ONESHOT  0x02            /* mode 1, one shot               */
#define TIMER_RATEGEN  0x04            /* mode 2, rate generator         */
#define TIMER_SQWAVE   0x06            /* mode 3, square wave            */
#define TIMER_SWSTROBE 0x08            /* mode 4, s/w triggered strobe   */
#define TIMER_HWSTROBE 0x0a            /* mode 5, h/w triggered strobe   */
#define TIMER_LATCH    0x00            /* latch counter for reading      */
#define TIMER_LSB      0x10            /* r/w counter LSB                */
#define TIMER_MSB      0x20            /* r/w counter MSB                */
#define TIMER_16BIT    0x30            /* r/w counter 16 bits, LSB first */
#define TIMER_BCD      0x01            /* count in BCD                   */

/*
 *  Simple spin delay in microsecond units for device drivers.
 *  This is very dependent on the clock speed of the target.
 */

#define rtems_bsp_delay( _microseconds ) \
  { \
    uint32_t         _counter; \
    \
    _counter = (_microseconds); \
    \
    asm volatile ( "0: nop;" \
                   " mov %0,%0 ;" \
                   " loop 0b" : "=c" (_counter) \
                              : "0"  (_counter) \
                 ); \
    \
  }

/* Constants */

#define RAM_START 0

/* replaced the earlier EI kludge of 0xfffff */

#define RAM_END   0x200000

/* Structures */

#define IDT_SIZE 256
#define GDT_SIZE 3

extern interrupt_gate_descriptor Interrupt_descriptor_table[IDT_SIZE];

#ifdef __cplusplus
}
#endif

#endif
