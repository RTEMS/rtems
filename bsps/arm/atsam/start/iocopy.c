/*
 * Copyright (c) 2018 embedded brains GmbH.  All rights reserved.
 *
 *  embedded brains GmbH
 *  Dornierstr. 4
 *  82178 Puchheim
 *  Germany
 *  <rtems@embedded-brains.de>
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rtems.org/license/LICENSE.
 */

#include <bsp/iocopy.h>

static void atsam_do_copy(
  uint8_t *dst,
  const uint8_t *src,
  size_t n,
  bool aligned
)
{
  if (aligned) {
    while (n > 3) {
      *(uint32_t *)dst = *(uint32_t *)src;
      dst += 4;
      src += 4;
      n -= 4;
    }
  }

  while (n > 0) {
    *dst = *src;
    ++dst;
    ++src;
    --n;
  }
}

void atsam_copy_to_io(void *dst, const void *src, size_t n)
{
  atsam_do_copy(dst, src, n, ((uintptr_t)dst) % 4 == 0);
}

void atsam_copy_from_io(void *dst, const void *src, size_t n)
{
  atsam_do_copy(dst, src, n, ((uintptr_t)src) % 4 == 0);
}
