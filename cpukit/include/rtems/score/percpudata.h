/**
 * @file
 *
 * @ingroup PerCPUData
 *
 * @brief Definition of custom per-CPU items.
 */

/*
 * Copyright (c) 2018 embedded brains GmbH.  All rights reserved.
 *
 *  embedded brains GmbH
 *  Dornierstr. 4
 *  82178 Puchheim
 *  Germany
 *  <rtems@embedded-brains.de>
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rtems.org/license/LICENSE.
 */

#ifndef _RTEMS_SCORE_PERCPUDATA_H
#define _RTEMS_SCORE_PERCPUDATA_H

#include <rtems/score/percpu.h>
#include <rtems/linkersets.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/**
 * @defgroup PerCPUData Flexible Per-CPU Data
 *
 * @ingroup PerCPU
 *
 * @brief Flexible Per-CPU Data
 *
 * Provides the definition of custom per-CPU items.  The items are collected in
 * a special linker set.  During system initialization the content of the
 * linker set is duplicated for all secondary processors using memory allocated
 * from the workspace.  The begin and end of the per-CPU data area is cache
 * line aligned (CPU_CACHE_LINE_BYTES).
 *
 * @{
 */

RTEMS_LINKER_RWSET_DECLARE( _Per_CPU_Data, char );

/**
 * @brief Declares a per-CPU item of the specified type.
 *
 * Items declared with this macro have external linkage.
 *
 * @param type The type of the item.
 * @param item The designator of the item.
 */
#define PER_CPU_DATA_ITEM_DECLARE( type, item ) \
  RTEMS_LINKER_RWSET_ITEM_DECLARE( _Per_CPU_Data, type, item ) \
  RTEMS_SECTION( ".rtemsrwset._Per_CPU_Data.content.1" )

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
