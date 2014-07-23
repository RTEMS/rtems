/**
 * @file
 *
 * @brief Adjust the Time of Time
 * @ingroup ScoreTOD
 */

/*
 *  COPYRIGHT (c) 1989-2014.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 */

#if HAVE_CONFIG_H
#include "config.h"
#endif

#include <rtems/score/threaddispatch.h>
#include <rtems/score/todimpl.h>

void _TOD_Adjust(
  const Timestamp_Control delta
)
{
  Timestamp_Control  tod;
  Timestamp_Control *tod_ptr;

  /*
   * Currently, RTEMS does the adjustment in one movement.
   * Given interest, requirements, and sponsorship, a future
   * enhancement would be to adjust the time in smaller increments
   * at each clock tick. Until then, there is no outstanding
   * adjustment.
   */

  /*
   * This prevents context switches while we are adjusting the TOD
   */
  _Thread_Disable_dispatch();

    tod_ptr = _TOD_Get_with_nanoseconds( &tod, &_TOD.now );

    _Timestamp_Add_to( tod_ptr, &delta );

    _TOD_Set_with_timestamp( tod_ptr );

  _Thread_Enable_dispatch();
}
