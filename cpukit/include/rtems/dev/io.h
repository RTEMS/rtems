/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup RTEMSDeviceIO
 *
 * @brief This header file provides the interfaces of the
 *   @ref RTEMSDeviceIO.
 */

/*
 * Copyright (C) 2017, 2023 embedded brains GmbH & Co. KG
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

#ifndef _RTEMS_DEV_IO_H
#define _RTEMS_DEV_IO_H

#include <rtems/score/basedefs.h>

#include <stdarg.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/**
 * @defgroup RTEMSDeviceIO Device I/O Support
 *
 * @ingroup RTEMSDeviceDrivers
 *
 * @brief This group contains the Device I/O Support API and implementation.
 *
 * @{
 */

/**
 * @brief This type defines the put character handler.
 *
 * @param c is the character to put.
 *
 * @param arg is the user-provided argument.
 */
typedef void ( *IO_Put_char )( int c, void *arg );

/**
 * @brief Prints characters using the put character handler according to the
 *   format string.
 *
 * @param put_char is the put character handler.
 *
 * @param arg is the user-provided argument for the put character handler.
 *
 * @param fmt is the printf()-style format string.
 *
 * @param ... is the list of parameters required by the format string.
 *
 * @return Returns the count of put characters.
 */
int _IO_Printf(
  IO_Put_char  put_char,
  void        *arg,
  char const  *fmt,
  ...
) RTEMS_PRINTFLIKE( 3, 4 );

/**
 * @brief Prints characters using the put character handler according to the
 *   format string.
 *
 * @param put_char is the put character handler.
 *
 * @param arg is the user-provided argument for the put character handler.
 *
 * @param fmt is the printf()-style format string.
 *
 * @param ap is the argument list required by the format string.
 *
 * @return Returns the count of put characters.
 */
int _IO_Vprintf(
  IO_Put_char  put_char,
  void        *arg,
  char const  *fmt,
  va_list      ap
);

/**
 * @brief Issues a couple of no-operation instructions.
 *
 * This function may be used to burn a couple of processor cycles with minimum
 * impact on the system bus.  It may be used in busy wait loops.
 */
void _IO_Relax( void );

/** @} */

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* _RTEMS_DEV_IO_H */
