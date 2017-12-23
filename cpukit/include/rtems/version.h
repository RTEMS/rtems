/**
 * @file
 *
 * @brief Version API.
 */

/*
 *  Copyright (C) 2017.
 *  Chris Johns <chrisj@rtems.org>
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 */

#ifndef _RTEMS_VERSION_H
#define _RTEMS_VERSION_H

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @defgroup ClassicVersion Version
 *
 * @ingroup ClassicVersion
 *
 * @brief The Version API provides functions to return the version or parts of
 * the version of RTEMS you are using.
 */
/**@{**/

/**
 * @brief Returns the version string.
 *
 * @retval text The version as a string.
 */
const char *rtems_version( void );

/**
 * @brief Returns the version's major number.
 *
 * @retval int The version's major number.
 */
int rtems_version_major( void );

/**
 * @brief Returns the version's minor number.
 *
 * @retval int The version's minor number.
 */
int rtems_version_minor( void );

/**
 * @brief Returns the version's revision number.
 *
 * @retval int The version's revision number.
 */
int rtems_version_revision( void );

/**
 * @brief Returns the version control key for the current version of code that
 * has been built. The key is specific to the version control system being used
 * and allows the built version to be identified.
 *
 * @retval int The version's version control key.
 */
const char *rtems_version_control_key( void );

/** @} */

#ifdef __cplusplus
}
#endif

#endif
/* end of include file */
