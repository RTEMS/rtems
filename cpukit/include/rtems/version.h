/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup RTEMSAPIClassicVersion
 *
 * @brief This header file provides the Version API.
 */

/*
 *  Copyright (C) 2017.
 *  Chris Johns <chrisj@rtems.org>
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef _RTEMS_VERSION_H
#define _RTEMS_VERSION_H

#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @defgroup RTEMSAPIClassicVersion Version
 *
 * @ingroup RTEMSAPIClassic
 *
 * @brief The Version API provides functions to return the version or parts of
 * the version of RTEMS you are using.
 *
 * A branch in the version control system will always fall back to a
 * NOT-RELEASED version number with a minor number of 0. Only the release
 * archives have a VERSION file with a final release number. That means for
 * example that the 5 development branch will still show a version 5.0.0 even
 * after the 5.1 release.
 *
 * The reason for that are the following:
 *
 * 1. All pre-release tests are performed with a specific git hash. A committed
 * VERSION file would need to be changed and committed afterwards for releasing
 * with the required release version causing the released version to have a
 * different git hash and the test results couldn't be linked to the released
 * version.
 *
 * 2. Users deploying RTEMS would need to commit a local change to a committed
 * VERSION file and that would clash with the project changes. Deployment can
 * use the project repos directly.
 *
 * 3. The VERSION file management and generation is the responsibility of the
 * release manager and the release process.
 *
 * @{
 */

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
 * has been built.
 *
 * The key is specific to the version control system being used and allows the
 * built version to be identified.
 *
 * Use rtems_version_control_key_is_valid() to check if the version control key
 * is valid.
 *
 * @return The version control key.
 */
const char *rtems_version_control_key( void );

/**
 * @brief Returns true, if the version control key is valid, otherwise false.
 *
 * @retval true The version control key is valid.
 * @retval false Otherwise.
 */
static inline bool rtems_version_control_key_is_valid( const char *key )
{
  return key[ 0 ] != '\0';
}

/**
 * @brief Returns the board support package name.
 *
 * @return The board support package name.
 */
const char *rtems_board_support_package( void );

/** @} */

#ifdef __cplusplus
}
#endif

#endif
/* end of include file */
