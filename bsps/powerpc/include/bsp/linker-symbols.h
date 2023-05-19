/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup powerpc_linker
 *
 * @brief Symbols defined in linker command base file.
 */

/*
 * Copyright (C) 2010, 2016 embedded brains GmbH & Co. KG
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

#ifndef LIBBSP_POWERPC_SHARED_LINKER_SYMBOLS_H
#define LIBBSP_POWERPC_SHARED_LINKER_SYMBOLS_H

#include <libcpu/powerpc-utility.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/**
 * @defgroup RTEMSBSPsPowerPCSharedLinker Linker Support
 *
 * @ingroup RTEMSBSPsPowerPCShared
 *
 * @brief Linker support.
 *
 * @{
 */

LINKER_SYMBOL(bsp_section_start_begin)
LINKER_SYMBOL(bsp_section_start_end)
LINKER_SYMBOL(bsp_section_start_size)

LINKER_SYMBOL(bsp_section_fast_text_begin)
LINKER_SYMBOL(bsp_section_fast_text_end)
LINKER_SYMBOL(bsp_section_fast_text_size)
LINKER_SYMBOL(bsp_section_fast_text_load_begin)
LINKER_SYMBOL(bsp_section_fast_text_load_end)

LINKER_SYMBOL(bsp_section_text_begin)
LINKER_SYMBOL(bsp_section_text_end)
LINKER_SYMBOL(bsp_section_text_size)
LINKER_SYMBOL(bsp_section_text_load_begin)
LINKER_SYMBOL(bsp_section_text_load_end)

LINKER_SYMBOL(bsp_section_rodata_begin)
LINKER_SYMBOL(bsp_section_rodata_end)
LINKER_SYMBOL(bsp_section_rodata_size)
LINKER_SYMBOL(bsp_section_rodata_load_begin)
LINKER_SYMBOL(bsp_section_rodata_load_end)

LINKER_SYMBOL(bsp_section_fast_data_begin)
LINKER_SYMBOL(bsp_section_fast_data_end)
LINKER_SYMBOL(bsp_section_fast_data_size)
LINKER_SYMBOL(bsp_section_fast_data_load_begin)
LINKER_SYMBOL(bsp_section_fast_data_load_end)

LINKER_SYMBOL(bsp_section_data_begin)
LINKER_SYMBOL(bsp_section_data_end)
LINKER_SYMBOL(bsp_section_data_size)
LINKER_SYMBOL(bsp_section_data_load_begin)
LINKER_SYMBOL(bsp_section_data_load_end)

LINKER_SYMBOL(bsp_section_bss_begin)
LINKER_SYMBOL(bsp_section_bss_end)
LINKER_SYMBOL(bsp_section_bss_size)

LINKER_SYMBOL(bsp_section_sdata_begin)
LINKER_SYMBOL(bsp_section_sdata_end)
LINKER_SYMBOL(bsp_section_sdata_size)

LINKER_SYMBOL(bsp_section_sbss_begin)
LINKER_SYMBOL(bsp_section_sbss_end)
LINKER_SYMBOL(bsp_section_sbss_size)

LINKER_SYMBOL(bsp_section_sdata_libdl_begin)
LINKER_SYMBOL(bsp_section_sdata_libdl_end)
LINKER_SYMBOL(bsp_section_sdata_libdl_size)

LINKER_SYMBOL(bsp_section_rtemsstack_begin)
LINKER_SYMBOL(bsp_section_rtemsstack_end)
LINKER_SYMBOL(bsp_section_rtemsstack_size)

LINKER_SYMBOL(bsp_section_noinit_begin)
LINKER_SYMBOL(bsp_section_noinit_end)
LINKER_SYMBOL(bsp_section_noinit_size)

LINKER_SYMBOL(bsp_section_work_begin)
LINKER_SYMBOL(bsp_section_work_end)
LINKER_SYMBOL(bsp_section_work_size)

LINKER_SYMBOL(bsp_section_stack_begin)
LINKER_SYMBOL(bsp_section_stack_end)
LINKER_SYMBOL(bsp_section_stack_size)

LINKER_SYMBOL(bsp_section_nocache_begin)
LINKER_SYMBOL(bsp_section_nocache_end)
LINKER_SYMBOL(bsp_section_nocache_size)
LINKER_SYMBOL(bsp_section_nocache_load_begin)
LINKER_SYMBOL(bsp_section_nocache_load_end)

LINKER_SYMBOL(bsp_section_nocachenoload_begin)
LINKER_SYMBOL(bsp_section_nocachenoload_end)
LINKER_SYMBOL(bsp_section_nocachenoload_size)

LINKER_SYMBOL(bsp_section_nocacheheap_begin)
LINKER_SYMBOL(bsp_section_nocacheheap_end)
LINKER_SYMBOL(bsp_section_nocacheheap_size)

LINKER_SYMBOL(bsp_section_nvram_begin)
LINKER_SYMBOL(bsp_section_nvram_end)
LINKER_SYMBOL(bsp_section_nvram_size)

#define BSP_FAST_TEXT_SECTION __attribute__((section(".bsp_fast_text")))

#define BSP_FAST_DATA_SECTION __attribute__((section(".bsp_fast_data")))

#define BSP_NOCACHE_SECTION __attribute__((section(".bsp_nocache")))

#define BSP_NOCACHE_SUBSECTION(subsection) \
  __attribute__((section(".bsp_nocache." # subsection)))

#define BSP_NOCACHENOLOAD_SECTION __attribute__((section(".bsp_noload_nocache")))

#define BSP_NOCACHENOLOAD_SUBSECTION(subsection) \
  __attribute__((section(".bsp_noload_nocache." # subsection)))

#define BSP_NVRAM_SECTION __attribute__((section(".bsp_nvram")))

#define BSP_NVRAM_SUBSECTION(subsection) \
  __attribute__((section(".bsp_nvram." # subsection)))

/** @} */

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* LIBBSP_POWERPC_SHARED_LINKER_SYMBOLS_H */
