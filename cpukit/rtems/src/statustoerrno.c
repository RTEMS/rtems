/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup RTEMSImplClassic
 *
 * @brief This source file contains the implementation of
 *   rtems_status_code_to_errno().
 */

/*  COPYRIGHT (c) 1989-2013.
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

#include <rtems/rtems/statusimpl.h>
#include <errno.h>

static const int status_code_to_errno [RTEMS_STATUS_CODES_LAST + 1] = {
  [RTEMS_SUCCESSFUL]               = 0,
  [RTEMS_TASK_EXITTED]             = EIO,
  [RTEMS_MP_NOT_CONFIGURED]        = EIO,
  [RTEMS_INVALID_NAME]             = EINVAL,
  [RTEMS_INVALID_ID]               = EIO,
  [RTEMS_TOO_MANY]                 = EIO,
  [RTEMS_TIMEOUT]                  = ETIMEDOUT,
  [RTEMS_OBJECT_WAS_DELETED]       = EIO,
  [RTEMS_INVALID_SIZE]             = EIO,
  [RTEMS_INVALID_ADDRESS]          = EIO,
  [RTEMS_INVALID_NUMBER]           = EBADF,
  [RTEMS_NOT_DEFINED]              = EIO,
  [RTEMS_RESOURCE_IN_USE]          = EBUSY,
  [RTEMS_UNSATISFIED]              = ENODEV,
  [RTEMS_INCORRECT_STATE]          = EIO,
  [RTEMS_ALREADY_SUSPENDED]        = EIO,
  [RTEMS_ILLEGAL_ON_SELF]          = EIO,
  [RTEMS_ILLEGAL_ON_REMOTE_OBJECT] = EIO,
  [RTEMS_CALLED_FROM_ISR]          = EIO,
  [RTEMS_INVALID_PRIORITY]         = EIO,
  [RTEMS_INVALID_CLOCK]            = EINVAL,
  [RTEMS_INVALID_NODE]             = EINVAL,
  [RTEMS_NOT_CONFIGURED]           = ENOSYS,
  [RTEMS_NOT_OWNER_OF_RESOURCE]    = EPERM,
  [RTEMS_NOT_IMPLEMENTED]          = ENOSYS,
  [RTEMS_INTERNAL_ERROR]           = EIO,
  [RTEMS_NO_MEMORY]                = ENOMEM,
  [RTEMS_IO_ERROR]                 = EIO,
  [RTEMS_INTERRUPTED]              = EINTR,
  [RTEMS_PROXY_BLOCKING]           = EIO
};

int rtems_status_code_to_errno(rtems_status_code sc)
{
  if (sc == RTEMS_SUCCESSFUL) {
    return 0;
  } else {
    int eno = EINVAL;

    if ((unsigned) sc <= RTEMS_STATUS_CODES_LAST) {
      eno = status_code_to_errno [sc];
    }

    errno = eno;

    return -1;
  }
}
