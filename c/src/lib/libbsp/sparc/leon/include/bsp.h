/*  bsp.h
 *
 *  This include file contains all SPARC simulator definitions.
 *
 *  COPYRIGHT (c) 1989-1998.
 *  On-Line Applications Research Corporation (OAR).
 *  Copyright assigned to U.S. Government, 1994.
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.OARcorp.com/rtems/license.html.
 *
 *  Ported to ERC32 implementation of the SPARC by On-Line Applications
 *  Research Corporation (OAR) under contract to the European Space 
 *  Agency (ESA).
 *
 *  ERC32 modifications of respective RTEMS file: COPYRIGHT (c) 1995. 
 *  European Space Agency.
 *
 *  $Id$
 */

#ifndef __SIS_h
#define __SIS_h

#ifdef __cplusplus
extern "C" {
#endif

#include <bspopts.h>

#include <rtems.h>
#include <iosupp.h>
#include <leon.h>
#include <clockdrv.h>

#include <console.h>

/*
 *  confdefs.h overrides for this BSP:
 *   - two termios serial ports
 *   - Interrupt stack space is not minimum if defined.
 */

#define CONFIGURE_NUMBER_OF_TERMIOS_PORTS 2
#define CONFIGURE_INTERRUPT_STACK_MEMORY  (16 * 1024)


/*
 *  Define the time limits for RTEMS Test Suite test durations.
 *  Long test and short test duration limits are provided.  These
 *  values are in seconds and need to be converted to ticks for the
 *  application.
 *
 */

#define MAX_LONG_TEST_DURATION       3   /* 3 seconds */
#define MAX_SHORT_TEST_DURATION      3   /* 3 seconds */

/*
 *  Define the interrupt mechanism for Time Test 27
 *
 *  NOTE: Since the interrupt code for the SPARC supports both synchronous
 *        and asynchronous trap handlers, support for testing with both
 *        is included.
 */

#define SIS_USE_SYNCHRONOUS_TRAP  0

/*
 *  The synchronous trap is an arbitrarily chosen software trap.
 */

#if (SIS_USE_SYNCHRONOUS_TRAP == 1)

#define TEST_VECTOR SPARC_SYNCHRONOUS_TRAP( 0x90 )

#define MUST_WAIT_FOR_INTERRUPT 1

#define Install_tm27_vector( handler ) \
  set_vector( (handler), TEST_VECTOR, 1 );

#define Cause_tm27_intr() \
  asm volatile( "ta 0x10; nop " );

#define Clear_tm27_intr()  

#define Lower_tm27_intr() 

/*
 *  The asynchronous trap is an arbitrarily chosen ERC32 interrupt source.
 */

#else   /* use a regular asynchronous trap */

#define TEST_INTERRUPT_SOURCE LEON_INTERRUPT_EXTERNAL_1
#define TEST_VECTOR LEON_TRAP_TYPE( TEST_INTERRUPT_SOURCE )
 
#define MUST_WAIT_FOR_INTERRUPT 1
 
#define Install_tm27_vector( handler ) \
  set_vector( (handler), TEST_VECTOR, 1 );
 
#define Cause_tm27_intr() \
  do { \
    LEON_Force_interrupt( TEST_INTERRUPT_SOURCE ); \
    nop(); \
    nop(); \
    nop(); \
  } while (0)
 
#define Clear_tm27_intr() \
  LEON_Clear_interrupt( TEST_INTERRUPT_SOURCE )
 
#define Lower_tm27_intr()

#endif

/*
 *  Simple spin delay in microsecond units for device drivers.
 *  This is very dependent on the clock speed of the target.
 */

extern void Clock_delay(rtems_unsigned32 microseconds);

#define rtems_bsp_delay( microseconds ) Clock_delay(microseconds)

/* Constants */

/*
 *  Information placed in the linkcmds file.
 */

extern int   RAM_START;
extern int   RAM_END;
extern int   RAM_SIZE;
 
extern int   PROM_START;
extern int   PROM_END;
extern int   PROM_SIZE;

extern int   CLOCK_SPEED;
 
extern int   end;        /* last address in the program */

/*
 *  Device Driver Table Entries
 */
 
/*
 * NOTE: Use the standard Console driver entry
 */
 
/*
 * NOTE: Use the standard Clock driver entry
 */
 
 
/* miscellaneous stuff assumed to exist */

void bsp_cleanup( void );

void bsp_start( void );

rtems_isr_entry set_vector(                     /* returns old vector */
    rtems_isr_entry     handler,                /* isr routine        */
    rtems_vector_number vector,                 /* vector number      */
    int                 type                    /* RTEMS or RAW intr  */
);

void DEBUG_puts( char *string );

void BSP_fatal_return( void );

void bsp_spurious_initialize( void );

extern rtems_configuration_table BSP_Configuration;     /* owned by BSP */

extern rtems_cpu_table           Cpu_table;             /* owned by BSP */

#ifdef __cplusplus
}
#endif

#endif
/* end of include file */
