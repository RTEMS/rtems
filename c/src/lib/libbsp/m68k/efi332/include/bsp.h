/*  bsp.h
 *
 *  This include file contains all efi332 board IO definitions.
 *
 *  COPYRIGHT (c) 1989-1998.
 *  On-Line Applications Research Corporation (OAR).
 *  Copyright assigned to U.S. Government, 1994.
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.OARcorp.com/rtems/license.html.
 *
 *  $Id$
 */

#ifndef __EFI332_BSP_h
#define __EFI332_BSP_h

#ifdef __cplusplus
extern "C" {
#endif

#include <rtems.h>
#include <clockdrv.h>
#include <console.h>
#include <iosupp.h>
#include <efi332.h>
#include <sim.h>
#include <qsm.h>

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
 */

/* XXX - JRS - I want to compile the tmtests */

#define MUST_WAIT_FOR_INTERRUPT 1 

#define Install_tm27_vector( handler )

#define Cause_tm27_intr()

#define Clear_tm27_intr()

#define Lower_tm27_intr() 

/*
 *  Simple spin delay in microsecond units for device drivers.
 *  This is very dependent on the clock speed of the target.
 */

#define delay( microseconds ) \
  { register rtems_unsigned32 _delay=(microseconds); \
    register rtems_unsigned32 _tmp=123; \
    asm volatile( "0: \
                     nbcd      %0 ; \
                     nbcd      %0 ; \
                     dbf       %1,0b" \
                  : "=d" (_tmp), "=d" (_delay) \
                  : "0"  (_tmp), "1"  (_delay) ); \
  }

/* externals */

extern char _endtext[];
extern char _sdata[];
extern char _edata[];
extern char __bss_start[];
extern char _end[];
extern char _copy_data_from_rom[];
extern char __end_of_ram[];

/* constants */

#define RAM_END ((int)__end_of_ram)

#ifdef __START_C__
#define STACK_SIZE "#0x800"
#else
#define STACK_SIZE 0x800
#endif

/* macros */

#define RAW_PUTS(str) \
  { register char *ptr = str; \
    while (*ptr) outbyte(*ptr++); \
  }

#define RAW_PUTI(n) { \
    register int i, j; \
    \
    RAW_PUTS("0x"); \
    for (i=28;i>=0;i -= 4) { \
      j = (n>>i) & 0xf; \
      outbyte( (j>9 ? j-10+'a' : j+'0') ); \
    } \
  }

/* miscellaneous stuff assumed to exist */

extern rtems_configuration_table BSP_Configuration;

extern m68k_isr_entry M68Kvec[];   /* vector table address */

extern int stack_size;

extern int stack_start;

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

m68k_isr_entry set_vector(
  rtems_isr_entry     handler,
  rtems_vector_number vector,
  int                 type
);

void console_init(void);

void Spurious_Initialize(void);

void _UART_flush(void);

void Clock_exit(void);

void outbyte(char);

#ifdef __cplusplus
}
#endif

#endif
/* end of include file */
