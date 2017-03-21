/**
 *  @file
 *
 *  @brief Baud Rate Functions
 *  @ingroup Termios
 */

/*
 *  COPYRIGHT (c) 1989-1999.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 */

#if HAVE_CONFIG_H
#include "config.h"
#endif

#include <rtems.h>
#if defined(RTEMS_NEWLIB)

#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>
#include <termios.h>
#include <rtems/termiostypes.h>
#include <rtems/seterr.h>

/**
 *  POSIX 1003.1b 7.1.3 - Baud Rate Functions
 */
int cfsetospeed(
  struct termios *tp,
  speed_t         speed
)
{
  if ( rtems_termios_baud_to_index( speed ) == -1 )
    rtems_set_errno_and_return_minus_one( EINVAL );

  tp->c_ospeed = speed;
  return 0;
}
#endif
