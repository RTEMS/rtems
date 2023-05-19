/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup bsp_stack
 *
 * @brief Task stack initialization, allocation and free functions.
 */

/*
 * Copyright (C) 2009, 2012 embedded brains GmbH & Co. KG
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
 * @ingroup RTEMSBSPsShared
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
