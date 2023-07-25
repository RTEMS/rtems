/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup POSIXAPI
 *
 * @brief This header file provides interfaces used by the POSIX API
 *   implementation.
 */

/*
 *  COPYRIGHT (c) 1989-2011.
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

#ifndef _RTEMS_POSIX_POSIXAPI_H
#define _RTEMS_POSIX_POSIXAPI_H

#include <rtems/config.h>
#include <rtems/score/assert.h>
#include <rtems/score/objectimpl.h>
#include <rtems/score/threadimpl.h>
#include <rtems/seterr.h>

#include <pthread.h>

/**
 * @defgroup POSIXAPI POSIX API
 *
 * @ingroup RTEMSImpl
 *
 * @brief This group contains definitions and modules which are used to
 *   implement the POSIX APIs supported by RTEMS.
 *
 * @{
 */

extern const int _POSIX_Get_by_name_error_table[ 3 ];

static inline int _POSIX_Get_by_name_error(
  Objects_Get_by_name_error error
)
{
  _Assert( (size_t) error < RTEMS_ARRAY_SIZE( _POSIX_Get_by_name_error_table ) );
  return _POSIX_Get_by_name_error_table[ error ];
}

static inline int _POSIX_Get_error( Status_Control status )
{
  return STATUS_GET_POSIX( status );
}

static inline int _POSIX_Get_error_after_wait(
  const Thread_Control *executing
)
{
  return _POSIX_Get_error( _Thread_Wait_get_status( executing ) );
}

static inline int _POSIX_Zero_or_minus_one_plus_errno(
  Status_Control status
)
{
  if ( status == STATUS_SUCCESSFUL ) {
    return 0;
  }

  rtems_set_errno_and_return_minus_one( _POSIX_Get_error( status ) );
}

/*
 * See also The Open Group Base Specifications Issue 7, IEEE Std 1003.1-2008,
 * 2016 Edition, subsection 2.9.9, Synchronization Object Copies and
 * Alternative Mappings.
 *
 * http://pubs.opengroup.org/onlinepubs/9699919799/functions/V2_chap02.html#tag_15_09_09
 */
static inline bool _POSIX_Is_valid_pshared( int pshared )
{
  return pshared == PTHREAD_PROCESS_PRIVATE ||
    pshared == PTHREAD_PROCESS_SHARED;
}

/** @} */

#endif
/* end of include file */
