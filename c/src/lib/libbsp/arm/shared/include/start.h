/**
 * @file
 *
 * @ingroup bsp_start
 *
 * @brief System low level start.
 */

/*
 * Copyright (c) 2008-2013 embedded brains GmbH.  All rights reserved.
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

#ifndef LIBBSP_ARM_SHARED_START_H
#define LIBBSP_ARM_SHARED_START_H

#include <string.h>

#include <bsp/linker-symbols.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/**
 * @defgroup bsp_start System Start
 *
 * @ingroup bsp_kit
 *
 * @brief System low level start.
 *
 * @{
 */

#define BSP_START_TEXT_SECTION __attribute__((section(".bsp_start_text")))

#define BSP_START_DATA_SECTION __attribute__((section(".bsp_start_data")))

/**
* @brief System start entry.
*/
void _start(void);

/**
* @brief Start entry hook 0.
*
* This hook will be called from the start entry code after all modes and
* stack pointers are initialized but before the copying of the exception
* vectors.
*/
void bsp_start_hook_0(void);

/**
* @brief Start entry hook 1.
*
* This hook will be called from the start entry code after copying of the
* exception vectors but before the call to boot_card().
*/
void bsp_start_hook_1(void);

/**
 * @brief Similar to standard memcpy().
 *
 * The memory areas must be word aligned.  Copy code will be executed from the
 * stack.  If @a dest equals @a src nothing will be copied.
 */
void bsp_start_memcpy(int *dest, const int *src, size_t n);

/**
 * @brief ARM entry point to bsp_start_memcpy().
 */
void bsp_start_memcpy_arm(int *dest, const int *src, size_t n);

/**
 * @brief Copies all standard sections from the load to the runtime area.
 */
BSP_START_TEXT_SECTION static inline void bsp_start_copy_sections(void)
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

  /* Copy .ARM.exidx section */
  bsp_start_memcpy(
    (int *) bsp_section_armexidx_begin,
    (const int *) bsp_section_armexidx_load_begin,
    (size_t) bsp_section_armexidx_size
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
}

BSP_START_TEXT_SECTION static inline void bsp_start_clear_bss(void)
{
  memset(bsp_section_bss_begin, 0, (size_t) bsp_section_bss_size);
}

/** @} */

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* LIBBSP_ARM_SHARED_START_H */
