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
#include <rtems/config.h>
#include <rtems/score/assert.h>

#include <string.h>

RTEMS_STATIC_ASSERT( RTEMS_RECORD_USER_0 == 512, RTEMS_RECORD_USER_0 );

RTEMS_STATIC_ASSERT( RTEMS_RECORD_LAST == 1023, RTEMS_RECORD_LAST );

RTEMS_STATIC_ASSERT(
  RTEMS_RECORD_LAST + 1 == ( 1 << RTEMS_RECORD_EVENT_BITS ),
  RTEMS_RECORD_EVENT_BITS
);

void rtems_record_produce( rtems_record_event event, rtems_record_data data )
{
  rtems_record_context context;

  rtems_record_prepare( &context );
  rtems_record_add( &context, event, data );
  rtems_record_commit( &context );
}

void rtems_record_produce_2(
  rtems_record_event event_0,
  rtems_record_data  data_0,
  rtems_record_event event_1,
  rtems_record_data  data_1
)
{
  rtems_record_context context;

  rtems_record_prepare( &context );
  rtems_record_add( &context, event_0, data_0 );
  rtems_record_add( &context, event_1, data_1 );
  rtems_record_commit( &context );
}

void rtems_record_produce_n(
  const rtems_record_item *items,
  size_t                   n
)
{
  rtems_record_context context;

  _Assert( n > 0 );

  rtems_record_prepare( &context );

  do {
    rtems_record_add( &context, items->event, items->data );
    ++items;
    --n;
  } while ( n > 0 );

  rtems_record_commit( &context );
}

size_t _Record_String_to_items(
  rtems_record_event  event,
  const char         *str,
  size_t              len,
  rtems_record_item  *items,
  size_t              item_count
)
{
  size_t s;
  size_t i;

  s = 0;
  i = 0;

  while ( s < len && i < item_count ) {
    rtems_record_data data;
    size_t            k;

    data = 0;

    for ( k = 0; s < len && k < sizeof( data ); ++k ) {
      rtems_record_data c;

      c = (unsigned char) str[ s ];
      data |= c << ( k * 8 );
      ++s;
    }

    items[ i ].event = event;
    items[ i ].data = data;
    ++i;
  }

  return i;
}

void _Record_Drain(
  Record_Control             *control,
  uint32_t                    cpu_index,
  rtems_record_drain_visitor  visitor,
  void                       *arg
)
{
  unsigned int tail;
  unsigned int head;

  tail = _Record_Tail( control );
  head = _Atomic_Load_uint( &control->head, ATOMIC_ORDER_ACQUIRE );

  if ( tail == head ) {
    return;
  }

  control->tail = head;

  control->Header[ 0 ].event = RTEMS_RECORD_PROCESSOR;
  control->Header[ 0 ].data = cpu_index;
  control->Header[ 1 ].event = RTEMS_RECORD_PER_CPU_TAIL;
  control->Header[ 1 ].data = tail;
  control->Header[ 2 ].event = RTEMS_RECORD_PER_CPU_HEAD;
  control->Header[ 2 ].data = head;
  ( *visitor )( control->Header, RTEMS_ARRAY_SIZE( control->Header ), arg );

  if ( _Record_Is_overflow( control, tail, head ) ) {
    tail = head + 1;
  }

  tail = _Record_Index( control, tail );
  head = _Record_Index( control, head );

  if ( tail < head ) {
    ( *visitor )( &control->Items[ tail ], head - tail, arg );
  } else {
    ( *visitor )( &control->Items[ tail ], control->mask + 1 - tail, arg );

    if ( head > 0 ) {
      ( *visitor )( &control->Items[ 0 ], head, arg );
    }
  }
}

void rtems_record_drain( rtems_record_drain_visitor visitor, void *arg )
{
  uint32_t cpu_max;
  uint32_t cpu_index;

  cpu_max = rtems_configuration_get_maximum_processors();

  for ( cpu_index = 0; cpu_index < cpu_max; ++cpu_index ) {
    Per_CPU_Control *cpu;

    cpu = _Per_CPU_Get_by_index( cpu_index );
    _Record_Drain( cpu->record, cpu_index, visitor, arg );
  }
}
