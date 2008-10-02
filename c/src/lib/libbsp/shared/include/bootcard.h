/**
 * @file
 *
 * @ingroup bsp_shared
 *
 * @brief Header file for basic BSP startup functions.
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
 *
 * $Id$
 */

/**
 * @defgroup bsp_shared Shared BSP Code
 */

#ifndef LIBBSP_SHARED_BOOTCARD_H
#define LIBBSP_SHARED_BOOTCARD_H

#include <stddef.h>
#include <stdint.h>
#include <sys/types.h>

#include <bspopts.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

void bsp_start(void);

void bsp_pretasking_hook(void);

void bsp_predriver_hook(void);

void bsp_postdriver_hook(void);

void bsp_cleanup(void);

void bsp_reset(void);

#define BSP_BOOTCARD_HEAP_USES_WORK_AREA NULL

#define BSP_BOOTCARD_HEAP_SIZE_DEFAULT 0

void bsp_get_work_area(
  void    **work_area_start,
  ssize_t  *work_area_size,
  void    **heap_start,
  ssize_t  *heap_size
);

int boot_card( int argc, char **argv, char **envp);

void bsp_libc_init( void *heap_start, size_t heap_size, size_t sbrk_amount);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* LIBBSP_SHARED_BOOTCARD_H */
