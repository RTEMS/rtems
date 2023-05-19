/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup arm_start
 *
 * @brief ARM system low level start.
 */

/*
 * Copyright (C) 2008, 2013 embedded brains GmbH & Co. KG
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
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
* @brief Can be used by bsp_start_hook_0() to jump back to the start code
*   instead of using the link register.
*/
void bsp_start_hook_0_done(void);

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
