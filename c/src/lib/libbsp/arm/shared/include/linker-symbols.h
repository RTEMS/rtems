/**
 * @file
 *
 * @brief Symbols defined in linker command base file.
 */

/*
 * Copyright (c) 2008
 * Embedded Brains GmbH
 * Obere Lagerstr. 30
 * D-82178 Puchheim
 * Germany
 * rtems@embedded-brains.de
 *
 * The license and distribution terms for this file may be found in the file
 * LICENSE in this distribution or at http://www.rtems.com/license/LICENSE.
 */

#ifndef LIBBSP_ARM_SHARED_LINKER_SYMBOLS_H
#define LIBBSP_ARM_SHARED_LINKER_SYMBOLS_H

#ifndef ASM
  #define LINKER_SYMBOL( sym) extern char sym [];
#else
  #define LINKER_SYMBOL( sym) .extern sym
#endif

LINKER_SYMBOL( bsp_stack_irq_size)
LINKER_SYMBOL( bsp_stack_irq_start)

LINKER_SYMBOL( bsp_stack_irq_size)
LINKER_SYMBOL( bsp_stack_fiq_start)

LINKER_SYMBOL( bsp_stack_abt_size)
LINKER_SYMBOL( bsp_stack_abt_start)

LINKER_SYMBOL( bsp_stack_undef_size)
LINKER_SYMBOL( bsp_stack_undef_start)

LINKER_SYMBOL( bsp_stack_svc_size)
LINKER_SYMBOL( bsp_stack_svc_start)

LINKER_SYMBOL( bsp_ram_int_start)
LINKER_SYMBOL( bsp_ram_int_end)
LINKER_SYMBOL( bsp_ram_int_size)

LINKER_SYMBOL( bsp_ram_ext_start)
LINKER_SYMBOL( bsp_ram_ext_load_start)
LINKER_SYMBOL( bsp_ram_ext_end)
LINKER_SYMBOL( bsp_ram_ext_size)

LINKER_SYMBOL( bsp_rom_start)
LINKER_SYMBOL( bsp_rom_end)
LINKER_SYMBOL( bsp_rom_size)

LINKER_SYMBOL( bsp_section_vector_start)
LINKER_SYMBOL( bsp_section_vector_end)
LINKER_SYMBOL( bsp_section_vector_size)

LINKER_SYMBOL( bsp_section_text_start)
LINKER_SYMBOL( bsp_section_text_end)
LINKER_SYMBOL( bsp_section_text_size)

LINKER_SYMBOL( bsp_section_data_start)
LINKER_SYMBOL( bsp_section_data_end)
LINKER_SYMBOL( bsp_section_data_size)

LINKER_SYMBOL( bsp_section_bss_start)
LINKER_SYMBOL( bsp_section_bss_end)
LINKER_SYMBOL( bsp_section_bss_size)

LINKER_SYMBOL( bsp_section_stack_start)
LINKER_SYMBOL( bsp_section_stack_end)
LINKER_SYMBOL( bsp_section_stack_size)

LINKER_SYMBOL( bsp_section_work_area_start)
LINKER_SYMBOL( bsp_section_work_area_end)
LINKER_SYMBOL( bsp_section_work_area_size)

#endif /* LIBBSP_ARM_SHARED_LINKER_SYMBOLS_H */
