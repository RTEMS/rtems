/* SPDX-License-Identifier: BSD-2-Clause */

/*
 * Copyright (c) 2007 Ray Xu <rayx.cn@gmail.com>
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

#include <bsp.h>
#include <bsp/bootcard.h>
#include <lpc22xx.h>

void bsp_reset( rtems_fatal_source source, rtems_fatal_code code )
{
#if ON_SKYEYE == 1
  #define SKYEYE_MAGIC_ADDRESS (*(volatile unsigned int *)(0xb0000000))

  SKYEYE_MAGIC_ADDRESS = 0xff;
#else
  rtems_interrupt_level level;

  rtems_interrupt_disable(level);
  (void) level;

  #ifdef __thumb__
    int tmp;
    __asm__ volatile (" .code 16            \n" \
                  "ldr %[tmp], =_start  \n" \
                  "bx  %[tmp]           \n" \
                  "nop                  \n" \
                  : [tmp]"=&r" (tmp) );
  #else
    __asm__ volatile ("b _start");
  #endif
  while(1);
#endif

  (void) source;
  (void) code;
}
