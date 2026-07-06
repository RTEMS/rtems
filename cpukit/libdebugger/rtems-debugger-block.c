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

int rtems_debugger_block_create(rtems_debugger_block* block, size_t step,
                                size_t size,
                                rtems_debugger_block_rebase rebaser) {
  block->level = 0;
  block->step = step;
  block->count = step;
  block->size = size;
  block->rebaser = rebaser;
  block->block = calloc(block->count, block->size);
  if (block->block == NULL) {
    errno = ENOMEM;
    return -1;
  }
  return 0;
}

int rtems_debugger_block_destroy(rtems_debugger_block* block) {
  free(block->block);
  block->block = NULL;
  block->level = 0;
  block->count = 0;
  block->size = 0;
  return 0;
}

int rtems_debugger_block_resize(rtems_debugger_block* block) {
  if (block->level >= block->count) {
    block->count += block->step;
    void* new_block = realloc(block->block, block->count * block->size);
    if (new_block == NULL) {
      block->block = NULL;
      block->level = 0;
      block->count = 0;
      errno = ENOMEM;
      return -1;
    }
    if (block->block != new_block && block->rebaser != NULL) {
      block->rebaser(block->block, new_block);
    }
    block->block = new_block;
  }
  return 0;
}

void* rtems_debugger_block_transform(void* previous, void* new, void* addr) {
  uintptr_t previous_u = (uintptr_t)previous;
  uintptr_t new_u = (uintptr_t)new;
  uintptr_t addr_u = (uintptr_t)addr;
  return (void*)((addr_u - previous_u) + new_u);
}
