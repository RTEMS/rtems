/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup RTEMSScoreTimestamp
 *
 * @brief This header file provides interfaces of the
 *   @ref RTEMSScoreTimestamp which are used by the implementation and the
 *   @ref RTEMSImplApplConfig.
 */

/*
 *  COPYRIGHT (c) 1989-2009.
 *  On-Line Applications Research Corporation (OAR).
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

#ifndef _RTEMS_SCORE_TIMESTAMP_H
#define _RTEMS_SCORE_TIMESTAMP_H

/**
 * @defgroup RTEMSScoreTimestamp Timestamp Handler
 *
 * @ingroup RTEMSScore
 *
 * @brief This group contains the Timestamp Handler implementation.
 *
 * This handler encapsulates functionality related to manipulating
 * SuperCore Timestamps.  SuperCore Timestamps may be used to
 * represent time of day, uptime, or intervals.
 *
 * The key attribute of the SuperCore Timestamp handler is that it
 * is a completely opaque handler.  There can be multiple implementations
 * of the required functionality and with a recompile, RTEMS can use
 * any implementation.  It is intended to be a simple wrapper.
 *
 * This handler can be implemented as either struct timespec or
 * unsigned64 bit numbers.  The use of a wrapper class allows the
 * the implementation of timestamps to change on a per architecture
 * basis.  This is an important option as the performance of this
 * handler is critical.
 *
 * @{
 */

#include <rtems/score/timespec.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 *   Define the Timestamp control type.
 */
typedef int64_t Timestamp_Control;

#ifdef __cplusplus
}
#endif

/** @} */

#endif
/* end of include file */
