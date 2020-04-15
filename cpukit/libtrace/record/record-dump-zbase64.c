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

static void *dump_zalloc( void *opaque, unsigned items, unsigned size )
{
  rtems_record_dump_base64_zlib_context *ctx;
  char                                  *mem_begin;
  size_t                                 mem_available;

  ctx = opaque;
  size *= items;
  mem_available = ctx->mem_available;

  if ( mem_available < size ) {
    return NULL;
  }

  mem_begin = ctx->mem_begin;
  ctx->mem_begin = mem_begin + size;
  ctx->mem_available = mem_available - size;
  return mem_begin;
}

static void dump_zfree( void *opaque, void *ptr )
{
  (void) opaque;
  (void) ptr;
}

static void put_char( int c, void *arg )
{
  rtems_record_dump_base64_zlib_context *ctx;

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
  rtems_record_dump_base64_zlib_context *ctx;

  ctx = arg;
  ctx->stream.next_in = RTEMS_DECONST( void *, data );
  ctx->stream.avail_in = length;

  while ( ctx->stream.avail_in > 0 ) {
    int err;

    err = deflate( &ctx->stream, Z_NO_FLUSH );
    if ( err != Z_OK ) {
      return;
    }

    if ( ctx->stream.avail_out == 0 ) {
      ctx->stream.next_out = &ctx->buf[ 0 ];
      ctx->stream.avail_out = sizeof( ctx->buf );

      _IO_Base64( put_char, ctx, ctx->buf, sizeof( ctx->buf ), NULL, INT_MAX );
    }
  }
}

static void flush( rtems_record_dump_base64_zlib_context *ctx )
{
  while ( true ) {
    int err;

    err = deflate( &ctx->stream, Z_FINISH );
    if ( err != Z_OK ) {
      break;
    }

    if ( ctx->stream.avail_out == 0 ) {
      ctx->stream.next_out = &ctx->buf[ 0 ];
      ctx->stream.avail_out = sizeof( ctx->buf );

      _IO_Base64( put_char, ctx, ctx->buf, sizeof( ctx->buf ), NULL, INT_MAX );
    }
  }

  _IO_Base64(
    put_char,
    ctx,
    ctx->buf,
    sizeof( ctx->buf ) - ctx->stream.avail_out,
    NULL,
    INT_MAX
  );
}

void rtems_record_dump_zlib_base64(
  rtems_record_dump_base64_zlib_context *ctx,
  void                                ( *put_char )( int, void * ),
  void                                  *arg
)
{
  int err;

  ctx->put_char = put_char;
  ctx->arg = arg;
  ctx->out = 0;
  ctx->stream.zalloc = dump_zalloc;
  ctx->stream.zfree = dump_zfree;
  ctx->stream.opaque = ctx;
  ctx->mem_begin = &ctx->mem[ 0 ];
  ctx->mem_available = sizeof( ctx->mem );

  err = deflateInit( &ctx->stream, Z_BEST_COMPRESSION );
  if (err != Z_OK) {
    return;
  }

  ctx->stream.next_out = &ctx->buf[ 0 ];
  ctx->stream.avail_out = sizeof( ctx->buf );

  rtems_record_dump( chunk, ctx );
  flush( ctx );
  deflateEnd( &ctx->stream );
}
