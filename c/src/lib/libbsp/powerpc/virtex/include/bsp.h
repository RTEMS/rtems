/*  bsp.h
 *
 *  This include file contains all GEN405 board IO definitions.
 *
 * derived from helas403/include/bsp.h:
 *  Id: bsp.h,v 1.4 2001/06/18 17:01:48 joel Exp
 *  Author:	Thomas Doerfler <td@imd.m.isar.de>
 *              IMD Ingenieurbuero fuer Microcomputertechnik
 *
 *  COPYRIGHT (c) 1998 by IMD
 *
 *  Changes from IMD are covered by the original distributions terms.
 *  This file has been derived from the papyrus BSP.
 *
 *  Author:	Andrew Bray <andy@i-cubed.co.uk>
 *
 *  COPYRIGHT (c) 1995 by i-cubed ltd.
 *
 *  To anyone who acknowledges that this file is provided "AS IS"
 *  without any express or implied warranty:
 *      permission to use, copy, modify, and distribute this file
 *      for any purpose is hereby granted without fee, provided that
 *      the above copyright notice and this notice appears in all
 *      copies, and that the name of i-cubed limited not be used in
 *      advertising or publicity pertaining to distribution of the
 *      software without specific, written prior permission.
 *      i-cubed limited makes no representations about the suitability
 *      of this software for any purpose.
 *
 *  Derived from c/src/lib/libbsp/no_cpu/no_bsp/include/bsp.h
 *
 *  COPYRIGHT (c) 1989-1999.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 *
 *  $Id$
 *
 */

#ifndef _BSP_H
#define _BSP_H

#ifdef __cplusplus
extern "C" {
#endif

#include <bspopts.h>

/*
 *  confdefs.h overrides for this BSP:
 *   - number of termios serial ports (defaults to 1)
 *   - Interrupt stack space is not minimum if defined.
 */

/* #define CONFIGURE_NUMBER_OF_TERMIOS_PORTS 2 */
#define CONFIGURE_INTERRUPT_STACK_MEMORY  (16 * 1024)

#ifdef ASM
/* Definition of where to store registers in alignment handler */
#define ALIGN_REGS 0x0140

#else
#include <rtems.h>
#include <rtems/console.h>
#include <rtems/clockdrv.h>
#include <rtems/console.h>
#include <rtems/iosupp.h>
#include <rtems/rtems_bsdnet.h>



/* Constants */

extern uint32_t _HeapSize;
extern uint32_t _heap_start;
extern uint32_t _heap_end;
extern uint32_t _top_of_ram;

/* miscellaneous stuff assumed to exist */

extern rtems_configuration_table BSP_Configuration;     /* owned by BSP */
extern rtems_cpu_table           Cpu_table;             /* owned by BSP */


/* Network Defines */
#if 1 /* EB/doe changes */
#define RTEMS_BSP_NETWORK_DRIVER_NAME     "eth0"
#else
#include "xiltemac.h"
#define RTEMS_BSP_NETWORK_DRIVER_NAME     XILTEMAC_DRIVER_PREFIX
#endif
extern xilTemac_driver_attach(struct rtems_bsdnet_ifconfig*, int );
#define RTEMS_BSP_NETWORK_DRIVER_ATTACH xilTemac_driver_attach

/*
 *  Device Driver Table Entries
 */

/*
 * NOTE: Use the standard Console driver entry
 */

/*
 * NOTE: Use the standard Clock driver entry
 */

/* functions */

rtems_isr_entry set_vector(                    /* returns old vector */
  rtems_isr_entry     handler,                  /* isr routine        */
  rtems_vector_number vector,                   /* vector number      */
  int                 type                      /* RTEMS or RAW intr  */
);
#endif /* ASM */

#ifdef __cplusplus
}
#endif

#endif
