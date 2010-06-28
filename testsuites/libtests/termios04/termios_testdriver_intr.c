/*
 *  This file contains a test fixture termios device driver
 *
 *  COPYRIGHT (c) 1989-2010.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 *
 *  $Id$
 */

#include "tmacros.h"
#include <rtems/libio.h>
#include <stdlib.h>
#include <termios.h>
#include <rtems/termiostypes.h>
#include <rtems/dumpbuf.h>
#include "termios_testdriver_intr.h"

rtems_id Timer;

#define TX_MAX 1024
uint8_t Tx_Buffer[TX_MAX];
int     Tx_Index = 0;
struct rtems_termios_tty *Ttyp;

void termios_test_driver_dump_tx(const char *c)
{
  printf( "%s %d characters\n", c, Tx_Index );
  rtems_print_buffer( Tx_Buffer, Tx_Index );
  Tx_Index = 0;
}
  
const uint8_t *Rx_Buffer;
int            Rx_Index = 0;
int            Rx_Length = 0;
bool           Rx_FirstTime = true;
bool           Rx_EnqueueNow = false;

rtems_timer_service_routine Rx_ISR(
  rtems_id  ignored_id,
  void     *ignored_address
)
{
  uint8_t ch;

  if ( Rx_Index >= Rx_Length )
    return;

  ch = Rx_Buffer[ Rx_Index++ ];
  rtems_termios_enqueue_raw_characters (Ttyp, (char *)&ch, 1);

  (void) rtems_timer_fire_after( Timer, 10, Rx_ISR, NULL );
}

void termios_test_driver_set_rx_enqueue_now(
  bool value
)
{
  Rx_EnqueueNow = value;
}

void termios_test_driver_set_rx(
  const void *p,
  size_t      len
)
{
  Rx_Buffer = p;
  Rx_Length = len;
  Rx_Index  = 0;

  if ( Rx_EnqueueNow == false) {
    (void) rtems_timer_fire_after( Timer, 10, Rx_ISR, NULL );
    return;
  }

  do {
    uint8_t ch;
    ch = Rx_Buffer[ Rx_Index++ ];
    rtems_termios_enqueue_raw_characters (Ttyp, (char *)&ch, 1);
  } while (Rx_Index < Rx_Length );
}

ssize_t termios_test_driver_write_helper(
  int         port,
  const char *buf,
  size_t      len
)
{
  size_t  i;

  for (i=0 ; i<len ; i++ )
    Tx_Buffer[Tx_Index++] = (uint8_t) buf[i];

  return len;
}

/*
 *  Set Attributes Handler
 */
int termios_test_driver_set_attributes(
  int                   minor,
  const struct termios *t
)
{
  return 0;
}

/*
 *  Test Device Driver Entry Points
 */
rtems_device_driver termios_test_driver_initialize(
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
  (void) rtems_io_register_name( TERMIOS_TEST_DRIVER_DEVICE_NAME, major, 0 );

  status = rtems_timer_create( rtems_build_name('T', 'M', '0', '1'), &Timer );
  if ( status )
    rtems_fatal_error_occurred(1);;

  return RTEMS_SUCCESSFUL;
}

rtems_device_driver termios_test_driver_open(
  rtems_device_major_number major,
  rtems_device_minor_number minor,
  void                    * arg
)
{
  rtems_status_code sc;
  rtems_libio_open_close_args_t *args = arg;
  static const rtems_termios_callbacks Callbacks = {
    NULL,                                    /* firstOpen */
    NULL,                                    /* lastClose */
    NULL,                                    /* pollRead */
    termios_test_driver_write_helper,        /* write */
    termios_test_driver_set_attributes,      /* setAttributes */
    NULL,                                    /* stopRemoteTx */
    NULL,                                    /* startRemoteTx */
    0                                        /* outputUsesInterrupts */
  };

  if ( minor > 2 ) {
    puts( "ERROR - Termios_testdriver - only 1 minor supported" );
    rtems_test_exit(0);
  }

  sc = rtems_termios_open (major, minor, arg, &Callbacks);

  Ttyp = args->iop->data1;   /* Keep cookie returned by termios_open */

  return RTEMS_SUCCESSFUL;
}

rtems_device_driver termios_test_driver_close(
  rtems_device_major_number major,
  rtems_device_minor_number minor,
  void                    * arg
)
{
  return rtems_termios_close (arg);
}

rtems_device_driver termios_test_driver_read(
  rtems_device_major_number major,
  rtems_device_minor_number minor,
  void                    * arg
)
{
  return rtems_termios_read (arg);
}

rtems_device_driver termios_test_driver_write(
  rtems_device_major_number major,
  rtems_device_minor_number minor,
  void                    * arg
)
{
  return rtems_termios_write (arg);
}

rtems_device_driver termios_test_driver_control(
  rtems_device_major_number major,
  rtems_device_minor_number minor,
  void                    * arg
)
{
  return rtems_termios_ioctl (arg);
}
