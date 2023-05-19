/* SPDX-License-Identifier: BSD-2-Clause */

/**
 *  @file
 *
 *  @brief RTEMS File System Eval Eat Delimiter Path
 *  @ingroup LibIOInternal
 */

/*
 * Copyright (c) 2012 embedded brains GmbH & Co. KG
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

#include <rtems/libio_.h>

void rtems_filesystem_eval_path_eat_delimiter(
  rtems_filesystem_eval_path_context_t *ctx
)
{
  const char *current = ctx->path;
  const char *end = current + ctx->pathlen;

  while (current != end && rtems_filesystem_is_delimiter(*current)) {
    ++current;
  }

  ctx->path = current;
  ctx->pathlen = (size_t) (end - current);
}

static void next_token(rtems_filesystem_eval_path_context_t *ctx)
{
  const char *begin = ctx->path;
  const char *end = begin + ctx->pathlen;
  const char *current = begin;

  while (current != end && !rtems_filesystem_is_delimiter(*current)) {
    ++current;
  }

  ctx->path = current;
  ctx->pathlen = (size_t) (end - current);
  ctx->token = begin;
  ctx->tokenlen = (size_t) (current - begin);
}

void rtems_filesystem_eval_path_next_token(
  rtems_filesystem_eval_path_context_t *ctx
)
{
  rtems_filesystem_eval_path_eat_delimiter(ctx);
  next_token(ctx);
}
