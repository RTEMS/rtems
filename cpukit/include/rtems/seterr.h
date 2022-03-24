/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup RTEMSAPISetErrno
 *
 * @brief This header file defines macros to set ``errno`` and return minus
 *   one.
 */

/*
 *  COPYRIGHT (c) 1989-2006.
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

#ifndef _RTEMS_SETERR_H
#define _RTEMS_SETERR_H

#include <errno.h>

/**
 * @defgroup RTEMSAPISetErrno Set Error Number Support
 *
 * @ingroup RTEMSAPI
 *
 * @{
 */

/**
 *  This is a helper macro which will set the variable errno and return
 *  the specified value to the caller.
 *
 *  @param[in] _error is the error code
 *  @param[in] _value is the value to return
 */
#define rtems_set_errno_and_return_value( _error, _value ) \
  do { errno = ( _error ); return ( _value ); } while ( 0 )

/**
 *  This is a helper macro which will set the variable errno and return
 *  -1 to the caller.  This pattern is common to many POSIX methods.
 *
 *  @param[in] _error is the error code
 */
#define rtems_set_errno_and_return_minus_one( _error ) \
  rtems_set_errno_and_return_value( _error, -1 )

/**@}*/
#endif
/* end of include file */
