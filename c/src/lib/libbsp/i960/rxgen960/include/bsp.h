/*  bsp.h
 *
 *  This include file contains some definitions specific to the
 *  Ramix PMC901 board
 *
 *  COPYRIGHT (c) 1989-1997.
 *  On-Line Applications Research Corporation (OAR).
 *  Copyright assigned to U.S. Government, 1994.
 *
 *  The license and distribution terms for this file may in
 *  the file LICENSE in this distribution or at
 *  http://www.OARcorp.com/rtems/license.html.
 *
 *  $Id$
 */

#ifndef __PMC901_h
#define __PMC901_h

#ifdef __cplusplus
extern "C" {
#endif

#include <rtems.h>
#include <libcpu/i960RP.h>
#include <iosupp.h>
#include <console.h>
#include <clockdrv.h>

/*
 *  confdefs.h overrides for this BSP:
 *   - number of termios serial ports (defaults to 1)
 *   - Interrupt stack space is not minimum if defined.
 */

/* #define CONFIGURE_NUMBER_OF_TERMIOS_PORTS 2 */
#define CONFIGURE_INTERRUPT_STACK_MEMORY  (8 * 1024)

/*
 *  Define the time limits for RTEMS Test Suite test durations.
 *  Long test and short test duration limits are provided.  These
 *  values are in seconds and need to be converted to ticks for the
 *  application.
 *
 */

#define MAX_LONG_TEST_DURATION       300 /* 5 minutes = 300 seconds */
#define MAX_SHORT_TEST_DURATION      3   /* 3 seconds */

/*
 *  Define the interrupt mechanism for Time Test 27
 *
 *  NOTE: Following are for i960CA and are board independent
 *
 */

#define MUST_WAIT_FOR_INTERRUPT 0

#define Install_tm27_vector( handler ) set_vector( (handler), 6, 1 )

#define Cause_tm27_intr()  i960_cause_intr( 0x62 )

#define Clear_tm27_intr()  i960_clear_intr( 6 )

#define Lower_tm27_intr()

/*
 *  Simple spin delay in microsecond units for device drivers.
 *  This is very dependent on the clock speed of the target.
 */

#define delay( microseconds ) \
  { register rtems_unsigned32 _delay=(microseconds); \
    register rtems_unsigned32 _tmp = 0; /* initialized to avoid warning */ \
    asm volatile( "0: \
                     remo      3,31,%0 ; \
                     cmpo      0,%0 ; \
                     subo      1,%1,%1 ; \
                     cmpobne.t 0,%1,0b " \
                  : "=d" (_tmp), "=d" (_delay) \
                  : "0"  (_tmp), "1"  (_delay) ); \
  }

/* Constants */

#define RAM_START 0xfed00000
#define RAM_END   0xfef00000 /* this should actually be 16MB, most likely */

/* NINDY console I/O requests:
 *   CO sends a single character to stdout,
 *   CI reads one.
 */

#define NINDY_INPUT   0
#define NINDY_OUTPUT  1

/*
 *  get_prcb
 *
 *  Returns the PRCB pointer.
 */

static inline i960_PRCB *get_prcb( void )
{
  register i960_PRCB *_prcb = 0;

  asm volatile( "calls 5; \
                 mov   g0,%0" \
                 : "=d" (_prcb) \
                 : "0" (_prcb) );
  return ( _prcb );
}

/*
#ifdef C961_INIT
#undef BSP_EXTERN
#define BSP_EXTERN
#else
#undef BSP_EXTERN
#define BSP_EXTERN extern
#endif
*/

#undef BSP_EXTERN
#define BSP_EXTERN

/* miscellaneous stuff assumed to exist */

extern rtems_configuration_table BSP_Configuration;

BSP_EXTERN i960_PRCB          *Prcb;
BSP_EXTERN i960_control_table *Ctl_tbl;

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
 * How many libio files we want
 */

#define BSP_LIBIO_MAX_FDS       20

/* functions */

void bsp_cleanup( void );

i960_isr_entry set_vector( rtems_isr_entry, unsigned int, unsigned int );
i960_isr_entry set_tmr_vector( rtems_isr_entry, unsigned int, unsigned int );

#ifdef __cplusplus
}
#endif

#endif
/* end of include file */
