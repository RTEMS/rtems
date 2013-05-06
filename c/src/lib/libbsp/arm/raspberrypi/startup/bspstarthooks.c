/**
 * @file
 *
 * @ingroup raspberrypi
 *
 * @brief Startup code.
 */

/*
 * Copyright (c) 2013 by Alan Cudmore
 * based on work by:
 * Copyright (c) 2009
 * embedded brains GmbH
 * Obere Lagerstr. 30
 * D-82178 Puchheim
 * Germany
 * <rtems@embedded-brains.de>
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rtems.com/license/LICENSE
 */

#include <stdbool.h>

#include <bspopts.h>
#include <bsp/start.h>
#include <bsp/raspberrypi.h>
#include <bsp/mmu.h>
#include <bsp/linker-symbols.h>
#include <bsp/uart-output-char.h>

static void BSP_START_TEXT_SECTION clear_bss(void)
{
  const int *end = (const int *) bsp_section_bss_end;
  int *out = (int *) bsp_section_bss_begin;

  /* Clear BSS */
  while (out != end) {
    *out = 0;
    ++out;
  }
}

static void BSP_START_TEXT_SECTION raspberrypi_cache_setup(void)
{
  uint32_t ctrl = 0;

  /* Disable MMU and cache, basic settings */
  ctrl = arm_cp15_get_control();
  ctrl &= ~(ARM_CP15_CTRL_I | ARM_CP15_CTRL_R | ARM_CP15_CTRL_C
    | ARM_CP15_CTRL_V | ARM_CP15_CTRL_M);
  ctrl |= ARM_CP15_CTRL_S;
  arm_cp15_set_control(ctrl);

  arm_cp15_cache_invalidate();
  arm_cp15_tlb_invalidate();

}


void BSP_START_TEXT_SECTION bsp_start_hook_0(void)
{
  raspberrypi_cache_setup();
}


void BSP_START_TEXT_SECTION bsp_start_hook_1(void)
{

    /* Copy .text section */
    arm_cp15_instruction_cache_invalidate();
    bsp_start_memcpy(
                     (int *) bsp_section_text_begin,
                     (const int *) bsp_section_text_load_begin,
                     (size_t) bsp_section_text_size
                     );

    /* Copy .rodata section */
    arm_cp15_instruction_cache_invalidate();
    bsp_start_memcpy(
                     (int *) bsp_section_rodata_begin,
                     (const int *) bsp_section_rodata_load_begin,
                     (size_t) bsp_section_rodata_size
                     );

    /* Copy .data section */
    arm_cp15_instruction_cache_invalidate();
    bsp_start_memcpy(
                     (int *) bsp_section_data_begin,
                     (const int *) bsp_section_data_load_begin,
                     (size_t) bsp_section_data_size
                     );

    /* Copy .fast_text section */
    arm_cp15_instruction_cache_invalidate();
    bsp_start_memcpy(
                     (int *) bsp_section_fast_text_begin,
                     (const int *) bsp_section_fast_text_load_begin,
                     (size_t) bsp_section_fast_text_size
                     );

    /* Copy .fast_data section */
    arm_cp15_instruction_cache_invalidate();
    bsp_start_memcpy(
                     (int *) bsp_section_fast_data_begin,
                     (const int *) bsp_section_fast_data_load_begin,
                     (size_t) bsp_section_fast_data_size
                     );

  /* Clear .bss section */
  clear_bss();

}
