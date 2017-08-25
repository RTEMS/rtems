/*
 * Copyright (c) 2016 embedded brains GmbH.  All rights reserved.
 *
 *  embedded brains GmbH
 *  Dornierstr. 4
 *  82178 Puchheim
 *  Germany
 *  <info@embedded-brains.de>
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rtems.org/license/LICENSE.
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


