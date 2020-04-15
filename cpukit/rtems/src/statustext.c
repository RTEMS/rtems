/**
 * @file
 *
 * @ingroup ClassicStatus
 */

/*
 * Copyright (c) 2014 embedded brains GmbH.  All rights reserved.
 *
 *  embedded brains GmbH
 *  Dornierstr. 4
 *  82178 Puchheim
 *  Germany
 *  <rtems@embedded-brains.de>
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rtems.org/license/LICENSE.
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <rtems.h>

static const char *const status_code_text[] = {
  "RTEMS_SUCCESSFUL",
  "RTEMS_TASK_EXITTED",
  "RTEMS_MP_NOT_CONFIGURED",
  "RTEMS_INVALID_NAME",
  "RTEMS_INVALID_ID",
  "RTEMS_TOO_MANY",
  "RTEMS_TIMEOUT",
  "RTEMS_OBJECT_WAS_DELETED",
  "RTEMS_INVALID_SIZE",
  "RTEMS_INVALID_ADDRESS",
  "RTEMS_INVALID_NUMBER",
  "RTEMS_NOT_DEFINED",
  "RTEMS_RESOURCE_IN_USE",
  "RTEMS_UNSATISFIED",
  "RTEMS_INCORRECT_STATE",
  "RTEMS_ALREADY_SUSPENDED",
  "RTEMS_ILLEGAL_ON_SELF",
  "RTEMS_ILLEGAL_ON_REMOTE_OBJECT",
  "RTEMS_CALLED_FROM_ISR",
  "RTEMS_INVALID_PRIORITY",
  "RTEMS_INVALID_CLOCK",
  "RTEMS_INVALID_NODE",
  "RTEMS_NOT_CONFIGURED",
  "RTEMS_NOT_OWNER_OF_RESOURCE",
  "RTEMS_NOT_IMPLEMENTED",
  "RTEMS_INTERNAL_ERROR",
  "RTEMS_NO_MEMORY",
  "RTEMS_IO_ERROR",
  "RTEMS_INTERRUPTED",
  "RTEMS_PROXY_BLOCKING"
};

const char *rtems_status_text( rtems_status_code code )
{
  size_t i = code;
  const char *text = "?";

  if ( i < RTEMS_ARRAY_SIZE( status_code_text ) ) {
    text = status_code_text [i];
  }

  return text;
}
