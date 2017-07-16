/*
 * Copyright (c) 2016 Chris Johns <chrisj@rtems.org>.
 * All rights reserved.
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
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */

#include <errno.h>
#include <stdlib.h>

#include "rtems-debugger-block.h"

int
rtems_debugger_block_create(rtems_debugger_block* block,
                            size_t                step,
                            size_t                size)
{
  int r = 0;
  block->level = 0;
  block->step = step;
  block->count = step;
  block->size = size;
  block->block = calloc(block->count, block->size);
  if (block->block == NULL)
    errno = ENOMEM;
  return r;
}

int
rtems_debugger_block_destroy(rtems_debugger_block* block)
{
  free(block->block);
  block->block = NULL;
  block->level = 0;
  block->count = 0;
  block->size = 0;
  return 0;
}

int
rtems_debugger_block_resize(rtems_debugger_block* block)
{
  int r = 0;
  if (block->level >= block->count) {
    block->count += block->step;
    block->block = realloc(block->block, block->count * block->size);
    if (block->block == NULL) {
      block->level = 0;
      block->count = 0;
      errno = ENOMEM;
      r = -1;
    }
  }
  return r;
}
