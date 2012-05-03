/*  bsp.h
 *
 *  Generic 405EX bsp.h
 *  derived from virtex/include/bsp.h
 *  by Michael Hamel ADInstruments Ltd 2008
 *
 * derived from helas403/include/bsp.h:
 *  Id: bsp.h,v 1.4 2001/06/18 17:01:48 joel Exp
 *  Author:  Thomas Doerfler <td@imd.m.isar.de>
 *              IMD Ingenieurbuero fuer Microcomputertechnik
 *
 *  COPYRIGHT (c) 1998 by IMD
 *
 *  Changes from IMD are covered by the original distributions terms.
 *  This file has been derived from the papyrus BSP.
 *
 *  Author:  Andrew Bray <andy@i-cubed.co.uk>
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
 *
 *
 */

#ifndef _BSP_H
#define _BSP_H

#ifdef __cplusplus
extern "C" {
#endif

#include <bspopts.h>

#ifdef ASM


  /* Definition of where to store registers in alignment handler */
  #define ALIGN_REGS 0x0140

#else

  #include <rtems.h>
  #include <rtems/console.h>
  #include <rtems/clockdrv.h>
  #include <libcpu/io.h>
  #include <rtems/console.h>
  #include <rtems/iosupp.h>
  #include <bsp/irq.h>
  #include <bsp/vectors.h>

  /* Network Defines */
  #define RTEMS_BSP_NETWORK_DRIVER_NAME     "eth0"

  struct rtems_bsdnet_ifconfig;
  int rtems_emac_driver_attach(struct rtems_bsdnet_ifconfig* config, int attaching);
  #define RTEMS_BSP_NETWORK_DRIVER_ATTACH rtems_emac_driver_attach

  #define BSP_UART_IOBASE_COM1  0xEF600200    /* PPC405EX */
  #define BSP_UART_IOBASE_COM2  0xEF600300

  #define BSP_CONSOLE_PORT    BSP_UART_COM1    /* console */

  #define BSP_UART_BAUD_BASE    (11059200 / 16)    /* Kilauea ext clock, max speed */

#endif /* ASM */

#ifdef __cplusplus
}
#endif

#endif /* BSP_H */
