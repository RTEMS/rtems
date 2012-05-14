/*
 *  This file implements simple console IO via JTAG UART.
 *
 *  Based on no_cpu/console.c
 *  COPYRIGHT (c) 1989-1999.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  Altera-specific code is
 *  COPYRIGHT (c) 2005-2006 Kolja Waschk, rtemsdev/ixo.de
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 */

#define NO_BSP_INIT

#include <bsp.h>
#include <rtems/libio.h>

/* #define JTAG_UART_REGS ((altera_avalon_jtag_uart_regs*)NIOS2_IO_BASE(JTAG_UART_BASE)) */

/*  is_character_ready
 *
 *  If a character is available, this routine reads it and stores
 *  it in
 *  reads the character and stores
 *
 *  Input parameters: NONE
 *
 *  Output parameters:  NONE
 *
 *  Return values:
 */

bool is_character_ready(
  char *ch
)
{
    altera_avalon_jtag_uart_regs *ajur = NIOS2_IO_BASE(JTAG_UART_BASE);
    unsigned int data = ajur->data;

    if (data & ALTERA_AVALON_JTAG_UART_DATA_RVALID_MSK)
    {
        *ch = (data & ALTERA_AVALON_JTAG_UART_DATA_DATA_MSK)
              >> ALTERA_AVALON_JTAG_UART_DATA_DATA_OFST;
        return true;
    };

    return false;
}

void console_initialize_hardware(void)
{
}

/*
 *  This routine reads a character from the SOURCE.
 *
 *  Input parameters: NONE
 *
 *  Output parameters:  NONE
 *
 *  Return values:
 *    character read from SOURCE
 */

int console_inbyte_nonblocking(
  int port
)
{
  char ch;
  /*
   *  Wait until a character is available.
   */

  if (is_character_ready(&ch))
    return ch;
  return -1;
}

/*
 *  This routine transmits a character out the SOURCE.  It may support
 *  XON/XOFF flow control.
 *
 *  Input parameters:
 *    ch  - character to be transmitted
 *
 *  Output parameters:  NONE
 */

void console_outbyte_polled(
  int  port,
  char ch
)
{
  altera_avalon_jtag_uart_regs *ajur = NIOS2_IO_BASE(JTAG_UART_BASE);
  /*
   *  Wait for the transmitter to be ready.
   *  Check for flow control requests and process.
   *  Then output the character.
   */

  while ((ajur->control & ALTERA_AVALON_JTAG_UART_CONTROL_WSPACE_MSK) == 0);

  ajur->data = ch;
}

/*
 * To support printk
 */

#include <rtems/bspIo.h>


void ISS_output_char(char c) { console_outbyte_polled( 0, c ); }

BSP_output_char_function_type           BSP_output_char = ISS_output_char;
BSP_polling_getchar_function_type       BSP_poll_char = NULL;

