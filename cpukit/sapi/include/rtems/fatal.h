/**
 * @file
 *
 * @ingroup ClassicFatal
 *
 * @brief Fatal API.
 */

/*
 *  COPYRIGHT (c) 1989-2011.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 */

#ifndef _RTEMS_FATAL_H
#define _RTEMS_FATAL_H

#include <rtems/score/basedefs.h> /* RTEMS_COMPILER_NO_RETURN_ATTRIBUTE */
#include <rtems/extension.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @defgroup ClassicFatal Fatal
 *
 * @ingroup ClassicRTEMS
 *
 * @brief The Fatal Manager provides functions for fatal system states and or
 * irrecoverable errors.
 *
 * @{
 */

/**
 * @brief Exception frame.
 */
typedef CPU_Exception_frame rtems_exception_frame;

/**
 *  @brief Prints the exception frame via printk().
 *
 *  @see rtems_fatal() and RTEMS_FATAL_SOURCE_EXCEPTION.
 */
static inline void rtems_exception_frame_print(
  const rtems_exception_frame *frame
)
{
  _CPU_Exception_frame_print( frame );
}

/**
 * @brief Invokes the internal error handler with a source of
 * INTERNAL_ERROR_RTEMS_API and is internal set to false.
 *
 * @param[in] the_error A 32-bit fatal error code.
 *
 * @return This function will not return.
 *
 * @see _Internal_error_Occurred().
 */
void rtems_fatal_error_occurred(
  uint32_t   the_error
) RTEMS_COMPILER_NO_RETURN_ATTRIBUTE;

/**
 * @brief Invokes the internal error handler with is internal set to false.
 *
 * @param[in] source The fatal source.
 * @param[in] error The fatal code.
 *
 * @return This function will not return.
 *
 * @see _Internal_error_Occurred().
 */
void rtems_fatal(
  rtems_fatal_source source,
  rtems_fatal_code error
) RTEMS_COMPILER_NO_RETURN_ATTRIBUTE;

/**
 * @brief Returns a description for an internal error code.
 *
 * @param[in] error The error code.
 *
 * @return The error code description or "?" in case the passed error code is
 * invalid.
 */
const char *rtems_internal_error_description( rtems_fatal_code error );

/** @} */

#ifdef __cplusplus
}
#endif

#endif
/* end of include file */
