/*
 * Copyright (c) 2011 Sebastian Huber.  All rights reserved.
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

#include <bspopts.h>
#include <bsp/start.h>
#include <bsp/linker-symbols.h>

static void BSP_START_TEXT_SECTION copy(int *dst, const int *src, int n)
{
  if (src != dst) {
    const int *end = dst + (n + sizeof(int) - 1) / sizeof(int);

    while (dst != end) {
      *dst = *src;
      ++src;
      ++dst;
    }
  }
}

static void BSP_START_TEXT_SECTION clear_bss(void)
{
  int *dst = (int *) bsp_section_bss_begin;
  int n = (int) bsp_section_bss_size;
  const int *end = dst + (n + sizeof(int) - 1) / sizeof(int);

  while (dst != end) {
    *dst = 0;
    ++dst;
  }
}

void BSP_START_TEXT_SECTION bsp_start_hook_0(void)
{
  /* Copy .text section */
  copy(
    (int *) bsp_section_text_begin,
    (const int *) bsp_section_text_load_begin,
    (size_t) bsp_section_text_size
  );

  /* Copy .rodata section */
  copy(
    (int *) bsp_section_rodata_begin,
    (const int *) bsp_section_rodata_load_begin,
    (size_t) bsp_section_rodata_size
  );

  /* Copy .data section */
  copy(
    (int *) bsp_section_data_begin,
    (const int *) bsp_section_data_load_begin,
    (size_t) bsp_section_data_size
  );

  /* Copy .fast_text section */
  copy(
    (int *) bsp_section_fast_text_begin,
    (const int *) bsp_section_fast_text_load_begin,
    (size_t) bsp_section_fast_text_size
  );

  /* Copy .fast_data section */
  copy(
    (int *) bsp_section_fast_data_begin,
    (const int *) bsp_section_fast_data_load_begin,
    (size_t) bsp_section_fast_data_size
  );

  /* Clear .bss section */
  clear_bss();

  /* At this point we can use objects outside the .start section */
}
