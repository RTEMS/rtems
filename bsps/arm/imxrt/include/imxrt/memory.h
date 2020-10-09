/* SPDX-License-Identifier: BSD-2-Clause */

/*
 * Copyright (C) 2020 embedded brains GmbH (http://www.embedded-brains.de)
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
 * ARE DISCLAIMED. IN NO EVENT SMEMORYL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef LIBBSP_ARM_IMXRT_IMXRT_MEMORY_H
#define LIBBSP_ARM_IMXRT_IMXRT_MEMORY_H

#ifdef __cplusplus
extern "C" {
#endif

extern char imxrt_memory_null_begin[];
extern char imxrt_memory_null_end[];
extern char imxrt_memory_null_size[];

extern char imxrt_memory_itcm_begin[];
extern char imxrt_memory_itcm_end[];
extern char imxrt_memory_itcm_size[];

extern char imxrt_memory_dtcm_begin[];
extern char imxrt_memory_dtcm_end[];
extern char imxrt_memory_dtcm_size[];

extern char imxrt_memory_ocram_begin[];
extern char imxrt_memory_ocram_end[];
extern char imxrt_memory_ocram_size[];

extern char imxrt_memory_peripheral_begin[];
extern char imxrt_memory_peripheral_end[];
extern char imxrt_memory_peripheral_size[];

extern char imxrt_memory_flexspi_config_begin[];
extern char imxrt_memory_flexspi_config_end[];
extern char imxrt_memory_flexspi_config_size[];

extern char imxrt_memory_flexspi_ivt_begin[];
extern char imxrt_memory_flexspi_ivt_end[];
extern char imxrt_memory_flexspi_ivt_size[];

extern char imxrt_memory_flexspi_begin[];
extern char imxrt_memory_flexspi_end[];
extern char imxrt_memory_flexspi_size[];

extern char imxrt_memory_flexspi_fifo_begin[];
extern char imxrt_memory_flexspi_fifo_end[];
extern char imxrt_memory_flexspi_fifo_size[];

extern char imxrt_memory_sdram_begin[];
extern char imxrt_memory_sdram_end[];
extern char imxrt_memory_sdram_size[];

extern char imxrt_memory_sdram_nocache_begin[];
extern char imxrt_memory_sdram_nocache_end[];
extern char imxrt_memory_sdram_nocache_size[];

#ifdef __cplusplus
}
#endif

#endif /* LIBBSP_ARM_IMXRT_IMXRT_MEMORY_H */
