/*  bsp.h
 *
 *  This include file contains all board IO definitions.
 *
 *  XXX : put yours in here
 *
 *  COPYRIGHT (c) 1989, 1990, 1991, 1992, 1993, 1994.
 *  On-Line Applications Research Corporation (OAR).
 *  All rights assigned to U.S. Government, 1994.
 *
 *  This material may be reproduced by or for the U.S. Government pursuant
 *  to the copyright license under the clause at DFARS 252.227-7013.  This
 *  notice must appear in all copies of this file and its derivatives.
 *
 *  $Id$
 */
/* @(#)bsp.h       03/15/96     1.1 */

#ifndef __NO_BSP_h
#define __NO_BSP_h

#ifdef __cplusplus
extern "C" {
#endif

#include <rtems.h>
#include <console.h>
#include <clockdrv.h>

extern void WriteDisplay( char * string );

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

#define MUST_WAIT_FOR_INTERRUPT 0

#define Install_tm27_vector( handler ) set_vector( (handler), 0, 1 )

#define Cause_tm27_intr()

#define Clear_tm27_intr()

#define Lower_tm27_intr()

extern unsigned32 mips_get_timer( void );

#define CPU_CLOCK_RATE_MHZ     (50)
#define CLOCKS_PER_MICROSECOND ( CPU_CLOCK_RATE_MHZ ) /* equivalent to CPU clock speed in MHz */

/*
 *  Simple spin delay in microsecond units for device drivers.
 *  This is very dependent on the clock speed of the target.
 *
 *  NOTE: This macro generates a warning like "integer constant out 
 *        of range" which is safe to ignore.  In 64 bit mode, unsigned32
 *        types are actually 64 bits long so that comparisons between
 *        unsigned32 types and pointers are valid.  The warning is caused
 *        by code in the delay macro that is necessary for 64 bit mode.
 */

#define delay( microseconds ) \
  { \
     unsigned32 _end_clock = mips_get_timer() + microseconds * CLOCKS_PER_MICROSECOND; \
     _end_clock %= 0x100000000;  /* make sure result is 32 bits */ \
     \
     /* handle timer overflow, if necessary */ \
     while ( _end_clock < mips_get_timer() );  \
     \
     while ( _end_clock > mips_get_timer() ); \
  }

/* Constants */

#define RAM_START 0
#define RAM_END   0x100000

/* miscellaneous stuff assumed to exist */

extern rtems_configuration_table BSP_Configuration;

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

mips_isr_entry set_vector(                    /* returns old vector */
  rtems_isr_entry     handler,                  /* isr routine        */
  rtems_vector_number vector,                   /* vector number      */
  int                 type                      /* RTEMS or RAW intr  */
);

#ifdef __cplusplus
}
#endif

#endif
/* end of include file */
