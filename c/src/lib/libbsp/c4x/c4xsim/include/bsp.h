/*  bsp.h
 *
 *  This include file contains all C4X Simulator IO definitions.
 *
 *  COPYRIGHT (c) 1989-1999.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 *
 *  $Id$
 */

#ifndef __C4XSIM_h
#define __C4XSIM_h

#ifdef __cplusplus
extern "C" {
#endif

#include <bspopts.h>

#ifdef ASM

#else
#include <rtems.h>
#include <console.h>
#include <clockdrv.h>
#include <console.h>
#include <iosupp.h>


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
 *  Stuff for Time Test 27
 */

#include <c4xio.h>
#define MUST_WAIT_FOR_INTERRUPT 0

/* XXX */
#define Install_tm27_vector( _handler ) \
  set_vector( (_handler), 0x20, 1 )

#define Cause_tm27_intr()  \
  do { \
     __asm__ volatile ( "trapu 0" ); \
  } while (0)

#define Clear_tm27_intr() \
  do { \
     ; \
  } while (0)

#define Lower_tm27_intr() \
  do { \
    c4x_global_interrupts_enable(); \
  } while (0)

/* Constants */

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
 *  Information placed in the linkcmds file.
 */

/* functions */

void bsp_start( void );

void bsp_cleanup( void );

rtems_isr_entry set_vector(                    /* returns old vector */
  rtems_isr_entry     handler,                  /* isr routine        */
  rtems_vector_number vector,                   /* vector number      */
  int                 type                      /* RTEMS or RAW intr  */
);

void BSP_fatal_return( void );

void bsp_spurious_initialize( void );

extern rtems_configuration_table BSP_Configuration;     /* owned by BSP */

extern rtems_cpu_table           Cpu_table;             /* owned by BSP */

extern rtems_unsigned32          bsp_isr_level;

#endif /* ASM */

#ifdef __cplusplus
}
#endif

#endif
/* end of include file */
