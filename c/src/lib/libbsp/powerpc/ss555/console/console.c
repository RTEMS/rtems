/*
 *  console.c
 *
 *  This file contains the Intec SS555 termios serial I/O package.
 *
 *  The SCI channels are assigned as follows
 *
 *   Channel     Device      Minor
 *    SCI1      /dev/tty0      0
 *    SCI2      /dev/tty1      1
 *
 *  All ports support termios. The use of termios is recommended for real-time
 *  applications. Termios provides buffering and input processing. When not
 *  using termios, processing is limited to the substitution of LF for CR on
 *  input, and the output of a CR following the output of a LF character.
 *  Note that the terminal should not send CR/LF pairs when the return key
 *  is pressed, and that output lines are terminated with LF/CR, not CR/LF
 *  (although that would be easy to change).
 *
 *  I/O may be interrupt-driven (recommended for real-time applications) or
 *  polled.
 *
 *  LIMITATIONS:
 *
 *  It is not possible to use different I/O modes on the different ports. The
 *  exception is with printk. The printk port can use a different mode from
 *  the other ports. If this is done, it is important not to open the printk
 *  port from an RTEMS application.
 *
 *  Currently, the I/O modes are determined at build time. It would be much
 *  better to have the mode selected at boot time based on parameters in
 *  NVRAM.
 *
 *  Interrupt-driven I/O requires termios.
 *
 *  TESTS:
 *
 *  TO RUN THE TESTS, USE POLLED I/O WITHOUT TERMIOS SUPPORT. Some tests
 *  play with the interrupt masks and turn off I/O. Those tests will hang
 *  when interrupt-driven I/O is used. Other tests, such as cdtest, do I/O
 *  from the static constructors before the console is open. This test
 *  will not work with interrupt-driven I/O. Because of the buffering
 *  performed in termios, test output may not be in sequence.The tests
 *  should all be fixed to work with interrupt-driven I/O and to
 *  produce output in the expected sequence. Obviously, the termios test
 *  requires termios support in the driver.
 *
 *  Set CONSOLE_MINOR to the appropriate device minor number in the
 *  config file. This allows the RTEMS application console to be different
 *  from the GDB port.
 *
 *  This driver handles both available serial ports: it distinguishes
 *  the sub-devices using minor device numbers. It is not possible to have
 *  other protocols running on the other ports when this driver is used as
 *  currently written.
 *
 *
 *  SS555 port sponsored by Defence Research and Development Canada - Suffield
 *  Copyright (C) 2004, Real-Time Systems Inc. (querbach@realtime.bc.ca)
 *
 *  Derived from c/src/lib/libbsp/powerpc/mbx8xx/console/console.c:
 *
 *  Based on code (alloc860.c in eth_comm port) by
 *  Jay Monkman (jmonkman@frasca.com),
 *  Copyright (C) 1998 by Frasca International, Inc.
 *
 *  Modifications by Darlene Stewart <Darlene.Stewart@iit.nrc.ca>
 *  and Charles-Antoine Gauthier <charles.gauthier@iit.nrc.ca>.
 *  Copyright (c) 2000, National Research Council of Canada
 *
 */
#include <stdarg.h>
#include <stdio.h>
#include <bsp.h>                /* Must be before libio.h */
#include <rtems/bspIo.h>
#include <rtems/libio.h>
#include <termios.h>

static void _BSP_output_char( char c );
static rtems_status_code do_poll_read( rtems_device_major_number major, rtems_device_minor_number minor, void * arg);
static rtems_status_code do_poll_write( rtems_device_major_number major, rtems_device_minor_number minor, void * arg);

static void _BSP_null_char( char c ) {return;}

BSP_output_char_function_type     BSP_output_char = _BSP_null_char;
BSP_polling_getchar_function_type BSP_poll_char = NULL;

