/**
 * @file
 *
 * @ingroup RTEMSImplClassicStatus
 *
 * @brief This header file provides the implementation interfaces of
 *   the @ref RTEMSImplClassicStatus support.
 */

/*  COPYRIGHT (c) 1989-2008.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 */

#ifndef _RTEMS_RTEMS_STATUSIMPL_H
#define _RTEMS_RTEMS_STATUSIMPL_H

#include <rtems/rtems/status.h>
#include <rtems/score/threadimpl.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @defgroup RTEMSImplClassicStatus Directive Status
 *
 * @ingroup RTEMSImplClassic
 *
 * @brief This group contains the implementation to support directive status
 *   codes.
 *
 * @{
 */

RTEMS_INLINE_ROUTINE rtems_status_code _Status_Get(
  Status_Control status
)
{
  return (rtems_status_code) STATUS_GET_CLASSIC( status );
}

RTEMS_INLINE_ROUTINE rtems_status_code _Status_Get_after_wait(
  const Thread_Control *executing
)
{
  return _Status_Get( _Thread_Wait_get_status( executing ) );
}

/**@}*/

#ifdef __cplusplus
}
#endif

#endif
/* end of include file */
