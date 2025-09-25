/* SPDX-License-Identifier: BSD-2-Clause */

/**
 *  @file
 *
 *  This file contains a test fixture termios device driver
 */

/*
 *  COPYRIGHT (c) 1989-2012.
 *  On-Line Applications Research Corporation (OAR).
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
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
#include "termios_testdriver_intr.h"

/* forward declarations to avoid warnings */
void termios_test_driver_wait_for_tx_to_complete(void);
rtems_timer_service_routine Rx_ISR(
  rtems_id  ignored_id,
  void     *ignored_address
);
rtems_timer_service_routine Tx_ISR(
  rtems_id  ignored_id,
  void     *ignored_address
);
ssize_t termios_test_driver_write_helper(
  int         port,
  const char *buf,
  size_t      len
);
int termios_test_driver_set_attributes(
  int                   minor,
  const struct termios *t
);
#if defined(TASK_DRIVEN)
  int termios_test_driver_inbyte_nonblocking(int port);
#endif

/* global variables */
rtems_id Rx_Timer;
rtems_id Tx_Timer;

#define TX_MAX 1024
uint8_t                   Tx_Buffer[TX_MAX];
int                       Tx_Index = 0;
struct rtems_termios_tty *Ttyp;

void termios_test_driver_wait_for_tx_to_complete(void)
{
  rtems_task_wake_after( 2 * rtems_clock_get_ticks_per_second() );
}

void termios_test_driver_dump_tx(const char *c)
{
  termios_test_driver_wait_for_tx_to_complete();

  printf( "%s %d characters\n", c, Tx_Index );
  rtems_print_buffer( Tx_Buffer, Tx_Index );
  Tx_Index = 0;
}
  
const uint8_t *Rx_Buffer;
int            Rx_Index = 0;
int            Rx_Length = 0;
bool           Rx_FirstTime = true;
bool           Rx_EnqueueNow = false;

#if defined(TASK_DRIVEN)
  int termios_test_driver_inbyte_nonblocking( int port )
  {
    (void) port;

    if ( Rx_FirstTime == true ) {
      Rx_FirstTime = false;
      return -1;
    }
    if ( Rx_Index >= Rx_Length )
      return -1;
    return Rx_Buffer[ Rx_Index++ ];
  }
#endif

rtems_timer_service_routine Rx_ISR(
  rtems_id  ignored_id,
  void     *ignored_address
)
{
  (void) ignored_id;
  (void) ignored_address;

  uint8_t ch;

  if ( Rx_Index >= Rx_Length )
    return;

  ch = Rx_Buffer[ Rx_Index++ ];
  rtems_termios_enqueue_raw_characters (Ttyp, (char *)&ch, 1);
  #if defined(TASK_DRIVEN)
    rtems_termios_rxirq_occured(Ttyp);
  #endif

  (void) rtems_timer_fire_after( Rx_Timer, 10, Rx_ISR, NULL );
}

rtems_timer_service_routine Tx_ISR(
  rtems_id  ignored_id,
  void     *ignored_address
)
{
  (void) ignored_id;
  (void) ignored_address;

  rtems_termios_dequeue_characters (Ttyp, 1);

  (void) rtems_timer_fire_after( Tx_Timer, 10, Tx_ISR, NULL );
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
    (void) rtems_timer_fire_after( Rx_Timer, 10, Rx_ISR, NULL );
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
  (void) port;

  if (len > 0) {
    Tx_Buffer[Tx_Index++] = buf[0];
    (void) rtems_timer_fire_after( Tx_Timer, 10, Tx_ISR, NULL );
  }

  return 0;
}

/*
 *  Set Attributes Handler
 */
int termios_test_driver_set_attributes(
  int                   minor,
  const struct termios *t
)
{
  (void) minor;
  (void) t;

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
  (void) minor;
  (void) arg;

  rtems_termios_initialize();

  /*
   *  Register Device Names
   */
  (void) rtems_io_register_name( TERMIOS_TEST_DRIVER_DEVICE_NAME, major, 0 );

  return RTEMS_SUCCESSFUL;
}

static int first_open(int major, int minor, void *arg)
{
  (void) major;
  (void) minor;
  (void) arg;

  rtems_status_code status;

  status = rtems_timer_create(rtems_build_name('T', 'M', 'R', 'X'), &Rx_Timer);
  if ( status != RTEMS_SUCCESSFUL )
    rtems_fatal_error_occurred(1);

  status = rtems_timer_create(rtems_build_name('T', 'M', 'T', 'X'), &Tx_Timer);
  if ( status != RTEMS_SUCCESSFUL )
    rtems_fatal_error_occurred(1);

  return 0;
}

static int last_close(int major, int minor, void *arg)
{
  (void) major;
  (void) minor;
  (void) arg;

  rtems_status_code status;

  status = rtems_timer_cancel(Rx_Timer);
  if ( status != RTEMS_SUCCESSFUL )
    rtems_fatal_error_occurred(1);

  status = rtems_timer_cancel(Tx_Timer);
  if ( status != RTEMS_SUCCESSFUL )
    rtems_fatal_error_occurred(1);

  status = rtems_timer_delete(Rx_Timer);
  if ( status != RTEMS_SUCCESSFUL )
    rtems_fatal_error_occurred(1);

  status = rtems_timer_delete(Tx_Timer);
  if ( status != RTEMS_SUCCESSFUL )
    rtems_fatal_error_occurred(1);

  return 0;
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
    first_open,                              /* firstOpen */
    last_close,                              /* lastClose */
    #if defined(TASK_DRIVEN)
      termios_test_driver_inbyte_nonblocking,/* pollRead */
    #else
      NULL,                                  /* pollRead */
    #endif
    termios_test_driver_write_helper,        /* write */
    termios_test_driver_set_attributes,      /* setAttributes */
    NULL,                                    /* stopRemoteTx */
    NULL,                                    /* startRemoteTx */
    #if defined(TASK_DRIVEN)
      TERMIOS_TASK_DRIVEN                    /* outputUsesInterrupts */
    #else
      TERMIOS_POLLED                         /* outputUsesInterrupts */
    #endif
  };

  if ( minor > 2 ) {
    puts( "ERROR - Termios_testdriver - only 1 minor supported" );
    rtems_test_exit(0);
  }

  sc = rtems_termios_open (major, minor, arg, &Callbacks);
  directive_failed( sc, "termios open" ); 

  Ttyp = args->iop->data1;   /* Keep cookie returned by termios_open */

  return RTEMS_SUCCESSFUL;
}

rtems_device_driver termios_test_driver_close(
  rtems_device_major_number major,
  rtems_device_minor_number minor,
  void                    * arg
)
{
  (void) major;
  (void) minor;

  return rtems_termios_close (arg);
}

rtems_device_driver termios_test_driver_read(
  rtems_device_major_number major,
  rtems_device_minor_number minor,
  void                    * arg
)
{
  (void) major;
  (void) minor;

  return rtems_termios_read (arg);
}

rtems_device_driver termios_test_driver_write(
  rtems_device_major_number major,
  rtems_device_minor_number minor,
  void                    * arg
)
{
  (void) major;
  (void) minor;

  return rtems_termios_write (arg);
}

rtems_device_driver termios_test_driver_control(
  rtems_device_major_number major,
  rtems_device_minor_number minor,
  void                    * arg
)
{
  (void) major;
  (void) minor;

  return rtems_termios_ioctl (arg);
}
