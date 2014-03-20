/**
 * @file
 *
 * @ingroup ClassicStatusImpl
 *
 * @brief Classic Status Implementation
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

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @defgroup ClassicStatusImpl Classic Status Implementation
 *
 * @ingroup ClassicStatus
 *
 * @{
 */

/**
 *  @brief Status Object Name Errors to Status Array
 *
 *  This array is used to map SuperCore Object Handler return
 *  codes to Classic API status codes.
 */
extern const rtems_status_code _Status_Object_name_errors_to_status[];

/**@}*/

#ifdef __cplusplus
}
#endif

#endif
/* end of include file */
