/*  bspstart.c
 *
 *  This set of routines starts the application.  It includes application,
 *  board, and monitor specific initialization and configuration.
 *  The generic CPU dependent initialization has been performed
 *  before any of these are invoked.
 *
 *  Called by RTEMS::RTEMS constructor in rtems-ctor.cc
 *
 *  COPYRIGHT (c) 1989, 1990, 1991, 1992, 1993, 1994, 1997.
 *  On-Line Applications Research Corporation (OAR).
 *  All rights assigned to U.S. Government, 1994.
 *
 *  This material may be reproduced by or for the U.S. Government pursuant
 *  to the copyright license under the clause at DFARS 252.227-7013.  This
 *  notice must appear in all copies of this file and its derivatives.
 *
 *  $Id$
 */

#include <bsp.h>
#include <rtems/libio.h>

#include <libcsupport.h>

#include <string.h>
#include <fcntl.h>

#ifdef STACK_CHECKER_ON
#include <stackchk.h>
#endif

/*
 *  The original table from the application and our copy of it with
 *  some changes.
 */
 
extern rtems_configuration_table  Configuration;
rtems_configuration_table         BSP_Configuration;

rtems_cpu_table   Cpu_table;
rtems_unsigned32  bsp_isr_level;

/*
 *  Tells us where to put the workspace in case remote debugger is present.
 */

#if 0
extern rtems_unsigned32  rdb_start;
#endif

/*
 * Amount to increment itimer by each pass
 * It is a variable instead of a #define to allow the 'looptest'
 * script to bump it without recompiling rtems
 *
 *  NOTE:  This is based on the PA-RISC simulator.  I don't know if we
 *         can actually pull this trick on the PPC simulator.
 */

rtems_unsigned32 CPU_PPC_CLICKS_PER_TICK;

#if PSIM_FAST_IDLE

/*
 * Many of the tests are very slow on the simulator because they have
 * have 5 second delays hardwired in.
 *
 * Try to speed those tests up by speeding up the clock when in the idle task.
 *
 *  NOTE:  At the current setting, 5 second delays in the tests take 
 *         approximately 5 seconds of wall time. 
 */

rtems_extension
fast_idle_switch_hook(rtems_tcb *current_task,
                      rtems_tcb *heir_task)
{
    static rtems_unsigned32 normal_clock = ~0;
    static rtems_unsigned32 fast_clock;

    /* init our params on first call */
    if (normal_clock == (rtems_unsigned32) ~0)
    {
        normal_clock = CPU_PPC_CLICKS_PER_TICK;
        fast_clock = 10000;
#if 0
        fast_clock = CPU_PPC_CLICKS_PER_TICK / 0x10 ;
#endif
        if (fast_clock == 0)    /* handle pathological case */
            fast_clock++;
    }

    /*
     * Run the clock faster when idle is in place.
     */

    if (heir_task == _Thread_Idle)
        CPU_PPC_CLICKS_PER_TICK = fast_clock;
    else if (current_task == _Thread_Idle)
        CPU_PPC_CLICKS_PER_TICK = normal_clock;
}

#endif

/*
 *  bsp_libc_init
 *
 *  Initialize whatever libc we are using called from bsp_postdriver_hook.
 */

void bsp_libc_init(void)
{
  extern int end;
  rtems_unsigned32 heap_start;
  rtems_unsigned32 heap_size;

  heap_start = (rtems_unsigned32) &end;
  if (heap_start & (CPU_ALIGNMENT-1))
    heap_start = (heap_start + CPU_ALIGNMENT) & ~(CPU_ALIGNMENT-1);

  heap_size = BSP_Configuration.work_space_start - (void *)&end;
  heap_size &= 0xfffffff0;  /* keep it as a multiple of 16 bytes */

  RTEMS_Malloc_Initialize((void *) heap_start, heap_size, 0);

  /*
   *  Init the RTEMS libio facility to provide UNIX-like system
   *  calls for use by newlib (ie: provide __rtems_open, __rtems_close, etc)
   *  Uses malloc() to get area for the iops, so must be after malloc init
   */
 
  rtems_libio_init();
 
  /*
   * Set up for the libc handling.
   */
 
  if (BSP_Configuration.ticks_per_timeslice > 0)
    libc_init(1);                /* reentrant if possible */
  else
    libc_init(0);                /* non-reentrant */

}


/*
 *  bsp_pretasking_hook
 *
 *  BSP pretasking hook.  Called just before drivers are initialized.
 *  Used to setup libc and install any BSP extensions.
 */

