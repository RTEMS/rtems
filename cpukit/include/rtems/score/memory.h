/**
 * @file
 *
 * @ingroup RTEMSScoreMemory
 *
 * @brief Memory Handler API
 */

/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (C) 2019 embedded brains GmbH
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

#ifndef _RTEMS_SCORE_MEMORY_H
#define _RTEMS_SCORE_MEMORY_H

#include <rtems/score/basedefs.h>
#include <rtems/score/assert.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/**
 * @defgroup RTEMSScoreMemory Memory Handler
 *
 * @ingroup RTEMSScore
 *
 * @brief Low level handler to provide memory areas for higher level memory
 * handlers such as the Workspace Handler.
 *
 * @{
 */

/**
 * @brief The memory area description.
 */
typedef struct {
  /**
   * @brief A pointer to the begin of the memory area.
   */
  const void *begin;

  /**
   * @brief A pointer to the begin of the free area of the memory area.
   */
  void *free;

  /**
   * @brief A pointer to the end of the memory area.
   */
  const void *end;
} Memory_Area;

/**
 * @brief The memory information.
 */
typedef struct {
  /**
   * @brief The count of memory areas.
   */
  size_t count;

  /**
   * @brief The memory area table.
   */
  Memory_Area *areas;
} Memory_Information;

/**
 * @brief Statically initialize a memory information.
 *
 * @param areas The designator of an array of the memory areas.
 */
#define MEMORY_INFORMATION_INITIALIZER( areas ) \
  { RTEMS_ARRAY_SIZE( areas ), ( areas ) }

/**
 * @brief Statically initialize a memory area.
 *
 * @param begin The begin of the memory area.
 * @param end The end of the memory area.
 */
#define MEMORY_INITIALIZER( begin, end ) { ( begin ), ( begin ), ( end ) }

/**
 * @brief Get the memory area count.
 *
 * @param information The memory information.
 *
 * @return The memory area count.
 */
RTEMS_INLINE_ROUTINE size_t _Memory_Get_count(
  const Memory_Information *information
)
{
  return information->count;
}

/**
 * @brief Get a memory area by index.
 *
 * @param information The memory information.
 * @param index The index of the memory area to return.
 *
 * @return The memory area of the specified index.
 */
RTEMS_INLINE_ROUTINE Memory_Area *_Memory_Get_area(
  const Memory_Information *information,
  size_t                    index
)
{
  _Assert( index < _Memory_Get_count( information ) );
  return &information->areas[ index ];
}

/**
 * @brief Initialize the memory area.
 *
 * @param area The memory area.
 * @param begin The begin of the memory area.
 * @param end The end of the memory area.
 */
RTEMS_INLINE_ROUTINE void _Memory_Initialize(
  Memory_Area *area,
  void        *begin,
  void        *end
)
{
  area->begin = begin;
  area->free = begin;
  area->end = end;
}

/**
 * @brief Initialize the memory area by size.
 *
 * @param area The memory area.
 * @param begin The begin of the memory area.
 * @param size The size of the memory area in bytes.
 */
RTEMS_INLINE_ROUTINE void _Memory_Initialize_by_size(
  Memory_Area *area,
  void        *begin,
  uintptr_t    size
)
{
  area->begin = begin;
  area->free = begin;
  area->end = (char *) begin + size;
}

/**
 * @brief Get the memory area begin.
 *
 * @param area The memory area.
 *
 * @return The memory area begin.
 */
RTEMS_INLINE_ROUTINE const void *_Memory_Get_begin( const Memory_Area *area )
{
  return area->begin;
}

/**
 * @brief Set the memory area begin.
 *
 * @param area The memory area.
 * @param begin The memory area begin.
 */
RTEMS_INLINE_ROUTINE void _Memory_Set_begin(
  Memory_Area *area,
  const void  *begin
)
{
  area->begin = begin;
}

/**
 * @brief Get the memory area end.
 *
 * @param area The memory area.
 *
 * @return The memory area end.
 */
RTEMS_INLINE_ROUTINE const void *_Memory_Get_end( const Memory_Area *area )
{
  return area->end;
}

/**
 * @brief Set the memory area end.
 *
 * @param area The memory area.
 * @param end The memory area end.
 */
RTEMS_INLINE_ROUTINE void _Memory_Set_end(
  Memory_Area *area,
  const void  *end
)
{
  area->end = end;
}

/**
 * @brief Get the memory area size.
 *
 * @param area The memory area.
 *
 * @return The memory area size in bytes.
 */
RTEMS_INLINE_ROUTINE uintptr_t _Memory_Get_size( const Memory_Area *area )
{
  return (uintptr_t) area->end - (uintptr_t) area->begin;
}

/**
 * @brief Get the begin of the free area of the memory area.
 *
 * @param area The memory area.
 *
 * @return The free memory area begin the memory area.
 */
RTEMS_INLINE_ROUTINE void *_Memory_Get_free_begin( const Memory_Area *area )
{
  return area->free;
}

/**
 * @brief Set the begin of the free area of the memory area.
 *
 * @param area The memory area.
 * @param begin The free memory area begin the memory area.
 */
RTEMS_INLINE_ROUTINE void _Memory_Set_free_begin(
  Memory_Area *area,
  void        *begin
)
{
  area->free = begin;
}

/**
 * @brief Get the size of the free memory area of the memory area.
 *
 * @param area The memory area.
 *
 * @return The free memory area size in bytes of the memory area.
 */
RTEMS_INLINE_ROUTINE uintptr_t _Memory_Get_free_size( const Memory_Area *area )
{
  return (uintptr_t) area->end - (uintptr_t) area->free;
}

/**
 * @brief Consume the specified size from the free memory area of the memory
 * area.
 *
 * @param area The memory area.
 * @param consume The bytes to consume from the free memory area of the memory
 *   area.
 */
RTEMS_INLINE_ROUTINE void _Memory_Consume(
  Memory_Area *area,
  uintptr_t    consume
)
{
  area->free = (char *) area->free + consume;
}

/**
 * @brief Return the memory information of this platform.
 *
 * This function is provided by the Board Support Package (BSP).  Using a
 * function gives the BSPs a bit more freedom with respect to the
 * implementation.  Calling this function shall not have side-effects.
 * Initialization steps to set up the memory information shall be done in a
 * system initialization handler (RTEMS_SYSINIT_MEMORY).
 *
 * @return The memory information.
 */
const Memory_Information *_Memory_Get( void );

/**
 * @brief Allocate a memory area from the memory information.
 *
 * It is not possible to free the memory area allocated by this function.
 *
 * @param information The memory information.
 * @param size The size in bytes of the memory area to allocate.
 * @param alignment The alignment in bytes of the memory area to allocate.  It
 *   must be a power of two.
 *
 * @retval NULL No such memory area available.
 * @retval begin The begin of the allocated memory area.
 */
void *_Memory_Allocate(
  const Memory_Information *information,
  uintptr_t                 size,
  uintptr_t                 alignment
);

/**
 * @brief Fill all free memory areas of the memory information with a constant
 * byte.
 *
 * @param information The memory information.
 * @param c The constant byte to fill the free memory areas.
 */
void _Memory_Fill( const Memory_Information *information, int c );

/**
 * @brief Indicates if the memory is zeroed during system initialization.
 *
 * This value is provided via <rtems/confdefs.h> in case
 * CONFIGURE_ZERO_WORKSPACE_AUTOMATICALLY is defined.
 */
extern const bool _Memory_Zero_before_use;

/**
 * @brief Zeros all free memory areas of the system.
 */
void _Memory_Zero_free_areas( void );

/**
 * @brief Dirty all free memory areas of the system.
 */
void _Memory_Dirty_free_areas( void );

/** @} */

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* _RTEMS_SCORE_MEMORY_H */
