/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup lpc32xx_boot
 *
 * @brief Boot support implementation.
 */

/*
 * Copyright (c) 2010 embedded brains GmbH & Co. KG
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

#include <string.h>

#include <bsp/boot.h>

void lpc32xx_setup_boot_block(
  lpc32xx_boot_block *boot_block,
  uint8_t icr,
  uint8_t page_count
)
{
  memset(boot_block, 0, sizeof(*boot_block));

  ++page_count;

  boot_block->field.d0 = icr;
  boot_block->field.d2 = icr;
  boot_block->field.d4 = page_count;
  boot_block->field.d6 = page_count;
  boot_block->field.d8 = page_count;
  boot_block->field.d10 = page_count;

  icr = (uint8_t) ~((unsigned) icr);
  page_count = (uint8_t) ~((unsigned) page_count);

  boot_block->field.d1 = icr;
  boot_block->field.d3 = icr;
  boot_block->field.d5 = page_count;
  boot_block->field.d7 = page_count;
  boot_block->field.d9 = page_count;
  boot_block->field.d11 = page_count;

  boot_block->field.d12 = 0xaa;
}

void lpc32xx_set_boot_block_bad(
  lpc32xx_boot_block *boot_block
)
{
  boot_block->field.d12 = 0;
}
