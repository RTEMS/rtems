/**
 * @file
 *
 * @ingroup RTEMSScoreThread
 */

/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (C) 2019, 2020 embedded brains GmbH
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

#include <rtems/score/thread.h>
#include <rtems/score/objectimpl.h>
#include <rtems/score/wkspace.h>

static void _Thread_Extend_information( Objects_Information *base )
{
  Thread_Information *information;
  Objects_Maximum     block;

  information = (Thread_Information *) base;
  block = _Objects_Extend_information( &information->Objects );

  if ( block > 0 ) {
    void *new_heads;

    new_heads = _Freechain_Extend(
      &information->Thread_queue_heads.Free,
      _Workspace_Allocate,
      _Objects_Extend_size( &information->Objects ),
      _Thread_queue_Heads_size
    );

    if ( new_heads == NULL ) {
      _Objects_Free_objects_block( &information->Objects, block );
    }
  }
}

Objects_Control *_Thread_Allocate_unlimited( Objects_Information *information )
{
  return _Objects_Allocate_with_extend(
    information,
    _Thread_Extend_information
  );
}
