/*
 *  COPYRIGHT (c) 2019 Chris Johns <chrisj@rtems.org>
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 */
/**
 * @file
 *
 * @ingroup rtems_rtld
 *
 * @brief RTEMS Run-Time Link Editor Bit Allocator
 *
 * A configurable allocator for small peices of memory where embedding
 * control blocks into the memory are not viable. The amount of memory
 * should be small and the minimum allocator a 32bit work or higher.
 */

#if HAVE_CONFIG_H
#include "config.h"
#endif

#include <errno.h>
#include <inttypes.h>
#include <stdio.h>
#include <string.h>

#include <rtems/rtl/rtl-allocator.h>
#include "rtl-bit-alloc.h"
#include "rtl-error.h"
#include <rtems/rtl/rtl-trace.h>

#define BITS_PER_WORD (sizeof (uint32_t) * 8)

static size_t
bit_blocks (rtems_rtl_bit_alloc* balloc, size_t size)
{
  if (size == 0)
    return 0;
  return ((size - 1) / balloc->block_size) + 1;
}

static size_t
bit_word (size_t bit)
{
  return bit / BITS_PER_WORD;
}

static size_t
bit_offset (size_t bit)
{
  return bit % (sizeof (uint32_t) * 8);
}

static uint32_t
bit_mask (size_t bit)
{
  return 1 << bit_offset (bit);
}

static void
bit_set (rtems_rtl_bit_alloc* balloc, size_t start, size_t bits)
{
  size_t b;
  for (b = start; b < (start + bits); ++b)
    balloc->bits[bit_word (b)] |= bit_mask (b);
}

static void
bit_clear (rtems_rtl_bit_alloc* balloc, size_t start, size_t bits)
{
  size_t b;
  for (b = start; b < (start + bits); ++b)
    balloc->bits[bit_word (b)] &= ~bit_mask (b);
}

static ssize_t
bit_find_clear (rtems_rtl_bit_alloc* balloc, size_t blocks)
{
  size_t base = 0;
  size_t clear = 0;
  size_t b;

  for (b = 0; b < balloc->blocks; ++b)
  {
    if (balloc->bits[b] != 0xffffffff)
    {
      uint32_t word = balloc->bits[b];
      size_t   o;
      for (o = 0; o < BITS_PER_WORD; ++o, word >>= 1)
      {
        if ((word & 1) == 0)
        {
          if (clear == 0)
            base = (b * BITS_PER_WORD) + o;
          ++clear;
          if (clear == blocks)
            return base;
        }
        else
        {
          clear = 0;
          base = 0;
        }
      }
    }
    else
    {
      clear = 0;
      base = 0;
    }
  }

  return -1;
}

rtems_rtl_bit_alloc*
rtems_rtl_bit_alloc_open (void* base, size_t size, size_t block_size, size_t used)
{
  rtems_rtl_bit_alloc* balloc;
  const size_t         base_size = base == NULL ? size : 0;;
  const size_t         blocks = (size / block_size) / BITS_PER_WORD;
  const size_t         bit_bytes = blocks * sizeof(uint32_t);

  if (rtems_rtl_trace (RTEMS_RTL_TRACE_BIT_ALLOC))
    printf ("rtl: balloc: open: base=%p size=%zu" \
            " block-size=%zu blocks=%zu used=%zu\n",
            base, size, block_size, blocks, used);

  if (size == 0)
  {
    rtems_rtl_set_error (ENOMEM, "bit allocator size is 0");
    return false;
  }

  if (used > size)
  {
    rtems_rtl_set_error (ENOMEM, "bad bit allocator used value");
    return false;
  }

  balloc = rtems_rtl_alloc_new (RTEMS_RTL_ALLOC_OBJECT,
                                sizeof (rtems_rtl_bit_alloc) + bit_bytes + base_size,
                                true);
  if (balloc == NULL)
  {
    rtems_rtl_set_error (ENOMEM, "not bit allocator memory");
    return NULL;
  }

  balloc->base =
    base == NULL ? ((void*) balloc) + sizeof (rtems_rtl_bit_alloc) + bit_bytes: base;
  balloc->size = size;
  balloc->bits = ((void*) balloc) + sizeof (rtems_rtl_bit_alloc);
  balloc->block_size = block_size;
  balloc->blocks = blocks;

  bit_set (balloc, 0, bit_blocks (balloc, used));

  return balloc;
}

void
rtems_rtl_bit_alloc_close (rtems_rtl_bit_alloc* balloc)
{
  if (rtems_rtl_trace (RTEMS_RTL_TRACE_BIT_ALLOC))
    printf ("rtl: balloc: close: base=%p size=%zu\n",
            balloc->base, balloc->size);
  rtems_rtl_alloc_del (RTEMS_RTL_ALLOC_OBJECT, balloc);
}

void*
rtems_rtl_bit_alloc_balloc (rtems_rtl_bit_alloc* balloc, size_t size)
{
  size_t  blocks = bit_blocks (balloc, size);
  ssize_t block = bit_find_clear (balloc, blocks);
  if (block < 0)
    return NULL;
  if (rtems_rtl_trace (RTEMS_RTL_TRACE_BIT_ALLOC))
    printf ("rtl: balloc: balloc: size=%zu blocks=%zu block=%zi\n",
            size, blocks, block);
  bit_set (balloc, block, blocks);
  return (void*) (balloc->base + (block * balloc->block_size));
}

void
rtems_rtl_bit_alloc_bfree (rtems_rtl_bit_alloc* balloc, void* addr, size_t size)
{
  const uint8_t* a = (const uint8_t*) addr;
  if (addr != NULL && a >= balloc->base && a < balloc->base + balloc->size)
  {
    size_t block = bit_blocks (balloc, a - balloc->base);
    size_t blocks = bit_blocks (balloc, size);
    bit_clear (balloc, block, blocks);
  }
}
