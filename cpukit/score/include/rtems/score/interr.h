/**
 *  @file  rtems/score/interr.h
 *
 *  This include file contains constants and prototypes related
 *  to the Internal Error Handler.
 */

/*
 *  COPYRIGHT (c) 1989-2009.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 */

#ifndef _RTEMS_SCORE_INTERR_H
#define _RTEMS_SCORE_INTERR_H

#include <stdbool.h>
#include <stdint.h>

#include <rtems/system.h>

/**
 *  @defgroup ScoreIntErr Internal Error Handler
 *
 *  @ingroup Score
 *
 *  This handler encapsulates functionality which provides the foundation
 *  Semaphore services used in all of the APIs supported by RTEMS.
 */
/**@{*/

#ifdef __cplusplus
extern "C" {
#endif

/**
 *  This type lists the possible sources from which an error
 *  can be reported.
 */
typedef enum {
  INTERNAL_ERROR_CORE,
  INTERNAL_ERROR_RTEMS_API,
  INTERNAL_ERROR_POSIX_API
} Internal_errors_Source;

/**
 *  A list of errors which are generated internally by the executive core.
 */
typedef enum {
  INTERNAL_ERROR_NO_CONFIGURATION_TABLE,
  INTERNAL_ERROR_NO_CPU_TABLE,
  INTERNAL_ERROR_TOO_LITTLE_WORKSPACE,
  INTERNAL_ERROR_WORKSPACE_ALLOCATION,
  INTERNAL_ERROR_INTERRUPT_STACK_TOO_SMALL,
  INTERNAL_ERROR_THREAD_EXITTED,
  INTERNAL_ERROR_INCONSISTENT_MP_INFORMATION,
  INTERNAL_ERROR_INVALID_NODE,
  INTERNAL_ERROR_NO_MPCI,
  INTERNAL_ERROR_BAD_PACKET,
  INTERNAL_ERROR_OUT_OF_PACKETS,
  INTERNAL_ERROR_OUT_OF_GLOBAL_OBJECTS,
  INTERNAL_ERROR_OUT_OF_PROXIES,
  INTERNAL_ERROR_INVALID_GLOBAL_ID,
  INTERNAL_ERROR_BAD_STACK_HOOK,
  INTERNAL_ERROR_BAD_ATTRIBUTES,
  INTERNAL_ERROR_IMPLEMENTATION_KEY_CREATE_INCONSISTENCY,
  INTERNAL_ERROR_IMPLEMENTATION_BLOCKING_OPERATION_CANCEL,
  INTERNAL_ERROR_MUTEX_OBTAIN_FROM_BAD_STATE,
  INTERNAL_ERROR_UNLIMITED_AND_MAXIMUM_IS_0,
  INTERNAL_ERROR_SHUTDOWN_WHEN_NOT_UP,
  INTERNAL_ERROR_GXX_KEY_ADD_FAILED,
  INTERNAL_ERROR_GXX_MUTEX_INIT_FAILED
} Internal_errors_Core_list;

typedef uint32_t Internal_errors_t;

/**
 *  This type holds the fatal error information.
 */
typedef struct {
  /** This is the source of the error. */
  Internal_errors_Source  the_source;
  /** This indicates if the error is internal of external. */
  bool                    is_internal;
  /** This is the error code. */
  Internal_errors_t       the_error;
} Internal_errors_Information;

/**
 *  When a fatal error occurs, the error information is stored here.
 */
SCORE_EXTERN Internal_errors_Information _Internal_errors_What_happened;

/**
 * @brief  Internal error Occurred
 *
 *  This routine is invoked when the application or the executive itself
 *  determines that a fatal error has occurred.
 */
void _Internal_error_Occurred(
  Internal_errors_Source  the_source,
  bool                    is_internal,
  Internal_errors_t       the_error
) RTEMS_COMPILER_NO_RETURN_ATTRIBUTE;

#ifdef __cplusplus
}
#endif

/**@}*/

#endif
/* end of include file */