/*
 *  do_poll_read
 *
 *  Input characters through polled I/O. Returns as soon as a character has
 *  been received. Otherwise, if we wait for the number of requested
 *  characters, we could be here forever!
 *
 *  CR is converted to LF on input. The terminal should not send a CR/LF pair
 *  when the return or enter key is pressed.
 *
 *  Input parameters:
 *    major - ignored. Should be the major number for this driver.
 *    minor - selected channel.
 *    arg->buffer - where to put the received characters.
 *    arg->count  - number of characters to receive before returning--Ignored.
 *
 *  Output parameters:
 *    arg->bytes_moved - the number of characters read. Always 1.
 *
 *  Return value: RTEMS_SUCCESSFUL
 *
 *  CANNOT BE COMBINED WITH INTERRUPT DRIVEN I/O!
 */
static rtems_status_code do_poll_read(
  rtems_device_major_number major,
  rtems_device_minor_number minor,
  void                    * arg
)
{
  rtems_libio_rw_args_t *rw_args = arg;
  int c;

  while( (c = m5xx_uart_pollRead(minor)) == -1 );
  rw_args->buffer[0] = (uint8_t)c;
  if( rw_args->buffer[0] == '\r' )
      rw_args->buffer[0] = '\n';
  rw_args->bytes_moved = 1;

  return RTEMS_SUCCESSFUL;
}

/*
 *  do_poll_write
 *
 *  Output characters through polled I/O. Returns only once every character has
 *  been sent.
 *
 *  CR is transmitted AFTER a LF on output.
 *
 *  Input parameters:
 *    major - ignored. Should be the major number for this driver.
 *    minor - selected channel
 *    arg->buffer - where to get the characters to transmit.
 *    arg->count  - the number of characters to transmit before returning.
 *
 *  Output parameters:
 *    arg->bytes_moved - the number of characters read
 *
 *  Return value: RTEMS_SUCCESSFUL
 *
 *  CANNOT BE COMBINED WITH INTERRUPT DRIVEN I/O!
 */
static rtems_status_code do_poll_write(
  rtems_device_major_number major,
  rtems_device_minor_number minor,
  void                    * arg
)
{
  rtems_libio_rw_args_t *rw_args = arg;
  uint32_t i;
  char cr ='\r';

  for( i = 0; i < rw_args->count; i++ ) {
    m5xx_uart_pollWrite(minor, &(rw_args->buffer[i]), 1);
    if ( rw_args->buffer[i] == '\n' )
      m5xx_uart_pollWrite(minor, &cr, 1);
  }
  rw_args->bytes_moved = i;

  return RTEMS_SUCCESSFUL;
}

/*
 *  Print functions prototyped in bspIo.h
 */

static void _BSP_output_char( char c )
{
  char cr = '\r';

  /*
   *  Can't rely on console_initialize having been called before this
   *  function is used, so it may fail.
   */

  m5xx_uart_pollWrite( PRINTK_MINOR, &c, 1 );
  if( c == '\n' )
    m5xx_uart_pollWrite( PRINTK_MINOR, &cr, 1 );
}

/*
 ***************
 * BOILERPLATE *
 ***************
 *
 *  All these functions are prototyped in rtems/c/src/lib/include/console.h.
 */

/*
 * Initialize and register the device
 */
rtems_device_driver console_initialize(
  rtems_device_major_number major,
  rtems_device_minor_number minor,
  void *arg
)
{
  rtems_status_code status;

  /*
   * Set up TERMIOS if needed
   */
  #if UARTS_USE_TERMIOS == 1
    rtems_termios_initialize ();
  #endif /* UARTS_USE_TERMIOS */

  /*
   * Do device-specific initialization
   */
  BSP_output_char = _BSP_output_char;

  m5xx_uart_initialize(SCI1_MINOR);
  status = rtems_io_register_name ("/dev/tty0", major, SCI1_MINOR);
  if (status != RTEMS_SUCCESSFUL)
    rtems_fatal_error_occurred (status);

  m5xx_uart_initialize(SCI2_MINOR);
  status = rtems_io_register_name ("/dev/tty1", major, SCI2_MINOR);
  if (status != RTEMS_SUCCESSFUL)
    rtems_fatal_error_occurred (status);

  /* Now register the RTEMS console */
  status = rtems_io_register_name ("/dev/console", major, CONSOLE_MINOR);
  if (status != RTEMS_SUCCESSFUL)
    rtems_fatal_error_occurred (status);

  return RTEMS_SUCCESSFUL;
}

