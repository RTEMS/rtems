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
 */
#include <rtems/libio.h>
#include <mpc860.h>
#include <mpc860/console.h>
#include <termios.h>

rtems_device_driver console_initialize(rtems_device_major_number major,
                                       rtems_device_minor_number minor,
                                       void *arg)
{
  rtems_status_code status;
  rtems_isr_entry old_handler;
  rtems_status_code sc;
  
#ifdef I_WANT_TERMIOS
  /*
   * Set up TERMIOS (for /dev/console)
   */
  rtems_termios_initialize();
#endif

  /*
   * Set up Buffer Descriptors
   */
  m860_console_initialize();

  /*
   * Do device-specific initialization
   */
  m860_scc_initialize(2);  /* /dev/console */
  m860_scc_initialize(3);  /* /dev/tty3   */
  m860_scc_initialize(4);  /* /dev/tty4   */
  m860_smc_initialize(1);  /* /dev/tty0   */
  m860_smc_initialize(2);  /* /dev/tty1   */
  
  sc = rtems_interrupt_catch (m860_scc2_console_interrupt_handler,
                              PPC_IRQ_CPM_SCC2,
                              &old_handler);
  sc = rtems_interrupt_catch (m860_scc3_console_interrupt_handler,
                              PPC_IRQ_CPM_SCC3,
                              &old_handler);
  sc = rtems_interrupt_catch (m860_scc4_console_interrupt_handler,
                              PPC_IRQ_CPM_SCC4,
                              &old_handler);
  sc = rtems_interrupt_catch (m860_smc1_console_interrupt_handler,
                              PPC_IRQ_CPM_SMC1,
                              &old_handler);
  sc = rtems_interrupt_catch (m860_smc2_console_interrupt_handler,
                              PPC_IRQ_CPM_SMC2,
                              &old_handler);

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
  volatile m860SCCRegisters_t *sccregs;

#ifdef I_WANT_TERMIOS
  static const rtems_termios_callbacks sccPollCallbacks = {
    NULL,                       /* firstOpen */
    NULL,                       /* lastClose */
    m860_char_poll_read,        /* pollRead */
    m860_char_poll_write,       /* write */
    m860_scc_set_attributes,    /* setAttributes */
    NULL,                       /* stopRemoteTx */
    NULL,                       /* startRemoteTx */
    0                           /* outputUsesInterrupts */
  };
#endif /* I_WANT_TERMIOS */

  sccregs = 0;

  switch (minor) {
  case 0:
    m860.smc1.smcm = 1;           /* Enable SMC1 RX interrupts */
    m860.cimr |= 1UL <<  4;       /* Enable SMC1 interrupts */
    break;
  case 1:
    m860.smc2.smcm = 1;           /* Enable SMC2 RX interrupts */
    m860.cimr |= 1UL <<  3;       /* Enable SMC2 interrupts */
    break;
  case 2:
    m860.cimr |= 1UL << 30;      /* Enable SCC1 interrupts */
    sccregs = &m860.scc1;
    break;
  case 3: 
#ifndef I_WANT_TERMIOS
    m860.cimr |= 1UL << 29;      /* Enable SCC2 interrupts */
#endif /* I_WANT_TERMIOS */
    sccregs = &m860.scc2;
    break;
  case 4:
    m860.cimr |= 1UL << 28;      /* Enable SCC3 interrupts */
    sccregs = &m860.scc3;
    break;
  case 5:
    m860.cimr |= 1UL << 27;      /* Enable SCC4 interrupts */
    sccregs = &m860.scc4;
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
  if (minor == SCC2_MINOR) {
    return rtems_termios_read(arg);
  }
  else {
    return m860_console_read(major, minor, arg);
  }
#else
  return m860_console_read(major, minor, arg);
#endif
}

rtems_device_driver console_write(rtems_device_major_number major,
                                  rtems_device_minor_number minor,
                                  void *arg)
{
#ifdef I_WANT_TERMIOS
  if (minor == SCC2_MINOR) {
    return rtems_termios_write(arg);
  }
  else {
    return m860_console_write(major, minor, arg);
  }
#else
  return m860_console_write(major, minor, arg);
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

