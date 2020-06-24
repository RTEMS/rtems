/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @brief This header file defines support services of the API.
 */

/*
 * Copyright (C) 2020, 2021 embedded brains GmbH (http://www.embedded-brains.de)
 * Copyright (C) 1988, 2008 On-Line Applications Research Corporation (OAR)
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

/*
 * This file is part of the RTEMS quality process and was automatically
 * generated.  If you find something that needs to be fixed or
 * worded better please post a report or patch to an RTEMS mailing list
 * or raise a bug report:
 *
 * https://www.rtems.org/bugs.html
 *
 * For information on updating and regenerating please refer to the How-To
 * section in the Software Requirements Engineering chapter of the
 * RTEMS Software Engineering manual.  The manual is provided as a part of
 * a release.  For development sources please refer to the online
 * documentation at:
 *
 * https://docs.rtems.org
 */

/* Generated from spec:/rtems/support/if/header */

#ifndef _RTEMS_RTEMS_SUPPORT_H
#define _RTEMS_RTEMS_SUPPORT_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <rtems/config.h>
#include <rtems/rtems/types.h>
#include <rtems/score/heapinfo.h>

#ifdef __cplusplus
extern "C" {
#endif

/* Generated from spec:/rtems/support/if/group */

/**
 * @defgroup RTEMSAPIClassicSupport Support Services
 *
 * @ingroup RTEMSAPIClassic
 *
 * @brief Items of this group should move to other groups.
 */

/* Generated from spec:/rtems/support/if/is-name-valid */

/**
 * @ingroup RTEMSAPIClassicSupport
 *
 * @brief Checks if the object name is valid.
 *
 * @param name is the object name to check.
 *
 * @return Returns true, if the object name is valid, otherwise false.
 *
 * @par Constraints
 * @parblock
 * The following constraints apply to this directive:
 *
 * * The directive may be called from within any runtime context.
 *
 * * The directive will not cause the calling task to be preempted.
 * @endparblock
 */
static inline bool rtems_is_name_valid( rtems_name name )
{
  return name != 0;
}

/* Generated from spec:/rtems/support/if/microseconds-to-ticks */

/**
 * @ingroup RTEMSAPIClassicSupport
 *
 * @brief Gets the number of clock ticks for the microseconds value.
 *
 * @param _us is the microseconds value to convert to clock ticks.
 *
 * @return Returns the number of clock ticks for the specified microseconds
 *   value.
 *
 * @par Notes
 * The number of clock ticks per second is defined by the
 * #CONFIGURE_MICROSECONDS_PER_TICK application configuration option.
 *
 * @par Constraints
 * @parblock
 * The following constraints apply to this directive:
 *
 * * The directive may be called from within any runtime context.
 *
 * * The directive will not cause the calling task to be preempted.
 * @endparblock
 */
#define RTEMS_MICROSECONDS_TO_TICKS( _us ) \
  ( ( _us ) / rtems_configuration_get_microseconds_per_tick() )

/* Generated from spec:/rtems/support/if/milliseconds-to-microseconds */

/**
 * @ingroup RTEMSAPIClassicSupport
 *
 * @brief Gets the number of microseconds for the milliseconds value.
 *
 * @param _ms is the milliseconds value to convert to microseconds.
 *
 * @return Returns the number of microseconds for the milliseconds value.
 *
 * @par Constraints
 * @parblock
 * The following constraints apply to this directive:
 *
 * * The directive is implemented by a macro and may be called from within
 *   C/C++ constant expressions.  In addition, a function implementation of the
 *   directive exists for bindings to other programming languages.
 *
 * * The directive will not cause the calling task to be preempted.
 * @endparblock
 */
#define RTEMS_MILLISECONDS_TO_MICROSECONDS( _ms ) ( ( _ms ) * 1000UL )

/* Generated from spec:/rtems/support/if/milliseconds-to-ticks */

/**
 * @ingroup RTEMSAPIClassicSupport
 *
 * @brief Gets the number of clock ticks for the milliseconds value.
 *
 * @param _ms is the milliseconds value to convert to clock ticks.
 *
 * @return Returns the number of clock ticks for the milliseconds value.
 *
 * @par Notes
 * The number of clock ticks per second is defined by the
 * #CONFIGURE_MICROSECONDS_PER_TICK application configuration option.
 *
 * @par Constraints
 * @parblock
 * The following constraints apply to this directive:
 *
 * * The directive may be called from within any runtime context.
 *
 * * The directive will not cause the calling task to be preempted.
 * @endparblock
 */
#define RTEMS_MILLISECONDS_TO_TICKS( _ms ) \
  RTEMS_MICROSECONDS_TO_TICKS( RTEMS_MILLISECONDS_TO_MICROSECONDS( _ms ) )

/* Generated from spec:/rtems/support/if/name-to-characters */

/**
 * @ingroup RTEMSAPIClassicSupport
 *
 * @brief Breaks the object name into the four component characters.
 *
 * @param name is the object name to break into four component characters.
 *
 * @param[out] c1 is the first character of the object name.
 *
 * @param[out] c2 is the second character of the object name.
 *
 * @param[out] c3 is the third character of the object name.
 *
 * @param[out] c4 is the fourth character of the object name.
 *
 * @par Constraints
 * @parblock
 * The following constraints apply to this directive:
 *
 * * The directive may be called from within any runtime context.
 *
 * * The directive will not cause the calling task to be preempted.
 * @endparblock
 */
static inline void rtems_name_to_characters(
  rtems_name name,
  char      *c1,
  char      *c2,
  char      *c3,
  char      *c4
)
{
  *c1 = (char) ( ( ( name ) >> 24 ) & 0xff );
  *c2 = (char) ( ( ( name ) >> 16 ) & 0xff );
  *c3 = (char) ( ( ( name ) >> 8 ) & 0xff );
  *c4 = (char) ( ( name ) & 0xff );
}

/* Generated from spec:/rtems/support/if/workspace-allocate */

/**
 * @ingroup RTEMSAPIClassicSupport
 *
 * @brief Allocates a memory area from the RTEMS Workspace.
 *
 * @param bytes is the number of bytes to allocated.
 *
 * @param[out] pointer is the pointer to a pointer variable.  When the
 *   directive call is successful, the begin address of the allocated memory
 *   area will be stored in this variable.
 *
 * @return Returns true, if the allocation was successful, otherwise false.
 *
 * @par Notes
 * This directive is intended to be used by tests of the RTEMS test suites.
 *
 * @par Constraints
 * @parblock
 * The following constraints apply to this directive:
 *
 * * The directive may be called from within device driver initialization
 *   context.
 *
 * * The directive may be called from within task context.
 *
 * * The directive may obtain and release the object allocator mutex.  This may
 *   cause the calling task to be preempted.
 * @endparblock
 */
bool rtems_workspace_allocate( size_t bytes, void **pointer );

/* Generated from spec:/rtems/support/if/workspace-free */

/**
 * @ingroup RTEMSAPIClassicSupport
 *
 * @brief Frees the memory area allocated from the RTEMS Workspace.
 *
 * @param pointer is the begin address of the memory area to free.
 *
 * @return Returns true, if freeing the memory area was successful, otherwise
 *   false.
 *
 * @par Constraints
 * @parblock
 * The following constraints apply to this directive:
 *
 * * The directive may be called from within device driver initialization
 *   context.
 *
 * * The directive may be called from within task context.
 *
 * * The directive may obtain and release the object allocator mutex.  This may
 *   cause the calling task to be preempted.
 * @endparblock
 */
bool rtems_workspace_free( void *pointer );

/* Generated from spec:/rtems/support/if/workspace-get-information */

/**
 * @ingroup RTEMSAPIClassicSupport
 *
 * @brief Gets information about the RTEMS Workspace.
 *
 * @param the_info is the pointer to a heap information variable.  When the
 *   directive call is successful, the heap information will be stored in this
 *   variable.
 *
 * @return Returns true, if getting the information was successful, otherwise
 *   false.
 *
 * @par Constraints
 * @parblock
 * The following constraints apply to this directive:
 *
 * * The directive may be called from within device driver initialization
 *   context.
 *
 * * The directive may be called from within task context.
 *
 * * The directive may obtain and release the object allocator mutex.  This may
 *   cause the calling task to be preempted.
 * @endparblock
 */
bool rtems_workspace_get_information( Heap_Information_block *the_info );

/* Generated from spec:/rtems/support/if/workspace-greedy-allocate */

/**
 * @ingroup RTEMSAPIClassicSupport
 *
 * @brief Greedy allocates that empties the RTEMS Workspace.
 *
 * @param block_sizes is the array of block sizes.
 *
 * @param block_count is the block count.
 *
 * Afterwards the heap has at most ``block_count`` allocatable blocks of sizes
 * specified by ``block_sizes``.  The ``block_sizes`` must point to an array
 * with ``block_count`` members.  All other blocks are used.
 *
 * @return The returned pointer value may be used to free the greedy allocation
 *   by calling rtems_workspace_greedy_free().
 */
void *rtems_workspace_greedy_allocate(
  const uintptr_t *block_sizes,
  size_t           block_count
);

/* Generated from spec:/rtems/support/if/workspace-greedy-allocate-all-except-largest */

/**
 * @ingroup RTEMSAPIClassicSupport
 *
 * @brief Greedy allocates all blocks of the RTEMS Workspace except the largest
 *   free block.
 *
 * @param allocatable_size is the remaining allocatable size.
 *
 * Afterwards the heap has at most one allocatable block.  This block is the
 * largest free block if it exists.  The allocatable size of this block is
 * stored in ``allocatable_size``.  All other blocks are used.
 *
 * @return The returned pointer value may be used to free the greedy allocation
 *   by calling rtems_workspace_greedy_free().
 *
 * @par Notes
 * This directive is intended to be used by tests of the RTEMS test suites.
 *
 * @par Constraints
 * @parblock
 * The following constraints apply to this directive:
 *
 * * The directive may be called from within device driver initialization
 *   context.
 *
 * * The directive may be called from within task context.
 *
 * * The directive may obtain and release the object allocator mutex.  This may
 *   cause the calling task to be preempted.
 * @endparblock
 */
void *rtems_workspace_greedy_allocate_all_except_largest(
  uintptr_t *allocatable_size
);

/* Generated from spec:/rtems/support/if/workspace-greedy-free */

/**
 * @ingroup RTEMSAPIClassicSupport
 *
 * @brief Frees space of a greedy allocation to the RTEMS Workspace.
 *
 * @param opaque is the pointer value returned by
 *   rtems_workspace_greedy_allocate() or
 *   rtems_workspace_greedy_allocate_all_except_largest().
 *
 * @par Notes
 * This directive is intended to be used by tests of the RTEMS test suites.
 *
 * @par Constraints
 * @parblock
 * The following constraints apply to this directive:
 *
 * * The directive may be called from within device driver initialization
 *   context.
 *
 * * The directive may be called from within task context.
 *
 * * The directive may obtain and release the object allocator mutex.  This may
 *   cause the calling task to be preempted.
 * @endparblock
 */
void rtems_workspace_greedy_free( void *opaque );

#ifdef __cplusplus
}
#endif

#endif /* _RTEMS_RTEMS_SUPPORT_H */
