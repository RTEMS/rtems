/* SPDX-License-Identifier: BSD-2-Clause */

/*
 * Copyright (c) 2016 embedded brains GmbH & Co. KG
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

#include <string.h>

#include <rtems/untar.h>

int Untar_GzChunkContext_Init(
  Untar_GzChunkContext *ctx,
  void *inflateBuffer,
  size_t inflateBufferSize
)
{
  int ret;
  int status = UNTAR_SUCCESSFUL;

  Untar_ChunkContext_Init(&ctx->base);
  ctx->inflateBuffer = inflateBuffer;
  ctx->inflateBufferSize = inflateBufferSize;
  memset(&ctx->strm, 0, sizeof(ctx->strm));
  ret = inflateInit2(&ctx->strm, 32 + MAX_WBITS);
  if (ret != Z_OK){
    status = UNTAR_FAIL;
  }
  return status;
}

int Untar_FromGzChunk_Print(
  Untar_GzChunkContext *ctx,
  void *chunk,
  size_t chunk_size,
  const rtems_printer *printer
)
{
  int untar_succesful;
  int status;

  ctx->strm.next_in = (Bytef *)chunk;
  ctx->strm.avail_in = (size_t)chunk_size;

    /* Inflate until output buffer is not full */
  do {
    ctx->strm.next_out = (Bytef *) ctx->inflateBuffer;
    ctx->strm.avail_out = ctx->inflateBufferSize;

    status = inflate(&ctx->strm, Z_NO_FLUSH);
    if (status == Z_OK || status == Z_STREAM_END) {
      size_t inflated_size =
        ctx->inflateBufferSize - ctx->strm.avail_out;
      untar_succesful = Untar_FromChunk_Print(&ctx->base,
        ctx->inflateBuffer, inflated_size, NULL);
      if (untar_succesful != UNTAR_SUCCESSFUL){
        return untar_succesful;
      }
    } else {
      untar_succesful = UNTAR_GZ_INFLATE_FAILED;
    }
  } while (ctx->strm.avail_out == 0 && ctx->strm.avail_in > 0
      && status == Z_OK);

  if (status != Z_OK) {
    if (untar_succesful != UNTAR_SUCCESSFUL){
      rtems_printf(printer, "Untar not successful\n");
    }

    if (status != Z_STREAM_END) {
      rtems_printf(printer, "Zlib inflate failed\n");
    }

    status = inflateEnd(&ctx->strm);
    if (status != Z_OK) {
      rtems_printf(printer, "Zlib inflate end failed\n");
    }
  }
  return untar_succesful;
}


