/*
 * Copyright (c) 2011 embedded brains GmbH.  All rights reserved.
 *
 *  embedded brains GmbH
 *  Obere Lagerstr. 30
 *  82178 Puchheim
 *  Germany
 *  <info@embedded-brains.de>
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rtems.com/license/LICENSE.
 */

#include <bspopts.h>
#include <rtems/powerpc/powerpc.h>

#if BSP_DATA_CACHE_ENABLED && PPC_CACHE_ALIGNMENT == 32

#include <string.h>
#include <stdint.h>
#include <stdbool.h>

#include <libcpu/powerpc-utility.h>

#define CACHE_LINE_SIZE 32

#define WORD_SIZE 4

#define WORD_MASK (WORD_SIZE - 1)

static bool aligned(const void *a, const void *b)
{
  return ((((uintptr_t) a) | ((uintptr_t) b)) & WORD_MASK) == 0;
}

void *memcpy(void *dst_ptr, const void *src_ptr, size_t n)
{
  uint8_t *dst = dst_ptr;
  const uint8_t *src = src_ptr;

  ppc_data_cache_block_touch(src);

  if (__builtin_expect(n >= WORD_SIZE && aligned(src, dst), 1)) {
    uint32_t *word_dst = (uint32_t *) dst - 1;
    const uint32_t *word_src = (const uint32_t *) src - 1;

    if (n >= 2 * CACHE_LINE_SIZE - WORD_SIZE) {
      while ((uintptr_t) (word_dst + 1) % CACHE_LINE_SIZE != 0) {
        uint32_t tmp;
        __asm__ volatile (
          "lwzu %[tmp], 0x4(%[src])\n"
          "stwu %[tmp], 0x4(%[dst])\n"
          : [src] "+b" (word_src),
            [dst] "+b" (word_dst),
            [tmp] "=&r" (tmp)
        );
        n -= WORD_SIZE;
      }

      while (n >= CACHE_LINE_SIZE) {
        uint32_t dst_offset = 4;
        uint32_t src_offset = 32 + 4;
        uint32_t tmp0;
        uint32_t tmp1;
        uint32_t tmp2;
        uint32_t tmp3;
        __asm__ volatile (
          "dcbz %[dst],  %[dst_offset]\n"
          "lwz  %[tmp0], 0x04(%[src])\n"
          "dcbt %[src],  %[src_offset]\n"
          "lwz  %[tmp1], 0x08(%[src])\n"
          "lwz  %[tmp2], 0x0c(%[src])\n"
          "lwz  %[tmp3], 0x10(%[src])\n"
          "stw  %[tmp0], 0x04(%[dst])\n"
          "stw  %[tmp1], 0x08(%[dst])\n"
          "stw  %[tmp2], 0x0c(%[dst])\n"
          "stw  %[tmp3], 0x10(%[dst])\n"
          "lwz  %[tmp0], 0x14(%[src])\n"
          "lwz  %[tmp1], 0x18(%[src])\n"
          "lwz  %[tmp2], 0x1c(%[src])\n"
          "lwzu %[tmp3], 0x20(%[src])\n"
          "stw  %[tmp0], 0x14(%[dst])\n"
          "stw  %[tmp1], 0x18(%[dst])\n"
          "stw  %[tmp2], 0x1c(%[dst])\n"
          "stwu %[tmp3], 0x20(%[dst])\n"
          : [src] "+b" (word_src),
            [dst] "+b" (word_dst),
            [tmp0] "=&r" (tmp0),
            [tmp1] "=&r" (tmp1),
            [tmp2] "=&r" (tmp2),
            [tmp3] "=&r" (tmp3)
          : [src_offset] "r" (src_offset),
            [dst_offset] "r" (dst_offset)
        );
        n -= CACHE_LINE_SIZE;
      }
    }

    while (n >= WORD_SIZE) {
      uint32_t tmp;
      __asm__ volatile (
        "lwzu %[tmp], 0x4(%[src])\n"
        "stwu %[tmp], 0x4(%[dst])\n"
        : [src] "+b" (word_src),
          [dst] "+b" (word_dst),
          [tmp] "=&r" (tmp)
      );
      n -= WORD_SIZE;
    }

    dst = (uint8_t *) word_dst + 4;
    src = (const uint8_t *) word_src + 4;
  }

  while (n > 0) {
    *dst = *src;
    ++src;
    ++dst;
    --n;
  }

  return dst_ptr;
}

#endif /* BSP_DATA_CACHE_ENABLED && PPC_CACHE_ALIGNMENT == 32 */
