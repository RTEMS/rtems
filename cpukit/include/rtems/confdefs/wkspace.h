/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup RTEMSApplicationConfiguration
 *
 * @brief Evaluate Workspace Configuration Options
 */

/*
 * Copyright (C) 2020 embedded brains GmbH (http://www.embedded-brains.de)
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

#ifndef _RTEMS_CONFDEFS_WKSPACE_H
#define _RTEMS_CONFDEFS_WKSPACE_H

#ifndef __CONFIGURATION_TEMPLATE_h
#error "Do not include this file directly, use <rtems/confdefs.h> instead"
#endif

#ifdef CONFIGURE_INIT

#include <rtems/confdefs/bdbuf.h>
#include <rtems/confdefs/inittask.h>
#include <rtems/confdefs/initthread.h>
#include <rtems/confdefs/objectsposix.h>
#include <rtems/confdefs/threads.h>
#include <rtems/confdefs/wkspacesupport.h>
#include <rtems/score/coremsg.h>
#include <rtems/score/context.h>
#include <rtems/score/memory.h>
#include <rtems/score/stack.h>
#include <rtems/sysinit.h>

#ifdef CONFIGURE_TASK_STACK_FROM_ALLOCATOR
  #define _Configure_From_stackspace( _stack_size ) \
    CONFIGURE_TASK_STACK_FROM_ALLOCATOR( _stack_size + CONTEXT_FP_SIZE )
#else
  #define _Configure_From_stackspace( _stack_size ) \
    _Configure_From_workspace( _stack_size + CONTEXT_FP_SIZE )
#endif

#ifndef CONFIGURE_EXTRA_TASK_STACKS
  #define CONFIGURE_EXTRA_TASK_STACKS 0
#endif

#ifndef CONFIGURE_EXECUTIVE_RAM_SIZE

#define CONFIGURE_MESSAGE_BUFFERS_FOR_QUEUE( _messages, _size ) \
  _Configure_From_workspace( \
    ( _messages ) * ( _Configure_Align_up( _size, sizeof( uintptr_t ) ) \
        + sizeof( CORE_message_queue_Buffer_control ) ) )

#ifndef CONFIGURE_MESSAGE_BUFFER_MEMORY
  #define CONFIGURE_MESSAGE_BUFFER_MEMORY 0
#endif

#ifndef CONFIGURE_MEMORY_OVERHEAD
  #define CONFIGURE_MEMORY_OVERHEAD 0
#endif

/*
 * We must be able to split the free block used for the second last allocation
 * into two parts so that we have a free block for the last allocation.  See
 * _Heap_Block_split().
 */
#define _CONFIGURE_HEAP_HANDLER_OVERHEAD \
  _Configure_Align_up( HEAP_BLOCK_HEADER_SIZE, CPU_HEAP_ALIGNMENT )

#define CONFIGURE_EXECUTIVE_RAM_SIZE \
  ( _CONFIGURE_MEMORY_FOR_POSIX_OBJECTS \
    + CONFIGURE_MESSAGE_BUFFER_MEMORY \
    + 1024 * CONFIGURE_MEMORY_OVERHEAD \
    + _CONFIGURE_HEAP_HANDLER_OVERHEAD )

#define _CONFIGURE_STACK_SPACE_SIZE \
  ( _CONFIGURE_INIT_TASK_STACK_EXTRA \
    + _CONFIGURE_POSIX_INIT_THREAD_STACK_EXTRA \
    + _CONFIGURE_LIBBLOCK_TASKS_STACK_EXTRA \
    + CONFIGURE_EXTRA_TASK_STACKS \
    + rtems_resource_maximum_per_allocation( _CONFIGURE_TASKS ) \
      * _Configure_From_stackspace( CONFIGURE_MINIMUM_TASK_STACK_SIZE ) \
    + rtems_resource_maximum_per_allocation( CONFIGURE_MAXIMUM_POSIX_THREADS ) \
      * _Configure_From_stackspace( CONFIGURE_MINIMUM_POSIX_THREAD_STACK_SIZE ) \
    + _CONFIGURE_HEAP_HANDLER_OVERHEAD )

#else /* CONFIGURE_EXECUTIVE_RAM_SIZE */

#if CONFIGURE_EXTRA_TASK_STACKS != 0
  #error "CONFIGURE_EXECUTIVE_RAM_SIZE defined with request for CONFIGURE_EXTRA_TASK_STACKS"
#endif

#define _CONFIGURE_STACK_SPACE_SIZE 0

#endif /* CONFIGURE_EXECUTIVE_RAM_SIZE */

#ifdef __cplusplus
extern "C" {
#endif

const uintptr_t _Workspace_Size = CONFIGURE_EXECUTIVE_RAM_SIZE;

#ifdef CONFIGURE_UNIFIED_WORK_AREAS
  const bool _Workspace_Is_unified = true;
#endif

uint32_t rtems_minimum_stack_size = CONFIGURE_MINIMUM_TASK_STACK_SIZE;

const uintptr_t _Stack_Space_size = _CONFIGURE_STACK_SPACE_SIZE;

#if defined(CONFIGURE_TASK_STACK_ALLOCATOR) \
  && defined(CONFIGURE_TASK_STACK_DEALLOCATOR)
  #ifdef CONFIGURE_TASK_STACK_ALLOCATOR_AVOIDS_WORK_SPACE
    const bool _Stack_Allocator_avoids_workspace = true;
  #else
    const bool _Stack_Allocator_avoids_workspace = false;
  #endif

  #ifdef CONFIGURE_TASK_STACK_ALLOCATOR_INIT
    const Stack_Allocator_initialize _Stack_Allocator_initialize =
      CONFIGURE_TASK_STACK_ALLOCATOR_INIT;
  #else
    const Stack_Allocator_initialize _Stack_Allocator_initialize = NULL;
  #endif

  const Stack_Allocator_allocate _Stack_Allocator_allocate =
    CONFIGURE_TASK_STACK_ALLOCATOR;

  const Stack_Allocator_free _Stack_Allocator_free =
    CONFIGURE_TASK_STACK_DEALLOCATOR;
#elif defined(CONFIGURE_TASK_STACK_ALLOCATOR) \
  || defined(CONFIGURE_TASK_STACK_DEALLOCATOR)
  #error "CONFIGURE_TASK_STACK_ALLOCATOR and CONFIGURE_TASK_STACK_DEALLOCATOR must be both defined or both undefined"
#endif

#ifdef CONFIGURE_DIRTY_MEMORY
  RTEMS_SYSINIT_ITEM(
    _Memory_Dirty_free_areas,
    RTEMS_SYSINIT_DIRTY_MEMORY,
    RTEMS_SYSINIT_ORDER_MIDDLE
  );
#endif

#ifdef CONFIGURE_ZERO_WORKSPACE_AUTOMATICALLY
  const bool _Memory_Zero_before_use = true;

  RTEMS_SYSINIT_ITEM(
    _Memory_Zero_free_areas,
    RTEMS_SYSINIT_ZERO_MEMORY,
    RTEMS_SYSINIT_ORDER_MIDDLE
  );
#endif

#ifdef __cplusplus
}
#endif

#endif /* CONFIGURE_INIT */

#endif /* _RTEMS_CONFDEFS_WKSPACE_H */
