/**
 *  @file
 *
 *  @brief Convert Baud Part of Termios control flags to an integral Baud Value
 *  @ingroup TermiostypesSupport
 */

/*
 *  COPYRIGHT (c) 1989-2008.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 */

#ifdef HAVE_CONFIG_H
  #include "config.h"
#endif

#include <rtems/termiostypes.h>

rtems_termios_baud_t rtems_termios_baud_to_number(speed_t spd)
{
  return rtems_assoc_local_by_remote(rtems_termios_baud_table, spd);
}
