/*  bsp.h
 *
 *  This include file contains all SPARC64 simulator definitions.
 *
 *  COPYRIGHT (c) 1989-1998.
 *  On-Line Applications Research Corporation (OAR).
 *  COPYRIGHT (c) 1995.
 *  European Space Agency.
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 *
 *  Ported to Simics Niagara version of SPARC64 
 *  Niagara modifications of respective RTEMS file: COPYRIGHT (c) 2010.
 *  Eugen Leontie.
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

/* support for simulated clock tick */
/*
Thread clock_driver_sim_idle_body(uintptr_t);
#define BSP_IDLE_TASK_BODY clock_driver_sim_idle_body
*/

/* this should be defined somewhere */
rtems_isr_entry set_vector(                     /* returns old vector */
    rtems_isr_entry     handler,                /* isr routine        */
    rtems_vector_number vector,                 /* vector number      */
    int                 type                    /* RTEMS or RAW intr  */
);

/*
 *  Simple spin delay in microsecond units for device drivers.
 *  This is very dependent on the clock speed of the target.
 */

#define rtems_bsp_delay( microseconds ) \
  { register uint32_t         _delay=(microseconds); \
    register uint32_t         _tmp = 0; /* initialized to avoid warning */ \
    __asm__ volatile( "0: \
                     remo      3,31,%0 ; \
                     cmpo      0,%0 ; \
                     subo      1,%1,%1 ; \
                     cmpobne.t 0,%1,0b " \
                  : "=d" (_tmp), "=d" (_delay) \
                  : "0"  (_tmp), "1"  (_delay) ); \
  }

#ifdef __cplusplus
}
#endif

#endif
