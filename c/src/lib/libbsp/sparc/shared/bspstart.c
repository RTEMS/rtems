/*
 *  This set of routines starts the application.  It includes application,
 *  board, and monitor specific initialization and configuration.
 *  The generic CPU dependent initialization has been performed
 *  before any of these are invoked.
 *
 *  COPYRIGHT (c) 1989-1999.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 *
 *  Ported to ERC32 implementation of the SPARC by On-Line Applications
 *  Research Corporation (OAR) under contract to the European Space 
 *  Agency (ESA).
 *
 *  ERC32 modifications of respective RTEMS file: COPYRIGHT (c) 1995. 
 *  European Space Agency.
 *
 *  $Id$
 */

/* must be identical to STACK_SIZE in start.S */
#define STACK_SIZE 16 * 1024

#include <string.h>

#include <bsp.h>
#include <rtems/libio.h>
#include <rtems/libcsupport.h>
 
/*
 *  The original table from the application and our copy of it with
 *  some changes.
 */
 
extern rtems_configuration_table  Configuration;
rtems_configuration_table         BSP_Configuration;

rtems_cpu_table   Cpu_table;

/*
 *  Tells us where to put the workspace in case remote debugger is present.
 */

extern rtems_unsigned32  rdb_start;

/*
 * Amount to increment itimer by each pass
 * It is a variable instead of a #define to allow the 'looptest'
 * script to bump it without recompiling rtems
 *
 *  NOTE:  This is based on the PA-RISC simulator.  I don't know if we
 *         can actually pull this trick on the SPARC simulator.
 */

rtems_unsigned32 CPU_SPARC_CLICKS_PER_TICK;

#if SIMSPARC_FAST_IDLE

/*
 * Many of the tests are very slow on the simulator because they have
 * have 5 second delays hardwired in.
 *
 * Try to speed those tests up by speeding up the clock when in the idle task.
 *
 *  NOTE:  At the current setting, 5 second delays in the tests take 
 *         approximately 5 seconds of wall time. 
 */

rtems_extension fast_idle_switch_hook(
  rtems_tcb *current_task,
  rtems_tcb *heir_task
)
{
    static rtems_unsigned32 normal_clock = ~0;
    static rtems_unsigned32 fast_clock;

    /* init our params on first call */
    if (normal_clock == ~0)
    {
        normal_clock = CPU_SPARC_CLICKS_PER_TICK;
        fast_clock = CPU_SPARC_CLICKS_PER_TICK / 0x08;
        if (fast_clock == 0)    /* handle pathological case */
            fast_clock++;
    }

    /*
     * Run the clock faster when idle is in place.
     */

    if (heir_task == _Thread_Idle)
        CPU_SPARC_CLICKS_PER_TICK = fast_clock;
    else if (current_task == _Thread_Idle)
        CPU_SPARC_CLICKS_PER_TICK = normal_clock;
}

#endif

/*
 *  Use the shared implementations of the following routines
 */
 
void bsp_postdriver_hook(void);
void bsp_libc_init( void *, unsigned32, int );
extern void bsp_spurious_initialize();

/*
 *  bsp_pretasking_hook
 *
 *  BSP pretasking hook.  Called just before drivers are initialized.
 *  Used to setup libc and install any BSP extensions.
 */

void bsp_pretasking_hook(void)
{
  extern int end;
  rtems_unsigned32 heap_start;
  rtems_unsigned32 heap_size;

  heap_start = (rtems_unsigned32) &end;
  if (heap_start & (CPU_ALIGNMENT-1))
    heap_start = (heap_start + CPU_ALIGNMENT) & ~(CPU_ALIGNMENT-1);

  heap_size = BSP_Configuration.work_space_start - (void *)&end - STACK_SIZE;
  heap_size &= 0xfffffff0;  /* keep it as a multiple of 16 bytes */

  bsp_libc_init((void *) heap_start, heap_size, 0);


#if SIMSPARC_FAST_IDLE
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

#ifdef RTEMS_DEBUG
  rtems_debug_enable( RTEMS_DEBUG_ALL_MASK );
#endif

  bsp_spurious_initialize();
}

/*
 *  bsp_start
 *
 *  This routine does the bulk of the system initialization.
 */

void bsp_start( void )
{
  unsigned char *work_space_start;

  /*
   * Set up our hooks
   * Make sure libc_init is done before drivers initialized so that
   * they can use atexit()
   */

  Cpu_table.pretasking_hook = bsp_pretasking_hook;    /* init libc, etc. */
  Cpu_table.postdriver_hook = bsp_postdriver_hook;

  /*
   *  SIS does zero out memory BUT only when IT begins execution.  Thus
   *  if we want to have a clean slate in the workspace each time we
   *  begin execution of OUR application, then we must zero the workspace.
   */
  Cpu_table.do_zero_of_workspace = TRUE;

  /*
   *  This should be enough interrupt stack.
   */

  Cpu_table.interrupt_stack_size = CONFIGURE_INTERRUPT_STACK_MEMORY;

  work_space_start = 
    (unsigned char *)rdb_start - BSP_Configuration.work_space_size;

  if ( work_space_start <= (unsigned char *)&end ) {
    DEBUG_puts( "bspstart: Not enough RAM!!!\n" );
    BSP_fatal_return();
  }

  BSP_Configuration.work_space_start = work_space_start;

#if SIMSPARC_FAST_IDLE
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
   */

  CPU_SPARC_CLICKS_PER_TICK = BSP_Configuration.microseconds_per_tick;
}
