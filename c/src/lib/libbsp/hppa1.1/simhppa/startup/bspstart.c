/*
 *  This routine starts the application.  It includes application,
 *  board, and monitor specific initialization and configuration.
 *  The generic CPU dependent initialization has been performed
 *  before this routine is invoked.
 *
 *  COPYRIGHT (c) 1989-1999.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 *
 *  $Id$
 */

#include <string.h>

#include <bsp.h>
#include <rtems/libio.h>
#include <rtems/libcsupport.h>

extern rtems_configuration_table  Configuration;

rtems_configuration_table BSP_Configuration;
rtems_cpu_table           Cpu_table;

int cpu_number;

#define WORKSPACE_SIZE (1024 * 1024)
rtems_unsigned8   MY_WORK_SPACE[ WORKSPACE_SIZE ];

/*
 * Amount to increment itimer by each pass
 * It is a variable instead of a #define to allow the 'looptest'
 * script to bump it without recompiling rtems
 */

rtems_unsigned32 CPU_HPPA_CLICKS_PER_TICK;

#if SIMHPPA_FAST_IDLE

/*
 * Many of the tests are very slow on the simulator because they have
 * have 5 second delays hardwired in.
 * Try to speed those tests up by speeding up the clock when in idle
 */

rtems_extension fast_idle_switch_hook(
  rtems_tcb *current_task,
  rtems_tcb *heir_task
)
{
    static rtems_unsigned32 normal_clock = ~0;
    static rtems_unsigned32 fast_clock;

    /* init our params on first call */
    if (normal_clock == (rtems_unsigned32) ~0)
    {
        normal_clock = CPU_HPPA_CLICKS_PER_TICK;
        fast_clock = CPU_HPPA_CLICKS_PER_TICK / 0x100;
        if (fast_clock == 0)    /* who? me?  pathological?  never! */
            fast_clock++;
    }

    /*
     * Checking for 'name' field of 'IDLE' is not the best/safest,
     * but its the best we could think of at the moment.
     */

    if (heir_task == _Thread_Idle)
        CPU_HPPA_CLICKS_PER_TICK = fast_clock;
    else if (current_task == _Thread_Idle)
        CPU_HPPA_CLICKS_PER_TICK = normal_clock;
}

#endif

/*
 *  Use the shared implementations of the following routines
 */
 
void bsp_postdriver_hook(void);
void bsp_libc_init( void *, unsigned32, int );

/*
 *  Function:   bsp_pretasking_hook
 *  Created:    95/03/10
 *
 *  Description:
 *      BSP pretasking hook.  Called just before drivers are initialized.
 *      Used to setup libc and install any BSP extensions.
 *
 *  Notes:
 *      Must not use libc (to do io) from here, since drivers are
 *      not yet initialized.
 */

void bsp_pretasking_hook(void)
{
    extern int end;
    rtems_unsigned32        heap_start;

    heap_start = (rtems_unsigned32) &end;
    if (heap_start & (CPU_ALIGNMENT-1))
        heap_start = (heap_start + CPU_ALIGNMENT) & ~(CPU_ALIGNMENT-1);

    bsp_libc_init((void *) heap_start, 64 * 1024, 0);

    /*
     * on MP systems, always use the print buffer
     *  instead of the (broken) system calls
     */

    if (BSP_Configuration.User_multiprocessing_table)
        use_print_buffer = 1;

#ifdef SIMHPPA_ROM
    use_print_buffer = 1;
#endif

#if SIMHPPA_FAST_IDLE
    /*
     * Install the fast idle task switch extension
     *
     * on MP systems, might now want to do this; it confuses at least
     * one test (mp06)
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

        rc = rtems_extension_create(rtems_build_name('F', 'D', 'L', 'E'),
                              &fast_idle_extension, &extension_id);
        if (rc != RTEMS_SUCCESSFUL)
            rtems_fatal_error_occurred(rc);
    }
#endif
}

void bsp_start(void)
{
    /*
     * Set cpu_number to accurately reflect our cpu number
     */

#ifdef hppa7200
    /*
     * Use HPPA_DR0 if supported
     */
    {
        int dr0;
        HPPA_ASM_MFCPU(HPPA_DR0, dr0);
        cpu_number = (dr0 >> 4) & 0x7;
    }
#else
    if (Configuration.User_multiprocessing_table)
        cpu_number = Configuration.User_multiprocessing_table->node - 1;
    else
        cpu_number = 0;
#endif

    BSP_Configuration.work_space_start = (void *)MY_WORK_SPACE;
    if (BSP_Configuration.work_space_size)
        BSP_Configuration.work_space_size = WORKSPACE_SIZE;

    /*
     * Set up our hooks
     * Make sure libc_init is done before drivers init'd so that
     * they can use atexit()
     */

    Cpu_table.pretasking_hook = bsp_pretasking_hook;    /* init libc, etc. */
    Cpu_table.postdriver_hook = bsp_postdriver_hook;    /* register drivers */

    /*
     *  Don't zero out the workspace.  The simulator did it for us.
     */

    Cpu_table.do_zero_of_workspace = FALSE;
    Cpu_table.interrupt_stack_size = CONFIGURE_INTERRUPT_STACK_MEMORY;

    /*
     * Set this artificially low for the simulator
     */

    Cpu_table.itimer_clicks_per_microsecond = 1;

#if 0
    /*
     * Commented by DIVISION INC.  External interrupt
     * processing is now divorced from RTEMS for HPPA.
     */

    /*
     * Determine the external interrupt processing order
     * the external interrupt handler walks thru this table, in
     * order checking for posted interrupts.
     */

    Cpu_table.external_interrupts = 0;

    Cpu_table.external_interrupt[ Cpu_table.external_interrupts ] =
                                   HPPA_INTERRUPT_EXTERNAL_INTERVAL_TIMER;
    Cpu_table.external_interrupts++;

    if ( Configuration.User_multiprocessing_table ) {
      Cpu_table.external_interrupt[ Cpu_table.external_interrupts ] =
                                               HPPA_INTERRUPT_EXTERNAL_10;
      Cpu_table.external_interrupts++;
    }
#endif

#if SIMHPPA_FAST_IDLE
    /*
     * Add 1 extension for fast idle
     */

    BSP_Configuration.maximum_extensions++;
#endif

    /*
     * Set the "clicks per tick" for the simulator
     *  used by libcpu/hppa/clock/clock.c to schedule interrupts
     *
     * Set it only if 0 to allow for simulator setting it via script
     *   on test startup.
     */

    if (CPU_HPPA_CLICKS_PER_TICK == 0)
        CPU_HPPA_CLICKS_PER_TICK = 0x4000;
}
