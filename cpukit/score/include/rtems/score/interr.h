/*  interr.h
 *
 *  This include file contains constants and prototypes related
 *  to the Internal Error Handler. 
 *
 *
 *  COPYRIGHT (c) 1989-1999.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 *
 *  $Id$
 */

#ifndef __RTEMS_INTERNAL_ERROR_h
#define __RTEMS_INTERNAL_ERROR_h

#ifdef __cplusplus
extern "C" {
#endif

/*
 *  This type lists the possible sources from which an error
 *  can be reported.
 */

typedef enum {
  INTERNAL_ERROR_CORE,
  INTERNAL_ERROR_RTEMS_API,
  INTERNAL_ERROR_POSIX_API,
  INTERNAL_ERROR_ITRON_API
} Internal_errors_Source;

/*
 *  A list of errors which are generated internally by the executive core.
 */

typedef enum {
  INTERNAL_ERROR_NO_CONFIGURATION_TABLE,
  INTERNAL_ERROR_NO_CPU_TABLE,
  INTERNAL_ERROR_INVALID_WORKSPACE_ADDRESS,
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
  INTERNAL_ERROR_BAD_ATTRIBUTES
} Internal_errors_Core_list;

/*
 *  This type holds the fatal error information.
 */
 
typedef struct {
  Internal_errors_Source  the_source;
  boolean                 is_internal;
  unsigned32              the_error;
} Internal_errors_Information;

/*
 *  When a fatal error occurs, the error information is stored here.
 */

SCORE_EXTERN Internal_errors_Information Internal_errors_What_happened;

/*
 *  _Internal_error_Occurred
 *
 *  DESCRIPTION:
 *
 *  This routine is invoked when the application or the executive itself
 *  determines that a fatal error has occurred.
 */

void volatile _Internal_error_Occurred(
  Internal_errors_Source  the_source,
  boolean                 is_internal,
  unsigned32              the_error
);

#ifdef __cplusplus
}
#endif

#endif
/* end of include file */
