/**
 * @file
 *
 * @ingroup ClassicRegionImpl
 *
 * @brief Classic Region Manager Data Structures
 */

/* COPYRIGHT (c) 1989-2013.
 * On-Line Applications Research Corporation (OAR).
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rtems.org/license/LICENSE.
 */

#ifndef _RTEMS_RTEMS_REGIONDATA_H
#define _RTEMS_RTEMS_REGIONDATA_H

#include <rtems/rtems/region.h>
#include <rtems/score/heap.h>
#include <rtems/score/objectdata.h>
#include <rtems/score/threadq.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @addtogroup ClassicRegionImpl
 *
 * @{
 */

/**
 *  The following records define the control block used to manage
 *  each region.
 */

typedef struct {
  Objects_Control       Object;
  Thread_queue_Control  Wait_queue;            /* waiting threads        */
  const Thread_queue_Operations *wait_operations;
  uintptr_t             maximum_segment_size;  /* in bytes               */
  rtems_attribute       attribute_set;
  Heap_Control          Memory;
}  Region_Control;

/** @} */

#ifdef __cplusplus
}
#endif

#endif
/* end of include file */
