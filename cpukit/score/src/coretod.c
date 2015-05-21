/**
 * @file
 *
 * @brief Initializes the Time of Day Handler
 *
 * @ingroup ScoreTOD
 */

/*  COPYRIGHT (c) 1989-2014.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 */

#if HAVE_CONFIG_H
#include "config.h"
#endif

#include <rtems/score/todimpl.h>

void _TOD_Handler_initialization(void)
{
  struct timespec ts;

  _Timecounter_Initialize();

  ts.tv_sec = TOD_SECONDS_1970_THROUGH_1988;
  ts.tv_nsec = 0;
  _Timecounter_Set_clock( &ts );

  /* TOD has not been set */
  _TOD.is_set = false;
}
