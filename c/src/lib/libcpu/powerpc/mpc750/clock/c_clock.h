/*
 *  Clock Tick Device Driver
 *
 *  This routine utilizes the Decrementer Register common to the PPC family.
 *
 *  The tick frequency is directly programmed to the configured number of
 *  microseconds per tick.
 *
 *  COPYRIGHT (c) 1989-1997.
 *  On-Line Applications Research Corporation (OAR).
 *  Copyright assigned to U.S. Government, 1994.
 *
 *  The license and distribution terms for this file may in
 *  the file LICENSE in this distribution or at
 *  http://www.OARcorp.com/rtems/license.html.
 *
 *  Modified to support the MPC750.
 *  Modifications Copyright (c) 1999 Eric Valette valette@crf.canon.fr
 *
 *  $Id$
 */

#ifndef _LIB_LIBCPU_C_CLOCK_H
#define _LIB_LIBCPU_C_CLOCK_H

#include <rtems.h>

/*
 * Theses functions and variables represent the API exported by the CPU to the BSP
 */
extern void clockOff	(void* unused);
extern void clockOn	(void* unused);
extern void clockIsr	(void);
extern int  clockIsOn	(void* unused);

/*
 * Theses functions and variables represent the expected API nedeed by the CPU
 * NOTE for JOEL : I would prefer to have an include of <bsp.h> as it would
 * materialize the API (and providing a default bsp.h) could formalize the
 * (generic + cpu_familly + cpu_specific)/ BSP interface.
 */
extern rtems_configuration_table  	BSP_Configuration;
extern unsigned int 			BSP_bus_frequency;

extern int BSP_disconnect_clock_handler (void);
extern int BSP_connect_clock_handler (void);
extern int printk(const char *, ...) __attribute__((format(printf, 1, 2)));

#endif





