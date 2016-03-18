/**
 * @file
 *
 * @brief POSIX API Implementation
 *
 * This include file defines the top level interface to the POSIX API
 * implementation in RTEMS.
 */

/*
 *  COPYRIGHT (c) 1989-2011.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 */

#ifndef _RTEMS_POSIX_POSIXAPI_H
#define _RTEMS_POSIX_POSIXAPI_H

#include <rtems/config.h>
#include <rtems/score/assert.h>
#include <rtems/score/objectimpl.h>

/**
 * @defgroup POSIXAPI RTEMS POSIX API
 *
 * RTEMS POSIX API definitions and modules.
 *
 */
/**@{**/

/**
 * @brief POSIX API Fatal domains.
 */
typedef enum {
  POSIX_FD_PTHREAD,      /**< A pthread thread error. */
  POSIX_FD_PTHREAD_ONCE  /**< A pthread once error. */
} POSIX_Fatal_domain;

/**
 * @brief POSIX API Fatal error.
 *
 * A common method of rasing a POSIX API fatal error.
 *
 * @param[in] domain The POSIX error domain.
 * @param[in] eno The error number as defined in errno.h.
 */
void _POSIX_Fatal_error( POSIX_Fatal_domain domain, int eno );

extern const int _POSIX_Get_by_name_error_table[ 3 ];

RTEMS_INLINE_ROUTINE int _POSIX_Get_by_name_error(
  Objects_Get_by_name_error error
)
{
  _Assert( (size_t) error < RTEMS_ARRAY_SIZE( _POSIX_Get_by_name_error_table ) );
  return _POSIX_Get_by_name_error_table[ error ];
}

/** @} */

#endif
/* end of include file */
