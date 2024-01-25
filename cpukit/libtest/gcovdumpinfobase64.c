/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup RTEMSImplGcov
 *
 * @brief This source file contains the implementation of
 *   _Gcov_Dump_info_base64().
 */

/*
 * Copyright (C) 2021, 2022 embedded brains GmbH & Co. KG
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

#include <rtems/test-gcov.h>

#include <limits.h>
#include <string.h>

#include <rtems/base64.h>

typedef struct {
  IO_Put_char put_char;
  void       *arg;
  int         out;
  size_t      index;
  char        buf[ 57 ];
} Gcov_Base64_context;

static void _Gcov_Base64_put_char( int c, void *arg )
{
  Gcov_Base64_context *ctx;

  ctx = arg;

  ( *ctx->put_char )( c, ctx->arg );
  ++ctx->out;

  if ( ctx->out >= 76 ) {
    ctx->out = 0;
    ( *ctx->put_char )( '\n', ctx->arg );
  }
}

static void _Gcov_Base64_encode( int c, void *arg )
{
  Gcov_Base64_context *ctx;
  size_t                  index;

  ctx = arg;
  index = ctx->index;
  ctx->buf[ index ] = (char) c;

  if ( index == RTEMS_ARRAY_SIZE( ctx->buf ) - 1 ) {
    index = 0;
    _Base64_Encode(
      _Gcov_Base64_put_char,
      ctx,
      ctx->buf,
      sizeof( ctx->buf ),
      NULL,
      INT_MAX
    );
  } else {
    ++index;
  }

  ctx->index = index;
}

void _Gcov_Dump_info_base64( IO_Put_char put_char, void *arg )
{
  Gcov_Base64_context ctx;

  memset( &ctx, 0, sizeof( ctx ) );
  ctx.put_char = put_char;
  ctx.arg = arg;
  _Gcov_Dump_info( _Gcov_Base64_encode, &ctx );
  _Base64_Encode( _Gcov_Base64_put_char, &ctx, ctx.buf, ctx.index, NULL, INT_MAX );
}
