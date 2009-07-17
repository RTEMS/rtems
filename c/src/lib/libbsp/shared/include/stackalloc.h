/**
 * @file
 *
 * @ingroup bsp_shared
 *
 * @brief Stack initialization, allocation and free functions.
 */

/*
 * Copyright (c) 2009
 * embedded brains GmbH
 * Obere Lagerstr. 30
 * D-82178 Puchheim
 * Germany
 * rtems@embedded-brains.de
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rtems.com/license/LICENSE.
 */

#ifndef LIBBSP_SHARED_STACK_ALLOC_H
#define LIBBSP_SHARED_STACK_ALLOC_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

void bsp_stack_initialize(void *start, intptr_t size);

void *bsp_stack_allocate(uint32_t size);

void bsp_stack_free(void *stack);

#define CONFIGURE_TASK_STACK_ALLOCATOR bsp_stack_allocate

#define CONFIGURE_TASK_STACK_DEALLOCATOR bsp_stack_free

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* LIBBSP_SHARED_STACK_ALLOC_H */
