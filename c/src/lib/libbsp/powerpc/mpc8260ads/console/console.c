/*
 *  console.c
 *
 *  This file contains the MBX8xx termios serial I/O package.
 *  Only asynchronous I/O is supported.
 *
 *  The SCCs and SMCs are assigned as follows
 *
 *   Channel     Device      Minor   Note
 *    SMC1      /dev/tty0      0
 *    SMC2      /dev/tty1      1
 *    SCC1                     2     N/A. Hardwired as ethernet port
 *    SCC2      /dev/tty2      3
 *    SCC3      /dev/tty3      4
 *    SCC4      /dev/tty4      5
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
 *  polled. Polled I/O may be performed by this device driver entirely, or
 *  in part by EPPCBug. With EPPCBug 1.1, polled I/O is limited to the
 *  EPPCBug debug console. This is a limitation of the firmware. Later
 *  firmware may be able to do I/O through any port. This code assumes
 *  that the EPPCBug console is the default: SMC1. If the console and
 *  printk ports are set to anything else with EPPCBug polled I/O, the
 *  system will hang. Only port SMC1 is usable with EPPCBug polled I/O.
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
 *  from the EPPBug debug console or the GDB port.
 *
 *  This driver handles all five available serial ports: it distinguishes
 *  the sub-devices using minor device numbers. It is not possible to have
 *  other protocols running on the other ports when this driver is used as
 *  currently written.
 *
 *  Based on code (alloc860.c in eth_comm port) by
 *  Jay Monkman (jmonkman@frasca.com),
 *  Copyright (C) 1998 by Frasca International, Inc.
 *
 *  Modifications by Darlene Stewart <Darlene.Stewart@iit.nrc.ca>
 *  and Charles-Antoine Gauthier <charles.gauthier@iit.nrc.ca>.
 *  Copyright (c) 2000, National Research Council of Canada
 *
 *  Modifications by Andy Dachs <iwe@fsmal.net> for MPC8260
 *  support.
 *
 * The SCCs and SMCs on the eval board are assigned as follows
 *
 *   Channel     Device      Minor   Termios
 *    SMC1      /dev/tty3      4       no
 *    SMC2      /dev/tty4      5       no
 *    SCC1      /dev/tty0      0       no
 *    SCC2      /dev/console   1       yes
 *    SCC3      /dev/tty1      2       no  	* USED FOR NETWORK I/F
 *    SCC4      /dev/tty2      3       no	* USED FOR NETWORK I/F
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

BSP_output_char_function_type     BSP_output_char = _BSP_output_char;
BSP_polling_getchar_function_type BSP_poll_char = NULL;

/*
 *  do_poll_read
 *
 *  Input characters through polled I/O. Returns has soon as a character has
 *  been received. Otherwise, if we wait for the number of requested characters,
 *  we could be here forever!
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

#define BSP_READ  m8xx_uart_pollRead

  while( (c = BSP_READ(minor)) == -1 );
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
  uint32_t   i;
  char cr ='\r';

#define BSP_WRITE m8xx_uart_pollWrite

  for( i = 0; i < rw_args->count; i++ ) {
    BSP_WRITE(minor, &(rw_args->buffer[i]), 1);
    if ( rw_args->buffer[i] == '\n' )
      BSP_WRITE(minor, &cr, 1);
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
   *  Can't rely on console_initialize having been called before this function
   *  is used, so it may fail unless output is done through EPPC-Bug.
   */
#define PRINTK_WRITE m8xx_uart_pollWrite

  PRINTK_WRITE( PRINTK_MINOR, &c, 1 );
  if( c == '\n' )
    PRINTK_WRITE( PRINTK_MINOR, &cr, 1 );

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
  rtems_device_minor_number console_minor;

  /*
   * Set up TERMIOS if needed
   */

    console_minor = CONSOLE_MINOR;

#if UARTS_USE_TERMIOS == 1

    rtems_termios_initialize ();
#else
    rtems_termios_initialize ();
#endif /* UARTS_USE_TERMIOS */

  /*
   *  Do common initialization.
   */
  m8xx_uart_initialize();

  /*
   * Do device-specific initialization
   */
#if 0
  m8xx_uart_smc_initialize(SMC1_MINOR); /* /dev/tty4 */
  m8xx_uart_smc_initialize(SMC2_MINOR); /* /dev/tty5 */
