/* SPDX-License-Identifier: BSD-2-Clause */

/*
 * Copyright (c) 2017 embedded brains GmbH & Co. KG
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
