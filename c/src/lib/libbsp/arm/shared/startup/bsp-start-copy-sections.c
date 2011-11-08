/*
 * Copyright (c) 2008-2011 embedded brains GmbH.  All rights reserved.
 *
 *  embedded brains GmbH
 *  Obere Lagerstr. 30
 *  82178 Puchheim
 *  Germany
 *  <rtems@embedded-brains.de>
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rtems.com/license/LICENSE.
 */

#include <bsp/start.h>
#include <bsp/linker-symbols.h>

static void BSP_START_TEXT_SECTION bsp_start_clear_bss(void)
{
  const int *end = (const int *) bsp_section_bss_end;
  int *out = (int *) bsp_section_bss_begin;

  /* Clear BSS */
  while (out != end) {
    *out = 0;
    ++out;
  }
}

void BSP_START_TEXT_SECTION bsp_start_copy_sections(void)
{
  /* Copy .text section */
  bsp_start_memcpy(
    (int *) bsp_section_text_begin,
    (const int *) bsp_section_text_load_begin,
    (size_t) bsp_section_text_size
  );

  /* Copy .rodata section */
  bsp_start_memcpy(
    (int *) bsp_section_rodata_begin,
    (const int *) bsp_section_rodata_load_begin,
    (size_t) bsp_section_rodata_size
  );

  /* Copy .data section */
  bsp_start_memcpy(
    (int *) bsp_section_data_begin,
    (const int *) bsp_section_data_load_begin,
    (size_t) bsp_section_data_size
  );

  /* Copy .fast_text section */
  bsp_start_memcpy(
    (int *) bsp_section_fast_text_begin,
    (const int *) bsp_section_fast_text_load_begin,
    (size_t) bsp_section_fast_text_size
  );

  /* Copy .fast_data section */
  bsp_start_memcpy(
    (int *) bsp_section_fast_data_begin,
    (const int *) bsp_section_fast_data_load_begin,
    (size_t) bsp_section_fast_data_size
  );

  bsp_start_clear_bss();
}
