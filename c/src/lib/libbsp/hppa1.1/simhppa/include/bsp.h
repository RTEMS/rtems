/*  bsp.h
 *
 *  This include file contains all HP PA-RISC simulator definitions.
 *
 *  COPYRIGHT (c) 1989-1999.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.OARcorp.com/rtems/license.html.
 *
 *  $Id$
 */

#ifndef __PXFL_BSP_h
#define __PXFL_BSP_h

#ifdef __cplusplus
extern "C" {
#endif

#include <bspopts.h>

#include <rtems.h>
#include <clockdrv.h>
#include <rtems/ttydrv.h>

/*
 *  confdefs.h overrides for this BSP:
 *   - number of termios serial ports (defaults to 1)
 *   - Interrupt stack space is not minimum if defined.
 */

/* #define CONFIGURE_NUMBER_OF_TERMIOS_PORTS 2 */
#define CONFIGURE_INTERRUPT_STACK_MEMORY  (4 * 1024)

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
#define rtems_bsp_delay( microseconds ) \
        Clock_delay(microseconds);

/*
 * Todo: this should be put somewhere else
 */

#undef CLOCK_DRIVER_TABLE_ENTRY
#define CLOCK_DRIVER_TABLE_ENTRY { Clock_initialize, NULL, NULL, NULL, NULL, Clock_control }
rtems_device_driver Clock_control(
  rtems_device_major_number major,
  rtems_device_minor_number minor,
  void *pargp
);

/*
 * We printf() to a buffer if multiprocessing, *or* if this is set.
 * ref: src/lib/libbsp/hppa/pxfl/iosupp/consupp.c
 */

extern int use_print_buffer;

#define HPPA_INTERRUPT_EXTERNAL_MPCI        HPPA_INTERRUPT_EXTERNAL_10

rtems_isr_entry set_vector(rtems_isr_entry, rtems_vector_number, int);

void bsp_start( void );
void bsp_cleanup( void );

/* miscellaneous stuff assumed to exist */

extern rtems_configuration_table BSP_Configuration;     /* owned by BSP */
extern rtems_cpu_table           Cpu_table;             /* owned by BSP */

extern int cpu_number;         /* from 0; cpu number in a multi cpu system */

#ifdef __cplusplus
}
#endif

#endif
/* end of include file */
