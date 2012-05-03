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

tcflag_t rtems_termios_number_to_baud(rtems_termios_baud_t baud)
{
  uint32_t remote_value = rtems_assoc_remote_by_local(
    rtems_termios_baud_table,
    baud
  );

  if (remote_value == 0) {
    remote_value = B0;
  }

  return (tcflag_t) remote_value;
}
