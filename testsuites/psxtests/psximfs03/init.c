/* SPDX-License-Identifier: BSD-2-Clause */

/*
 * Copyright (c) 2025 On-Line Application Research Corporation (OAR)
 * Copyright (c) 2025 Bhavya Shah <bhavyashah8443@gmail.com>
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

#include <tmacros.h>
#include "test_support.h"

#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/statvfs.h>
#include <fcntl.h>
#include <errno.h>
#include <rtems/libcsupport.h>
#include <rtems/imfs.h>

const char rtems_test_name[] = "PSXIMFS 3";
#define BLOCK_SIZE 16

typedef struct block{
  struct block * next;
  char data[BLOCK_SIZE - sizeof(struct block*)];
} block;

/* forward declarations to avoid warnings */

void *allocator(void);
void deallocator(void *);
size_t free_space(void);
void init_memory(void);
void *empty_space(void);
void fill_space(void *);
void validate_data_blocks_count(void);

#define MEMORY_SIZE 8000

char g_memory[MEMORY_SIZE];
static int g_alloc_blocks_count = 0;
block *g_freelist = NULL;

void *allocator(void)
{
  if (NULL == g_freelist) {
    return NULL;
  }
  block *blk = g_freelist;
  g_freelist = blk->next;
  memset(blk, 0, BLOCK_SIZE);
  g_alloc_blocks_count++;
  return blk;
}

void deallocator(
  void *memory
)
{
  block *blk = (block *)memory;

  if (NULL == blk) {
    return;
  }

  memset(blk, 0, BLOCK_SIZE);
  blk->next = g_freelist;
  g_freelist = blk;
  g_alloc_blocks_count--;
}

size_t free_space(void)
{
  return MEMORY_SIZE - g_alloc_blocks_count * BLOCK_SIZE;
}

void init_memory(void)
{
  rtems_test_assert( sizeof(block) == BLOCK_SIZE );

  block *first = (block *)g_memory;
  const char *last = g_memory + sizeof(g_memory);

  while ( first + 1 <= (block *)last ) {
    first->next = g_freelist;
    g_freelist = first;
    first += 1;
  }
  g_alloc_blocks_count = 0;
}

void *empty_space(void)
{
  block *memory = g_freelist;
  g_freelist = NULL;
  return memory;
}

void fill_space(void *memory)
{
  g_freelist = memory;
}

void validate_data_blocks_count(void)
{
  int indirect_block_count = 1 + BLOCK_SIZE / sizeof(void *);
  int double_indirect_block_count = 1 + (BLOCK_SIZE / sizeof(void *)) * indirect_block_count;
  int triple_indirect_block_count = 1 + (BLOCK_SIZE / sizeof(void *)) * double_indirect_block_count;
  int expected_data_blocks_counts = indirect_block_count;
  expected_data_blocks_counts += double_indirect_block_count;
  expected_data_blocks_counts += triple_indirect_block_count;

  rtems_test_assert( expected_data_blocks_counts == g_alloc_blocks_count );
}

/* configuration information */

#define CONFIGURE_IMFS_MEMFILE_BYTES_PER_BLOCK BLOCK_SIZE
#define CONFIGURE_IMFS_MEMFILE_OPS \
  { \
    .allocate_block = allocator, \
    .free_block = deallocator, \
    .get_free_space = free_space \
  }
#define CONFIGURE_INIT

#include "../psximfs/system.h"
/* end of file */
