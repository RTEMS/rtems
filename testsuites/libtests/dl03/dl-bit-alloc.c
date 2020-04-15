/*
 * Copyright (c) 2016, 2018 Chris Johns <chrisj@rtems.org>.  All rights reserved.
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rtems.org/license/LICENSE.
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "tmacros.h"

#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>

#include <rtems.h>
#include <rtems/dumpbuf.h>

#include <rtems/rtl/rtl.h>
#include <rtems/rtl/rtl-trace.h>

#include "dl-bit-alloc.h"

#include "rtl-bit-alloc.h"

#define NUM(m) (sizeof(m) / sizeof(m[0]))

typedef struct
{
  size_t          size;
  const uint32_t* map;
  size_t          map_size;
} alloc_check;

const uint32_t map_1[] = {
  0x0000003f,
  0x00000000,
  0x00000000,
  0x00000000
};

const uint32_t map_a_1[] = {
  0xffffffff,
  0x00000000,
  0x00000000,
  0x00000000
};

const uint32_t map_a_2[] = {
  0xffffffff,
  0x00000001,
  0x00000000,
  0x00000000
};

const uint32_t map_a_3[] = {
  0xffffffff,
  0x00000007,
  0x00000000,
  0x00000000
};

const uint32_t map_a_4[] = {
  0xffffffff,
  0x00000fff,
  0x00000000,
  0x00000000
};

const uint32_t map_a_5[] = {
  0xffffffff,
  0x00001fff,
  0x00000000,
  0x00000000
};

const uint32_t map_a_6[] = {
  0xffffffff,
  0x00007fff,
  0x00000000,
  0x00000000
};

const uint32_t map_a_7[] = {
  0xffffffff,
  0x0007ffff,
  0x00000000,
  0x00000000
};

const uint32_t map_a_8[] = {
  0xffffffff,
  0x07ffffff,
  0x00000000,
  0x00000000
};

const uint32_t map_b_1[] = {
  0xffffffff,
  0x07ffffff,
  0x00000000,
  0x00000000
};

const uint32_t map_b_2[] = {
  0xffffffff,
  0x07ffffff,
  0x00000000,
  0x00000000
};

const uint32_t map_b_3[] = {
  0xffffffff,
  0x07ffffff,
  0x00000000,
  0x00000000
};

const uint32_t map_b_4[] = {
  0xffffffff,
  0x07ffffff,
  0x00000000,
  0x00000000
};

const uint32_t map_free_1[] = {
  0xffffffff,
  0x07ffffff,
  0x00000000,
  0x00000000
};

alloc_check a_allocs[] = {
  { 26 * sizeof(uint32_t), map_a_1, NUM(map_a_1) },
  { 4,                     map_a_2, NUM(map_a_2) },
  { 8,                     map_a_3, NUM(map_a_3) },
  { 34,                    map_a_4, NUM(map_a_4) },
  { 4,                     map_a_5, NUM(map_a_5) },
  { 8,                     map_a_6, NUM(map_a_6) },
  { 16,                    map_a_7, NUM(map_a_7) },
  { 32,                    map_a_8, NUM(map_a_8) }
};

alloc_check b_allocs[] = {
  { 24, map_b_1, NUM(map_b_1) },
  { 30, map_b_2, NUM(map_b_2) },
  { 8,  map_b_3, NUM(map_b_3) },
  { 4,  map_b_4, NUM(map_b_4) }
};

static bool check_bit_map(rtems_rtl_bit_alloc* balloc,
                          const uint32_t*      map,
                          size_t               size)
{
  size_t i;
  rtems_print_buffer ((const unsigned char *) balloc->bits, size * sizeof(uint32_t));
  for (i = 0; i < size; ++i)
  {
    if (balloc->bits[i] != map[i])
    {
      printf("bit-map mismatch: %zu: %08" PRIx32 "\n", i, map[i]);
      return false;
    }
  }
  return true;
}

static void delta_bit_map(rtems_rtl_bit_alloc* balloc, uint32_t* last, size_t size)
{
  size_t i;
  for (i = 0; i < size; ++i)
    last[i] = last[i] ^ balloc->bits[i];
  printf("Delta:\n");
  rtems_print_buffer ((const unsigned char *) last, size * sizeof(uint32_t));
  memcpy(last, balloc->bits, size * sizeof(uint32_t));
}

static void dl_init_rtl(void)
{
  /*
   * Check the RTL object is created and can be locked and unlocked.
   */
  rtems_test_assert(rtems_rtl_lock () != NULL);
  rtems_rtl_unlock ();
  rtems_test_assert(rtems_rtl_data_unprotected () != NULL);
  rtems_rtl_trace_set_mask(RTEMS_RTL_TRACE_ALL | RTEMS_RTL_TRACE_CACHE);
}

