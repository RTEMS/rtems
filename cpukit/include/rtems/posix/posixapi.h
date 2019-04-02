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
#include <rtems/score/threadimpl.h>
#include <rtems/seterr.h>

#include <pthread.h>

/**
 * @defgroup POSIXAPI RTEMS POSIX API
 *
 * @ingroup RTEMSInternal
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

RTEMS_INLINE_ROUTINE int _POSIX_Get_error( Status_Control status )
{
  return STATUS_GET_POSIX( status );
}

RTEMS_INLINE_ROUTINE int _POSIX_Get_error_after_wait(
  const Thread_Control *executing
)
{
  return _POSIX_Get_error( _Thread_Wait_get_status( executing ) );
}

RTEMS_INLINE_ROUTINE int _POSIX_Zero_or_minus_one_plus_errno(
  Status_Control status
)
{
  if ( status == STATUS_SUCCESSFUL ) {
    return 0;
  }

  rtems_set_errno_and_return_minus_one( _POSIX_Get_error( status ) );
}

/*
 * See also The Open Group Base Specifications Issue 7, IEEE Std 1003.1-2008,
 * 2016 Edition, subsection 2.9.9, Synchronization Object Copies and
 * Alternative Mappings.
 *
 * http://pubs.opengroup.org/onlinepubs/9699919799/functions/V2_chap02.html#tag_15_09_09
 */
RTEMS_INLINE_ROUTINE bool _POSIX_Is_valid_pshared( int pshared )
{
  return pshared == PTHREAD_PROCESS_PRIVATE ||
    pshared == PTHREAD_PROCESS_SHARED;
}

/** @} */

#endif
/* end of include file */
