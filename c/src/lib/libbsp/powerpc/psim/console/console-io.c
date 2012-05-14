/*
 *  This file contains the hardware specific portions of the TTY driver
 *  for the simulated serial port on the PowerPC simulator.
 *
 *  COPYRIGHT (c) 1989-2004.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 */

#include <bsp.h>
#include <rtems/libio.h>
#include <stdlib.h>
#include <assert.h>

/*
 *  console_initialize_hardware
 *
 *  This routine initializes the console hardware.
 *
 */

void console_initialize_hardware(void)
{
}

/* external prototypes for monitor interface routines */

void outbyte( char );
char inbyte( void );

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
  outbyte( ch );
}

/*
 *  console_inbyte_nonblocking
 *
 *  This routine polls for a character.
 */

int console_inbyte_nonblocking(
  int port
)
{
  char c;

  c = inbyte();
  if (!c)
    return -1;
  return c;
}

/*
 *  To support printk
 */

#include <rtems/bspIo.h>

void PSIM_output_char(char c) { console_outbyte_polled( 0, c ); }

BSP_output_char_function_type           BSP_output_char = PSIM_output_char;
BSP_polling_getchar_function_type       BSP_poll_char = NULL;
