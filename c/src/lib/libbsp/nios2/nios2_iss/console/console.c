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
 *
 *  $Id$
 */

#define NO_BSP_INIT

#include <bsp.h>
#include <rtems/libio.h>

/* #define JTAG_UART_REGS ((altera_avalon_jtag_uart_regs*)NIOS2_IO_BASE(JTAG_UART_BASE)) */

#if 0
#define XOFFchar              0x13
#define XONchar               0x11
#endif

/*  console_initialize
 *
 *  This routine initializes the console IO driver.
 *
 *  Input parameters: NONE
 *
 *  Output parameters:  NONE
 *
 *  Return values:
 */

rtems_device_driver console_initialize(
  rtems_device_major_number  major,
  rtems_device_minor_number  minor,
  void                      *arg
)
{
  rtems_status_code status;

  status = rtems_io_register_name(
    "/dev/console",
    major,
    (rtems_device_minor_number) 0
  );

  if (status != RTEMS_SUCCESSFUL)
    rtems_fatal_error_occurred(status);

  return RTEMS_SUCCESSFUL;
}

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

rtems_boolean is_character_ready(
  char *ch
)
{
    altera_avalon_jtag_uart_regs *ajur = NIOS2_IO_BASE(JTAG_UART_BASE);
    unsigned int data = ajur->data;

    if (data & ALTERA_AVALON_JTAG_UART_DATA_RVALID_MSK)
    {
        *ch = (data & ALTERA_AVALON_JTAG_UART_DATA_DATA_MSK)
              >> ALTERA_AVALON_JTAG_UART_DATA_DATA_OFST;
        return TRUE;
    };

    return FALSE;
}

/*  inbyte
 *
 *  This routine reads a character from the SOURCE.
 *
 *  Input parameters: NONE
 *
 *  Output parameters:  NONE
 *
 *  Return values:
 *    character read from SOURCE
 */

char inbyte( void )
{
    /*
     *  Wait until a character is available.
     */
    char ch;
    while(!is_character_ready(&ch));
    return ch;
}

/*  outbyte
 *
 *  This routine transmits a character out the SOURCE.  It may support
 *  XON/XOFF flow control.
 *
 *  Input parameters:
 *    ch  - character to be transmitted
 *
 *  Output parameters:  NONE
 */

void outbyte(
  char ch
)
{
  altera_avalon_jtag_uart_regs *ajur = NIOS2_IO_BASE(JTAG_UART_BASE);
  /*
   *  Carriage Return/New line translation.
   */

  if ( ch == '\n' )
    outbyte( '\r' );

  /*
   *  Wait for the transmitter to be ready.
   *  Check for flow control requests and process.
   *  Then output the character.
   */

  while ((ajur->control & ALTERA_AVALON_JTAG_UART_CONTROL_WSPACE_MSK) == 0);

  ajur->data = ch;
}

/*
 *  Open entry point
 */

rtems_device_driver console_open(
  rtems_device_major_number major,
  rtems_device_minor_number minor,
  void                    * arg
)
{
  return RTEMS_SUCCESSFUL;
}

/*
 *  Close entry point
 */

rtems_device_driver console_close(
  rtems_device_major_number major,
  rtems_device_minor_number minor,
  void                    * arg
)
{
  return RTEMS_SUCCESSFUL;
}

/*
 * read bytes from the serial port. We only have stdin.
 */

rtems_device_driver console_read(
  rtems_device_major_number major,
  rtems_device_minor_number minor,
  void                    * arg
)
{
  rtems_libio_rw_args_t *rw_args;
  char *buffer;
  int maximum;
  int count = 0;

  rw_args = (rtems_libio_rw_args_t *) arg;

  buffer = rw_args->buffer;
  maximum = rw_args->count;

  for (count = 0; count < maximum; count++) {
    buffer[ count ] = inbyte();
    if (buffer[ count ] == '\n' || buffer[ count ] == '\r') {
      buffer[ count++ ]  = '\n';
      break;
    }
  }

  rw_args->bytes_moved = count;
  return (count >= 0) ? RTEMS_SUCCESSFUL : RTEMS_UNSATISFIED;
}

/*
 * write bytes to the serial port. Stdout and stderr are the same.
 */

rtems_device_driver console_write(
  rtems_device_major_number major,
  rtems_device_minor_number minor,
  void                    * arg
)
{
  int count;
  int maximum;
  rtems_libio_rw_args_t *rw_args;
  char *buffer;

  rw_args = (rtems_libio_rw_args_t *) arg;

  buffer = rw_args->buffer;
  maximum = rw_args->count;

  for (count = 0; count < maximum; count++) {
    if ( buffer[ count ] == '\n') {
      outbyte('\r');
    }
    outbyte( buffer[ count ] );
  }

  rw_args->bytes_moved = maximum;
  return 0;
}

/*
 *  IO Control entry point
 */

rtems_device_driver console_control(
  rtems_device_major_number major,
  rtems_device_minor_number minor,
  void                    * arg
)
{
  return RTEMS_SUCCESSFUL;
}
