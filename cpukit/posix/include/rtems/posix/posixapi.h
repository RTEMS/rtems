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

/**
 * @brief Initialize POSIX API.
 *
 * This method is responsible for initializing each of the POSIX
 * API managers.
 */
void _POSIX_API_Initialize(void);

/**
 * @brief Queries the object identifier @a id for a @a name.
 *
 * @param[in] information Object information.
 * @param[in] name Zero terminated name string to look up.
 * @param[out] id Pointer for identifier.  The pointer must be valid.
 * @param[out] len Pointer for string length.  The pointer must be valid.
 *
 * @retval 0 Successful operation.
 * @retval EINVAL The @a name pointer is @c NULL or the @a name string has
 * zero length.
 * @retval ENAMETOOLONG The @a name string length is greater than or equal to
 * @c NAME_MAX.
 * @retval ENOENT Found no corresponding identifier.
 */
int _POSIX_Name_to_id(
  Objects_Information *information,
  const char          *name,
  Objects_Id          *id,
  size_t              *len
);

/** @} */

#endif
/* end of include file */
