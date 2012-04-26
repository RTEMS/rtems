/**
 * @file
 *
 * @ingroup bsp_stack
 *
 * @brief Task stack initialization, allocation and free functions.
 */

/*
 * Copyright (c) 2009-2012 embedded brains GmbH.  All rights reserved.
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

#ifndef LIBBSP_SHARED_STACK_ALLOC_H
#define LIBBSP_SHARED_STACK_ALLOC_H

#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/**
 * @defgroup bsp_stack Task Stack Allocator
 *
 * @ingroup bsp_kit
 *
 * @brief Task stack initialization, allocation and free functions.
 *
 * Initialize the task stack allocator with bsp_stack_initialize().  To enable
 * the task stack allocator use the following in the system configuration:
 *
 * @code
 * #include <bsp/stackalloc.h>
 *
 * #define CONFIGURE_INIT
 *
 * #include <confdefs.h>
 * @endcode
 *
 * @{
 */

/**
 * @brief Task stack allocator initialization for
 * @ref CONFIGURE_TASK_STACK_ALLOCATOR_INIT.
 */
void bsp_stack_allocate_init(size_t stack_space_size);

/**
 * @brief Task stack allocator for @ref CONFIGURE_TASK_STACK_ALLOCATOR.
 *
 * In case the designated task stack space from bsp_stack_initialize() is
 * completely in use the work space will be used to allocate the stack.
 */
void *bsp_stack_allocate(size_t size);

/**
 * @brief Task stack free function for @ref CONFIGURE_TASK_STACK_DEALLOCATOR.
 */
void bsp_stack_free(void *stack);

/**
 * @brief Task stack allocator initialization configuration option.
 */
#define CONFIGURE_TASK_STACK_ALLOCATOR_INIT bsp_stack_allocate_init

/**
 * @brief Task stack allocator configuration option.
 */
#define CONFIGURE_TASK_STACK_ALLOCATOR bsp_stack_allocate

/**
 * @brief Task stack deallocator configuration option.
 */
#define CONFIGURE_TASK_STACK_DEALLOCATOR bsp_stack_free

/** @} */

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* LIBBSP_SHARED_STACK_ALLOC_H */
