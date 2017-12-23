/**
 * @file rtems/rtems/status.h
 *
 * @defgroup ClassicStatus Status Codes
 *
 * @ingroup ClassicRTEMS
 * @brief Status Codes Returned from Executive Directives
 *
 * This include file contains the status codes returned from the
 * executive directives.
 */

/* COPYRIGHT (c) 1989-2013.
 * On-Line Applications Research Corporation (OAR).
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rtems.org/license/LICENSE.
 */

#ifndef _RTEMS_RTEMS_STATUS_H
#define _RTEMS_RTEMS_STATUS_H

#include <rtems/score/basedefs.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 *  @defgroup ClassicStatus Status Codes
 *
 *  @ingroup ClassicRTEMS
 *
 *  This encapsulates functionality related to the status codes returned
 *  by Classic API directives.
 */
/**@{*/

/**
 *  @brief Classic API Status
 *
 *  This enumerates the possible status values returned b
 *  Classic API directives.
 */
typedef enum {
  /**
   *  This is the status to indicate successful completion.
   */
  RTEMS_SUCCESSFUL               =  0,
  /**
   *  This is the status to indicate that a thread exited.
   */
  RTEMS_TASK_EXITTED             =  1,
  /**
   *  This is the status to indicate multiprocessing is not configured.
   */
  RTEMS_MP_NOT_CONFIGURED        =  2,
  /**
   *  This is the status to indicate that the object name was invalid.
   */
  RTEMS_INVALID_NAME             =  3,
  /**
   *  This is the status to indicate that the object Id was invalid.
   */
  RTEMS_INVALID_ID               =  4,
  /**
   *  This is the status to indicate you have attempted to create too many
   *  instances of a particular object class.
   */
  RTEMS_TOO_MANY                 =  5,
  /**
   *  This is the status to indicate that a blocking directive timed out.
   */
  RTEMS_TIMEOUT                  =  6,
  /**
   *  This is the status to indicate the the object was deleted
   *  while the task was blocked waiting.
   */
  RTEMS_OBJECT_WAS_DELETED       =  7,
  /**
   *  This is the status to indicate that the specified size was invalid.
   */
  RTEMS_INVALID_SIZE             =  8,
  /**
   *  This is the status to indicate that the specified address is invalid.
   */
  RTEMS_INVALID_ADDRESS          =  9,
  /**
   *  This is the status to indicate that the specified number was invalid.
   */
  RTEMS_INVALID_NUMBER           = 10,
  /**
   *  This is the status to indicate that the item has not been initialized.
   */
  RTEMS_NOT_DEFINED              = 11,
  /**
   *  This is the status to indicate that the object still has
   *  resources in use.
   */
  RTEMS_RESOURCE_IN_USE          = 12,
  /**
   *  This is the status to indicate that the request was not satisfied.
   */
  RTEMS_UNSATISFIED              = 13,
  /**
   *  This is the status to indicate that a thread is in wrong state
   *  was in the wrong execution state for the requested operation.
   */
  RTEMS_INCORRECT_STATE          = 14,
  /**
   *  This is the status to indicate thread was already suspended.
   */
  RTEMS_ALREADY_SUSPENDED        = 15,
  /**
   *  This is the status to indicate that the operation is illegal
   *  on calling thread.
   */
  RTEMS_ILLEGAL_ON_SELF          = 16,
  /**
   *  This is the status to indicate illegal for remote object.
   */
  RTEMS_ILLEGAL_ON_REMOTE_OBJECT = 17,
  /**
   *  This is the status to indicate that the operation should not be
   *  called from from this excecution environment.
   */
  RTEMS_CALLED_FROM_ISR          = 18,
  /**
   *  This is the status to indicate that an invalid thread priority
   *  was provided.
   */
  RTEMS_INVALID_PRIORITY         = 19,
  /**
   *  This is the status to indicate that the specified date/time was invalid.
   */
  RTEMS_INVALID_CLOCK            = 20,
  /**
   *  This is the status to indicate that the specified node Id was invalid.
   */
  RTEMS_INVALID_NODE             = 21,
  /**
   *  This is the status to indicate that the directive was not configured.
   */
  RTEMS_NOT_CONFIGURED           = 22,
  /**
   *  This is the status to indicate that the caller is not the
   *  owner of the resource.
   */
  RTEMS_NOT_OWNER_OF_RESOURCE    = 23,
  /**
   *  This is the status to indicate the the directive or requested
   *  portion of the directive is not implemented.  This is a hint
   *  that you have stumbled across an opportunity to submit code
   *  to the RTEMS Project.
   */
  RTEMS_NOT_IMPLEMENTED          = 24,
  /**
   *  This is the status to indicate that an internal RTEMS inconsistency
   *  was detected.
   */
  RTEMS_INTERNAL_ERROR           = 25,
  /**
   *  This is the status to indicate that the directive attempted to allocate
   *  memory but was unable to do so.
   */
  RTEMS_NO_MEMORY                = 26,
  /**
   *  This is the status to indicate an driver IO error.
   */
  RTEMS_IO_ERROR                 = 27,
  /**
   *  This is the status is used internally to RTEMS when performing
   *  operations on behalf of remote tasks.  This is referred to as
   *  proxying operations and this status indicates that the operation
   *  could not be completed immediately and the "proxy is blocking."
   *
   *  @note This status will @b NOT be returned to the user.
   */
  RTEMS_PROXY_BLOCKING           = 28
} rtems_status_code;