int dl_bit_alloc_test(void)
{
  rtems_rtl_bit_alloc* balloc;
  void*                a[NUM(a_allocs)];
  void*                b[NUM(b_allocs)];
  uint32_t             last_map[4] = { 0 };
  size_t               i;

  /*
   * Make sure the RTL can initialise.
   */
  dl_init_rtl();

  printf("bit-alloc: open\n");
  balloc = rtems_rtl_bit_alloc_open(NULL, 64 * 1024, sizeof(uint32_t), 21);
  rtems_test_assert(balloc != NULL);

  rtems_test_assert(check_bit_map(balloc, map_1, NUM(map_1)));

  for (i = 0; i < NUM(a_allocs); ++i)
  {
    printf("balloc: %zu\n", a_allocs[i].size);
    a[i] = rtems_rtl_bit_alloc_balloc(balloc, a_allocs[i].size);
    rtems_test_assert(a[i] != NULL);
    printf("balloc: a[%zu] %zu %p\n", i, a_allocs[i].size, a[i]);
    rtems_test_assert(check_bit_map(balloc, a_allocs[i].map, a_allocs[i].map_size));
    delta_bit_map(balloc, last_map, NUM(last_map));
  }

  printf("bfree: %p %zu\n", a[3], a_allocs[3].size);
  rtems_rtl_bit_alloc_bfree(balloc, a[3], a_allocs[3].size);
  delta_bit_map(balloc, last_map, NUM(last_map));
  a[3] = NULL;

  for (i = 0; i < NUM(b_allocs); ++i)
  {
    printf("balloc: %zu\n", b_allocs[i].size);
    b[i] = rtems_rtl_bit_alloc_balloc(balloc, b_allocs[i].size);
    rtems_test_assert(b[i] != NULL);
    printf("balloc: b[%zu] %zu %p\n", i, b_allocs[i].size, b[i]);
    rtems_print_buffer ((const unsigned char *) balloc->bits, 8 * sizeof(uint32_t));
    delta_bit_map(balloc, last_map, NUM(last_map));
  }

  for (i = 0; i < NUM(a_allocs); ++i)
  {
    if (a[i] != NULL)
    {
      printf("bfree: a[%zu] %p %zu\n", i, a[i], a_allocs[i].size);
      rtems_rtl_bit_alloc_bfree(balloc, a[i], a_allocs[i].size);
      rtems_print_buffer ((const unsigned char *) balloc->bits, 8 * sizeof(uint32_t));
      delta_bit_map(balloc, last_map, NUM(last_map));
    }
  }

  for (i = 0; i < NUM(b_allocs); ++i)
  {
    if (b[i] != NULL)
    {
      printf("bfree: b[%zu] %p %zu\n", i, b[i], b_allocs[i].size);
      rtems_rtl_bit_alloc_bfree(balloc, b[i], b_allocs[i].size);
      rtems_print_buffer ((const unsigned char *) balloc->bits, 8 * sizeof(uint32_t));
    }
  }

  printf("bit-alloc: close\n");
  rtems_rtl_bit_alloc_close(balloc);

  return 0;
}
