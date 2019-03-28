/**
 * @file
 *
 * @brief Fatal API.
 */

/*
 *  COPYRIGHT (c) 1989-2011.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 */

#ifndef _RTEMS_FATAL_H
#define _RTEMS_FATAL_H

#include <rtems/score/basedefs.h> /* RTEMS_NO_RETURN */
#include <rtems/extension.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @defgroup ClassicFatal Fatal
 *
 * @ingroup RTEMSAPIClassic
 *
 * @brief The Fatal Manager provides functions for fatal system states and or
 * irrecoverable errors.
 */
/**@{**/

/**
 * @brief Assert context.
 */
typedef struct {
  const char *file;
  int         line;
  const char *function;
  const char *failed_expression;
} rtems_assert_context;

/**
 * @brief Exception frame.
 */
typedef CPU_Exception_frame rtems_exception_frame;

/**
 * @brief Prints the exception frame via printk().
 *
 * @see rtems_fatal() and RTEMS_FATAL_SOURCE_EXCEPTION.
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
 * @param[in] the_error is a 32-bit fatal error code.
 *
 * @see _Terminate().
 */
void rtems_fatal_error_occurred(
  uint32_t   the_error
) RTEMS_NO_RETURN;

/**
 * @brief Terminates the system.
 *
 * @param[in] fatal_source The fatal source.
 * @param[in] error_code The error code.
 *
 * @see _Terminate().
 */
RTEMS_NO_RETURN RTEMS_INLINE_ROUTINE void rtems_fatal(
  rtems_fatal_source fatal_source,
  rtems_fatal_code   error_code
)
{
  _Terminate( fatal_source, error_code );
}

/**
 * @brief Prints the specified message via printk() and terminates the system.
 *
 * @param[in] fmt The message format.
 * @param[in] ... The message parameters.
 *
 * @see _Terminate().
 */
RTEMS_NO_RETURN void rtems_panic(
  const char *fmt, ...
) RTEMS_PRINTFLIKE( 1, 2 );

/**
 * @brief Returns a text for a fatal source.
 *
 * The text for each fatal source is the enumerator constant.
 *
 * @param[in] source is the fatal source.
 *
 * @retval text The fatal source text.
 * @retval "?" The passed fatal source is invalid.
 */
const char *rtems_fatal_source_text( rtems_fatal_source source );

/**
 * @brief Returns a text for an internal error code.
 *
 * The text for each internal error code is the enumerator constant.
 *
 * @param[in] error is the error code.
 *
 * @retval text The error code text.
 * @retval "?" The passed error code is invalid.
 */
const char *rtems_internal_error_text( rtems_fatal_code error );

/** @} */

#ifdef __cplusplus
}
#endif

#endif
/* end of include file */
