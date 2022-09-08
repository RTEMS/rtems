/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup RTEMSImplClassicStatus
 *
 * @brief This header file provides the implementation interfaces of
 *   the @ref RTEMSImplClassicStatus support.
 */

/*  COPYRIGHT (c) 1989-2008.
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

#ifndef _RTEMS_RTEMS_STATUSIMPL_H
#define _RTEMS_RTEMS_STATUSIMPL_H

#include <rtems/rtems/status.h>
#include <rtems/score/threadimpl.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @defgroup RTEMSImplClassic Classic API
 *
 * @ingroup RTEMSImpl
 *
 * @brief This group contains the Classic API implementation.
 */

/**
 * @defgroup RTEMSImplClassicStatus Directive Status
 *
 * @ingroup RTEMSImplClassic
 *
 * @brief This group contains the implementation to support directive status
 *   codes.
 *
 * @{
 */

static inline rtems_status_code _Status_Get(
  Status_Control status
)
{
  return (rtems_status_code) STATUS_GET_CLASSIC( status );
}

static inline rtems_status_code _Status_Get_after_wait(
  const Thread_Control *executing
)
{
  return _Status_Get( _Thread_Wait_get_status( executing ) );
}

/**@}*/

#ifdef __cplusplus
}
#endif

#endif
/* end of include file */
