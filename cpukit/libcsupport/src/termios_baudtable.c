/*
 *  COPYRIGHT (c) 1989-2010.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 */

#ifdef HAVE_CONFIG_H
  #include "config.h"
#endif

#include <rtems/termiostypes.h>

const rtems_assoc_t rtems_termios_baud_table [] = {
  { "B0",      0,      B0 },
  { "B50",     50,     B50 },
  { "B75",     75,     B75 },
  { "B110",    110,    B110 },
  { "B134",    134,    B134 },
  { "B150",    150,    B150 },
  { "B200",    200,    B200 },
  { "B300",    300,    B300 },
  { "B600",    600,    B600 },
  { "B1200",   1200,   B1200 },
  { "B1800",   1800,   B1800 },
  { "B2400",   2400,   B2400 },
  { "B4800",   4800,   B4800 },
  { "B9600",   9600,   B9600 },
  { "B19200",  19200,  B19200 },
  { "B38400",  38400,  B38400 },
  { "B57600",  57600,  B57600 },
  { "B115200", 115200, B115200 },
  { "B230400", 230400, B230400 },
  { "B460800", 460800, B460800 },
  { NULL,      0,      0 }
};
