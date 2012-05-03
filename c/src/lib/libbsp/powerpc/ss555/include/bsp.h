/*  bsp.h
 *
 *  This include file contains all board IO definitions.
 *
 *  This file includes definitions for the Intec SS555.
 *
 *
 *  SS555 port sponsored by Defence Research and Development Canada - Suffield
 *  Copyright (C) 2004, Real-Time Systems Inc. (querbach@realtime.bc.ca)
 *
 *  Derived from c/src/lib/libbsp/powerpc/mbx8xx/include/bsp.h:
 *
 *  COPYRIGHT (c) 1989-1998.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 */

#ifndef _BSP_H
#define _BSP_H

#ifdef __cplusplus
extern "C" {
#endif

#include <bspopts.h>

#include <rtems.h>
#include <rtems/console.h>
#include <rtems/clockdrv.h>
#include <mpc5xx.h>
#include <mpc5xx/console.h>
#include <libcpu/vectors.h>
#include <bsp/irq.h>

/*
 * Clock definitions
 */

#define BSP_CRYSTAL_HZ 	 4000000	/* crystal frequency, Hz */
#define BSP_CLOCK_HZ	40000000	/* CPU clock frequency, Hz

/*
 * I/O definitions
 *
 * The SS555 board includes a CPLD to control on-board features and
 * off-board devices.
 */
typedef struct cpld_ {
  uint8_t	cs3a[32];		/* Chip select 3A */
  uint8_t	pad0[0x200000 - 0x000020];

  uint8_t	cs3b[32];		/* Chip select 3B */
  uint8_t	pad2[0x400000 - 0x200020];

  uint8_t	cs3c[32];		/* Chip select 3C */
  uint8_t	pad4[0x600000 - 0x400020];

  uint8_t	cs3d[32];		/* Chip select 3D */
  uint8_t	pad6[0x800000 - 0x600020];

  uint8_t	serial_ints;	/* Enable/disable serial interrupts */
  uint8_t	serial_resets;	/* Enable/disable serial resets */
  uint8_t	serial_ack;	/* Acknowledge serial transfers */
  uint8_t	pad8[0xA00000 - 0x800003];

  uint8_t	iflash_writess;	/* Enable/disable internal-flash writes */
  uint8_t	nflash_writess;	/* Enable/disable NAND-flash writes */
  uint8_t	padA[0xC00000 - 0xA00002];
} cpld_t;

extern volatile cpld_t cpld;              /* defined in linkcmds */

/* clock/p_clock.c */
extern int BSP_disconnect_clock_handler (void);

extern int BSP_connect_clock_handler (rtems_irq_hdl hdl);

#ifdef __cplusplus
}
#endif

#endif
