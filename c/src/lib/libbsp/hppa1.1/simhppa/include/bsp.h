/*  bsp.h
 *
 *  This include file contains all HP PA-RISC simulator definitions.
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

#ifndef __SIMHPPA_h
#define __SIMHPPA_h

#ifdef __cplusplus
extern "C" {
#endif

#include <rtems.h>
#include <iosupp.h>

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
 */

#define MUST_WAIT_FOR_INTERRUPT 1

#define Install_tm27_vector( handler ) \
    ( void ) set_vector( handler, HPPA_INTERRUPT_EXTERNAL_INTERVAL_TIMER, 1 );

#define Cause_tm27_intr() \
  set_itimer( get_itimer() + 20 )

#define Clear_tm27_intr()  \
  set_eirr( 0x80000000 )

#define Lower_tm27_intr() \
  { \
    register unsigned32 ignored; \
    HPPA_ASM_SSM( HPPA_PSW_I, ignored ); \
  }

/*
 *  Simple spin delay in microsecond units for device drivers.
 *  This is very dependent on the clock speed of the target.
 */

extern void Clock_delay(rtems_unsigned32 microseconds);
#define delay( microseconds ) \
        Clock_delay(microseconds);

/*
 * We printf() to a buffer if multiprocessing, *or* if this is set.
 * ref: src/lib/libbsp/hppa/simhppa/iosupp/consupp.c
 */

extern int use_print_buffer;

#define HPPA_INTERRUPT_EXTERNAL_MPCI        HPPA_INTERRUPT_EXTERNAL_10

void bsp_start( void );
void bsp_cleanup( void );

/* miscellaneous stuff assumed to exist */

extern rtems_configuration_table BSP_Configuration;     /* owned by BSP */
extern rtems_cpu_table           Cpu_table;             /* owned by BSP */

extern rtems_unsigned32          bsp_isr_level;

extern int cpu_number;         /* from 0; cpu number in a multi cpu system */

#ifdef __cplusplus
}
#endif

#endif
/* end of include file */
