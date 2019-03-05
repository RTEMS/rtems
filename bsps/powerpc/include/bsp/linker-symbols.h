/**
 * @file
 *
 * @ingroup powerpc_linker
 *
 * @brief Symbols defined in linker command base file.
 */

/*
 * Copyright (c) 2010, 2016 embedded brains GmbH.  All rights reserved.
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
