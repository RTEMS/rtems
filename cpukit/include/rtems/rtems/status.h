/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @brief This header file provides the status codes of Classic API directives
 *   and support functions.
 */

/*
 * Copyright (C) 2014, 2020 embedded brains GmbH (http://www.embedded-brains.de)
 * Copyright (C) 1989, 2008 On-Line Applications Research Corporation (OAR)
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

/*
 * This file is part of the RTEMS quality process and was automatically
 * generated.  If you find something that needs to be fixed or
 * worded better please post a report or patch to an RTEMS mailing list
 * or raise a bug report:
 *
 * https://www.rtems.org/bugs.html
 *
 * For information on updating and regenerating please refer to the How-To
 * section in the Software Requirements Engineering chapter of the
 * RTEMS Software Engineering manual.  The manual is provided as a part of
 * a release.  For development sources please refer to the online
 * documentation at:
 *
 * https://docs.rtems.org
 */

/* Generated from spec:/rtems/status/if/header */

#ifndef _RTEMS_RTEMS_STATUS_H
#define _RTEMS_RTEMS_STATUS_H

#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

/* Generated from spec:/rtems/status/if/group */

/**
 * @defgroup RTEMSAPIClassicStatus Directive Status Codes
 *
 * @ingroup RTEMSAPIClassic
 *
 * @brief This group contains the Classic API directive status codes and
 *   support functions.
 */

/* Generated from spec:/rtems/status/if/code */

/**
 * @ingroup RTEMSAPIClassicStatus
 *
 * @brief This enumeration provides status codes for directives of the Classic
 *   API.
 */
typedef enum {
  /**
   * @brief This status code indicates successful completion of a requested
   *   operation.
   */
  RTEMS_SUCCESSFUL = 0,

  /**
   * @brief This status code indicates that a thread exitted.
   */
  RTEMS_TASK_EXITTED = 1,

  /**
   * @brief This status code indicates that multiprocessing was not configured.
   */
  RTEMS_MP_NOT_CONFIGURED = 2,

  /**
   * @brief This status code indicates that an object name was invalid.
   */
  RTEMS_INVALID_NAME = 3,

  /**
   * @brief This status code indicates that an object identifier was invalid.
   */
  RTEMS_INVALID_ID = 4,

  /**
   * @brief This status code indicates you have attempted to create too many
   *   instances of a particular object class.
   */
  RTEMS_TOO_MANY = 5,

  /**
   * @brief This status code indicates that a blocking directive timed out.
   */
  RTEMS_TIMEOUT = 6,

  /**
   * @brief This status code indicates the object was deleted while the thread
   *   was blocked waiting.
   */
  RTEMS_OBJECT_WAS_DELETED = 7,

  /**
   * @brief This status code indicates that a specified size was invalid.
   */
  RTEMS_INVALID_SIZE = 8,

  /**
   * @brief This status code indicates that a specified address was invalid.
   */
  RTEMS_INVALID_ADDRESS = 9,

  /**
   * @brief This status code indicates that a specified number was invalid.
   */
  RTEMS_INVALID_NUMBER = 10,

  /**
   * @brief This status code indicates that the item has not been initialized.
   */
  RTEMS_NOT_DEFINED = 11,

  /**
   * @brief This status code indicates that the object still had resources in
   *   use.
   */
  RTEMS_RESOURCE_IN_USE = 12,

  /**
   * @brief This status code indicates that the request was not satisfied.
   */
  RTEMS_UNSATISFIED = 13,

  /**
   * @brief This status code indicates that an object was in wrong state for the
   *   requested operation.
   */
  RTEMS_INCORRECT_STATE = 14,

  /**
   * @brief This status code indicates that the thread was already suspended.
   */
  RTEMS_ALREADY_SUSPENDED = 15,

  /**
   * @brief This status code indicates that the operation was illegal on the
   *   calling thread.
   */
  RTEMS_ILLEGAL_ON_SELF = 16,

  /**
   * @brief This status code indicates that the operation was illegal on a remote
   *   object.
   */
  RTEMS_ILLEGAL_ON_REMOTE_OBJECT = 17,

  /**
   * @brief This status code indicates that the operation should not be called
   *   from this execution environment.
   */
  RTEMS_CALLED_FROM_ISR = 18,

  /**
   * @brief This status code indicates that an invalid thread priority was
   *   provided.
   */
  RTEMS_INVALID_PRIORITY = 19,

  /**
   * @brief This status code indicates that a specified date or time was invalid.
   */
  RTEMS_INVALID_CLOCK = 20,

  /**
   * @brief This status code indicates that a specified node identifier was
   *   invalid.
   */
  RTEMS_INVALID_NODE = 21,

  /**
   * @brief This status code indicates that the directive was not configured.
   */
  RTEMS_NOT_CONFIGURED = 22,

  /**
   * @brief This status code indicates that the caller was not the owner of the
   *   resource.
   */
  RTEMS_NOT_OWNER_OF_RESOURCE = 23,

  /**
   * @brief This status code indicates the directive or requested portion of the
   *   directive is not implemented.
   *
   * This is a hint that you have stumbled across an opportunity to submit code
   * to the RTEMS Project.
   */
  RTEMS_NOT_IMPLEMENTED = 24,

  /**
   * @brief This status code indicates that an internal RTEMS inconsistency was
   *   detected.
   */
  RTEMS_INTERNAL_ERROR = 25,

  /**
   * @brief This status code indicates that the directive attempted to allocate
   *   memory but was unable to do so.
   */
  RTEMS_NO_MEMORY = 26,

  /**
   * @brief This status code indicates a device driver IO error.
   */
  RTEMS_IO_ERROR = 27,

  /**
   * @brief This status code is used internally by the implementation to indicate
   *   a blocking device driver call has been interrupted and should be reflected
   *   to the caller as interrupted.
   */
  RTEMS_INTERRUPTED = 28,

  /**
   * @brief This status code is used internally by the implementation when
   *   performing operations on behalf of remote tasks.
   *
   * This is referred to as proxying operations and this status indicates that
   * the operation could not be completed immediately and the proxy is blocking.
   *
   * @par Notes
   * This status will not be returned to the user.
   */
  RTEMS_PROXY_BLOCKING = 29
} rtems_status_code;

