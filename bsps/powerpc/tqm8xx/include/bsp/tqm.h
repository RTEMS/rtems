/* SPDX-License-Identifier: BSD-2-Clause */

/*
 * RTEMS BSP support for TQ modules
 *
 * This file contains definitions to interact with TQC's
 * processor modules.
 */

/*
 * Copyright (c) 2007 embedded brains GmbH & Co. KG
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

#ifndef __TQM_H__
#define __TQM_H__
#include <rtems.h>

typedef struct {
  uint32_t sdram_size;  /* existing SDRAM size */
  uint32_t flash_base;  /* start address flash */
  uint32_t flash_size;  /* existing Flash size */
  uint32_t flash_offset;
  uint32_t sram_base;   /* start address sram                */
  uint32_t sram_size;   /* existing sram size                */
  uint32_t immr_base;   /* start address internal memory map */
  uint32_t reboot;      /* reboot flag                       */
  uint8_t  ip_addr[4];  /* IP address                        */
  uint8_t  eth_addr[6]; /* ethernet (MAC) address            */
  uint8_t  gap_42[2];   /* gap for alignment                 */
  void (*put_char)(int c); /* function to output characters  */
} tqm_bd_info_t;

#define TQM_BD_INFO_ADDR 0x3400
#define TQM_BD_INFO (*(tqm_bd_info_t *)TQM_BD_INFO_ADDR)

#define TQM_CONF_INFO_BLOCK_ADDR 0x4001fe00

#define IMAP_ADDR		((unsigned int)0xfa200000)
#define IMAP_SIZE		((unsigned int)(64 * 1024))

#endif /* __TQM_H__ */
