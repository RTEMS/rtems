/*
 *  This set of routines starts the application.  It includes application,
 *  board, and monitor specific initialization and configuration.
 *  The generic CPU dependent initialization has been performed
 *  before any of these are invoked.
 *
 *  COPYRIGHT (c) 1989-1998.
 *  On-Line Applications Research Corporation (OAR).
 *  All rights assigned to U.S. Government, 1994.
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.OARcorp.com/rtems/license.html.
 *
 *  $Id$
 */

#include <bsp.h>
#include <rtems/libio.h>

#include <libcsupport.h>

#include <string.h>
#include <fcntl.h>

/*
 *  The original table from the application and our copy of it with
 *  some changes.
 */
 
rtems_configuration_table         BSP_Configuration;

rtems_cpu_table   Cpu_table;



/*
 *  Use the shared implementations of the following routines
 */
 
void bsp_postdriver_hook(void);
void bsp_libc_init( void *, unsigned32, int );

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
    rtems_isr_entry old_handler;
    rtems_status_code sc;

    heap_start = (rtems_unsigned32) &end;
    if (heap_start & (CPU_ALIGNMENT-1)) {
        heap_start = (heap_start + CPU_ALIGNMENT) & ~(CPU_ALIGNMENT-1);
    }

    heap_size = BSP_Configuration.work_space_start - (void *)&end;
    heap_size &= 0xfffffff0;  /* keep it as a multiple of 16 bytes */

    bsp_libc_init((void *) heap_start, heap_size, 0);

    sc = interrupt_controller_init();



#ifdef RTEMS_DEBUG
    rtems_debug_enable( RTEMS_DEBUG_ALL_MASK );
#endif

}

/*
 *  bsp_start
 *
 *  This routine does the bulk of the system initialization.
 */

void bsp_start( void )
{
    unsigned char *work_space_start;
    unsigned32 RAM_end = 0;

#if 1
    /* 
    * Set MSR to show vectors at 0 XXX
    */
    rtems_unsigned32 msr_value;

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
    Cpu_table.postdriver_hook = bsp_postdriver_hook;

    Cpu_table.interrupt_stack_size = (12 * 1024);

    /*
    *  The monitor likes the exception table to be at 0x0.
    */

    Cpu_table.exceptions_in_RAM = TRUE;

    /*
    #if defined(RTEMS_POSIX_API)
    BSP_Configuration.work_space_size *= 3;
    #endif
    */

    /* from PSIM - why? */
    BSP_Configuration.work_space_size += 1024;

    /* determine memory size */
    switch (Falcon_MEMCR.MemSize) {
        case RAM_16_MB:
            RAM_end = 16 * 1024 * 1024;
            break;
        case RAM_32_MB:
            RAM_end = 32 * 1024 * 1024;
            break;
        case RAM_64_MB:
            RAM_end = 64 * 1024 * 1024;
            break;
        case RAM_128_MB:
            RAM_end = 128 * 1024 * 1024;
            break;
    }
    RAM_end -= 1024 * 1024;  /* reserve memory for PPCBUG */
    work_space_start = 
            (unsigned char *)RAM_end - BSP_Configuration.work_space_size;

    if ( work_space_start <= (unsigned char *)&end ) {
        printk( "bspstart: Not enough RAM!!!\n" );
        bsp_cleanup();
    }

    BSP_Configuration.work_space_start = work_space_start;

    /*
    *  Account for the console's resources
    */

    console_reserve_resources( &BSP_Configuration );

}
