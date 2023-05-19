/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup RTEMSImplClassic
 *
 * @brief This source file contains the implementation of
 *   rtems_status_text().
 */

/*
 * Copyright (c) 2014 embedded brains GmbH & Co. KG
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
