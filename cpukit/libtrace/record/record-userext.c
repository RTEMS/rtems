/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (C) 2018, 2019 embedded brains GmbH
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

#if HAVE_CONFIG_H
#include "config.h"
#endif

#include <rtems/record.h>
#include <rtems/score/threadimpl.h>

bool _Record_Thread_create(
  struct _Thread_Control *executing,
  struct _Thread_Control *created
)
{
  rtems_record_data data;
  char              name[ 2 * THREAD_DEFAULT_MAXIMUM_NAME_SIZE ];
  rtems_record_item items[ 1 + sizeof( name ) / sizeof( data ) ];
  size_t            len;
  size_t            used;

  items[ 0 ].event = RTEMS_RECORD_THREAD_CREATE;
  items[ 0 ].data = created->Object.id;

  len = _Thread_Get_name( created, name, sizeof( name ) );
  used = _Record_String_to_items(
    RTEMS_RECORD_THREAD_NAME,
    name,
    len,
    &items[ 1 ],
    RTEMS_ARRAY_SIZE( items ) - 1
  );
  rtems_record_produce_n( items, used + 1 );

  return true;
}

void _Record_Thread_start(
  struct _Thread_Control *executing,
  struct _Thread_Control *started
)
{
  rtems_record_produce(
    RTEMS_RECORD_THREAD_START,
    started->Object.id
  );
}

void _Record_Thread_restart(
  struct _Thread_Control *executing,
  struct _Thread_Control *restarted
)
{
  rtems_record_produce(
    RTEMS_RECORD_THREAD_RESTART,
    restarted->Object.id
  );
}

void _Record_Thread_delete(
  struct _Thread_Control *executing,
  struct _Thread_Control *deleted
)
{
  rtems_record_produce(
    RTEMS_RECORD_THREAD_DELETE,
    deleted->Object.id
  );
}

void _Record_Thread_switch(
  struct _Thread_Control *executing,
  struct _Thread_Control *heir
)
{
  rtems_record_item items[ 3 ];

  items[ 0 ].event = RTEMS_RECORD_THREAD_SWITCH_OUT;
  items[ 0 ].data = executing->Object.id;
  items[ 1 ].event = RTEMS_RECORD_THREAD_STACK_CURRENT;
  items[ 1 ].data =
#if defined(__GNUC__)
    (uintptr_t) __builtin_frame_address( 0 )
      - (uintptr_t) executing->Start.Initial_stack.area;
#else
    0;
#endif
  items[ 2 ].event = RTEMS_RECORD_THREAD_SWITCH_IN;
  items[ 2 ].data = heir->Object.id;
  rtems_record_produce_n( items, RTEMS_ARRAY_SIZE( items ) );
}

void _Record_Thread_begin( struct _Thread_Control *executing )
{
  rtems_record_produce(
    RTEMS_RECORD_THREAD_BEGIN,
    executing->Object.id
  );
}

void _Record_Thread_exitted( struct _Thread_Control *executing )
{
  rtems_record_produce(
    RTEMS_RECORD_THREAD_EXITTED,
    executing->Object.id
  );
}

void _Record_Thread_terminate( struct _Thread_Control *executing )
{
  rtems_record_produce(
    RTEMS_RECORD_THREAD_TERMINATE,
    executing->Object.id
  );
}
