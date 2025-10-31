/* SPDX-License-Identifier: BSD-2-Clause */

/*
 * Copyright (C) 2024 Contemporary Software
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
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS AS IS
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

#if !defined(_ZQSPIFLASH_JEDEC_H_)
#define _ZQSPIFLASH_JEDEC_H_

#include <stdint.h>

typedef struct {
  uint32_t jedec_id;
  uint32_t sec_size;
  uint32_t page_size;
  uint32_t flash_size;
  uint32_t num_die;
  char label[16];
} flash_definition;

typedef flash_definition* flash_def;

flash_definition flash_dev_table[] = {
  {0x20BA18, 0x10000, 0x100, 0x1000000, 1, "MT25QL128ABA"},
  {0x20BB18, 0x10000, 0x100, 0x1000000, 1, "MT25QL128ABB"},
  {0x012018, 0x10000, 0x100, 0x1000000, 1, "S25FL128P_64K"},
  {0x010218, 0x400000, 0x200, 0x4000000, 1, "S25FL512P_256K"},
  {0x0, 0x0, 0x0, 0x0, 0, "end of table"}
};

#endif
