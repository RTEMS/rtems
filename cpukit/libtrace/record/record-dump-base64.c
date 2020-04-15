/* SPDX-License-Identifier: BSD-2-Clause */

/*
 * Copyright (C) 2020 embedded brains GmbH (http://www.embedded-brains.de)
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
#include <rtems/score/io.h>

#include <limits.h>
#include <string.h>

typedef struct {
  IO_Put_char  put_char;
  void        *arg;
  int          out;
  size_t       index;
  char         buf[ 57 ];
} dump_context;

static void put_char( int c, void *arg )
{
  dump_context *ctx;

  ctx = arg;

  ( *ctx->put_char )( c, ctx->arg );
  ++ctx->out;

  if ( ctx->out >= 76 ) {
    ctx->out = 0;
    ( *ctx->put_char )( '\n', ctx->arg );
  }
}

static void chunk( void *arg, const void *data, size_t length )
{
  dump_context *ctx;
  size_t        index;
  const char   *in;
  const void   *end;

  ctx = arg;
  index = ctx->index;
  in = data;
  end = in + length;

  while ( in != end ) {
    ctx->buf[ index ] = *in;
    ++in;

    if ( index == RTEMS_ARRAY_SIZE( ctx->buf ) - 1 ) {
      index = 0;
      _IO_Base64(
        put_char,
        ctx,
        ctx->buf,
        sizeof( ctx->buf ),
        NULL,
        INT_MAX
      );
    } else {
      ++index;
    }
  }

  ctx->index = index;
}

static void flush( dump_context *ctx )
{
  _IO_Base64( put_char, ctx, ctx->buf, ctx->index, NULL, INT_MAX );
}

void rtems_record_dump_base64( IO_Put_char put_char, void *arg )
{
  dump_context ctx;

  memset( &ctx, 0, sizeof( ctx ) );
  ctx.put_char = put_char;
  ctx.arg = arg;

  rtems_record_dump( chunk, &ctx );
  flush( &ctx );
}
