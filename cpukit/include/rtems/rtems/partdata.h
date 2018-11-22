/**
 * @file
 *
 * @ingroup ClassicPartImpl
 *
 * @brief Classic Partition Manager Data Structures
 */

/* COPYRIGHT (c) 1989-2008.
 * On-Line Applications Research Corporation (OAR).
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rtems.org/license/LICENSE.
 */

#ifndef _RTEMS_RTEMS_PARTDATA_H
#define _RTEMS_RTEMS_PARTDATA_H

#include <rtems/rtems/part.h>
#include <rtems/score/isrlock.h>
#include <rtems/score/objectdata.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @addtogroup ClassicPartImpl
 *
 * @{
 */

/**
 *  The following defines the control block used to manage each partition.
 */
typedef struct {
  /** This field is the object management portion of a Partition instance. */
  Objects_Control     Object;
  /** This field is the lock of the Partition. */
  ISR_LOCK_MEMBER(    Lock )
  /** This field is the physical starting address of the Partition. */
  void               *starting_address;
  /** This field is the size of the Partition in bytes. */
  uintptr_t           length;
  /** This field is the size of each buffer in bytes */
  size_t              buffer_size;
  /** This field is the attribute set provided at create time. */
  rtems_attribute     attribute_set;
  /** This field is the of allocated buffers. */
  uintptr_t           number_of_used_blocks;
  /** This field is the chain used to manage unallocated buffers. */
  Chain_Control       Memory;
}   Partition_Control;

/**
 * @brief The Classic Partition objects information.
 */
extern Objects_Information _Partition_Information;

#if defined(RTEMS_MULTIPROCESSING)
/**
 *  @brief Partition_MP_Send_extract_proxy
 *
 *  This routine is invoked when a task is deleted and it
 *  has a proxy which must be removed from a thread queue and
 *  the remote node must be informed of this.
 */
void _Partition_MP_Send_extract_proxy (
  Thread_Control *the_thread,
  Objects_Id      id
);
#endif

/**
 * @brief Macro to define the objects information for the Classic Partition
 * objects.
 *
 * This macro should only be used by <rtems/confdefs.h>.
 *
 * @param max The configured object maximum (the OBJECTS_UNLIMITED_OBJECTS flag
 * may be set).
 */
#define PARTITION_INFORMATION_DEFINE( max ) \
  OBJECTS_INFORMATION_DEFINE( \
    _Partition, \
    OBJECTS_CLASSIC_API, \
    OBJECTS_RTEMS_PARTITIONS, \
    Partition_Control, \
    max, \
    OBJECTS_NO_STRING_NAME, \
    _Partition_MP_Send_extract_proxy \
  )

/** @} */

#ifdef __cplusplus
}
#endif

#endif
/* end of include file */