void bsp_pretasking_hook(void)
{
  bsp_libc_init();

#if PSIM_FAST_IDLE
  /*
   *  Install the fast idle task switch extension
   *
   *  On MP systems, might not want to do this; it confuses at least
   *  one test (mp06) on the PA-RISC simulator
   */

#if 0
  if (BSP_Configuration.User_multiprocessing_table == 0)
#endif
  {
    rtems_extensions_table  fast_idle_extension;
    rtems_id                extension_id;
    rtems_status_code       rc;

    memset(&fast_idle_extension, 0, sizeof(fast_idle_extension));

    fast_idle_extension.thread_switch  = fast_idle_switch_hook;

    rc = rtems_extension_create(
      rtems_build_name('F', 'D', 'L', 'E'),
      &fast_idle_extension, 
      &extension_id
    );
    if (rc != RTEMS_SUCCESSFUL)
      rtems_fatal_error_occurred(rc);
  }
#endif


#ifdef STACK_CHECKER_ON
  /*
   *  Initialize the stack bounds checker
   *  We can either turn it on here or from the app.
   */

  Stack_check_Initialize();
#endif

#ifdef RTEMS_DEBUG
  rtems_debug_enable( RTEMS_DEBUG_ALL_MASK );
#endif

}

/*
 *  Use the shared bsp_postdriver_hook() implementation 
 */
 
void bsp_postdriver_hook(void);

/*
 *  bsp_start
 *
 *  This routine does the bulk of the system initialization.
 */

void bsp_start( void )
{
  unsigned char *work_space_start;

#if 0
  /* 
   * Set MSR to show vectors at 0 XXX
   */
  _CPU_MSR_Value( msr_value );
  msr_value &= ~PPC_MSR_EP;
  _CPU_MSR_SET( msr_value );
#endif

  /*
   * Set up our hooks
   * Make sure libc_init is done before drivers initialized so that
   * they can use atexit()
   */

  Cpu_table.pretasking_hook = bsp_pretasking_hook;    /* init libc, etc. */

  Cpu_table.predriver_hook = NULL; /* bsp_spurious_initialize;*/

  Cpu_table.postdriver_hook = bsp_postdriver_hook;

  Cpu_table.idle_task = NULL;  /* do not override system IDLE task */

  /*
   *  PSIM does zero out memory BUT only when IT begins execution.  Thus
   *  if we want to have a clean slate in the workspace each time we
   *  begin execution of OUR application, then we must zero the workspace.
   */

  Cpu_table.do_zero_of_workspace = FALSE;

  /*
   *  This should be enough interrupt stack.
   */

  Cpu_table.interrupt_stack_size = (12 * 1024);

  /*
   *  SIS does not support MP configurations so there is really no way
   *  to check this out.
   */

  Cpu_table.extra_mpci_receive_server_stack = 0;

  /*
   *  The monitor likes the exception table to be at 0x0.
   */

  Cpu_table.exceptions_in_RAM = TRUE;

  /*
   *  Copy the table and allocate memory for the RTEMS Workspace
   */

  BSP_Configuration = Configuration;

#if defined(RTEMS_POSIX_API)
  BSP_Configuration.work_space_size *= 3;
#endif


#if 0
  work_space_start = 
    (unsigned char *)rdb_start - BSP_Configuration.work_space_size;
#endif

  work_space_start = 
    (unsigned char *)&RAM_END - BSP_Configuration.work_space_size;

  if ( work_space_start <= (unsigned char *)&end ) {
    DEBUG_puts( "bspstart: Not enough RAM!!!\n" );
    bsp_cleanup();
  }

  BSP_Configuration.work_space_start = work_space_start;

  /*
   * Add 1 region for RTEMS Malloc
   */

  BSP_Configuration.RTEMS_api_configuration->maximum_regions++;

#ifdef RTEMS_NEWLIB
  /*
   * Add 1 extension for newlib libc
   */

  BSP_Configuration.maximum_extensions++;
#endif

#ifdef STACK_CHECKER_ON
  /*
   * Add 1 extension for stack checker
   */

  BSP_Configuration.maximum_extensions++;
#endif

#if PSIM_FAST_IDLE
  /*
   * Add 1 extension for fast idle
   */

  BSP_Configuration.maximum_extensions++;
#endif

  /*
   * Add 1 extension for MPCI_fatal
   */

  if (BSP_Configuration.User_multiprocessing_table)
    BSP_Configuration.maximum_extensions++;

  /*
   * Set the "clicks per tick" for the simulator
   *  used by XXX/clock/clock.c to schedule interrupts
   *
   *  NOTE: On psim, each click of the decrementer register corresponds
   *        to 1 instruction.  By setting this to 100, we are indicating
   *        that we are assuming it can execute 100 instructions per
   *        microsecond.  This corresponds to sustaining 1 instruction
   *        per cycle at 100 Mhz.  Whether this is a good guess or not
   *        is anyone's guess.
   */

  {
    extern int PSIM_INSTRUCTIONS_PER_MICROSECOND;

    CPU_PPC_CLICKS_PER_TICK = BSP_Configuration.microseconds_per_tick * 
      (int) &PSIM_INSTRUCTIONS_PER_MICROSECOND;
  }

  /*
   *  Initialize RTEMS. main() will finish it up and start multitasking.
   */

  rtems_libio_config( &BSP_Configuration, BSP_LIBIO_MAX_FDS );
}
