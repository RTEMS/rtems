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
    unsigned long ramSpace;

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
  extern int _end;
  extern int WorkspaceBase;
  /* Configure Number of Register Caches */

  Cpu_table.pretasking_hook = bsp_pretasking_hook;  /* init libc, etc. */
  Cpu_table.postdriver_hook = bsp_postdriver_hook;
  Cpu_table.interrupt_stack_size = 4096;

  /* HACK -- tied to value linkcmds */
  if ( BSP_Configuration.work_space_size >(4096*1024) )
   _sys_exit( 1 );

  BSP_Configuration.work_space_start = (void *) &WorkspaceBase;

  MONGOOSEV_WRITE( MONGOOSEV_PERIPHERAL_FUNCTION_INTERRUPT_CAUSE_REGISTER, 0 );
  mips_set_sr( 0xff00 );  /* all interrupts unmasked but globally off */
                          /* depend on the IRC to take care of things */
  mips_install_isr_entries();
}

/* XXX */
void clear_cache( void *address, size_t n )
{
}

/* Structure filled in by get_mem_info.  Only the size field is
   actually used (to clear bss), so the others aren't even filled in.  */

struct s_mem
{
  unsigned int size;
  unsigned int icsize;
  unsigned int dcsize;
};


void
get_mem_info (mem)
     struct s_mem *mem;
{
  mem->size = 0x1000000;        /* XXX figure out something here */
}

