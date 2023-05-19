/* SPDX-License-Identifier: BSD-2-Clause */

/*
 * Copyright (C) 2008, 2013 embedded brains GmbH & Co. KG
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
#include <bsp/u-boot.h>

#include <libcpu/powerpc-utility.h>

void bsp_restart(void *addr)
{
  rtems_interrupt_level level;
  void (*start)(void) = addr;
  #ifdef HAS_UBOOT
    const void *mem_begin = (const void *) bsp_uboot_board_info.bi_memstart;
    size_t mem_size = bsp_uboot_board_info.bi_memsize;
  #else /* HAS_UBOOT */
    const void *mem_begin = bsp_ram_start;
    size_t mem_size = (size_t) bsp_ram_size;
  #endif /* HAS_UBOOT */
  uint32_t hid0;

  rtems_interrupt_disable(level);
  (void) level; /* avoid set but not used warning */

  PPC_SPECIAL_PURPOSE_REGISTER(HID0, hid0);

  if ((hid0 & HID0_DCE) != 0) {
    rtems_cache_flush_multiple_data_lines(mem_begin, mem_size);
  }

  hid0 &= ~(HID0_DCE | HID0_ICE);

  PPC_SET_SPECIAL_PURPOSE_REGISTER(HID0, hid0);

  (*start)();
}
