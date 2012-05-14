/*===============================================================*\
| File: termios_printk.c                                          |
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
| TERMIOS printk support                                          |
| this module performs low-level printk output using              |
| a polled termios driver                                         |
|                                                                 |
+-----------------------------------------------------------------+
|   date                      history                        ID   |
| ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ |
| 13.05.02  creation                                         doe  |
|*****************************************************************|
\*===============================================================*/

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <rtems.h>
#include <rtems/libio_.h>
#include <errno.h>
#include <stdio.h>
#include <fcntl.h>
#include <termios.h>

#include <rtems/termiostypes.h>
#include <rtems/bspIo.h>
#include <rtems/termios_printk.h>

/*
 * internal variables
 */
int termios_printk_fd = -1;
struct rtems_termios_tty *termios_printk_tty;

static void _termios_printk_null_char(
	char c __attribute__((unused)))
{
  return;
}

BSP_output_char_function_type BSP_output_char = _termios_printk_null_char;
BSP_polling_getchar_function_type BSP_poll_char;

/*=========================================================================*\
| Function:                                                                 |
\*-------------------------------------------------------------------------*/
void termios_printk_outputchar
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
  static const char cr = '\r';
  /*
   * check, whether printk serial port is available
   */

  if ((termios_printk_tty != NULL) &&
      (termios_printk_tty->device.write != NULL)) {
    /*
     * call termios_printk polling callout, if available
     */
    if (termios_printk_conf.callout != NULL) {
      termios_printk_conf.callout();
    }
    /*
     * send character to debug serial port
     */
    if (c == '\n') {
      termios_printk_tty->device.write(termios_printk_tty->minor,&cr,1);
    }
    termios_printk_tty->device.write(termios_printk_tty->minor,&c,1);
  }
}

/*=========================================================================*\
| Function:                                                                 |
\*-------------------------------------------------------------------------*/
int termios_printk_inputchar
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
  if ((termios_printk_tty != NULL) &&
      (termios_printk_tty->device.pollRead != NULL)) {
    do {
      /*
       * call termios_printk polling callout, if available
       */
      if (termios_printk_conf.callout != NULL) {
	termios_printk_conf.callout();
      }
      /*
       * get character from debug serial port
       */
      c = termios_printk_tty->device.pollRead(termios_printk_tty->minor);
    } while (c < 0);
  }
  return c;
}


/*=========================================================================*\
| Function:                                                                 |
\*-------------------------------------------------------------------------*/
int termios_printk_open

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

  if (termios_printk_fd >= 0) {
    /*
     * already initialized
     */
    return 0;
  }
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
  */
  if (!err_occurred &&
      (dev_name != NULL) &&
      (dev_name[0] != '\0')) {
    termios_printk_fd = open(dev_name,O_RDWR);
    if (termios_printk_fd < 0) {
      err_occurred = true;
    }
  }
  /*
   * capture tty structure
   */
  if (!err_occurred) {
    iop = &rtems_libio_iops[termios_printk_fd];
    termios_printk_tty = iop->data1;
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
      (0 != tcgetattr(termios_printk_fd,&act_termios))) {
      err_occurred = true;
  }
  if (!err_occurred) {

    cfsetospeed(&act_termios,baudcode);
    cfsetispeed(&act_termios,baudcode);

    if (0 != tcsetattr(termios_printk_fd,TCSANOW,&act_termios)) {
	err_occurred = true;
    }
  }
  if (!err_occurred) {
    BSP_output_char = termios_printk_outputchar;
    BSP_poll_char = termios_printk_inputchar;
  }
  return (err_occurred
	  ? -1
	  : 0);
}
