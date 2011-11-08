/**
 * @file
 *
 * @ingroup bsp_start
 *
 * @brief System low level start.
 */

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

#ifndef LIBBSP_ARM_SHARED_START_H
#define LIBBSP_ARM_SHARED_START_H

#include <stddef.h>

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
void bsp_start_copy_sections(void);

/** @} */

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* LIBBSP_ARM_SHARED_START_H */
