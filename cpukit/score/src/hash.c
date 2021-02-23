/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup RTEMSScoreHash
 *
 * @brief This source file the implementation of _Hash_Finalize().
 */

/*
 * Copyright (C) 2021 embedded brains GmbH (http://www.embedded-brains.de)
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

#include <rtems/score/hash.h>
#include <rtems/score/assert.h>
#include <rtems/score/io.h>

#include <limits.h>

static void _Hash_Put_char( int c, void *arg )
{
  Hash_Context *context;
  size_t        index;

  context = arg;
  index = context->index;
  _Assert( index < sizeof( context->hash->chars ) );
  context->hash->chars[ context->index ] = (char) c;
  context->index = index + 1;
}

void _Hash_Finalize( Hash_Context *context, Hash_Control *hash )
{
  unsigned char digest[ 32 ];
  int           n;

  SHA256_Final( digest, &context->Context );
  context->hash = hash;
  context->index = 0;
  hash->chars[ sizeof( *hash ) - 1 ] = '\0';
  n = _IO_Base64url(
    _Hash_Put_char,
    context,
    digest,
    sizeof( digest ),
    NULL,
    INT_MAX
  );
  _Assert_Unused_variable_equals( n, sizeof( *hash ) - 1 );
}
