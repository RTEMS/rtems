/*  bsp.h
 *
 *  This include file contains some definitions specific to a board
 *  based upon the generic capabilities of a Mongoose-V.
 *
 *  COPYRIGHT (c) 1989-2001.
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
#include <libcpu/mongoose-v.h>

#ifndef CPU_CLOCK_RATE
#define CLOCK_RATE	12000000
#endif

#define CPU_CLOCK_RATE_HZ	CLOCK_RATE
#define CPU_CLOCK_RATE_MHZ	(CLOCK_RATE/1000000)

/*
 * Useful defines set here so we can avoid duplicating them all over
 * creation.
 *
 */

/*
 * assertSoftwareInt defined in vectorisrs.c the prototype is here so
 * userspace code can get to it directly.
 *  */

extern void assertSoftwareInterrupt(uint32_t);

#define CLOCK_VECTOR	MONGOOSEV_IRQ_TIMER1

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

/* functions */

void bsp_cleanup( void );

rtems_isr_entry set_vector( rtems_isr_entry, rtems_vector_number, int );

#ifdef __cplusplus
}
#endif

#endif

