/* SPDX-License-Identifier: BSD-2-Clause */

/*
 *  COPYRIGHT (C) 2022 Contemporary Software
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

#ifndef _RTEMS_DEV_PM_PM_TRACE_H
#define _RTEMS_DEV_PM_PM_TRACE_H

#ifndef PM_ENABLE_TRACE
#define PM_ENABLE_TRACE 1
#endif

#include <inttypes.h>
#include <stdarg.h>
#include <stdio.h>

#ifdef __cplusplus
extern "C" {
#endif

#define PM_TRACE_OFF        0
#define PM_TRACE_INFO       1
#define PM_TRACE_DEBUG      2
#define PM_TRACE_LEVEL_MASK 0xff
#define PM_TRACE_IOCTL      (1 << 8)

extern int rtems_pm_trace;

#if PM_ENABLE_TRACE
#define PRINTFLIKE( _format_pos, _ap_pos ) \
  __attribute__(( __format__( __printf__, _format_pos, _ap_pos ) ))
#define pm_trace(_level) \
  (PM_ENABLE_TRACE && \
   ((rtems_pm_trace & PM_TRACE_LEVEL_MASK) >= _level || \
    (rtems_pm_trace & _level) != 0))
static inline int pm_print(const int level, const char* format, ...) PRINTFLIKE(2, 3);
static inline int pm_debug(const char* format, ...) PRINTFLIKE(1, 2);
static int pm_info(const char* format, ...) PRINTFLIKE(1, 2);
static inline int pm_print(const int level, const char* format, ...) {
  int len = 0;
  if (pm_trace(level)) {
    va_list ap;
    va_start(ap, format);
    len = vprintf(format, ap);
    va_end(ap);
  }
  return len;
}
static inline int pm_debug(const char* format, ...) {
  int len = 0;
  if (pm_trace(PM_TRACE_DEBUG)) {
    va_list ap;
    va_start(ap, format);
    len = vprintf(format, ap);
    va_end(ap);
  }
  return len;
}
static inline int pm_info(const char* format, ...) {
  int len = 0;
  if (pm_trace(PM_TRACE_INFO)) {
    va_list ap;
    va_start(ap, format);
    len = vprintf(format, ap);
    va_end(ap);
  }
  return len;
}
#else /* PM_ENABLE_TRACE */
#define pm_trace(_level) (false)
#define pm_print(_level, _fmt, __VA_ARGS__)
#define pm_debug(_fmt, __VA_ARGS__)
#define pm_info(_fmt, __VA_ARGS__)
#endif /* PM_ENABLE_TRACE */

#ifdef __cplusplus
}
#endif

#endif /* _RTEMS_DEV_PM_PM_TRACE_H */
