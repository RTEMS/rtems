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

int termios_number_to_baud(
  int baud
)
{
  int termios_baud;

  switch (baud) {
    case 0:       termios_baud = B0;      break;
    case 50:      termios_baud = B50;     break;
    case 75:      termios_baud = B75;     break;
    case 110:     termios_baud = B110;    break;
    case 134:     termios_baud = B134;    break;
    case 150:     termios_baud = B150;    break;
    case 200:     termios_baud = B200;    break;
    case 300:     termios_baud = B300;    break;
    case 600:     termios_baud = B600;    break;
    case 1200:    termios_baud = B1200;   break;
    case 1800:    termios_baud = B1800;   break;
    case 2400:    termios_baud = B2400;   break;
    case 4800:    termios_baud = B4800;   break;
    case 9600:    termios_baud = B9600;   break;
    case 19200:   termios_baud = B19200;  break;
    case 38400:   termios_baud = B38400;  break;
    case 57600:   termios_baud = B57600;  break;
    case 115200:  termios_baud = B115200; break;
    case 230400:  termios_baud = B230400; break;
    case 460800:  termios_baud = B460800; break;
    default:      termios_baud = -1;      break;
  }

  return termios_baud;
}
