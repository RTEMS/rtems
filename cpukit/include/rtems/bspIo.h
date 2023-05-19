/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup RTEMSAPIKernelCharIO
 *
 * @brief This header file provides the kernel character input/output support
 *   API.
 */

/*
 * Copyright (C) 2020, 2021 embedded brains GmbH & Co. KG
 * Copyright (C) 2015 On-Line Applications Research Corporation (OAR)
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

/* Generated from spec:/rtems/io/if/header-3 */

#ifndef _RTEMS_BSPIO_H
#define _RTEMS_BSPIO_H

#include <stdarg.h>
#include <rtems/score/basedefs.h>

#ifdef __cplusplus
extern "C" {
#endif

/* Generated from spec:/rtems/io/if/group-3 */

/**
 * @defgroup RTEMSAPIKernelCharIO Kernel Character I/O Support
 *
 * @ingroup RTEMSAPIClassicIO
 *
 * @brief The kernel character input/output support is an extension of the @ref
 *   RTEMSAPIClassicIO to output characters to the kernel character output
 *   device and receive characters from the kernel character input device using
 *   a polled and non-blocking implementation.
 *
 * The directives may be used to print debug and test information.  The kernel
 * character input/output support should work even if no Console Driver is
 * configured, see @ref CONFIGURE_APPLICATION_NEEDS_CONSOLE_DRIVER.  The kernel
 * character input and output device is provided by the BSP. Applications may
 * change the device.
 */

/* Generated from spec:/rtems/io/if/bsp-output-char-function-type */

/**
 * @ingroup RTEMSAPIKernelCharIO
 *
 * @brief Polled character output functions shall have this type.
 */
typedef void ( *BSP_output_char_function_type )( char );

/* Generated from spec:/rtems/io/if/bsp-output-char */

/**
 * @ingroup RTEMSAPIKernelCharIO
 *
 * @brief This function pointer references the kernel character output
 *   implementation.
 *
 * This function pointer shall never be NULL.  It shall be provided by the BSP
 * and statically initialized.  The referenced function shall output exactly
 * the character specified by the parameter.  In particular, it shall not
 * perform character translations, for example ``NL`` to ``CR`` followed by
 * ``NR``.  The function shall not block.
 */
extern BSP_output_char_function_type BSP_output_char;

/* Generated from spec:/rtems/io/if/putc */

/**
 * @ingroup RTEMSAPIKernelCharIO
 *
 * @brief Outputs the character to the kernel character output device.
 *
 * @param c is the character to output.
 *
 * The directive outputs the character specified by ``c`` to the kernel
 * character output device using the polled character output implementation
 * provided by #BSP_output_char.  The directive performs a character
 * translation from ``NL`` to ``CR`` followed by ``NR``.
 *
 * If the kernel character output device is concurrently accessed, then
 * interleaved output may occur.
 *
 * @par Constraints
 * @parblock
 * The following constraints apply to this directive:
 *
 * * The directive may be called from within any runtime context.
 *
 * * The directive will not cause the calling task to be preempted.
 * @endparblock
 */
void rtems_putc( char c );

/* Generated from spec:/rtems/io/if/put-char */

/**
 * @ingroup RTEMSAPIKernelCharIO
 *
 * @brief Puts the character using rtems_putc()
 *
 * @param c is the character to output.
 *
 * @param unused is an unused argument.
 *
 * @par Notes
 * The directive is provided to support the RTEMS Testing Framework.
 *
 * @par Constraints
 * @parblock
 * The following constraints apply to this directive:
 *
 * * The directive may be called from within any runtime context.
 *
 * * The directive will not cause the calling task to be preempted.
 * @endparblock
 */
void rtems_put_char( int c, void *unused );

/* Generated from spec:/rtems/io/if/putk */

/**
 * @ingroup RTEMSAPIKernelCharIO
 *
 * @brief Outputs the characters of the string and a newline character to the
 *   kernel character output device.
 *
 * @param s is the string to output.
 *
 * @return Returns the number of characters output to the kernel character
 *   output device.
 *
 * @par Notes
 * @parblock
 * The directive may be used to print debug and test information.  It uses
 * rtems_putc() to output the characters.  This directive performs a character
 * translation from ``NL`` to ``CR`` followed by ``NR``.
 *
 * If the kernel character output device is concurrently accessed, then
 * interleaved output may occur.
 * @endparblock
 *
 * @par Constraints
 * @parblock
 * The following constraints apply to this directive:
 *
 * * The directive may be called from within any runtime context.
 *
 * * The directive will not cause the calling task to be preempted.
 * @endparblock
 */
int putk( const char *s );

/* Generated from spec:/rtems/io/if/printk */

/**
 * @ingroup RTEMSAPIKernelCharIO
 *
 * @brief Outputs the characters defined by the format string and the arguments
 *   to the kernel character output device.
 *
 * @param fmt is a printf()-style format string.
 *
 * @param ... is a list of optional parameters required by the format string.
 *
 * @return Returns the number of characters output to the kernel character
 *   output device.
 *
 * @par Notes
 * @parblock
 * The directive may be used to print debug and test information.  It uses
 * rtems_putc() to output the characters.  This directive performs a character
 * translation from ``NL`` to ``CR`` followed by ``NR``.
 *
 * If the kernel character output device is concurrently accessed, then
 * interleaved output may occur.
 * @endparblock
 *
 * @par Constraints
 * @parblock
 * The following constraints apply to this directive:
 *
 * * The directive may be called from within any runtime context.
 *
 * * The directive will not cause the calling task to be preempted.
 *
 * * Formatting of floating point numbers is not supported.
 * @endparblock
 */
RTEMS_PRINTFLIKE( 1, 2 ) int printk( const char *fmt, ... );

/* Generated from spec:/rtems/io/if/vprintk */

/**
 * @ingroup RTEMSAPIKernelCharIO
 *
 * @brief Outputs the characters defined by the format string and the variable
 *   argument list to the kernel character output device.
 *
 * @param fmt is a printf()-style format string.
 *
 * @param ap is the variable argument list required by the format string.
 *
 * @return Returns the number of characters output to the kernel character
 *   output device.
 *
 * @par Notes
 * @parblock
 * The directive may be used to print debug and test information.  It uses
 * rtems_putc() to output the characters.  This directive performs a character
 * translation from ``NL`` to ``CR`` followed by ``NR``.
 *
 * If the kernel character output device is concurrently accessed, then
 * interleaved output may occur.
 * @endparblock
 *
 * @par Constraints
 * @parblock
 * The following constraints apply to this directive:
 *
 * * The directive may be called from within any runtime context.
 *
 * * The directive will not cause the calling task to be preempted.
 *
 * * Formatting of floating point numbers is not supported.
 * @endparblock
 */
int vprintk( const char *fmt, va_list ap );

/* Generated from spec:/rtems/io/if/printk-printer */

/**
 * @ingroup RTEMSAPIKernelCharIO
 *
 * @brief Outputs the characters defined by the format string and the variable
 *   argument list to the kernel character output device.
 *
 * @param unused is an unused argument.
 *
 * @param fmt is a printf()-style format string.
 *
 * @param ap is the variable argument list required by the format string.
 *
 * @return Returns the number of characters output to the kernel character
 *   output device.
 *
 * @par Notes
 * @parblock
 * The directive may be used to print debug and test information.  It uses
 * rtems_putc() to output the characters.  This directive performs a character
 * translation from ``NL`` to ``CR`` followed by ``NR``.
 *
 * If the kernel character output device is concurrently accessed, then
 * interleaved output may occur.
 * @endparblock
 *
 * @par Constraints
 * @parblock
 * The following constraints apply to this directive:
 *
 * * The directive may be called from within any runtime context.
 *
 * * The directive will not cause the calling task to be preempted.
 *
 * * Formatting of floating point numbers is not supported.
 * @endparblock
 */
int rtems_printk_printer( void *unused, const char *fmt, va_list ap );

/* Generated from spec:/rtems/io/if/bsp-polling-getchar-function-type */

/**
 * @ingroup RTEMSAPIKernelCharIO
 *
 * @brief Polled character input functions shall have this type.
 */
typedef int (* BSP_polling_getchar_function_type )( void );

/* Generated from spec:/rtems/io/if/bsp-poll-char */

/**
 * @ingroup RTEMSAPIKernelCharIO
 *
 * @brief This function pointer may reference the kernel character input
 *   implementation.
 *
 * This function pointer may be NULL.  It may reference a function provided by
 * the BSP.  Referenced functions shall dequeue the least recently received
 * character from the device and return it as an unsigned character.  If no
 * character is enqueued on the device, then the function shall immediately
 * return the value minus one.
 */
extern BSP_polling_getchar_function_type BSP_poll_char;

/* Generated from spec:/rtems/io/if/getchark */

/**
 * @ingroup RTEMSAPIKernelCharIO
 *
 * @brief Tries to dequeue a character from the kernel character input device.
 *
 * The directive tries to dequeue a character from the kernel character input
 * device using the polled character input implementation referenced by
 * #BSP_poll_char if it is available.
 *
 * @retval -1 The #BSP_poll_char pointer was equal to NULL.
 *
 * @retval -1 There was no character enqueued on the kernel character input
 *   device.
 *
 * @return Returns the character least recently enqueued on the kernel
 *   character input device as an unsigned character value.
 *
 * @par Constraints
 * @parblock
 * The following constraints apply to this directive:
 *
 * * The directive may be called from within any runtime context.
 *
 * * The directive will not cause the calling task to be preempted.
 * @endparblock
 */
int getchark( void );

#ifdef __cplusplus
}
#endif

#endif /* _RTEMS_BSPIO_H */
