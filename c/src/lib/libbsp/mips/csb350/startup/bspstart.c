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

au1x00_uart_t *uart0 = (au1x00_uart_t *)AU1X00_UART0_ADDR;
au1x00_uart_t *uart3 = (au1x00_uart_t *)AU1X00_UART3_ADDR;

/*
 *  bsp_start
 *
 *  This routine does the bulk of the system initialization.
 */
void bsp_start( void )
{
  extern void mips_install_isr_entries(void);
  unsigned int compare = 0; 

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
