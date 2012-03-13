/*
 * Copyright (c) 2012 embedded brains GmbH.  All rights reserved.
 *
 *  embedded brains GmbH
 *  Obere Lagerstr. 30
 *  82178 Puchheim
 *  Germany
 *  <rtems@embedded-brains.de>
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rtems.com/license/LICENSE.
 */

#if HAVE_CONFIG_H
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
