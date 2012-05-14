/*
 *  COPYRIGHT (c) 1989-2008.
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

rtems_termios_baud_t rtems_termios_baud_to_number(tcflag_t c_cflag)
{
  uint32_t remote_value = (uint32_t) (c_cflag & CBAUD);

  return rtems_assoc_local_by_remote(rtems_termios_baud_table, remote_value);
}
