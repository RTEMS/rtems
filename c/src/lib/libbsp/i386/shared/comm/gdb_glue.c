/*  gdb_glue
 *
 *  Interface to initialize the GDB.
 *
 *  COPYRIGHT (c) 1989-1998.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 */

#include <bsp.h>
#include <stdio.h>
#include <uart.h>

#define BREAKPOINT() __asm__ ("   int $3");

extern int BSPConsolePort;
void i386_stub_glue_init(int);
void i386_stub_glue_init_breakin(void);
void set_debug_traps(void);

 /* Init GDB glue  */
void init_remote_gdb( void )
{
  if(BSPConsolePort != BSP_UART_COM2)
    {
      /*
       * If com2 is not used as console use it for
       * debugging
       */

      i386_stub_glue_init(BSP_UART_COM2);
      printf( "Remote GDB using COM2...\n" );

    }
  else
    {
      /* Otherwise use com1 */
      i386_stub_glue_init(BSP_UART_COM1);
      printf( "Remote GDB using COM1...\n" );
    }

  printf( "Remote GDB: setting traps...\n" );
  /* Init GDB stub itself */
  set_debug_traps();

  printf( "Remote GDB: waiting remote connection....\n" );

  /*
   * Init GDB break in capability,
   * has to be called after
   * set_debug_traps
   */
  i386_stub_glue_init_breakin();

  /* Put breakpoint in */
  /* breakpoint();     */
  /* BREAKPOINT();     */
}
