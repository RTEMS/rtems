#define I_WANT_TERMIOS 
/*
 *  BSP specific Serial I/O Functions for the eth-comm BSP
 *
 * This file contains the BSP specific functions for 
 * performing serial I/O. These are the functions
 * RTEMS uses (the 6 listed in the device driver 
 * structure)
 *
 * The SCCs and SMCs are assigned as follows
 *
 *   Channel     Device      Minor   Termios
 *    SMC1      /dev/tty0      0       no
 *    SMC2      /dev/tty1      1       no
 *    SCC1      ethernet
 *    SCC2      /dev/console   3       yes
 *    SCC3      /dev/tty3      4       no
 *    SCC4      /dev/tty4      5       no
 *
 * FIXME: This should use termios for /dev/console, but it doesn't
 *  appear to work correctly yet. On startup, with termios enabled,
 *  the board hangs for a few seconds before running correctly
 *
 * Author: Jay Monkman (jmonkman@frasca.com) 
 * Copyright (C) 1998 by Frasca International, Inc.
 *
 *  $Id$
 */

#include <bsp.h>                /* Must be before libio.h */
#include <rtems/libio.h>
#include <termios.h>
#include <bspIo.h>

static void _BSP_null_char( char c ) {return;}
BSP_output_char_function_type BSP_output_char = _BSP_null_char;

rtems_device_driver console_initialize(rtems_device_major_number major,
                                       rtems_device_minor_number minor,
                                       void *arg)
{
  rtems_status_code status;
  
#ifdef I_WANT_TERMIOS
  /*
   * Set up TERMIOS (for /dev/console)
   */
  rtems_termios_initialize();
#endif

  /*
   *  Do common initialization.
   */
  m8xx_uart_initialize();

  /*
   * Do device-specific initialization
   */
  m8xx_uart_smc_initialize(SMC1_MINOR); /* /dev/tty0 */
  m8xx_uart_smc_initialize(SMC2_MINOR); /* /dev/tty1 */                             
  m8xx_uart_scc_initialize(SCC2_MINOR); /* /dev/tty2    */
  m8xx_uart_scc_initialize(SCC3_MINOR); /* /dev/tty3    */
  m8xx_uart_scc_initialize(SCC4_MINOR); /* /dev/tty4    */
  
  /*
   * Register the devices
   */
  status = rtems_io_register_name ("/dev/console", major, SCC2_MINOR);
  if (status != RTEMS_SUCCESSFUL)
    rtems_fatal_error_occurred (status);
  status = rtems_io_register_name ("/dev/tty0", major, SMC1_MINOR);
  if (status != RTEMS_SUCCESSFUL)
    rtems_fatal_error_occurred (status);
  status = rtems_io_register_name ("/dev/tty1", major, SMC2_MINOR);
  if (status != RTEMS_SUCCESSFUL)
    rtems_fatal_error_occurred (status);
  status = rtems_io_register_name ("/dev/tty3", major, SCC3_MINOR);
  if (status != RTEMS_SUCCESSFUL)
    rtems_fatal_error_occurred (status);
  status = rtems_io_register_name ("/dev/tty4", major, SCC4_MINOR);
  if (status != RTEMS_SUCCESSFUL)
    rtems_fatal_error_occurred (status);
  return RTEMS_SUCCESSFUL;
}
 
rtems_device_driver console_open(rtems_device_major_number major,
                                 rtems_device_minor_number minor,
                                 void *arg)
{
  volatile m8xxSCCRegisters_t *sccregs;

#ifdef I_WANT_TERMIOS
  static const rtems_termios_callbacks sccPollCallbacks = {
    NULL,                       /* firstOpen */
    NULL,                       /* lastClose */
    m8xx_uart_pollRead,        /* pollRead */
    m8xx_uart_pollWrite,       /* write */
    m8xx_uart_setAttributes,    /* setAttributes */
    NULL,                       /* stopRemoteTx */
    NULL,                       /* startRemoteTx */
    0                           /* outputUsesInterrupts */
  };
#endif /* I_WANT_TERMIOS */

  sccregs = 0;

  switch (minor) {
  case 0:
    m8xx.smc1.smcm = 1;           /* Enable SMC1 RX interrupts */
    break;
  case 1:
    m8xx.smc2.smcm = 1;           /* Enable SMC2 RX interrupts */
    break;
  case 2:
    sccregs = &m8xx.scc1;
    break;
  case 3: 
    sccregs = &m8xx.scc2;
    break;
  case 4:
    sccregs = &m8xx.scc3;
    break;
  case 5:
    sccregs = &m8xx.scc4;
    break;
  default:
    rtems_panic ("CONSOLE: bad minor number");
  }

  if (sccregs)
    sccregs->sccm=0x3;

#ifdef I_WANT_TERMIOS
  if (minor == SCC2_MINOR) {
    return rtems_termios_open (major, minor, arg, &sccPollCallbacks);
  }
  else { 
    return RTEMS_SUCCESSFUL;
  }
#else
  return RTEMS_SUCCESSFUL;
#endif
}

rtems_device_driver console_close(rtems_device_major_number major,
                                  rtems_device_minor_number minor,
                                  void *arg)
{
#ifdef I_WANT_TERMIOS
  if (minor == SCC2_MINOR) {
    return rtems_termios_close (arg);
  }
  else {
    return RTEMS_SUCCESSFUL;
  }
#else
  return RTEMS_SUCCESSFUL;
#endif
}

rtems_device_driver console_read(rtems_device_major_number major,
                                 rtems_device_minor_number minor,
                                 void *arg)
{
#ifdef I_WANT_TERMIOS
  /*
  if (minor == SCC2_MINOR) {
  */
    return rtems_termios_read(arg);
  /*
  }
  else {
    return m8xx_console_read(major, minor, arg);
  }
  */
#else
  return m8xx_console_read(major, minor, arg);
#endif
}

rtems_device_driver console_write(rtems_device_major_number major,
                                  rtems_device_minor_number minor,
                                  void *arg)
{
#ifdef I_WANT_TERMIOS
  /*
  if (minor == SCC2_MINOR) {
  */
    return rtems_termios_write(arg);
  /*
  }
  else {
    return m8xx_console_write(major, minor, arg);
  }
  */
#else
  return m8xx_console_write(major, minor, arg);
#endif
}

/*
 * Handle ioctl request.
 * Should set hardware line speed, bits/char, etc.
 */
rtems_device_driver console_control(rtems_device_major_number major,
                                    rtems_device_minor_number minor,
                                    void *arg)
{ 
#ifdef I_WANT_TERMIOS
  if (minor == SCC2_MINOR) {
    return rtems_termios_ioctl (arg);
  }
  else {
    return RTEMS_SUCCESSFUL;
  }
#else
  return RTEMS_SUCCESSFUL;
#endif
}

/*
 *  Support routine for console-generic
 */

int mbx8xx_console_get_configuration(void)
{
#if NVRAM_CONFIGURE == 1
  return nvram->console_mode;
#else
#if UARTS_IO_MODE == 1
  return 0x02;
#else
  return 0;
#endif
#endif

}