/*
 * Open the device
 */
rtems_device_driver console_open(
  rtems_device_major_number major,
  rtems_device_minor_number minor,
  void *arg
)
{
  rtems_status_code sc;

  if ( minor > NUM_PORTS - 1 )
    return RTEMS_INVALID_NUMBER;

  #if (UARTS_USE_TERMIOS == 1)
  {
    #if (UARTS_IO_MODE == 1)    /* RTEMS interrupt-driven I/O with termios */

      static const rtems_termios_callbacks callbacks = {
        m5xx_uart_firstOpen,           	/* firstOpen */
        m5xx_uart_lastClose,           	/* lastClose */
        NULL,                           /* pollRead */
        m5xx_uart_write,       	        /* write */
        m5xx_uart_setAttributes,    	/* setAttributes */
        NULL,                       	/* stopRemoteTx */
        NULL,                       	/* startRemoteTx */
        TERMIOS_IRQ_DRIVEN           	/* outputUsesInterrupts */
      };
      sc = rtems_termios_open( major, minor, arg, &callbacks );

    #else /* UARTS_IO_MODE != 1	*/	/* RTEMS polled I/O with termios */

      static const rtems_termios_callbacks callbacks = {
        m5xx_uart_firstOpen,           	/* firstOpen */
        m5xx_uart_lastClose,           	/* lastClose */
        m5xx_uart_pollRead,             /* pollRead */
        m5xx_uart_pollWrite,            /* write */
        m5xx_uart_setAttributes,        /* setAttributes */
        NULL,                           /* stopRemoteTx */
        NULL,                       	/* startRemoteTx */
        TERMIOS_POLLED                 	/* outputUsesInterrupts */
      };
      sc = rtems_termios_open( major, minor, arg, &callbacks );

    #endif

    return sc;
  }

  #else		/* no termios -- default to polled I/O */
  {
    sc = RTEMS_SUCCESSFUL;
  }
  #endif

  return sc;
}

/*
 * Close the device
 */
rtems_device_driver console_close(
  rtems_device_major_number major,
  rtems_device_minor_number minor,
  void *arg
)
{
  if ( minor > NUM_PORTS-1 )
    return RTEMS_INVALID_NUMBER;

  #if UARTS_USE_TERMIOS == 1
    return rtems_termios_close( arg );
  #else
    return RTEMS_SUCCESSFUL;
  #endif
}

/*
 * Read from the device
 */
rtems_device_driver console_read(
  rtems_device_major_number major,
  rtems_device_minor_number minor,
  void *arg
)
{
  if ( minor > NUM_PORTS-1 )
    return RTEMS_INVALID_NUMBER;

  #if UARTS_USE_TERMIOS == 1
    return rtems_termios_read( arg );
  #else
    return do_poll_read( major, minor, arg );
  #endif
}

/*
 * Write to the device
 */
rtems_device_driver console_write(
  rtems_device_major_number major,
  rtems_device_minor_number minor,
  void *arg
)
{
  if ( minor > NUM_PORTS-1 )
    return RTEMS_INVALID_NUMBER;

  #if UARTS_USE_TERMIOS == 1
    return rtems_termios_write( arg );
  #else
    return do_poll_write( major, minor, arg );
  #endif
}

/*
 * Handle ioctl request.
 */
rtems_device_driver console_control(
  rtems_device_major_number major,
  rtems_device_minor_number minor,
  void *arg
)
{
  if ( minor > NUM_PORTS-1 )
    return RTEMS_INVALID_NUMBER;

  #if UARTS_USE_TERMIOS == 1
    return rtems_termios_ioctl( arg );
  #else
    return RTEMS_SUCCESSFUL;
  #endif
}
