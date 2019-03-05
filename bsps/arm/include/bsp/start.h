/**
 * @file
 *
 * @ingroup arm_start
 *
 * @brief ARM system low level start.
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
 * http://www.rtems.org/license/LICENSE.
 */

#ifndef LIBBSP_ARM_SHARED_START_H
#define LIBBSP_ARM_SHARED_START_H

#include <string.h>

#include <bsp/linker-symbols.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/**
 * @defgroup arm_start System Start
 *
 * @ingroup RTEMSBSPsARMShared
 *
 * @brief ARM system low level start.
 *
 * @{
 */

#define BSP_START_TEXT_SECTION __attribute__((section(".bsp_start_text")))

#define BSP_START_DATA_SECTION __attribute__((section(".bsp_start_data")))

/*
* Many ARM boot loaders pass arguments to loaded OS kernel
*/
#ifdef BSP_START_HOOKS_WITH_LOADER_ARGS
#define BSP_START_HOOKS_LOADER_ARGS int saved_psr, int saved_machid, int saved_dtb_adr
#else
#define BSP_START_HOOKS_LOADER_ARGS void
#endif

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
void bsp_start_hook_0(BSP_START_HOOKS_LOADER_ARGS);

/**
* @brief Start entry hook 1.
*
* This hook will be called from the start entry code after copying of the
* exception vectors but before the call to boot_card().
*/
void bsp_start_hook_1(BSP_START_HOOKS_LOADER_ARGS);

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

BSP_START_TEXT_SECTION static inline void
bsp_start_memcpy_libc(void *dest, const void *src, size_t n)
{
  if (dest != src) {
    memcpy(dest, src, n);
  }
}

/**
 * @brief Copies the .data, .fast_text and .fast_data sections from the load to
 * the runtime area using the C library memcpy().
 *
 * Works only in case the .start, .text and .rodata sections reside in one
 * memory region.
 */
BSP_START_TEXT_SECTION static inline void bsp_start_copy_sections_compact(void)
{
  /* Copy .data section */
  bsp_start_memcpy_libc(
    bsp_section_data_begin,
    bsp_section_data_load_begin,
    (size_t) bsp_section_data_size
  );

  /* Copy .fast_text section */
  bsp_start_memcpy_libc(
    bsp_section_fast_text_begin,
    bsp_section_fast_text_load_begin,
    (size_t) bsp_section_fast_text_size
  );

  /* Copy .fast_data section */
  bsp_start_memcpy_libc(
    bsp_section_fast_data_begin,
    bsp_section_fast_data_load_begin,
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
