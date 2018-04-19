/*
 * Copyright (c) 2015 University of York.
 * Hesham ALMatary <hmka501@york.ac.uk>
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
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */

#include <bsp.h>
#include <bsp/console-polled.h>
#include <rtems/libio.h>
#include <stdlib.h>
#include <assert.h>
#include <stdio.h>

static void outbyte_console( char );
static char inbyte_console( void );

void console_initialize_hardware(void)
{
  /* Do nothing */
}

/* Epiphany simulator would handle this system call */
static void outbyte_console(char c)
{
  register int chan asm("r0") = STDOUT_FILENO;
  register void* addr asm("r1") = &c;
  register int len asm("r2") = 1;

  /* Invoke write system call to be handled by Epiphany simulator */
  __asm__ __volatile__ ("trap 0" : : "r" (chan), "r" (addr), "r" (len));
}

static char inbyte_console(void)
{
  char c;
  register int chan asm("r0") = STDIN_FILENO;
  register void* addr asm("r1") = &c;
  register int len asm("r2") = 1;

  /* Invoke read system call to be handled by Epiphany simulator */
  asm ("trap 1" :: "r" (chan), "r" (addr), "r" (len));
  return c;
}

/*
 *  console_outbyte_polled
 *
 *  This routine transmits a character using polling.
 */
void console_outbyte_polled(
  int  port,
  char ch
)
{
  outbyte_console( ch );
}

/*
 *  console_inbyte_nonblocking
 *
 *  This routine polls for a character.
 */

int console_inbyte_nonblocking(int port)
{
  char c;

  c = inbyte_console();
  if (!c)
    return -1;
  return (int) c;
}

/*
 *  To support printk
 */

#include <rtems/bspIo.h>

static void Epiphany_output_char(char c) { console_outbyte_polled( 0, c ); }

BSP_output_char_function_type BSP_output_char = Epiphany_output_char;
BSP_polling_getchar_function_type BSP_poll_char =
  (void *)console_inbyte_nonblocking;
