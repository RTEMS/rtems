/*  Console Driver for eZKit533
 * 
 *  Copyright (c) 2006 by Atos Automacao Industrial Ltda.
 *             written by Alain Schaefer <alain.schaefer@easc.ch>
 *                    and Antonio Giovanini <antonio@atos.com.br>
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 *
 *  $Id$
 */
 

#include <bsp.h>
#include <rtems/libio.h>
#include <stdlib.h>
#include <assert.h>


/*
 *  console_outbyte_polled
 *
 *  This routine transmits a character using polling.
 */

void console_outbyte_polled(int  port, char ch);

/* body is in console-io.c */

/*
 *  console_inbyte_nonblocking
 *
 *  This routine polls for a character.
 */

int console_inbyte_nonblocking( int port );
/* body is in console-io.c */

/*
 * 
 * This routine initialize uart hardware
 */

void console_initialize_hardware(void);

/* body is in console-io.c */

/*
 *  Interrupt driven console IO
 */

#if (CONSOLE_USE_INTERRUPTS)

/*
 *  Buffers between task and ISRs
 */

#include <rtems/ringbuf.h>

void *console_termios_data[ 1 ];

/*
 *  console_initialize_interrupts
 *
 *  This routine initializes the console's receive and transmit
 *  ring buffers and loads the appropriate vectors to handle the interrupts.
 *
 *  Input parameters:  NONE
 *
 *  Output parameters: NONE
 *
 *  Return values:     NONE
 */
void console_initialize_interrupts( void );
/* body is in console-io.c */

/*
 *  console_outbyte_interrupt
 *
 *  This routine transmits a character out.
 *
 *  Input parameters:
 *    port - port to transmit character to
 *    ch  - character to be transmitted
 *
 *  Output parameters:  NONE
 *
 *  Return values:      NONE
 */

void console_outbyte_interrupt(int   port, char  ch);

#endif /* CONSOLE_USE_INTERRUPTS */

/*
 *  Console Termios Support Entry Points
 *
 */

int console_write_support (int minor, const char *buf, int len)
{
  int nwrite = 0;

  while (nwrite < len) {
#if (CONSOLE_USE_INTERRUPTS)
    console_outbyte_interrupt( minor, *buf++ );
#else
    console_outbyte_polled( minor, *buf++ );
#endif
    nwrite++;
  }
  return nwrite;
}

/*
 *  Console Device Driver Entry Points
 *
 */

rtems_device_driver console_initialize(
  rtems_device_major_number  major,
  rtems_device_minor_number  minor,
  void                      *arg
)
{
  rtems_status_code status;

  rtems_termios_initialize();

  /*
   *  Register Device Names
   */

  status = rtems_io_register_name( "/dev/console", major, 0 );
  if (status != RTEMS_SUCCESSFUL)
    rtems_fatal_error_occurred(status);

  /*
   *  Initialize Hardware
   */
   
#if (CONSOLE_USE_INTERRUPTS)
  console_initialize_interrupts();
#endif

   console_initialize_hardware();

  return RTEMS_SUCCESSFUL;
}

rtems_device_driver console_open(
  rtems_device_major_number major,
  rtems_device_minor_number minor,
  void                    * arg
)
{
  rtems_status_code sc;
#if (CONSOLE_USE_INTERRUPTS)
  rtems_libio_open_close_args_t *args = arg;
  static const rtems_termios_callbacks intrCallbacks = {
    NULL,                        /* firstOpen */
    NULL,                        /* lastClose */
    NULL,                        /* pollRead */
    console_write_support,       /* write */
    NULL,                        /* setAttributes */
    NULL,                        /* stopRemoteTx */
    NULL,                        /* startRemoteTx */
    0                            /* outputUsesInterrupts */
  };
#else
  static const rtems_termios_callbacks pollCallbacks = {
    NULL,                        /* firstOpen */
    NULL,                        /* lastClose */
   console_inbyte_nonblocking,  /* pollRead */
    console_write_support,       /* write */
  NULL,                        /* setAttributes */
    NULL,                        /* stopRemoteTx */
    NULL,                        /* startRemoteTx */
    0                            /* outputUsesInterrupts */
  };
#endif

  assert( minor <= 1 );
  if ( minor > 2 )
    return RTEMS_INVALID_NUMBER;

#if (CONSOLE_USE_INTERRUPTS)
  sc = rtems_termios_open (major, minor, arg, &intrCallbacks);

  console_termios_data[ minor ] = args->iop->data1;
#else
  sc = rtems_termios_open (major, minor, arg, &pollCallbacks);
#endif

  return RTEMS_SUCCESSFUL;
}

rtems_device_driver console_close(
  rtems_device_major_number major,
  rtems_device_minor_number minor,
  void                    * arg
)
{
  return rtems_termios_close (arg);
}

rtems_device_driver console_read(
  rtems_device_major_number major,
  rtems_device_minor_number minor,
  void                    * arg
)
{
  return rtems_termios_read (arg);
}

rtems_device_driver console_write(
  rtems_device_major_number major,
  rtems_device_minor_number minor,
  void                    * arg
)
{
  return rtems_termios_write (arg);
}

rtems_device_driver console_control(
  rtems_device_major_number major,
  rtems_device_minor_number minor,
  void                    * arg
)
{
  return rtems_termios_ioctl (arg);
}
