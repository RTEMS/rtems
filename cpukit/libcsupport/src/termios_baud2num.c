/*
 *  COPYRIGHT (c) 1989-2008.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 *
 *  $Id$
 */

#include <sys/termios.h>

int termios_baud_to_number(
  int termios_baud
)
{
  int baud;

  switch (termios_baud) {
    case B0:        baud =      0;  break;
    case B50:       baud =     50;  break;
    case B75:       baud =     75;  break;
    case B110:      baud =    110;  break;
    case B134:      baud =    135;  break;
    case B150:      baud =    150;  break;
    case B200:      baud =    200;  break;
    case B300:      baud =    300;  break;
    case B600:      baud =    600;  break;
    case B1200:     baud =   1200;  break;
    case B1800:     baud =   1800;  break;
    case B2400:     baud =   2400;  break;
    case B4800:     baud =   4800;  break;
    case B9600:     baud =   9600;  break;
    case B19200:    baud =  19200;  break;
    case B38400:    baud =  38400;  break;
    case B57600:    baud =  57600;  break;
    case B115200:   baud = 115200;  break;
    case B230400:   baud = 230400;  break;
    case B460800:   baud = 460800;  break;
    default:        baud =     -1;  break;
  }

  return baud;
}
