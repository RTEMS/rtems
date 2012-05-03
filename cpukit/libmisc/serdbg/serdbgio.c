/*===============================================================*\
| File: serdbgio.c                                                |
+-----------------------------------------------------------------+
|                    Copyright (c) 2002 IMD                       |
|      Ingenieurbuero fuer Microcomputertechnik Th. Doerfler      |
|      Hebststr. 8, 82178 Puchheim, Germany                       |
|      <Thomas.Doerfler@imd-systems.de>                           |
|  The license and distribution terms for this file may be        |
|  found in the file LICENSE in this distribution or at           |
|  http://www.rtems.com/license/LICENSE.                     |
|                       all rights reserved                       |
+-----------------------------------------------------------------+
| TERMIOS serial gdb interface support                            |
| the functions in this file allow the standard gdb stubs like    |
| "m68k-stub.c" to access any serial interfaces that work with    |
| RTEMS termios in polled mode                                    |
|                                                                 |
+-----------------------------------------------------------------+
|   date                      history                        ID   |
| ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ |
| 10.05.02  creation                                         doe  |
|*****************************************************************|
\*===============================================================*/

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <rtems.h>
#include <rtems/libio_.h>
#include <errno.h>
#include <unistd.h> /* close */
#include <stdio.h>
#include <fcntl.h>
#include <termios.h>

#include <rtems/termiostypes.h>
#include <rtems/serdbg.h>


/*
 * internal variables
 */
int serdbg_fd = -1;
struct rtems_termios_tty *serdbg_tty;

/*=========================================================================*\
| Function:                                                                 |
\*-------------------------------------------------------------------------*/
int serdbg_open

/*-------------------------------------------------------------------------*\
| Purpose:                                                                  |
|    try to open given serial debug port                                    |
+---------------------------------------------------------------------------+
| Input Parameters:                                                         |
\*-------------------------------------------------------------------------*/
(
 const char *dev_name, /* name of device to open */
 uint32_t   baudrate   /* baud rate to use       */
)
/*-------------------------------------------------------------------------*\
| Return Value:                                                             |
|    0 on success, -1 and errno otherwise                                   |
\*=========================================================================*/
{
  bool err_occurred = false;
  rtems_libio_t *iop = NULL;
  struct termios act_termios;
  tcflag_t baudcode = B0;

#define FD_STORE_CNT 3
  int fd_store[FD_STORE_CNT];
  int fd_store_used = 0;

  /*
   * translate baudrate into baud code
   */
  switch(baudrate) {
  case     50: baudcode =     B50; break;
  case     75: baudcode =     B75; break;
  case    110: baudcode =    B110; break;
  case    134: baudcode =    B134; break;
  case    150: baudcode =    B150; break;
  case    200: baudcode =    B200; break;
  case    300: baudcode =    B300; break;
  case    600: baudcode =    B600; break;
  case   1200: baudcode =   B1200; break;
  case   1800: baudcode =   B1800; break;
  case   2400: baudcode =   B2400; break;
  case   4800: baudcode =   B4800; break;
  case   9600: baudcode =   B9600; break;
  case  19200: baudcode =  B19200; break;
  case  38400: baudcode =  B38400; break;
  case  57600: baudcode =  B57600; break;
  case 115200: baudcode = B115200; break;
  case 230400: baudcode = B230400; break;
  case 460800: baudcode = B460800; break;
  default    :   err_occurred = true; errno = EINVAL; break;
  }

 /*
  * open device for serdbg operation
  * skip any fds that are between 0..2, because they are
  * reserved for stdin/out/err
  */
  if (!err_occurred &&
      (dev_name != NULL) &&
      (dev_name[0] != '\0')) {
    do {
      serdbg_fd = open(dev_name,O_RDWR);
      if (serdbg_fd < 0) {
	err_occurred = true;
      }
      else {
	if (serdbg_fd < 3) {
	  if (fd_store_used >= FD_STORE_CNT) {
	    err_occurred = true;
	  }
	  else {
	    fd_store[fd_store_used++] = serdbg_fd;
	  }
	}
      }
    } while (!err_occurred &&
	     (serdbg_fd < 3));
  }
  /*
   * close any fds, that have been placed in fd_store
   * so fd 0..2 are reusable again
   */
  while (--fd_store_used >= 0) {
    close(fd_store[fd_store_used]);
  }

  /*
   * capture tty structure
   */
  if (!err_occurred) {
    iop = &rtems_libio_iops[serdbg_fd];
    serdbg_tty = iop->data1;
  }
  /*
   * set device baudrate
   * (and transp mode, this is not really needed)
   * ...
   */
  /*
   * ... get fd settings
   */
  if (!err_occurred &&
      (0 != tcgetattr(serdbg_fd,&act_termios))) {
      err_occurred = true;
  }
  if (!err_occurred) {
    act_termios.c_iflag
      &=  ~(IGNBRK|BRKINT|PARMRK|ISTRIP
	    |INLCR|IGNCR|ICRNL|IXON);
    act_termios.c_oflag
      &= ~OPOST;

    act_termios.c_lflag
      &= ~(ECHO|ECHONL|ICANON|ISIG|IEXTEN);

    cfsetospeed(&act_termios,baudcode);
    cfsetispeed(&act_termios,baudcode);

    if (0 != tcsetattr(serdbg_fd,TCSANOW,&act_termios)) {
	err_occurred = true;
    }
  }
  return (err_occurred
	  ? -1
	  : 0);
}

void putDebugChar(char c) __attribute__ ((__weak__));
/*=========================================================================*\
| Function:                                                                 |
\*-------------------------------------------------------------------------*/
void putDebugChar
/*-------------------------------------------------------------------------*\
| Purpose:                                                                  |
|    send one character to serial port                                      |
+---------------------------------------------------------------------------+
| Input Parameters:                                                         |
\*-------------------------------------------------------------------------*/
(
 char c  /* character to print */
)
/*-------------------------------------------------------------------------*\
| Return Value:                                                             |
|    <none>                                                                 |
\*=========================================================================*/
{
  /*
   * call serdbg polling callout, if available
   */
  if (serdbg_conf.callout != NULL) {
    serdbg_conf.callout();
  }
  /*
   * check, whether debug serial port is available
   */
  if ((serdbg_tty != NULL) &&
      (serdbg_tty->device.write != NULL)) {
    /*
     * send character to debug serial port
     */
    serdbg_tty->device.write(serdbg_tty->minor,&c,1);
  }
}

int getDebugChar(void) __attribute__ ((__weak__));
/*=========================================================================*\
| Function:                                                                 |
\*-------------------------------------------------------------------------*/
int getDebugChar
/*-------------------------------------------------------------------------*\
| Purpose:                                                                  |
|    wait for one character from serial port                                |
+---------------------------------------------------------------------------+
| Input Parameters:                                                         |
\*-------------------------------------------------------------------------*/
(
 void  /* none */
)
/*-------------------------------------------------------------------------*\
| Return Value:                                                             |
|    received character                                                     |
\*=========================================================================*/
{
  int c = -1;
  /*
   * check, whether debug serial port is available
   */
  if ((serdbg_tty != NULL) &&
      (serdbg_tty->device.pollRead != NULL)) {
    do {
      /*
       * call serdbg polling callout, if available
       */
      if (serdbg_conf.callout != NULL) {
	serdbg_conf.callout();
      }
      /*
       * get character from debug serial port
       */
      c = serdbg_tty->device.pollRead(serdbg_tty->minor);
    } while (c < 0);
  }
  return c;
}
