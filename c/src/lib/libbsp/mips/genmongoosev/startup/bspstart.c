/*
 *  This routine starts the application.  It includes application,
 *  board, and monitor specific initialization and configuration.
 *  The generic CPU dependent initialization has been performed
 *  before this routine is invoked.
 *
 *  COPYRIGHT (c) 1989-2001.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.OARcorp.com/rtems/license.html.
 *
 *  $Id$
 *
 * Modification History:
 *        12/10/01  A.Ferrer, NASA/GSFC, Code 582
 *           Set interrupt mask to 0xAF00 (Line 139).
 */

#include <string.h>

#include <bsp.h>
#include <rtems/libio.h>
#include <rtems/libcsupport.h>
#include <libcpu/mongoose-v.h>




/*
 *  The original table from the application and our copy of it with
 *  some changes.
 */

extern rtems_configuration_table Configuration;

rtems_configuration_table  BSP_Configuration;

rtems_cpu_table Cpu_table;

char *rtems_progname;

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
 *  NOTES:
 *      Must not use libc (to do io) from here, since drivers are
 *      not yet initialized.
 *
 */

void bsp_pretasking_hook(void)
{
    extern int HeapBase;
    extern int HeapSize;
    void         *heapStart = &HeapBase;
    unsigned long heapSize = (unsigned long)&HeapSize;

    bsp_libc_init(heapStart, (unsigned32) heapSize, 0);

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
   extern void _sys_exit(int);
   extern int WorkspaceBase;
   extern void mips_install_isr_entries();
   extern void mips_gdb_stub_install(void);
   
   /* Configure Number of Register Caches */

   Cpu_table.pretasking_hook = bsp_pretasking_hook;  /* init libc, etc. */
   Cpu_table.postdriver_hook = bsp_postdriver_hook;
   Cpu_table.interrupt_stack_size = 4096;

   /* HACK -- tied to value linkcmds */
   if ( BSP_Configuration.work_space_size > (4096*1024) )
      _sys_exit( 1 );

   BSP_Configuration.work_space_start = (void *) &WorkspaceBase;

   /* mask off any interrupts */
   MONGOOSEV_WRITE( MONGOOSEV_PERIPHERAL_FUNCTION_INTERRUPT_MASK_REGISTER, 0 );

   /* reset the config register & clear any pending peripheral interrupts */
   MONGOOSEV_WRITE( MONGOOSEV_PERIPHERAL_COMMAND_REGISTER, 0 );
   MONGOOSEV_WRITE( MONGOOSEV_PERIPHERAL_COMMAND_REGISTER, MONGOOSEV_UART_CMD_RESET_BOTH_PORTS );
   MONGOOSEV_WRITE( MONGOOSEV_PERIPHERAL_COMMAND_REGISTER, 0 );

   /* reset both timers */
   MONGOOSEV_WRITE_REGISTER( MONGOOSEV_TIMER1_BASE, MONGOOSEV_TIMER_INITIAL_COUNTER_REGISTER, 0xffffffff );
   MONGOOSEV_WRITE_REGISTER( MONGOOSEV_TIMER1_BASE, MONGOOSEV_TIMER_CONTROL_REGISTER, 0);

   MONGOOSEV_WRITE_REGISTER( MONGOOSEV_TIMER2_BASE, MONGOOSEV_TIMER_INITIAL_COUNTER_REGISTER, 0xffffffff );
   MONGOOSEV_WRITE_REGISTER( MONGOOSEV_TIMER2_BASE, MONGOOSEV_TIMER_CONTROL_REGISTER, 0);

   /* clear any pending interrupts */
   MONGOOSEV_WRITE( MONGOOSEV_PERIPHERAL_STATUS_REGISTER, 0xffffffff );

   /* clear any writable bits in the cause register */
   mips_set_cause( 0 );

   /* set interrupt mask, but globally off. */

   /*
   **  Bit 15 | Bit 14 | Bit 13 | Bit 12 | Bit 11 | Bit 10 | Bit  9 | Bit  8 |
   **  periph | unused |  FPU   | unused | timer2 | timer1 | swint1 | swint2 |
   **  extern |        |        |        |        |        |        |        |
   **
   **    1        0        1        0        0        1        0        0
   **
   **    0x8C00   Enable only internal Mongoose V timers.
   **    0xA400   Enable Peripherial ints, FPU and timer1
   **    0x0400   Timer1 only
   */

   /* mips_set_sr( (SR_CU0 | SR_CU1 | 0xA400) ); */

   /* to start up, only enable coprocessor 0 & timer int. per-task
   ** processor settings will be applied as they are created, this 
   ** is just to configure the processor for startup
   */
   mips_set_sr( (SR_CU0 | 0x400) );

   mips_install_isr_entries();
}




void clear_cache( void )
{
   extern void promCopyIcacheFlush(void);       /* from start.S */
   extern void promCopyDcacheFlush(void);

   promCopyIcacheFlush();
   promCopyDcacheFlush();
}




/*
 
//Structure filled in by get_mem_info.


struct s_mem
{
  unsigned int size;
  unsigned int icsize;
  unsigned int dcsize;
};


extern unsigned32 _RamSize;

void get_mem_info ( struct s_mem *mem )
{
   mem->size = (unsigned32)&_RamSize;
   mem->icsize = MONGOOSEV_IC_SIZE;
   mem->dcsize = MONGOOSEV_DC_SIZE;
}

*/

