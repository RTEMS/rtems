/**
 * @file
 *
 * @ingroup bsp_linker
 *
 * @brief Symbols defined in linker command base file.
 */

/*
 * Copyright (c) 2008, 2009
 * embedded brains GmbH
 * Obere Lagerstr. 30
 * D-82178 Puchheim
 * Germany
 * <rtems@embedded-brains.de>
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rtems.com/license/LICENSE.
 */

#ifndef LIBBSP_ARM_SHARED_LINKER_SYMBOLS_H
#define LIBBSP_ARM_SHARED_LINKER_SYMBOLS_H

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/**
 * @defgroup bsp_linker Linker Support
 *
 * @ingroup bsp_kit
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

LINKER_SYMBOL(bsp_stack_irq_begin)
LINKER_SYMBOL(bsp_stack_irq_end)
LINKER_SYMBOL(bsp_stack_irq_size)

LINKER_SYMBOL(bsp_stack_fiq_begin)
LINKER_SYMBOL(bsp_stack_fiq_end)
LINKER_SYMBOL(bsp_stack_irq_size)

LINKER_SYMBOL(bsp_stack_abt_begin)
LINKER_SYMBOL(bsp_stack_abt_end)
LINKER_SYMBOL(bsp_stack_abt_size)

LINKER_SYMBOL(bsp_stack_und_begin)
LINKER_SYMBOL(bsp_stack_und_end)
LINKER_SYMBOL(bsp_stack_und_size)

LINKER_SYMBOL(bsp_stack_svc_begin)
LINKER_SYMBOL(bsp_stack_svc_end)
LINKER_SYMBOL(bsp_stack_svc_size)

LINKER_SYMBOL(bsp_section_start_begin)
LINKER_SYMBOL(bsp_section_start_end)
LINKER_SYMBOL(bsp_section_start_size)

LINKER_SYMBOL(bsp_section_vector_begin)
LINKER_SYMBOL(bsp_section_vector_end)
LINKER_SYMBOL(bsp_section_vector_size)

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

LINKER_SYMBOL(bsp_section_data_begin)
LINKER_SYMBOL(bsp_section_data_end)
LINKER_SYMBOL(bsp_section_data_size)
LINKER_SYMBOL(bsp_section_data_load_begin)
LINKER_SYMBOL(bsp_section_data_load_end)

LINKER_SYMBOL(bsp_section_fast_text_begin)
LINKER_SYMBOL(bsp_section_fast_text_end)
LINKER_SYMBOL(bsp_section_fast_text_size)
LINKER_SYMBOL(bsp_section_fast_text_load_begin)
LINKER_SYMBOL(bsp_section_fast_text_load_end)

LINKER_SYMBOL(bsp_section_fast_data_begin)
LINKER_SYMBOL(bsp_section_fast_data_end)
LINKER_SYMBOL(bsp_section_fast_data_size)
LINKER_SYMBOL(bsp_section_fast_data_load_begin)
LINKER_SYMBOL(bsp_section_fast_data_load_end)

LINKER_SYMBOL(bsp_section_bss_begin)
LINKER_SYMBOL(bsp_section_bss_end)
LINKER_SYMBOL(bsp_section_bss_size)

LINKER_SYMBOL(bsp_section_work_begin)
LINKER_SYMBOL(bsp_section_work_end)
LINKER_SYMBOL(bsp_section_work_size)

LINKER_SYMBOL(bsp_section_stack_begin)
LINKER_SYMBOL(bsp_section_stack_end)
LINKER_SYMBOL(bsp_section_stack_size)

LINKER_SYMBOL(bsp_vector_table_begin)
LINKER_SYMBOL(bsp_vector_table_end)
LINKER_SYMBOL(bsp_vector_table_size)

LINKER_SYMBOL(bsp_start_vector_table_begin)
LINKER_SYMBOL(bsp_start_vector_table_end)
LINKER_SYMBOL(bsp_start_vector_table_size)

#define BSP_FAST_TEXT_SECTION __attribute__((section(".bsp_fast_text")))

#define BSP_FAST_DATA_SECTION __attribute__((section(".bsp_fast_data")))

/** @} */

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* LIBBSP_ARM_SHARED_LINKER_SYMBOLS_H */
