/*  bsp.h
 *
 *  This include file contains all DY-4 DMV170 board IO definitions.
 *
 *  COPYRIGHT (c) 1989-1997.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  $Id$
 */

#ifndef __DMV170_BSP_h
#define __DMV170_BSP_h

#ifdef __cplusplus
extern "C" {
#endif

#include <bspopts.h>

/*
 *  confdefs.h overrides for this BSP:
 *   - termios serial ports (defaults to 1)
 *   - Interrupt stack space is not minimum if defined.
 */

#define CONFIGURE_NUMBER_OF_TERMIOS_PORTS 4
#define CONFIGURE_INTERRUPT_STACK_MEMORY  (12 * 1024)
  
#ifdef ASM
/* Definition of where to store registers in alignment handler */
#define ALIGN_REGS 0x0140

#else
#include <rtems.h>
#include <console.h>
#include <clockdrv.h>
#include <console.h>
#include <iosupp.h>

#include <dmv170.h>

#if 0
#define Enable_Debug() \
  DMV170_WRITE( 0xffffbd0c, 0 )

#define Debug_Entry( num ) \
  DMV170_WRITE( 0xffffbd06, num )
#else
#define Enable_Debug()
#define Debug_Entry( num )
#endif

/*
 * Network driver configuration
 */
struct rtems_bsdnet_ifconfig;
int rtems_dmv177_sonic_driver_attach(struct rtems_bsdnet_ifconfig *config);
#define RTEMS_BSP_NETWORK_DRIVER_NAME   "sonic1"
#define RTEMS_BSP_NETWORK_DRIVER_ATTACH rtems_dmv177_sonic_driver_attach


/*
 * The following macro calculates the Baud constant. For the Z8530 chip.
 */
#define Z8530_Baud( _frequency, _clock_by, _baud_rate  )   \
  ( (_frequency /( _clock_by * 2 * _baud_rate))  - 2)


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

#define MUST_WAIT_FOR_INTERRUPT 1

#define Install_tm27_vector( _handler ) \
  set_vector( (_handler), PPC_IRQ_DECREMENTER, 1 )

#define Cause_tm27_intr()  \
  do { \
    unsigned32 _clicks = 1; \
    asm volatile( "mtdec %0" : "=r" ((_clicks)) : "r" ((_clicks)) ); \
  } while (0)

#define Clear_tm27_intr() \
  do { \
    unsigned32 _clicks = 0xffffffff; \
    unsigned32 _msr = 0; \
    _ISR_Set_level( 0 ); \
    asm volatile( "mfmsr %0 ;" : "=r" (_msr) : "r" (_msr) ); \
    _msr &=  ~0x8000; \
    asm volatile( "mtmsr %0 ;" : "=r" (_msr) : "r" (_msr) ); \
    asm volatile( "mtdec %0" : "=r" ((_clicks)) : "r" ((_clicks)) ); \
  } while (0)

#define Lower_tm27_intr() \
  do { \
    unsigned32 _msr = 0; \
    _ISR_Set_level( 0 ); \
    asm volatile( "mfmsr %0 ;" : "=r" (_msr) : "r" (_msr) ); \
    _msr |=  0x8002; \
    asm volatile( "mtmsr %0 ;" : "=r" (_msr) : "r" (_msr) ); \
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

extern int   RAM_START;
extern int   RAM_END;
extern int   RAM_SIZE;

extern int   PROM_START;
extern int   PROM_END;
extern int   PROM_SIZE;

extern int   CLOCK_SPEED;

extern int   end;        /* last address in the program */

/*
 * How many libio files we want
 */
 
#define BSP_LIBIO_MAX_FDS       20

/* functions */

/* 
 * genvec.c
 */
rtems_isr_entry  set_EE_vector(
  rtems_isr_entry     handler,                  /* isr routine        */
  rtems_vector_number vector                    /* vector number      */
);
void initialize_external_exception_vector ();

/*
 * console.c
 */
void DEBUG_puts( char *string );

void BSP_fatal_return( void );



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

extern int   CPU_PPC_CLICKS_PER_MS;

#endif /* ASM */

#ifdef __cplusplus
}
#endif

#endif
/* end of include file */
