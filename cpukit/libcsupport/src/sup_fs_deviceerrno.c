/*
 *  IMFS Device Node Handlers
 *
 *  This file contains the set of handlers used to map operations on
 *  IMFS device nodes onto calls to the RTEMS Classic API IO Manager.
 *
 *  COPYRIGHT (c) 1989-2008.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 */

#if HAVE_CONFIG_H
  #include "config.h"
#endif

#include <rtems/deviceio.h>

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
  [RTEMS_PROXY_BLOCKING]           = EIO
};

int rtems_deviceio_errno(rtems_status_code sc)
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
