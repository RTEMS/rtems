/*
 * Board Support Package for `Generic' Motorola MC68360
 *
 * Based on the `gen68302' board support package, and covered by the
 * original distribution terms.
 *
 * W. Eric Norum
 * Saskatchewan Accelerator Laboratory
 * University of Saskatchewan
 * Saskatoon, Saskatchewan, CANADA
 * eric@skatter.usask.ca
 *
 *  $Id$
 */

/*  bsp.h
 *
 *  COPYRIGHT (c) 1989-1999.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.OARcorp.com/rtems/license.html.
 */

#ifndef __GEN68360_BSP_h
#define __GEN68360_BSP_h

#ifdef __cplusplus
extern "C" {
#endif

#include <bspopts.h>

#include <rtems.h>
#include <console.h>
#include <iosupp.h>
#include <clockdrv.h>

/*
 *  confdefs.h overrides for this BSP:
 *   - termios serial ports (defaults to 1)
 *   - Interrupt stack space is not minimum if defined.
 */

/* #define CONFIGURE_NUMBER_OF_TERMIOS_PORTS 2 */
#define CONFIGURE_INTERRUPT_STACK_MEMORY  (4 * 1024)
  
/*
 * Network driver configuration
 */
struct rtems_bsdnet_ifconfig;
extern int rtems_scc1_driver_attach (struct rtems_bsdnet_ifconfig *config, int attaching);
#define RTEMS_BSP_NETWORK_DRIVER_NAME	"scc1"
#define RTEMS_BSP_NETWORK_DRIVER_ATTACH	rtems_scc1_driver_attach

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
 *  Don't bother with hardware -- just use a software-interrupt
 */

#define MUST_WAIT_FOR_INTERRUPT 0

#define Install_tm27_vector( handler ) set_vector( (handler), 34, 1 )

#define Cause_tm27_intr()	asm volatile ("trap #2");

#define Clear_tm27_intr()

#define Lower_tm27_intr()

/*
 *  Simple spin delay in microsecond units for device drivers.
 *  This is very dependent on the clock speed of the target.
 */

#define rtems_bsp_delay( microseconds ) \
  { register rtems_unsigned32 _delay=(microseconds); \
    register rtems_unsigned32 _tmp=123; \
    asm volatile( "0: \
                     nbcd      %0 ; \
                     nbcd      %0 ; \
                     dbf       %1,0b" \
                  : "=d" (_tmp), "=d" (_delay) \
                  : "0"  (_tmp), "1"  (_delay) ); \
  }

/* Constants */

/* Structures */

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

extern rtems_configuration_table BSP_Configuration;

extern m68k_isr_entry M68Kvec[];   /* vector table address */

/* functions */

void bsp_cleanup( void );

void M360ExecuteRISC( rtems_unsigned16 command );
void *M360AllocateBufferDescriptors( int count );
void *M360AllocateRiscTimers( int count );
extern char M360DefaultWatchdogFeeder;

m68k_isr_entry set_vector(
  rtems_isr_entry     handler,
  rtems_vector_number vector,
  int                 type
);

/*
 * Values assigned by link editor
 */
extern void *_RomBase, *_RamBase;

/*
 * Definitions for Atlas Computer Equipment Inc. High Speed Bridge (HSB)
 */
#define ATLASHSB_ESR    0x20010000L
#define ATLASHSB_USICR  0x20010001L
#define ATLASHSB_DSRR   0x20010002L
#define ATLASHSB_LED4   0x20010004L
#define ATLASHSB_ROM_U6 0xFF080000L	/* U6 flash ROM socket */

#ifdef __cplusplus
}
#endif

#endif
/* end of include file */
