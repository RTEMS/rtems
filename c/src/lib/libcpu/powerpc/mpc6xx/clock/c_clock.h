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
 *
 *  The license and distribution terms for this file may in
 *  the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 *
 *  Modified to support the MPC750.
 *  Modifications Copyright (c) 1999 Eric Valette valette@crf.canon.fr
 */

#ifndef _LIBCPU_C_CLOCK_H
#define _LIBCPU_C_CLOCK_H

#include <rtems.h>

/*
 * These functions and variables represent the API exported by the
 * CPU to the BSP.
 */

extern void clockOff	(void* unused);
extern void clockOn	(void* unused);
extern void clockIsr	(void* unused);
/* bookE decrementer is slightly different */
extern void clockIsrBookE (void *unused);
extern int  clockIsOn	(void* unused);

/*
 *  These functions and variables represent the assumptions of this
 *  driver on the BSP.
 */

extern int BSP_disconnect_clock_handler (void);
/*
 * PCI Bus Frequency
 */
extern unsigned int BSP_bus_frequency;
/*
 * processor clock frequency
 */
extern unsigned int BSP_processor_frequency;
/*
 * Time base divisior (how many tick for 1 second).
 */
extern unsigned int BSP_time_base_divisor;

#endif
