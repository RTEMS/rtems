/*  bsp.h
 *
 *  This include file contains all POSIX BSP definitions.
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

#ifndef __POSIX_BSP_h
#define __POSIX_BSP_h

#ifdef __cplusplus
extern "C" {
#endif

#include <rtems.h>
#include <clockdrv.h>
#include <console.h>
#include <iosupp.h>
#include <libcsupport.h>
#include <signal.h>

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

#define Install_tm27_vector( handler ) \
    set_vector( (handler), 16, 1 )

#define Cause_tm27_intr()  \
    raise( 16 )

#define Clear_tm27_intr()

#define Lower_tm27_intr() \
    _ISR_Set_level( 0 );

#define RAM_START 0
#define RAM_END   0x100000

/* miscellaneous stuff assumed to exist */

extern rtems_configuration_table BSP_Configuration;

/*
 * Define this to use signals for MPCI shared memory driver.
 * If undefined, the shared memory driver will poll from the
 * clock interrupt.
 * Ref: ../shmsupp/getcfg.c
 *
 * BEWARE:: many UN*X kernels and debuggers become severely confused when
 *          debugging programs which use signals.  The problem is *much*
 *          worse when using multiple signals, since ptrace(2) tends to
 *          drop all signals except 1 in the case of multiples.
 *          On hpux9, this problem was so bad, we couldn't use interrupts
 *          with the shared memory driver if we ever hoped to debug
 *          RTEMS programs.
 *          Maybe systems that use /proc don't have this problem...
 */
 
/* #define INTERRUPT_EXTERNAL_MPCI        SIGUSR1 */

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

rtems_isr_entry set_vector(rtems_isr_entry, rtems_vector_number, int);
void bsp_start( void );
void bsp_cleanup( void );

/* miscellaneous stuff assumed to exist */

extern rtems_configuration_table BSP_Configuration;     /* owned by BSP */
extern rtems_cpu_table           Cpu_table;             /* owned by BSP */

extern int                       rtems_argc;
extern char                    **rtems_argv;
extern char                    **rtems_envp;

extern rtems_unsigned32          bsp_isr_level;

extern char *rtems_progname;    /* UNIX executable name */

extern int cpu_number;

#ifdef __cplusplus
}
#endif

#endif
/* end of include file */
