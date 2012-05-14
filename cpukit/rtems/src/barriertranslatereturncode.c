/*
 *  Barrier Manager -- Translate SuperCore Status
 *
 *  COPYRIGHT (c) 1989-2007.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 */

#if HAVE_CONFIG_H
#include "config.h"
#endif

#include <rtems/system.h>
#include <rtems/rtems/status.h>
#include <rtems/rtems/support.h>
#include <rtems/rtems/barrier.h>

/*
 *  _Barrier_Translate_core_barrier_return_code
 *
 *  Input parameters:
 *    the_barrier_status - barrier status code to translate
 *
 *  Output parameters:
 *    rtems status code - translated RTEMS status code
 *
 */

rtems_status_code _Barrier_Translate_core_barrier_return_code_[] = {
  RTEMS_SUCCESSFUL,         /* CORE_BARRIER_STATUS_SUCCESSFUL */
  RTEMS_SUCCESSFUL,         /* CORE_BARRIER_STATUS_AUTOMATICALLY_RELEASED */
  RTEMS_OBJECT_WAS_DELETED, /* CORE_BARRIER_WAS_DELETED */
  RTEMS_TIMEOUT             /* CORE_BARRIER_TIMEOUT */
};

rtems_status_code _Barrier_Translate_core_barrier_return_code (
  CORE_barrier_Status  the_barrier_status
)
{
  /*
   *  Internal consistency check for bad status from SuperCore
   */
  #if defined(RTEMS_DEBUG)
    if ( the_barrier_status > CORE_BARRIER_STATUS_LAST )
      return RTEMS_INTERNAL_ERROR;
  #endif
  return _Barrier_Translate_core_barrier_return_code_[the_barrier_status];
}
