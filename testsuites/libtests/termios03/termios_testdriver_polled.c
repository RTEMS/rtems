/**
 *  @file
 *
 *  This file contains a test fixture termios device driver
 */

/*
 *  COPYRIGHT (c) 1989-2012.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "tmacros.h"
#include <rtems/libio.h>
#include <stdlib.h>
#include <termios.h>
#include <rtems/termiostypes.h>
#include <rtems/dumpbuf.h>
#include "termios_testdriver_polled.h"

/* forward declarations to avoid warnings */
int termios_test_driver_inbyte_nonblocking(int port);
void termios_test_driver_outbyte_polled(int port, char ch);
ssize_t termios_test_driver_write_support(
  int         minor,
  const char *buf,
  size_t      len
);
int termios_test_driver_set_attributes(
  int                   minor,
  const struct termios *t
);

#define TX_MAX 1024
uint8_t Tx_Buffer[TX_MAX];
int     Tx_Index = 0;

void termios_test_driver_dump_tx(const char *c)
{
  printf( "%s %d characters\n", c, Tx_Index );
  rtems_print_buffer( Tx_Buffer, Tx_Index );
  Tx_Index = 0;
}
  
const uint8_t *Rx_Buffer;
int            Rx_Index;
int            Rx_Length;
bool           Rx_FirstTime = true;

void termios_test_driver_set_rx(
  const void *p,
  size_t      len
)
{
  Rx_Buffer = p;
  Rx_Length = len;
  Rx_Index  = 0;
}

int termios_test_driver_inbyte_nonblocking( int port )
{
  if ( Rx_FirstTime == true ) {
    Rx_FirstTime = false;
    return -1;
  }
  if ( Rx_Index >= Rx_Length )
    return -1;
  return Rx_Buffer[ Rx_Index++ ];
}

void termios_test_driver_outbyte_polled(
  int  port,
  char ch
)
{
  Tx_Buffer[Tx_Index++] = (uint8_t) ch;
}

ssize_t termios_test_driver_write_support (int minor, const char *buf, size_t len)
{
  size_t nwrite = 0;

  while (nwrite < len) {
    termios_test_driver_outbyte_polled( minor, *buf++ );
    nwrite++;
  }
  return nwrite;
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
  rtems_termios_initialize();

  /*
   *  Register Device Names
   */
  (void) rtems_io_register_name( TERMIOS_TEST_DRIVER_DEVICE_NAME, major, 0 );

  return RTEMS_SUCCESSFUL;
}

rtems_device_driver termios_test_driver_open(
  rtems_device_major_number major,
  rtems_device_minor_number minor,
  void                    * arg
)
{
  rtems_status_code sc;
  static const rtems_termios_callbacks Callbacks = {
    NULL,                                    /* firstOpen */
    NULL,                                    /* lastClose */
    termios_test_driver_inbyte_nonblocking,  /* pollRead */
    termios_test_driver_write_support,       /* write */
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
  directive_failed( sc, "termios open" ); 

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
