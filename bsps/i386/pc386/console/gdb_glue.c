/* SPDX-License-Identifier: BSD-2-Clause */

/*  gdb_glue
 *
 *  Interface to initialize the GDB.
 *
 *  COPYRIGHT (c) 1989-1998.
 *  On-Line Applications Research Corporation (OAR).
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
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
