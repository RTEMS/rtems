/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup RTEMSScorePerCPUData
 *
 * @brief This header file provides the interfaces of the
 *   @ref RTEMSScorePerCPUData.
 */

/*
 * Copyright (c) 2018 embedded brains GmbH & Co. KG
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

#ifndef _RTEMS_SCORE_PERCPUDATA_H
#define _RTEMS_SCORE_PERCPUDATA_H

#include <rtems/score/percpu.h>
#include <rtems/linkersets.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/**
 * @defgroup RTEMSScorePerCPUData Flexible Per-CPU Data
 *
 * @ingroup RTEMSScorePerCPU
 *
 * @brief This group contains the implementation to support flexible per-CPU
 *   data.
 *
 * Macros to define custom per-CPU items are provided.  The items are collected
 * in a special linker set.  During system initialization the content of the
 * linker set is duplicated for all secondary processors using memory allocated
 * from the workspace.  The begin and end of the per-CPU data area is cache
 * line aligned (CPU_CACHE_LINE_BYTES).
 *
 * @{
 */

RTEMS_LINKER_RWSET_DECLARE( _Per_CPU_Data, char );

/**
 * @brief Translation units which define per-CPU items shall call this macro
 *   exactly once at file scope.
 */
#ifdef RTEMS_SMP
#define PER_CPU_DATA_NEED_INITIALIZATION() \
  static const char * const _Per_CPU_Data_reference \
  RTEMS_SECTION( ".rtemsroset.reference" ) RTEMS_USED = \
  RTEMS_LINKER_SET_BEGIN( _Per_CPU_Data )
#else
#define PER_CPU_DATA_NEED_INITIALIZATION() \
  RTEMS_LINKER_RWSET_DECLARE( _Per_CPU_Data, char )
#endif

/**
 * @brief Declares a per-CPU item of the specified type.
 *
 * Items declared with this macro have external linkage.
 *
 * @param type The type of the item.
 * @param item The designator of the item.
 */
#define PER_CPU_DATA_ITEM_DECLARE( type, item ) \
  RTEMS_LINKER_RWSET_ITEM_DECLARE( _Per_CPU_Data, type, item )

/**
 * @brief Defines a per-CPU item of the specified type.
 *
 * @param type The type of the item.
 * @param item The designator of the item.
 */
#define PER_CPU_DATA_ITEM( type, item ) \
  RTEMS_LINKER_RWSET_ITEM( _Per_CPU_Data, type, item )

/**
 * @brief Returns the offset of the per-CPU item to the begin of the per-CPU
 * data area.
 *
 * @param item The designator of the item.
 */
#define PER_CPU_DATA_OFFSET( item ) \
  ( (uintptr_t) &_Linker_set__Per_CPU_Data_##item \
    - (uintptr_t) RTEMS_LINKER_SET_BEGIN( _Per_CPU_Data ) )

/**
 * @brief Returns a pointer of the specified type to the per-CPU item at the
 * specified offset for the specified processor.
 *
 * @param cpu The processor of the item.
 * @param type The type of the item.
 * @param offset The offset of the item.
 */
#ifdef RTEMS_SMP
#define PER_CPU_DATA_GET_BY_OFFSET( cpu, type, offset ) \
  (type *) ( cpu->data + offset )
#else
#define PER_CPU_DATA_GET_BY_OFFSET( cpu, type, offset ) \
  (type *) ( (uintptr_t) RTEMS_LINKER_SET_BEGIN( _Per_CPU_Data ) + offset ), \
    (void) cpu
#endif

/**
 * @brief Returns a pointer of the specified type to the specified per-CPU item
 * for the specified processor.
 *
 * @param cpu The processor of the item.
 * @param type The type of the item.
 * @param item The designator of the item.
 */
#ifdef RTEMS_SMP
#define PER_CPU_DATA_GET( cpu, type, item ) \
  PER_CPU_DATA_GET_BY_OFFSET( cpu, type, PER_CPU_DATA_OFFSET( item ) )
#else
#define PER_CPU_DATA_GET( cpu, type, item ) \
  &_Linker_set__Per_CPU_Data_##item, (void) cpu
#endif

/** @} */

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* _RTEMS_SCORE_PERCPUDATA_H */
