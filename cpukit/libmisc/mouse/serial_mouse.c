/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup libmisc_mouse Serial Mouse Driver
 *
 * @brief Serial Mouse Driver
 */

/*
 *  COPYRIGHT (c) 1989-2011.
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

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include <rtems/bspIo.h>
#include <rtems/libio.h>
#include <termios.h>
#include <rtems/termiostypes.h>
#include <rtems/mouse_parser.h>
#include <rtems/serial_mouse.h>

int         serial_mouse_fd = -1;
const char *serial_mouse_device;
const char *serial_mouse_type;

static int serial_mouse_l_rint(int c, struct rtems_termios_tty *tp)
{
  (void) tp;

  unsigned char buf = c;

  /* call mouse_parser( void *ptr, char *buffer, int size ) */
  mouse_parser_enqueue( &buf, 1 );
  return 0;
}

static struct rtems_termios_linesw serial_mouse_linesw = {
  .l_open = NULL,
  .l_close = NULL,
  .l_read  = NULL,
  .l_write = NULL,
  .l_rint  = serial_mouse_l_rint,
  .l_start = NULL,
  .l_ioctl = NULL,
  .l_modem = NULL
};


/*
 *  Serial Mouse - device driver INITIALIZE entry point.
 */
rtems_device_driver serial_mouse_initialize(
  rtems_device_major_number  major,
  rtems_device_minor_number  minor,
  void                      *arg
)
{
  (void) minor;
  (void) arg;

  bsp_get_serial_mouse_device(
    &serial_mouse_device,
    &serial_mouse_type
  );

  (void) rtems_io_register_name( "/dev/mouse", major, 0 );

  rtems_termios_linesw[ 6 ] = serial_mouse_linesw;

  return RTEMS_SUCCESSFUL;
}

/*
 * serial_mouse - device driver OPEN entry point
 */
rtems_device_driver serial_mouse_open(
  rtems_device_major_number  major,
  rtems_device_minor_number  minor,
  void                      *args
)
{
  (void) major;
  (void) minor;
  (void) args;

  struct termios  termios_attr;
  int             status;
  int             disc = 6;

  /* XXX open(2) the configured /dev/comX */
  /* XXX save the file descriptor */
  serial_mouse_fd = open( serial_mouse_device, O_RDONLY );
  if ( serial_mouse_fd == -1 ) {
   printk(
     "Error opening serial_mouse device on %s\n",
     serial_mouse_device
   );
   return RTEMS_IO_ERROR;
  }

  /* 1200-8-N-1, without hardware flow control */
  /* BSP_uart_init( BSP_UART_PORT, 1200, CHR_8_BITS, 0, 0, 0 ); */
  status = tcgetattr(serial_mouse_fd, &termios_attr );
  if (status != 0) {
    printk("Error getting mouse attributes\n");
    return RTEMS_IO_ERROR;
  }
  termios_attr.c_lflag &= ~(ICANON|ECHO|ECHONL|ECHOK|ECHOE|ECHOPRT|ECHOCTL);
  termios_attr.c_iflag &= ~(IXON|IXANY|IXOFF);
  /*
  termios_attr.c_cc[VMIN] = itask_VMIN;
  termios_attr.c_cc[VTIME] = itask_VTIME;
  */
  termios_attr.c_cflag |= B1200;
  termios_attr.c_cflag |= CS8;
  status = tcsetattr( serial_mouse_fd, TCSANOW, &termios_attr );
  if (status != 0) {
    printk("Error setting mouse attributes\n");
    return RTEMS_IO_ERROR;
  }

  status = ioctl(serial_mouse_fd, TIOCSETD, &disc);
  if (status != 0) {
    printk("Error setting mouse attributes\n");
    return RTEMS_IO_ERROR;
  }

  sleep(5);
  return RTEMS_SUCCESSFUL;
}

rtems_device_driver serial_mouse_close(
  rtems_device_major_number  major,
  rtems_device_minor_number  minor,
  void                      *arg
)
{
  (void) major;
  (void) minor;
  (void) arg;

  close( serial_mouse_fd );

  return RTEMS_SUCCESSFUL;
}

rtems_device_driver serial_mouse_read(
  rtems_device_major_number  major,
  rtems_device_minor_number  minor,
  void                      *arg
)
{
  (void) major;
  (void) minor;
  (void) arg;

  return RTEMS_SUCCESSFUL;
}


rtems_device_driver serial_mouse_write(
  rtems_device_major_number  major,
  rtems_device_minor_number  minor,
  void                      *arg
)
{
  (void) major;
  (void) minor;
  (void) arg;

  return RTEMS_SUCCESSFUL;
}


rtems_device_driver serial_mouse_control(
  rtems_device_major_number  major,
  rtems_device_minor_number  minor,
  void                      *arg
)
{
  (void) major;
  (void) minor;

  rtems_libio_ioctl_args_t *args = (rtems_libio_ioctl_args_t *)arg;

  switch( args->command ) {

    case MW_UID_REGISTER_DEVICE:
      printk( "SerialMouse: reg=%s\n", (const char*) args->buffer );
      mouse_parser_initialize( serial_mouse_type );
      break;

    case MW_UID_UNREGISTER_DEVICE:
      break;

    default:
      args->ioctl_return = ioctl(serial_mouse_fd, args->command, args->buffer );
      if ( !args->ioctl_return )
        return RTEMS_SUCCESSFUL;
      return RTEMS_IO_ERROR;
  }
  args->ioctl_return = 0;
  return RTEMS_SUCCESSFUL;
}
