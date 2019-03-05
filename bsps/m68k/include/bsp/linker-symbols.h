/**
 * @file
 *
 * @ingroup bsp_linker
 *
 * @brief Symbols defined in linker command base file.
 */

/*
 * Copyright (c) 2008-2013 embedded brains GmbH.  All rights reserved.
 *
 *  embedded brains GmbH
 *  Dornierstr. 4
 *  82178 Puchheim
 *  Germany
 *  <info@embedded-brains.de>
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rtems.org/license/LICENSE.
 */

#ifndef LIBBSP_M68K_SHARED_LINKER_SYMBOLS_H
#define LIBBSP_M68K_SHARED_LINKER_SYMBOLS_H

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/**
 * @defgroup RTEMSBSPsM68kSharedLinker Linker Support
 *
 * @ingroup RTEMSBSPsM68kShared
 *
 * @brief Linker support.
 *
 * @{
 */

#ifndef ASM
  #define LINKER_SYMBOL(sym) extern char sym [];
#else
  #define LINKER_SYMBOL(sym) .extern sym
#endif

LINKER_SYMBOL(bsp_vector0_begin)
LINKER_SYMBOL(bsp_vector0_end)
LINKER_SYMBOL(bsp_vector0_size)

LINKER_SYMBOL(bsp_vector1_begin)
LINKER_SYMBOL(bsp_vector1_end)
LINKER_SYMBOL(bsp_vector1_size)

LINKER_SYMBOL(bsp_section_text_begin)
LINKER_SYMBOL(bsp_section_text_end)
LINKER_SYMBOL(bsp_section_text_size)
LINKER_SYMBOL(bsp_section_text_load_begin)
LINKER_SYMBOL(bsp_section_text_load_end)

LINKER_SYMBOL(bsp_section_data_begin)
LINKER_SYMBOL(bsp_section_data_end)
LINKER_SYMBOL(bsp_section_data_size)
LINKER_SYMBOL(bsp_section_data_load_begin)
LINKER_SYMBOL(bsp_section_data_load_end)

LINKER_SYMBOL(bsp_section_bss_begin)
LINKER_SYMBOL(bsp_section_bss_end)
LINKER_SYMBOL(bsp_section_bss_size)

LINKER_SYMBOL(bsp_section_work_begin)
LINKER_SYMBOL(bsp_section_work_end)
LINKER_SYMBOL(bsp_section_work_size)

LINKER_SYMBOL(bsp_initstack_begin)
LINKER_SYMBOL(bsp_initstack_end)
LINKER_SYMBOL(bsp_initstack_size)

/** @} */

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* LIBBSP_M68K_SHARED_LINKER_SYMBOLS_H */
