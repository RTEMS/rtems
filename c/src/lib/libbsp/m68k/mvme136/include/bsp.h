/*  bsp.h
 *
 *  This include file contains all MVME136 board IO definitions.
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

#ifndef __MVME136_h
#define __MVME136_h

#ifdef __cplusplus
extern "C" {
#endif

#include <bspopts.h>

#include <rtems.h>
#include <clockdrv.h>
#include <console.h>
#include <iosupp.h>

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

#define MAX_LONG_TEST_DURATION       300 /* 5 minutes = 300 seconds */
#define MAX_SHORT_TEST_DURATION      3   /* 3 seconds */

/*
 *  Define the interrupt mechanism for Time Test 27
 *
 *  NOTE: Use the MPCSR vector for the MVME136
 */

#define MUST_WAIT_FOR_INTERRUPT 0

#define Install_tm27_vector( handler ) set_vector( (handler), 75, 1 )

#define Cause_tm27_intr()  (*(volatile rtems_unsigned8 *)0xfffb006b) = 0x80

#define Clear_tm27_intr()  (*(volatile rtems_unsigned8 *)0xfffb006b) = 0x00

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

#define RAM_START 0
#define RAM_END   0x100000

#define M681ADDR      0xfffb0040         /* address of the M68681 chip */
#define RXRDYB        0x01               /* status reg recv ready mask */
#define TXRDYB        0x04               /* status reg trans ready mask */
#define PARITYERR     0x20               /* status reg parity error mask */
#define FRAMEERR      0x40               /* status reg frame error mask */


#define FOREVER       1                  /* infinite loop */

/* Structures */

struct r_m681_info {
  char fill1[ 5 ];                       /* channel A regs ( not used ) */
  char isr;                              /* interrupt status reg */
  char fill2[ 2 ];                       /* counter regs (not used) */
  char mr1mr2b;                          /* MR1B and MR2B regs */
  char srb;                              /* status reg channel B */
  char fill3;                            /* do not access */
  char rbb;                              /* receive buffer channel B */
  char ivr;                              /* interrupt vector register */
};

struct w_m681_info {
  char fill1[ 4 ];                       /* channel A regs (not used) */
  char acr;                              /* auxillary control reg */
  char imr;                              /* interrupt mask reg */
  char fill2[ 2 ];                       /* counter regs (not used) */
  char mr1mr2b;                          /* MR1B and MR2B regs */
  char csrb;                             /* clock select reg */
  char crb;                              /* command reg */
  char tbb;                              /* transmit buffer channel B */
  char ivr;                              /* interrupt vector register */
};

#ifdef M136_INIT
#undef EXTERN
#define EXTERN
#else
#undef EXTERN
#define EXTERN extern
#endif

/* miscellaneous stuff assumed to exist */

extern rtems_configuration_table BSP_Configuration;

/* M68681 DUART chip register variables */

EXTERN volatile struct r_m681_info *_Read_m681;  /* M68681 read registers */
EXTERN volatile struct w_m681_info *_Write_m681; /* M68681 write registers */

extern m68k_isr_entry M68Kvec[];   /* vector table address */

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

m68k_isr_entry set_vector(
  rtems_isr_entry     handler,
  rtems_vector_number vector,
  int                 type
);

#ifdef __cplusplus
}
#endif

#endif
/* end of include file */