/**
 *  This is the lowest valid value for a Classic API status code.
 */
#define RTEMS_STATUS_CODES_FIRST RTEMS_SUCCESSFUL

/**
 *  This is the highest valid value for a Classic API status code.
 */
#define RTEMS_STATUS_CODES_LAST  RTEMS_PROXY_BLOCKING

/**
 *  @brief Checks if the status code is equal to RTEMS_SUCCESSFUL.
 *
 *  This function returns TRUE if the status code is equal to RTEMS_SUCCESSFUL,
 *  and FALSE otherwise.
 */
RTEMS_INLINE_ROUTINE bool rtems_is_status_successful(
  rtems_status_code code
)
{
  return (code == RTEMS_SUCCESSFUL);
}

/**
 *  @brief Checks if the status code1 is equal to code2.
 *
 *  This function returns TRUE if the status code1 is equal to code2,
 *  and FALSE otherwise.
 */
RTEMS_INLINE_ROUTINE bool rtems_are_statuses_equal(
  rtems_status_code code1,
  rtems_status_code code2
)
{
   return (code1 == code2);
}

/**
 *  @brief RTEMS Status Code to Errno Mapping Function
 *
 *  This function recieves an RTEMS status code and returns an
 *  errno error code. The retval values show the mappings between
 *  rtems_status_codes and errno error codes.
 *
 *  @retval 0 RTEMS_SUCCESSFUL
 *  @retval EIO RTEMS_TASK_EXITED, RTEMS_MP_NOT_CONFIGURED, RTEMS_INVALID_ID,
 *   RTEMS_TOO_MANY, RTEMS_OBJECT_WAS_DELETED, RTEMS_INVALID_SIZE,
 *   RTEMS_INVALID_ADDRESS, RTEMS_NOT_DEFINED, RTEMS_INCORRECT_STATE,
 *   RTEMS_ILLEGAL_ON_SELF, RTEMS_ILLEGAL_ON_REMOTE_OBJECT,
 *   RTEMS_CALLED_FROM_ISR, RTEMS_INVALID_PRIORITY, RTEMS_INTERNAL_ERROR,
 *   RTEMS_IO_ERROR, RTEMS_PROXY_BLOCKING
 *  @retval EINVAL RTEMS_INVALID_NAME, RTEMS_INVALID_CLOCK, RTEMS_INVALID_NODE
 *  @retval ETIMEDOUT RTEMS_TIMEOUT
 *  @retval EBADF RTEMS_INVALID_NUMBER
 *  @retval EBUSY RTEMS_RESOURCE_IN_USE
 *  @retval ENODEV RTEMS_UNSATISFIED
 *  @retval ENOSYS RTEMS_NOT_IMPLEMENTED, RTEMS_NOT_CONFIGURED
 *  @retval ENOMEM RTEMS_NO_MEMORY
 */
int rtems_status_code_to_errno(rtems_status_code sc);

/**
 * @brief Returns a text for a status code.
 *
 * The text for each status code is the enumerator constant.
 *
 * @param[in] code The status code.
 *
 * @retval text The status code text.
 * @retval "?" The passed status code is invalid.
 */
const char *rtems_status_text( rtems_status_code code );

/**@}*/

#ifdef __cplusplus
}
#endif

#endif
/* end of include file */
