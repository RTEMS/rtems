/*
 *  This routine starts the application.  It includes application,
 *  board, and monitor specific initialization and configuration.
 *  The generic CPU dependent initialization has been performed
 *  before this routine is invoked.
 *
 *  COPYRIGHT (c) 1989-2000.
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
#include <libcpu/au1x00.h>
#include <rtems/libio.h>
#include <rtems/libcsupport.h>

/*
 *  The original table from the application and our copy of it with
 *  some changes.
 */

extern void            *_sdram_size;
extern void            *_sdram_base;
extern void            *_bss_free_start;

unsigned long           free_mem_start;
unsigned long           free_mem_end;

au1x00_uart_t *uart0 = (au1x00_uart_t *)AU1X00_UART0_ADDR;
au1x00_uart_t *uart3 = (au1x00_uart_t *)AU1X00_UART3_ADDR;

/*
 *  Use the shared implementations of the following routines
 */
 
void bsp_libc_init( void *, uint32_t, int );

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
    uint32_t heap_start;
    uint32_t heap_size;

    /* 
     * Set up the heap. 
     */
    heap_start =  free_mem_start;
    heap_size = free_mem_end - free_mem_start;

	/* call rtems lib init - malloc stuff */
    bsp_libc_init((void *)heap_start, heap_size, 0);

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
  extern void mips_install_isr_entries(void);
  unsigned int compare = 0; 

  /* Place RTEMS workspace at beginning of free memory. */
  Configuration.work_space_start = (void *)&_bss_free_start;
  
  free_mem_start = ((uint32_t)&_bss_free_start + 
                    rtems_configuration_get_work_space_size());
  
  free_mem_end = ((uint32_t)&_sdram_base + (uint32_t)&_sdram_size);
  
  mips_set_sr( 0x7f00 );  /* all interrupts unmasked but globally off */
                          /* depend on the IRC to take care of things */
  asm volatile ("mtc0 %0, $11\n" :: "r" (compare));
  mips_install_isr_entries();
}


/* These replace the ones in newlib. I'm not sure why the newlib ones
 * don't work.
 */
void _init(void)
{
}

void _fini(void)
{
}
