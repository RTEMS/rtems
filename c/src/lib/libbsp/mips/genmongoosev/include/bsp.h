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
 *  http://www.OARcorp.com/rtems/license.html.
 *
 *  $Id$
 */

#ifndef __GENERIC_MONGOOSE_V_h
#define __GENERIC_MONGOOSE_V_h

#ifdef __cplusplus
extern "C" {
#endif

#include <bspopts.h>

#include <rtems.h>
#include <iosupp.h>
#include <console.h>
#include <clockdrv.h>
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

extern void assertSoftwareInterrupt(unsigned32);





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
 *  NOTE: Following are for XXX and are board independent
 *
 */


#define CLOCK_VECTOR	MONGOOSEV_IRQ_TIMER1


#define MUST_WAIT_FOR_INTERRUPT 1

#if 1

#define Install_tm27_vector( handler ) \
    (void) set_vector( handler, MONGOOSEV_IRQ_SOFTWARE_1, 1 ); 



#define Cause_tm27_intr()   assertSoftwareInterrupt(0);

#define Clear_tm27_intr()  
#define Lower_tm27_intr()

#else

#define Install_tm27_vector( handler ) \
    (void) set_vector( handler, MONGOOSEV_IRQ_TIMER1, 1 ); 

#define Cause_tm27_intr() \
  do { \
    ; \
  } while(0)

#define Clear_tm27_intr() 
#define Lower_tm27_intr()

#endif

/* Constants */

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

/* functions */

void bsp_cleanup( void );

rtems_isr_entry set_vector( rtems_isr_entry, rtems_vector_number, int );
   
#ifdef __cplusplus
}
#endif

#endif

/* end of include file */
