/*
 * Copyright (c) 2017 embedded brains GmbH.  All rights reserved.
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

/*
 * ATTENTION: THIS IS A VERY LIMITED ENTROPY SOURCE.
 *
 * This implementation uses a time-based value for it's entropy. The only thing
 * that makes it random are interrupts from external sources. Don't use it if
 * you need for example a strong crypto.
 */

#include <sys/param.h>
#include <unistd.h>
#include <string.h>
#include <rtems/bsd.h>
#include <rtems/counter.h>
#include <rtems/sysinit.h>

static uint32_t state;

int getentropy(void *ptr, size_t n)
{
  uint8_t *dest = ptr;

  state ^= rtems_counter_read();
  state *= 25169206;
  state += 1679610226;

  while (n > 0) {
    size_t m;

    m = MIN(n, sizeof(state));
    memcpy(dest, &state, m);
    n -= m;
    dest += m;
    state *= 85236167;
    state += 30557471;
  }

  return 0;
}

static void getentropy_init(void)
{
  struct bintime bt;

  rtems_bsd_bintime(&bt);
  state = (uint32_t) bt.frac;
  state ^= (uint32_t) (bt.frac >> 32);
  state ^= (uint32_t) bt.sec;
  state ^= (uint32_t) (bt.sec >> 32);
}

RTEMS_SYSINIT_ITEM(
  getentropy_init,
  RTEMS_SYSINIT_DEVICE_DRIVERS,
  RTEMS_SYSINIT_ORDER_LAST_BUT_5
);
