/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @brief RTEMS Error Reporting
 *
 * Defines and externs for rtems error reporting
 *
 * These routines provide general purpose error reporting.
 * rtems_error reports an error to stderr and allows use of
 * printf style formatting.  A newline is appended to all messages.
 *
 * error_flag can be specified as any of the following:
 *
 *  RTEMS_ERROR_ERRNO       -- include errno text in output
 *  RTEMS_ERROR_PANIC       -- halts local system after output
 *  RTEMS_ERROR_ABORT       -- abort after output
 *
 * It can also include a rtems_status value which can be OR'd
 * with the above flags. *
 *
 * Example 1:
 * @code
 *  #include <rtems.h>
 *  #include <rtems/error.h>
 *  rtems_error(0, "stray interrupt %d", intr);
 * @endcode
 *
 * Example 2:
 * @code
 *        if ((status = rtems_task_create(...)) != RTEMS_SUCCCESSFUL)
 *        {
 *            rtems_error(status | RTEMS_ERROR_ABORT,
 *                        "could not create task");
 *        }
 * @endcode
 *
 * Example 3:
 * @code
 *        if ((fd = open(pathname, O_RDNLY)) < 0)
 *        {
 *            rtems_error(RTEMS_ERROR_ERRNO, "open of '%s' failed", pathname);
 *            goto failed;
 *        }
 * @endcode
 */

/*
 * Copyright (C) 1995 Tony Bennett <tbennett@divnc.com>
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

#ifndef _RTEMS_RTEMS_ERROR_H
#define _RTEMS_RTEMS_ERROR_H

#include <rtems/rtems/status.h>
#include <rtems/fatal.h>

#include <stdarg.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 *  @defgroup ErrorPanicSupport Error And Panic Support
 *
 *  @ingroup libcsupport
 *
 *  @brief Defines and externs for rtems error reporting
 *
 */
typedef Internal_errors_t rtems_error_code_t;

/*
 * rtems_error(), rtems_verror() and rtems_panic() support
 */

#if 0
/* not 16bit-int host clean */
#define RTEMS_ERROR_ERRNO  (1<<((sizeof(rtems_error_code_t) * CHAR_BIT) - 2)) /* hi bit; use 'errno' */
#define RTEMS_ERROR_PANIC  (RTEMS_ERROR_ERRNO / 2)       /* err fatal; no return */
#define RTEMS_ERROR_ABORT  (RTEMS_ERROR_ERRNO / 4)       /* err is fatal; panic */
#else
#define RTEMS_ERROR_ERRNO  (0x40000000) /* hi bit; use 'errno' */
#define RTEMS_ERROR_PANIC  (0x20000000) /* err fatal; no return */
#define RTEMS_ERROR_ABORT  (0x10000000) /* err is fatal; panic */
#endif

#define RTEMS_ERROR_MASK \
  (RTEMS_ERROR_ERRNO | RTEMS_ERROR_ABORT | RTEMS_ERROR_PANIC) /* all */

/**
 *  @brief Report an Error
 *
 *  @param[in] error_code can be specified as any of the following:
 *  RTEMS_ERROR_ERRNO       -- include errno text in output
 *  RTEMS_ERROR_PANIC       -- halts local system after output
 *  RTEMS_ERROR_ABORT       -- abort after output
 *
 *  @param[in] printf_format is a normal printf(3) format string,
 *  with its concommitant arguments
 *
 *  @return the number of characters written.
 */
int   rtems_error(
  rtems_error_code_t error_code,
  const char *printf_format,
  ...
);

/**
 *  @brief Report an Error
 *
 *  @param[in] error_code can be specified as any of the following:
 *  RTEMS_ERROR_ERRNO       -- include errno text in output
 *  RTEMS_ERROR_PANIC       -- halts local system after output
 *  RTEMS_ERROR_ABORT       -- abort after output
 *
 *  @param[in] printf_format is a normal printf(3) format string,
 *  with its concommitant arguments
 *  @param[in] arglist is a varargs list corresponding to
 *  printf_format
 *
 *  @return the number of characters written.
 */
int rtems_verror(
  rtems_error_code_t  error_code,
  const char         *printf_format,
  va_list             arglist
);

extern int rtems_panic_in_progress;

#ifdef __cplusplus
}
#endif


#endif
/* end of include file */