/* Generated from spec:/rtems/status/if/code-to-errno */

/**
 * @ingroup RTEMSAPIClassicStatus
 *
 * @brief Maps the RTEMS status code to a POSIX error number.
 *
 * @param status_code is the status code to map.
 *
 * @retval 0 The status code was ::RTEMS_SUCCESSFUL.
 *
 * @retval EBADF The status code was ::RTEMS_INVALID_NUMBER.
 *
 * @retval EBUSY The status code was ::RTEMS_RESOURCE_IN_USE.
 *
 * @retval EINTR The status code was ::RTEMS_INTERRUPTED.
 *
 * @retval EINVAL The status code was ::RTEMS_INVALID_CLOCK,
 *   ::RTEMS_INVALID_NAME, or ::RTEMS_INVALID_NODE.
 *
 * @retval EIO The status code was ::RTEMS_ALREADY_SUSPENDED,
 *   ::RTEMS_CALLED_FROM_ISR, ::RTEMS_ILLEGAL_ON_REMOTE_OBJECT,
 *   ::RTEMS_ILLEGAL_ON_SELF, ::RTEMS_INCORRECT_STATE, ::RTEMS_INTERNAL_ERROR,
 *   ::RTEMS_INVALID_ADDRESS, ::RTEMS_INVALID_ID, ::RTEMS_INVALID_PRIORITY,
 *   ::RTEMS_INVALID_SIZE, ::RTEMS_IO_ERROR, ::RTEMS_MP_NOT_CONFIGURED,
 *   ::RTEMS_NOT_DEFINED, ::RTEMS_OBJECT_WAS_DELETED, ::RTEMS_PROXY_BLOCKING,
 *   ::RTEMS_TASK_EXITTED, or ::RTEMS_TOO_MANY.
 *
 * @retval ENODEV The status code was ::RTEMS_UNSATISFIED.
 *
 * @retval ENOMEM The status code was ::RTEMS_NO_MEMORY.
 *
 * @retval ENOSYS The status code was ::RTEMS_NOT_CONFIGURED or
 *   ::RTEMS_NOT_IMPLEMENTED.
 *
 * @retval EPERM The status code was ::RTEMS_NOT_OWNER_OF_RESOURCE.
 *
 * @retval ETIMEDOUT The status code was ::RTEMS_TIMEOUT.
 */
int rtems_status_code_to_errno( rtems_status_code status_code );

/* Generated from spec:/rtems/status/if/first */

/**
 * @ingroup RTEMSAPIClassicStatus
 *
 * @brief This constant represents the lowest valid value for a Classic API
 *   directive status code.
 */
#define RTEMS_STATUS_CODES_FIRST RTEMS_SUCCESSFUL

/* Generated from spec:/rtems/status/if/is-equal */

/**
 * @ingroup RTEMSAPIClassicStatus
 *
 * @brief Checks if the status codes are equal.
 *
 * @param left_status_code is the left hand side status code.
 *
 * @param right_status_code is the right hand side status code.
 *
 * @return Returns true, if the left hand side status code is equal to the
 *   right hand side status code, otherwise false.
 */
static inline bool rtems_are_statuses_equal(
  rtems_status_code left_status_code,
  rtems_status_code right_status_code
)
{
  return left_status_code == right_status_code;
}

/* Generated from spec:/rtems/status/if/is-successful */

/**
 * @ingroup RTEMSAPIClassicStatus
 *
 * @brief Checks if the status code is ::RTEMS_SUCCESSFUL.
 *
 * @param status_code is the status code.
 *
 * @return Returns true, if the status code is equal to ::RTEMS_SUCCESSFUL,
 *   otherwise false.
 */
static inline bool rtems_is_status_successful( rtems_status_code status_code )
{
  return status_code == RTEMS_SUCCESSFUL;
}

/* Generated from spec:/rtems/status/if/last */

/**
 * @ingroup RTEMSAPIClassicStatus
 *
 * @brief This constant represents the highest valid value for a Classic API
 *   directive status code.
 */
#define RTEMS_STATUS_CODES_LAST RTEMS_PROXY_BLOCKING

/* Generated from spec:/rtems/status/if/text */

/**
 * @ingroup RTEMSAPIClassicStatus
 *
 * @brief Maps the status code to a descriptive text.
 *
 * @param status_code is the status code.
 *
 * The text for each status code is the enumerator constant.
 *
 * @retval "?" The status code is invalid.
 *
 * @return Returns a text describing the status code, if the status code is
 *   valid.
 */
const char *rtems_status_text( rtems_status_code status_code );

#ifdef __cplusplus
}
#endif

#endif /* _RTEMS_RTEMS_STATUS_H */
