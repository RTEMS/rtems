/*
 *	@(#)bspstart.c	1.16 - 95/06/28
 */

/*  bsp_start()
 *
 *  This routine starts the application.  It includes application,
 *  board, and monitor specific initialization and configuration.
 *  The generic CPU dependent initialization has been performed
 *  before this routine is invoked.
 *
 *  Called by RTEMS::RTEMS constructor in startup-ctor.cc
 *
 *  INPUT:  NONE
 *
 *  OUTPUT: NONE
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

#include <rtems.h>
#include <bsp.h>
#include <rtems/libio.h>

#include <libcsupport.h>

#include <string.h>
#include <fcntl.h>

#ifdef STACK_CHECKER_ON
#include <stackchk.h>
#endif

extern rtems_configuration_table  Configuration;

rtems_configuration_table BSP_Configuration;
rtems_cpu_table           Cpu_table;
rtems_unsigned32          bsp_isr_level;

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

rtems_extension
fast_idle_switch_hook(rtems_tcb *current_task,
                      rtems_tcb *heir_task)
{
    static rtems_unsigned32 normal_clock = ~0;
    static rtems_unsigned32 fast_clock;

    /* init our params on first call */
    if (normal_clock == ~0)
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

    if (heir_task->name == rtems_build_name('I', 'D', 'L', 'E'))
        CPU_HPPA_CLICKS_PER_TICK = fast_clock;
    else if (current_task->name == rtems_build_name('I', 'D', 'L', 'E'))
        CPU_HPPA_CLICKS_PER_TICK = normal_clock;
}

#endif

/*
 *  Function:   bsp_libc_init
 *  Created:    94/12/6
 *
 *  Description:
 *      Initialize whatever libc we are using
 *      called from bsp_postdriver_hook
 *
 *
 *  Parameters:
 *      none
 *
 *  Returns:
 *      none.
 *
 *  Side Effects:
 *
 *
 *  Notes:
 *
 *  Deficiencies/ToDo:
 *
 *
 */

void
bsp_libc_init(void)
{
    extern int end;
    rtems_unsigned32        heap_start;

    heap_start = (rtems_unsigned32) &end;
    if (heap_start & (CPU_ALIGNMENT-1))
        heap_start = (heap_start + CPU_ALIGNMENT) & ~(CPU_ALIGNMENT-1);

    RTEMS_Malloc_Initialize((void *) heap_start, 64 * 1024, 0);

    /*
     * Init the RTEMS libio facility to provide UNIX-like system
     *  calls for use by newlib (ie: provide __open, __close, etc)
     *  Uses malloc() to get area for the iops, so must be after malloc init
     */

    rtems_libio_init();

    /*
     * Set up for the libc handling.
     * XXX; this should allow for case of some other non-clock interrupts
     */

    if (BSP_Configuration.ticks_per_timeslice > 0)
        libc_init(1);                /* reentrant if possible */
    else
        libc_init(0);                /* non-reentrant */

    /*
     * on MP systems, always use the print buffer
     *  instead of the (broken) system calls
     */

    if (BSP_Configuration.User_multiprocessing_table)
        use_print_buffer = 1;

#ifdef SIMHPPA_ROM
    use_print_buffer = 1;
#endif
}


/*
 *  Function:   bsp_pretasking_hook
 *  Created:    95/03/10
 *
 *  Description:
 *      BSP pretasking hook.  Called just before drivers are initialized.
 *      Used to setup libc and install any BSP extensions.
 *
 *  Parameters:
 *      none
 *
 *  Returns:
 *      nada
 *
 *  Side Effects:
 *      installs a few extensions
 *
 *  Notes:
 *      Must not use libc (to do io) from here, since drivers are
 *      not yet initialized.
 *
 *  Deficiencies/ToDo:
 *
 *
 */

void
bsp_pretasking_hook(void)
{
    bsp_libc_init();

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

        fast_idle_extension.task_switch  = fast_idle_switch_hook;

        rc = rtems_extension_create(rtems_build_name('F', 'D', 'L', 'E'),
                              &fast_idle_extension, &extension_id);
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
}

/*
 * After drivers are setup, register some "filenames"
 * and open stdin, stdout, stderr files
 *
 * Newlib will automatically associate the files with these
 * (it hardcodes the numbers)
 */

