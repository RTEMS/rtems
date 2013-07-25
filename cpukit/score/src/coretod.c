/**
 * @file
 *
 * @brief Initializes the Time of Day Handler
 *
 * @ingroup ScoreTODConstants
 */

/*  COPYRIGHT (c) 1989-2008.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 */

#if HAVE_CONFIG_H
#include "config.h"
#endif

#include <rtems/score/tod.h>

void _TOD_Handler_initialization(void)
{
  /* POSIX format TOD (timespec) */
  _Timestamp_Set( &_TOD.now, TOD_SECONDS_1970_THROUGH_1988, 0 );

  /* Uptime (timespec) */
  _Timestamp_Set_to_zero( &_TOD.uptime );

  /* TOD has not been set */
  _TOD.is_set = false;
  _TOD_Activate();
}
