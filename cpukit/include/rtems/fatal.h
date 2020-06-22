/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @brief This header file defines the Fatal Error Manager API.
 */

/*
 * Copyright (C) 2020, 2021 embedded brains GmbH (http://www.embedded-brains.de)
 * Copyright (C) 1988, 2008 On-Line Applications Research Corporation (OAR)
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

/* Generated from spec:/rtems/fatal/if/header */

#ifndef _RTEMS_FATAL_H
#define _RTEMS_FATAL_H

#include <stdint.h>
#include <rtems/extension.h>
#include <rtems/score/basedefs.h>
#include <rtems/score/cpu.h>
#include <rtems/score/interr.h>

#ifdef __cplusplus
extern "C" {
#endif

/* Generated from spec:/rtems/fatal/if/group */

/**
 * @defgroup RTEMSAPIClassicFatal Fatal Error Manager
 *
 * @ingroup RTEMSAPIClassic
 *
 * @brief The Fatal Error Manager processes all fatal or irrecoverable errors
 *   and other sources of system termination (for example after exit()).  Fatal
 *   errors are identified by the fatal source and code pair.
 */

/* Generated from spec:/rtems/fatal/if/assert-context */

/**
 * @ingroup RTEMSAPIClassicFatal
 *
 * @brief This structure provides the context in which an assertion failed.
 */
typedef struct {
  /**
   * @brief This member provides the file name of the source code file containing
   *   the failed assertion statement.
   */
  const char *file;

  /**
   * @brief This member provides the line number in the source code file
   *   containing the failed assertion statement.
   */
  int line;

  /**
   * @brief This member provides the function name containing the failed
   *   assertion statement.
   */
  const char *function;

  /**
   * @brief This member provides the expression of the failed assertion
   *   statement.
   */
  const char *failed_expression;
} rtems_assert_context;

/* Generated from spec:/rtems/fatal/if/exception-frame */

/**
 * @ingroup RTEMSAPIClassicFatal
 *
 * @brief This structure represents an architecture-dependent exception frame.
 */
typedef CPU_Exception_frame rtems_exception_frame;

/* Generated from spec:/rtems/fatal/if/fatal */

/**
 * @ingroup RTEMSAPIClassicFatal
 *
 * @brief Invokes the fatal error handler.
 *
 * @param fatal_source is the fatal source.
 *
 * @param fatal_code is the fatal code.
 *
 * This directive processes fatal errors.  The fatal source is set to the value
 * of the ``fatal_source`` parameter.  The fatal code is set to the value of
 * the ``fatal_code`` parameter.
 *
 * @par Constraints
 * @parblock
 * The following constraints apply to this directive:
 *
 * * The directive may be called from within any runtime context.
 *
 * * The directive will not return to the caller.
 *
 * * The directive invokes the fatal error extensions in extension forward
 *   order.
 *
 * * The directive does not invoke handlers registered by atexit() or
 *   on_exit().
 *
 * * The directive may terminate the system.
 * @endparblock
 */
RTEMS_NO_RETURN static inline void rtems_fatal(
  rtems_fatal_source fatal_source,
  rtems_fatal_code   fatal_code
)
{
  _Terminate( fatal_source, fatal_code );
}

/* Generated from spec:/rtems/fatal/if/panic */

/**
 * @ingroup RTEMSAPIClassicFatal
 *
 * @brief Prints the message and invokes the fatal error handler.
 *
 * @param fmt is the message format.
 *
 * @param ... is a list of optional parameters required by the message format.
 *
 * This directive prints a message via printk() specified by the ``fmt``
 * parameter and optional parameters and then invokes the fatal error handler.
 * The fatal source is set to RTEMS_FATAL_SOURCE_PANIC.  The fatal code is set
 * to the value of the ``fmt`` parameter value.
 *
 * @par Constraints
 * @parblock
 * The following constraints apply to this directive:
 *
 * * The directive may be called from within any runtime context.
 *
 * * The directive will not return to the caller.
 *
 * * The directive invokes the fatal error extensions in extension forward
 *   order.
 *
 * * The directive does not invoke handlers registered by atexit() or
 *   on_exit().
 *
 * * The directive may terminate the system.
 * @endparblock
 */
RTEMS_NO_RETURN RTEMS_PRINTFLIKE( 1, 2 ) void rtems_panic(
  const char *fmt,
  ...
);

/* Generated from spec:/rtems/fatal/if/exception-frame-print */

/**
 * @ingroup RTEMSAPIClassicFatal
 *
 * @brief Prints the exception frame.
 *
 * @param frame is the reference to the exception frame to print.
 *
 * The exception frame is printed in an architecture-dependent format using
 * printk().
 */
static inline void rtems_exception_frame_print(
  const rtems_exception_frame *frame
)
{
  _CPU_Exception_frame_print( frame );
}

/* Generated from spec:/rtems/fatal/if/source-text */

/**
 * @ingroup RTEMSAPIClassicFatal
 *
 * @brief Returns a descriptive text for the fatal source.
 *
 * @param fatal_source is the fatal source.
 *
 * @retval "?" The ``fatal_source`` parameter value was not a fatal source.
 *
 * @return Returns a descriptive text for the fatal source.  The text for the
 *   fatal source is the enumerator constant name.
 *
 * @par Constraints
 * @parblock
 * The following constraints apply to this directive:
 *
 * * The directive may be called from within any runtime context.
 * @endparblock
 */
const char *rtems_fatal_source_text( rtems_fatal_source fatal_source );

/* Generated from spec:/rtems/fatal/if/internal-error-text */

/**
 * @ingroup RTEMSAPIClassicFatal
 *
 * @brief Returns a descriptive text for the internal error code.
 *
 * @param internal_error_code is the internal error code.
 *
 * @retval "?" The ``internal_error_code`` parameter value was not an internal
 *   error code.
 *
 * @return Returns a descriptive text for the internal error code.  The text
 *   for the internal error code is the enumerator constant name.
 *
 * @par Constraints
 * @parblock
 * The following constraints apply to this directive:
 *
 * * The directive may be called from within any runtime context.
 * @endparblock
 */
const char *rtems_internal_error_text( rtems_fatal_code internal_error_code );

/* Generated from spec:/rtems/fatal/if/error-occurred */

/**
 * @ingroup RTEMSAPIClassicFatal
 *
 * @brief Invokes the fatal error handler.
 *
 * @param fatal_code is the fatal code.
 *
 * This directive processes fatal errors.  The fatal source is set to
 * INTERNAL_ERROR_RTEMS_API.  The fatal code is set to the value of the
 * ``fatal_code`` parameter.
 *
 * @par Notes
 * This directive is deprecated and should not be used in new code.  It is
 * recommended to not use this directive since error locations cannot be
 * uniquely identified.  A recommended alternative directive is rtems_fatal().
 *
 * @par Constraints
 * @parblock
 * The following constraints apply to this directive:
 *
 * * The directive may be called from within any runtime context.
 *
 * * The directive will not return to the caller.
 *
 * * The directive invokes the fatal error extensions in extension forward
 *   order.
 *
 * * The directive does not invoke handlers registered by atexit() or
 *   on_exit().
 *
 * * The directive may terminate the system.
 * @endparblock
 */
RTEMS_NO_RETURN void rtems_fatal_error_occurred( uint32_t fatal_code );

#ifdef __cplusplus
}
#endif

#endif /* _RTEMS_FATAL_H */