void
bsp_postdriver_hook(void)
{
    int stdin_fd, stdout_fd, stderr_fd;
    
    if ((stdin_fd = __open("/dev/tty00", O_RDONLY, 0)) == -1)
        rtems_fatal_error_occurred('STD0');

    if ((stdout_fd = __open("/dev/tty00", O_WRONLY, 0)) == -1)
        rtems_fatal_error_occurred('STD1');

    if ((stderr_fd = __open("/dev/tty00", O_WRONLY, 0)) == -1)
        rtems_fatal_error_occurred('STD2');

    if ((stdin_fd != 0) || (stdout_fd != 1) || (stderr_fd != 2))
        rtems_fatal_error_occurred('STIO');
}

/*
 *  Function:   bsp_start
 *  Created:    94/12/6
 *
 *  Description:
 *      called by crt0 as our "main" equivalent
 *
 *
 *
 *  Parameters:
 *
 *
 *  Returns:
 *
 *
 *  Side Effects:
 *
 *
 *  Notes:
 *
 *
 *  Deficiencies/ToDo:
 *
 *
 */


void
bsp_start(void)
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

    /*
     *  Copy the table
     */

    BSP_Configuration = Configuration;

    BSP_Configuration.work_space_start = (void *)MY_WORK_SPACE;
    if (BSP_Configuration.work_space_size)
        BSP_Configuration.work_space_size = WORKSPACE_SIZE;

    /*
     * Set up our hooks
     * Make sure libc_init is done before drivers init'd so that
     * they can use atexit()
     */

    Cpu_table.pretasking_hook = bsp_pretasking_hook;    /* init libc, etc. */

    Cpu_table.predriver_hook = NULL;

    Cpu_table.postdriver_hook = bsp_postdriver_hook;    /* register drivers */

    Cpu_table.idle_task = NULL;  /* do not override system IDLE task */

    /*
     *  Don't zero out the workspace.  The simulator did it for us.
     */

    Cpu_table.do_zero_of_workspace = FALSE;

    Cpu_table.interrupt_stack_size = (12 * 1024);

    Cpu_table.extra_system_initialization_stack = 0;

    /*
     * Set this artificially low for the simulator
     */

    Cpu_table.itimer_clicks_per_microsecond = 1;

    /*
     * Determine the external interrupt processing order
     * the external interrupt handler walks thru this table, in
     * order checking for posted interrupts.
     */

    Cpu_table.external_interrupts = 0;

    Cpu_table.external_interrupt[ Cpu_table.external_interrupts ] =
         HPPA_INTERRUPT_EXTERNAL_INTERVAL_TIMER - HPPA_INTERRUPT_EXTERNAL_BASE;
    Cpu_table.external_interrupts++;

    if ( Configuration.User_multiprocessing_table ) {
      Cpu_table.external_interrupt[ Cpu_table.external_interrupts ] =
         HPPA_INTERRUPT_EXTERNAL_10 - HPPA_INTERRUPT_EXTERNAL_BASE;
      Cpu_table.external_interrupts++;
    }

    /*
     * Add 1 region for RTEMS Malloc
     */

    BSP_Configuration.maximum_regions++;

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

#if SIMHPPA_FAST_IDLE
    /*
     * Add 1 extension for fast idle
     */

    BSP_Configuration.maximum_extensions++;
#endif

    /*
     * Tell libio how many fd's we want and allow it to tweak config
     */

    rtems_libio_config(&BSP_Configuration, BSP_LIBIO_MAX_FDS);

    /*
     * Add 1 extension for MPCI_fatal
     */

    if (BSP_Configuration.User_multiprocessing_table)
        BSP_Configuration.maximum_extensions++;

    /*
     * Set the "clicks per tick" for the simulator
     *  used by libcpu/hppa/clock/clock.c to schedule interrupts
     *
     * Set it only if 0 to allow for simulator setting it via script
     *   on test startup.
     */

    if (CPU_HPPA_CLICKS_PER_TICK == 0)
        CPU_HPPA_CLICKS_PER_TICK = 0x4000;

    /*
     *  Start most of RTEMS
     *  main() will start the rest
     */

    bsp_isr_level = rtems_initialize_executive_early(
      &BSP_Configuration,
      &Cpu_table
    );
}
