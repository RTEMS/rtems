/* SPDX-License-Identifier: BSD-2-Clause */

/*
 * Copyright (C) 2020, 2024 embedded brains GmbH & Co. KG
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

#include <rtems/recorddump.h>
#include <rtems/score/threadimpl.h>

typedef struct {
  rtems_record_dump_chunk  chunk;
  void                    *arg;
} dump_context;

static void dump_chunk( dump_context *ctx, const void *data, size_t length )
{
  ( *ctx->chunk )( ctx->arg, data, length );
}

static bool thread_names_visitor( rtems_tcb *tcb, void *arg )
{
  dump_context      *ctx;
  char               name[ 2 * THREAD_DEFAULT_MAXIMUM_NAME_SIZE ];
  size_t             n;
  size_t             i;
  rtems_record_item  item;
  rtems_record_data  data;

  ctx = arg;
  item.event = RTEMS_RECORD_THREAD_ID;
  item.data = tcb->Object.id;
  dump_chunk( ctx, &item, sizeof( item ) );

  n = _Thread_Get_name( tcb, name, sizeof( name ) );
  i = 0;

  while ( i < n ) {
    size_t j;

    data = 0;

    for ( j = 0; i < n && j < sizeof( data ); ++j ) {
      rtems_record_data c;

      c = (unsigned char) name[ i ];
      data |= c << ( j * 8 );
      ++i;
    }

    item.event = RTEMS_RECORD_THREAD_NAME;
    item.data = data;
    dump_chunk( ctx, &item, sizeof( item ) );
  }

  return false;
}

void rtems_record_dump(
  rtems_record_dump_chunk  chunk,
  void                    *arg
)
{
  Record_Stream_header       header;
  size_t                     size;
  dump_context               ctx;
  rtems_record_fetch_control control;
  rtems_record_item          items[ 128 ];
  rtems_record_fetch_status  status;

  ctx.chunk = chunk;
  ctx.arg = arg;

  size = _Record_Stream_header_initialize( &header );
  dump_chunk( &ctx, &header, size );
  _Thread_Iterate( thread_names_visitor, &ctx );
  rtems_record_fetch_initialize(
    &control,
    &items[ 0 ],
    RTEMS_ARRAY_SIZE( items )
  );

  do {
    status = rtems_record_fetch( &control );
    dump_chunk(
      &ctx,
      control.fetched_items,
      control.fetched_count * sizeof( *control.fetched_items )
    );
  } while ( status == RTEMS_RECORD_FETCH_CONTINUE );
}
