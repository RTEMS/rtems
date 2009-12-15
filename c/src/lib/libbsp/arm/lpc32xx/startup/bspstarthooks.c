/**
 * @file
 *
 * @ingroup lpc32xx
 *
 * @brief Startup code.
 */

/*
 * Copyright (c) 2009
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

#include <stdbool.h>

#include <bspopts.h>
#include <bsp/start.h>
#include <bsp/lpc32xx.h>
#include <bsp/linker-symbols.h>

#define BSP_START_SECTION __attribute__((section(".bsp_start")))

static void BSP_START_SECTION lpc32xx_clear_bss(void)
{
  const int *end = (const int *) bsp_section_bss_end;
  int *out = (int *) bsp_section_bss_begin;

  /* Clear BSS */
  while (out != end) {
    *out = 0;
    ++out;
  }
}

void BSP_START_SECTION bsp_start_hook_0(void)
{
  /* TODO */
}

void BSP_START_SECTION bsp_start_hook_1(void)
{
  /* TODO */

  /* Copy .text section */
  bsp_start_memcpy_arm(
    (int *) bsp_section_text_begin,
    (const int *) bsp_section_text_load_begin,
    (size_t) bsp_section_text_size
  );

  /* Copy .rodata section */
  bsp_start_memcpy_arm(
    (int *) bsp_section_rodata_begin,
    (const int *) bsp_section_rodata_load_begin,
    (size_t) bsp_section_rodata_size
  );

  /* Copy .data section */
  bsp_start_memcpy_arm(
    (int *) bsp_section_data_begin,
    (const int *) bsp_section_data_load_begin,
    (size_t) bsp_section_data_size
  );

  /* Copy .fast section */
  bsp_start_memcpy_arm(
    (int *) bsp_section_fast_begin,
    (const int *) bsp_section_fast_load_begin,
    (size_t) bsp_section_fast_size
  );

  /* Clear .bss section */
  lpc32xx_clear_bss();

  /* At this point we can use objects outside the .start section */
}
