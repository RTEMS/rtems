/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup RTEMSTestSuitesValidation
 *
 * @brief This source file contains the definition of MemorySave() and
 * MemoryRestore().
 */

/*
 * Copyright (C) 2021 embedded brains GmbH & Co. KG
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

#include "tx-support.h"

#include <rtems/test.h>
#include <rtems/score/memory.h>

void MemorySave( MemoryContext *ctx )
{
  const Memory_Information *mem;
  size_t                    n;
  size_t                    i;

  mem = _Memory_Get();
  n = _Memory_Get_count( mem );
  ctx->count = n;
  T_assert_le_sz( n, RTEMS_ARRAY_SIZE( ctx->areas ) );

  for ( i = 0; i < n; ++i ) {
    Memory_Area *area;

    area = _Memory_Get_area( mem, i );
    ctx->areas[ i ].begin = _Memory_Get_begin( area );
    ctx->areas[ i ].free_begin = _Memory_Get_free_begin( area );
    ctx->areas[ i ].end = _Memory_Get_end( area );
  }
}

void MemoryRestore( const MemoryContext *ctx )
{
  const Memory_Information *mem;
  size_t                    n;
  size_t                    i;

  mem = _Memory_Get();
  n = ctx->count;

  for ( i = 0; i < n; ++i ) {
    Memory_Area *area;

    area = _Memory_Get_area( mem, i );
    _Memory_Set_begin( area, ctx->areas[ i ].begin );
    _Memory_Set_free_begin( area, ctx->areas[ i ].free_begin );
    _Memory_Set_end( area, ctx->areas[ i ].end );
  }
}
