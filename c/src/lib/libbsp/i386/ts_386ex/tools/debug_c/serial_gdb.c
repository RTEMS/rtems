/*  serial_gdb
 *
 *  Interface to initialize the GDB.
 *
 *  COPYRIGHT (c) 1989-1999.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.OARcorp.com/rtems/license.html.
 *
 *  $Id$
 */

#include <uart.h>
#include <rtems/bspIo.h>

extern int BSPConsolePort;

void i386_stub_glue_init(int);
void i386_stub_glue_init_breakin(void);
void set_debug_traps(void);


 /* Init GDB glue  */

void init_serial_gdb( void )
{
  if(BSPConsolePort != BSP_UART_COM2)
    {
      /*
       * If com2 is not used as console use it for
       * debugging
       */

      i386_stub_glue_init(BSP_UART_COM2);
      printk( "Remote GDB using COM2...\n" );

    }
  else
    {
      /* Otherwise use com1 */
      i386_stub_glue_init(BSP_UART_COM1);
      printk( "Remote GDB using COM1...\n" );
    }

  printk( "Remote GDB: setting traps...\n" );
  /* Init GDB stub itself */
  set_debug_traps();


  printk( "Remote GDB: waiting for remote connection...\n" );

  /*
   * Init GDB break in capability,
   * has to be called after
   * set_debug_traps
   */
  i386_stub_glue_init_breakin();

}
