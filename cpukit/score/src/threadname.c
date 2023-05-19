/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup RTEMSScoreThread
 *
 * @brief This source file contains the implementation of
 *   _Thread_Set_name() and _Thread_Get_name().
 */

/*
 * Copyright (c) 2017 embedded brains GmbH & Co. KG
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

#include <rtems/score/threadimpl.h>

#include <string.h>

Status_Control _Thread_Set_name(
  Thread_Control *the_thread,
  const char     *name
)
{
  size_t length;

  length = strlcpy(
    RTEMS_DECONST( char *, the_thread->Join_queue.Queue.name ),
    name,
    _Thread_Maximum_name_size
  );

  if ( length >= _Thread_Maximum_name_size ) {
    return STATUS_RESULT_TOO_LARGE;
  }

  return STATUS_SUCCESSFUL;
}

size_t _Thread_Get_name(
  const Thread_Control *the_thread,
  char                 *buffer,
  size_t                buffer_size
)
{
  const char *name;

  name = the_thread->Join_queue.Queue.name;

  if ( name != NULL && name[ 0 ] != '\0' ) {
    return strlcpy( buffer, name, buffer_size );
  } else {
    return _Objects_Name_to_string(
      the_thread->Object.name,
      false,
      buffer,
      buffer_size
    );
  }
}