#endif

  m8xx_uart_scc_initialize(SCC1_MINOR); /* /dev/tty0    */
  m8xx_uart_scc_initialize(SCC2_MINOR); /* /dev/tty1    */

#if 0  /* used as network connections */
  m8xx_uart_scc_initialize(SCC3_MINOR); /* /dev/tty2    */
  m8xx_uart_scc_initialize(SCC4_MINOR); /* /dev/tty3    */
#endif

  /*
   * Set up interrupts
   */
  m8xx_uart_interrupts_initialize();

  status = rtems_io_register_name ("/dev/tty0", major, SCC1_MINOR);
  if (status != RTEMS_SUCCESSFUL)
    rtems_fatal_error_occurred (status);
  chmod("/dev/tty0",0660);
  chown("/dev/tty0",2,0);

  status = rtems_io_register_name ("/dev/tty1", major, SCC2_MINOR);
  if (status != RTEMS_SUCCESSFUL)
    rtems_fatal_error_occurred (status);
  chmod("/dev/tty1",0660);
  chown("/dev/tty1",2,0);

#if 0
  status = rtems_io_register_name ("/dev/tty2", major, SCC3_MINOR);
  if (status != RTEMS_SUCCESSFUL)
    rtems_fatal_error_occurred (status);

  status = rtems_io_register_name ("/dev/tty3", major, SCC4_MINOR);
  if (status != RTEMS_SUCCESSFUL)
    rtems_fatal_error_occurred (status);

  status = rtems_io_register_name ("/dev/tty4", major, SMC1_MINOR);
  if (status != RTEMS_SUCCESSFUL)
    rtems_fatal_error_occurred (status);

  status = rtems_io_register_name ("/dev/tty5", major, SMC2_MINOR);
  if (status != RTEMS_SUCCESSFUL)
    rtems_fatal_error_occurred (status);
#endif
  /* Now register the RTEMS console */
  status = rtems_io_register_name ("/dev/console", major, console_minor);
  if (status != RTEMS_SUCCESSFUL)
    rtems_fatal_error_occurred (status);
  chmod("/dev/console",0666);
  chown("/dev/console",2,0);

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
#if UARTS_IO_MODE == 1  /* RTEMS interrupt-driven I/O with termios */
  /* Used to track termios private data for callbacks */
  extern struct rtems_termios_tty *ttyp[];
  rtems_libio_open_close_args_t *args = arg;

  static const rtems_termios_callbacks intrCallbacks = {
    NULL,                       	/* firstOpen */
    NULL,                       	/* lastClose */
    NULL,                         /* pollRead */
    m8xx_uart_write,       	      /* write */
    m8xx_uart_setAttributes,    	/* setAttributes */
    NULL,                       	/* stopRemoteTx */
    NULL,                       	/* startRemoteTx */
    1                           	/* outputUsesInterrupts */
  };
#else
#if (UARTS_USE_TERMIOS == 1) && (UARTS_IO_MODE != 1)
  static const rtems_termios_callbacks pollCallbacks = {
    NULL,                       	/* firstOpen */
    NULL,                       	/* lastClose */
    m8xx_uart_pollRead,        	  /* pollRead */
    m8xx_uart_pollWrite,       	  /* write */
    m8xx_uart_setAttributes,      /* setAttributes */
    NULL,                       	/* stopRemoteTx */
    NULL,                       	/* startRemoteTx */
    0                           	/* outputUsesInterrupts */
  };
#endif

#endif

  rtems_status_code sc;

  if ( minor > NUM_PORTS-1 )
    return RTEMS_INVALID_NUMBER;

#if UARTS_USE_TERMIOS == 1

#if UARTS_IO_MODE == 1  /* RTEMS interrupt-driven I/O with termios */
  sc = rtems_termios_open( major, minor, arg, &intrCallbacks );
  ttyp[minor] = args->iop->data1;        /* Keep cookie returned by termios_open */
#else                     /* RTEMS polled I/O with termios */
  sc = rtems_termios_open( major, minor, arg, &pollCallbacks );
#endif

#else /* UARTS_USE_TERMIOS != 1 */
  /* no termios -- default to polled I/O */
  sc = RTEMS_SUCCESSFUL;
#endif /* UARTS_USE_TERMIOS != 1 */

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
    /* no termios -- default to polled */
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

/*
 *  Support routine for console-generic
 */

int mbx8xx_console_get_configuration(void)
{
#if UARTS_IO_MODE == 1
  return 0x02;
#else
  return 0;
#endif

}
